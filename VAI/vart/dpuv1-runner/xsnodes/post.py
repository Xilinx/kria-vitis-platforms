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
import numpy as np
import time, timeit
from vai.dpuv1.rt import xdnn, xdnn_io
from vai.dpuv1.rt import xstream
from vai.dpuv1.rt.xsnodes.base import XstreamNode
import zmq
from six import itervalues, next

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
    self.xdnnCPUOp = xdnn.XDNNCPUOp(args['weights'])

    print("Post is starting loop")
    self.run()

  #
  # This function post-processes FPGA output:
  # 1) Compute the final FC + Softmax layers
  # 2) Print classification & accuracy
  #
  def _run(self, imgList, fpgaOutput):
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

      self.fcOutput = np.empty((self._args['batch_sz'], self._args['outsz'],),
        dtype=np.float32, order='C')

    self.xdnnCPUOp.computeFC(fpgaOutput, self.fcOutput)

    smaxOutput = self.xdnnCPUOp.computeSoftmax(self.fcOutput)
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

  def run(self):
    self.fpgaOutputShape = next(itervalues(xdnn.CompilerJsonParser(self._args['netcfg']).getOutputs()))
    self.fpgaOutputShape[0] = self._args['batch_sz']
    super(Node, self).run()

  def loop(self, payload):
    if self.numProcessed == 0:
      self.startTime = timeit.default_timer()

    (meta, buf) = payload
    imgList = []

    for ri,r in enumerate(meta['requests']):
      imgList.append(r['path'])

    if not self._args["benchmarkmode"]:
      fpgaOutput = np.copy(np.frombuffer(buf, dtype=np.float32)\
        .reshape(self.fpgaOutputShape))
      ret = self._run(imgList, fpgaOutput)

      if ret and 'predictions' in ret:
        meta['requests'][ri]['predictions'] = ret['predictions']

        if 'resized_shape' in meta['requests'][ri]:
          labels = ""
          for p in ret['predictions'][0]:
            labels += "%.0f%% %s\n" % (float(p['score'])*100, p['label'])
          boxes = [[ 0, 0, 0, 0, labels ]]
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
      % (float(self.numProcessed) / (timeit.default_timer() - self.startTime)))

    if not self._args["benchmarkmode"] \
      and self._args['golden'] and self.numProcessed:
      print ("\nAverage accuracy (n=%d) Top-1: %.1f%%, Top-5: %.1f%%\n" \
        % (self.numProcessed,
          float(self.top1Count)/float(self.numProcessed)*100.,
          float(self.top5Count)/float(self.numProcessed)*100.))

    super(Node, self).finish()
