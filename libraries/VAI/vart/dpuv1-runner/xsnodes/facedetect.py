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
import base64
from six import itervalues, next

import cv2
import numpy as np

from vai.dpuv1.rt import xdnn, xdnn_io
from vai.dpuv1.rt.xsnodes.base import XstreamNode, XstreamNodeArgs

class Node(XstreamNode):
    """
    This class implements the Facedetect node. Initializing the class creates
    and starts the node through the loop() method

    Args:
        XstreamNode (XstreamNode): The base class of all nodes

    Returns:
        Facedetect: An instance of the class
    """

    def initialize(self, args):
        self._parser = xdnn.CompilerJsonParser(args['netcfg'])
        self._fpgaRT = xdnn.XDNNFPGAOp(args)
        self._indictnames = args["input_names"]
        self._outdictnames = args["output_names"]

        self._firstInputShape = next(itervalues(self._parser.getInputs()))
        self._meanarr = np.zeros(
            (
                self._firstInputShape[2],
                self._firstInputShape[3],
                self._firstInputShape[1],
            ),
            dtype=np.float32, order='C'
        )

        self.expand_scale_ = 0.0
        self.force_gray_ = False
        self.input_mean_value_ = 128.0
        self.input_scale_ = 1.0
        self.pixel_blob_name_ = 'pixel-prob'
        self.bb_blob_name_ = 'bb-output-tiled'

        self.res_stride_ = 4
        self.det_threshold_ = 0.7
        self.nms_threshold_ = 0.3
        self.input_channels_ = 3

        self._streamIds = [0, 1, 2, 3, 4, 5, 6, 7] # Allow 8 streams

        print("Facedetect: Starting loop")
        self.run()

    # --------------------------------------------------------
    # Fast R-CNN
    # Copyright (c) 2015 Microsoft
    # Licensed under The MIT License [see LICENSE for details]
    # Written by Ross Girshick
    # --------------------------------------------------------
    @staticmethod
    def nms(dets, thresh):
        """
        Implementation of Fast R-CNN using non-maximum supression (nms)

        Args:
            dets ([type]): [description]
            thresh ([type]): [description]

        Returns:
            [type]: [description]
        """
        x1 = dets[:, 0]
        y1 = dets[:, 1]
        x2 = dets[:, 2]
        y2 = dets[:, 3]
        scores = dets[:, 4]

        areas = (x2 - x1 + 1) * (y2 - y1 + 1)
        order = scores.argsort()[::-1]

        keep = []
        while order.size > 0:
            i = order[0]
            keep.append(i)
            xx1 = np.maximum(x1[i], x1[order[1:]])
            yy1 = np.maximum(y1[i], y1[order[1:]])
            xx2 = np.minimum(x2[i], x2[order[1:]])
            yy2 = np.minimum(y2[i], y2[order[1:]])

            w = np.maximum(0.0, xx2 - xx1 + 1)
            h = np.maximum(0.0, yy2 - yy1 + 1)
            inter = w * h
            ovr = inter / (areas[i] + areas[order[1:]] - inter)

            inds = np.where(ovr <= thresh)[0]
            order = order[inds + 1]

        return keep

    def GSTilingLayer_forward(self, bottom, stride):
        stride_sq = stride**2

        input_batch = bottom.shape[0]
        input_channels = bottom.shape[1]
        input_height = bottom.shape[2]
        input_width = bottom.shape[3]

        output_batch = input_batch
        output_channels = input_channels//stride_sq
        output_height = input_height*stride
        output_width = input_width*stride

        top = np.zeros(
            [output_batch, output_channels, output_height, output_width],
            dtype=np.float32
        )

        for n in range(input_batch):
            for ic in range(input_channels):
                off_div = (ic // output_channels) // stride
                off_mod = (ic // output_channels) % stride
                oc = ic % output_channels
                for iy in range(input_height):
                    oy = iy * stride + off_div
                    ox = off_mod - stride
                    top[n,oc,oy,off_mod::stride] = bottom[n,ic,iy,:input_width]

        return top


    def SoftmaxLayer_forward(self, bottom):
        input_batch = bottom.shape[0]
        input_channels = bottom.shape[1]
        input_height = bottom.shape[2]
        input_width = bottom.shape[3]

        top = np.zeros(
            [input_batch, input_channels, input_height, input_width],
            dtype=np.float32
        )

        for n in range(input_batch):

            scale_data = np.zeros([input_height,input_width],dtype=np.float32)
            scale_data = bottom[n,0,...]

            for c in range(1,input_channels):
                scale_data = np.maximum(scale_data, bottom[n, c, ...])

            tmp_bottom = bottom[n, ...] - scale_data
            tmp_bottom = np.exp(tmp_bottom)

            scale_data = np.sum(tmp_bottom, axis=0)
            tmp_bottom = tmp_bottom / scale_data
            top[n] = tmp_bottom

        return top

    # Called for every batch
    def forward(self, bottom):
        top = [0]*len(self._outdictnames)

        indict = {}
        outdict = {}

        for i, n in enumerate(self._indictnames):
            # default to 1 batch
            indict[n] = np.ascontiguousarray(np.expand_dims(bottom[i], 0))

        for i, name in enumerate(self._outdictnames):
            dim = self._parser.getOutputs()[name]
            top[i] = np.empty(dim, dtype=np.float32)
            outdict[name] = top[i]

        # Get a free stream if available
        if self._streamIds:
            streamId = self._streamIds.pop(0)
        else:
            return None

        start_time = time.time()
        # self.fpgaRT.execute(indict, outdict, streamId)
        self._fpgaRT.exec_async(indict, outdict, streamId)
        self._fpgaRT.get_result(streamId)
        end_time = time.time()

        self._streamIds.append(streamId) # Return stream

        return outdict

    def detect(self, image):
        # transpose HWC (0,1,2) to CHW (2,0,1)
        transformed_image = np.transpose(image, (2, 0, 1))

        transformed_image = (transformed_image-self.input_mean_value_)*self.input_scale_
        sz = image.shape

        # Call FPGA
        output = self.forward([transformed_image.astype(np.float32)])

        # Put CPU layers into postprocess
        pixel_conv = output['pixel-conv']
        pixel_conv_tiled = self.GSTilingLayer_forward(pixel_conv, 8)
        prob = self.SoftmaxLayer_forward(pixel_conv_tiled)
        prob = prob[0, 1, ...]

        bb = output['bb-output']
        bb = self.GSTilingLayer_forward(bb, 8)
        bb = bb[0, ...]


        ##import pdb; pdb.set_trace()
        gy = np.arange(0, sz[0], self.res_stride_)
        gx = np.arange(0, sz[1], self.res_stride_)
        gy = gy[0 : bb.shape[1]]
        gx = gx[0 : bb.shape[2]]
        [x, y] = np.meshgrid(gx, gy)

        #print bb.shape[1],len(gy),sz[0],sz[1]
        bb[0, :, :] += x
        bb[2, :, :] += x
        bb[1, :, :] += y
        bb[3, :, :] += y
        bb = np.reshape(bb, (4, -1)).T
        prob = np.reshape(prob, (-1, 1))
        bb = bb[prob.ravel() > self.det_threshold_, :]
        prob = prob[prob.ravel() > self.det_threshold_, :]
        rects = np.hstack((bb, prob))
        keep = self.nms(rects, self.nms_threshold_)
        rects = rects[keep, :]
        rects_expand = []
        for rect in rects:
            rect_expand = []
            rect_w = rect[2] - rect[0]
            rect_h = rect[3] - rect[1]
            rect_expand.append(int(max(0,rect[0]-rect_w*self.expand_scale_)))
            rect_expand.append(int(max(0,rect[1]-rect_h*self.expand_scale_)))
            rect_expand.append(int(min(sz[1],rect[2]+rect_w*self.expand_scale_)))
            rect_expand.append(int(min(sz[0],rect[3]+rect_h*self.expand_scale_)))
            rect_expand.append('face')
            rects_expand.append(rect_expand)

        return rects_expand

    def loop(self, payload):
        (meta, buf) = payload
        meta['callback'] = self._callback

        npimg = np.frombuffer(buf, dtype=getattr(np, meta['dtype']))
        image = np.reshape(npimg, (meta['image_height'], meta['image_width'], meta['image_channels']))

        image_resize = cv2.resize(
            image,
            (320, 320),
            interpolation=cv2.INTER_LINEAR
        )
        if image_resize is not None:
            boxes = self.detect(image_resize)
            meta['boxes'] = boxes
            meta['status'] = 0
            retval, img_str = cv2.imencode(".jpg", image_resize)

            if retval:
                base64_str = base64.b64encode(img_str)
                meta['img'] = base64_str.decode('utf-8')
            else:
                raise NotImplementedError

        # delete objects to decrement obj store refcount
        # -- this is so the object can be auto-deleted,
        #        so we can reuse the id in the put() below
        del buf
        del payload

        self.put(meta)

class Args(XstreamNodeArgs):
    def __init__(self):
        super(Args, self).__init__()

        self.add_start_arg(
            'netcfg',
            "(?): insert description of netcfg"
        )
        self.add_start_arg(
            '_parser',
            "(?): insert description of _parser"
        )
        self.add_start_arg(
            '_indictnames',
            "(str): insert description of _indictnames"
        )
        self.add_start_arg(
            '_outdictnames',
            "(str): insert description of _outdictnames"
        )
        self.add_start_arg(
            'XDNNFPGAOp_args',
            "(?): Not sure which arguments might be needed for the XDNNFPGAOp instance"
        )

        self.add_input_serve_arg(
            'BUFFER',
            "The buffer should contain an image to be parsed with np.dtype"

        )
        self.add_input_serve_arg(
            'dtype',
            "(str): This should be an attribute of np to indicate how to interpret the" +
                "numpy array"
        )
        self.add_input_serve_arg(
            'image_height',
            "(int): Height of the image in pixels"
        )
        self.add_input_serve_arg(
            'image_width',
            "(int): Width of the image in pixels"
        )
        self.add_input_serve_arg(
            'image_channels',
            "(int): Channels in the image (usually should be 3)"
        )

        self.add_output_serve_arg(
            'callback',
            "(str): The meta is updated with the callback value of this node"
        )
        self.add_output_serve_arg(
            'BUFFER',
            "The buffer is emptied"
        )
        self.add_output_serve_arg(
            'status',
            "0 - success"
        )
        self.add_output_serve_arg(
            'boxes',
            "(dict): Detected faces in this format: TBD"
        )
        self.add_output_serve_arg(
            'img',
            "(str): Base64-encoded image URI from the input buffer"
        )
