# Copyright 2019 Xilinx Inc.
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
import time
import timeit
from six import itervalues, next

import nms
if not hasattr(nms, "do_baseline_nms"): # in release, nms exists in site-packages
  from nms import nms
import numpy as np
import zmq

from vai.dpuv1.rt import xdnn, xdnn_io, xstream
from vai.dpuv1.rt.xsnodes.base import XstreamNode

class ZmqResultPublisher:
  def __init__(self, devid):
    import zmq
    self.context = zmq.Context()
    self.socket = self.context.socket(zmq.PUB)
    self.socket.bind("tcp://*:55{}5".format(devid))

  def send(self, data):
    self.socket.send(data)

class Node(XstreamNode):
  def __init__(self, name='post', inputs=['post'], outputs=['DONE'], args=None):
    super(Node, self).__init__(\
      name=name, inputs=inputs, outputs=outputs, args=args)

  def initialize(self, args):
    self.numProcessed = 0
    self.startTime = timeit.default_timer()

    print("Post is starting loop")
    self.run()

  # Utility function, uses numpy to do sigmoid
  @staticmethod
  def sigmoid(x):
    pos_mask = (x >= 0)
    neg_mask = (x < 0)
    z = np.zeros_like(x)
    z[pos_mask] = np.exp(-x[pos_mask])
    z[neg_mask] = np.exp(x[neg_mask])
    top = np.ones_like(x)
    top[neg_mask] = z[neg_mask]
    return top / (1 + z)

  # Utility function, uses numpy to do softmax
  @staticmethod
  def softmax(x):
    e_x = np.exp(x-np.max(x))
    return (e_x)/(e_x.sum(axis=1,keepdims=True))

  #
  # This function post-processes FPGA output:
  # 1) Compute the final FC + Softmax layers
  # 2) Print classification & accuracy
  #
  def _run(self, imgList, imgShape, fpgaOutput):
    if self.numProcessed == 0:
      self.startTime = timeit.default_timer()
      self.labels = xdnn_io.get_labels(self._args['labels'])
      self.zmqPub = None
      if self._args['zmqpub']:
        self.zmqPub = ZmqResultPublisher(self._args['deviceID'])
      self.goldenMap = None
      if self._args['golden']:
        self.goldenMap = xdnn_io.getGoldenMap(self._args['golden'])
        self.top5Count = 0
        self.top1Count = 0

    # Get shape of fpgaOutput
    (out_b,out_c,out_h,out_w) = fpgaOutput.shape # batches, channels, height, width

    # Reshape to reduce number of python for loops
    fpgaOutput = fpgaOutput.reshape(out_b,self._args['anchor_boxes'],5+self._args['classes'],out_h,out_w)

    # Apply sigmoid to 1st, 2nd, 4th channel for all batches, and anchor boxes
    fpgaOutput[:,:,0:2,:,:] = self.sigmoid(fpgaOutput[:,:,0:2,:,:]) # (X,Y) Predictions are squashed between 0,1
    fpgaOutput[:,:,4,:,:]   = self.sigmoid(fpgaOutput[:,:,4,:,:])   # Objectness / Box Confidence, is squashed to a probability between 0,1

    # Apply softmax on the class scores foreach anchor box in each batch
    for i in range(self._args['batch_sz']):
      for j in range(self._args['anchor_boxes']):
        fpgaOutput[i,j,5:,:,:]  = self.softmax(fpgaOutput[i,j,5:,:,:])

    bboxes = []
    for i in range(self._args['batch_sz']):
      bboxes.append(nms.do_baseline_nms(
        fpgaOutput[i].flat,
        imgShape[i][1],
        imgShape[i][0],
        32*out_h,
        32*out_w,
        out_h,
        out_w,
        self._args['anchor_boxes'],
        self._args['classes'],
        self._args['scorethresh'],
        self._args['iouthresh']))
      for j in range(len(bboxes[i])):
        bboxes[i][j]['label'] = self.labels[bboxes[i][j]['classid']]

    if self._args['servermode']:
      return bboxes
    #logger.info("POST BLO - " + str(bboxes))

    """
    if self._args['golden']:
      for i,p in enumerate ( imgList ):
        self.top1Count += xdnn_io.isTopK(\
          smaxOutput[i], self.goldenMap, p, self.labels, 1)
        self.top5Count += xdnn_io.isTopK(\
          smaxOutput[i], self.goldenMap, p, self.labels, 5)

    if self._args['servermode']:
      return xdnn_io.getClassification(\
        smaxOutput, imgList, self.labels, returnDict=True)
    elif self.zmqPub is not None:
      predictMsg = xdnn_io.getClassification(\
        smaxOutput, imgList, self.labels, zmqPub=True)
      self.zmqPub.send(predictMsg)
    """

  def run(self):
    self.fpgaOutputShape = next(itervalues(xdnn.CompilerJsonParser(self._args['netcfg']).getOutputs()))
    self.fpgaOutputShape[0] = self._args['batch_sz']
    super(Node, self).run()

  def loop(self, payload):
    if self.numProcessed == 0:
      self.startTime = timeit.default_timer()

    (meta, buf) = payload
    imgList = []
    imgShape = []

    for ri,r in enumerate(meta['requests']):
      imgList.append(r['path'])
      imgShape.append(r['resized_shape'])

    if not self._args["benchmarkmode"]:
      fpgaOutput = np.copy(np.frombuffer(buf, dtype=np.float32)\
        .reshape(self.fpgaOutputShape))

      image_detections = self._run(imgList, imgShape, fpgaOutput)  # N images with K detections per image, each detection is a dict... list of list of dict

      #[[{"classid": 21, "ll": {"y": 663, "x": 333}, "ur": {"y": 238, "x": 991}, "prob": 0.6764760613441467, "label": "bear"}]]

      for ri,r in enumerate(meta['requests']):
        detections = image_detections[ri] # Examine result for first image
        boxes = []
        # detection will be a dict
        for detection in detections:
          x1 = detection["ll"]["x"]
          #y1 = detection["ll"]["y"] # ONEHACK to conform to the way facedetect does corners
          y1 = detection["ur"]["y"]
          x2 = detection["ur"]["x"]
          #y2 = detection["ur"]["y"]
          y2 = detection["ll"]["y"]
          label = detection["label"]
          boxes.append([x1,y1,x2,y2,label])

        meta['requests'][ri]['boxes'] = boxes
        meta['requests'][ri]['callback'] = self._callback

    self.numProcessed += len(meta['requests'])

    # TODO shouldn't meta always have requests?
    if 'requests' in meta:
      for r in meta['requests']:
        self.put(r)
    del buf
    del payload

  def finish(self):
    print ( "%g images/s" \
      % (float(self.numProcessed) / (timeit.default_timer() \
         - self.startTime) ))

    if not self._args["benchmarkmode"] \
      and self._args['golden'] and self.numProcessed:
      print ("\nAverage accuracy (n=%d) Top-1: %.1f%%, Top-5: %.1f%%\n") \
        % (self.numProcessed,
           float(self.top1Count)/float(self.numProcessed)*100.,
           float(self.top5Count)/float(self.numProcessed)*100.)

    print("yolopost is ending %s" % self._outputs[0])
    self.end(0)
