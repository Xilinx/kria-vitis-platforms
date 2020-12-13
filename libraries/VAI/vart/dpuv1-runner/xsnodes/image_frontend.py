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
import logging
import requests

import numpy as np
import cv2

from vai.dpuv1.rt.xsnodes.base import XstreamNode, XstreamNodeArgs

logger = logging.getLogger(__name__)

class Node(XstreamNode):
    """
    This node parses image data that may be sent in one of a few ways and places
    it in the buffer for downstream nodes to use.

    Required Keys:
        mode: Must be one of url, image*, local or none

    Situational Keys:
        url: Used if mode=url to specify the path to the image
        dtype: Used if mode=(url|image) to specify the attribute from np
            to read the data as e.g. uint8 or float32

    Added Keys:
        image_height: height of image in pixels
        image_width = width of image in pixels
        image_channels = channels in the image

    Raises:
        NotImplementedError: Raised if unsupported else clauses execute
    """
    def loop(self, payload):
        (meta, buf) = payload
        meta['status'] = 0
        if meta['mode'] == 'url':
            try:
                response = requests.get(meta['url'], stream=True)
            except requests.exceptions.SSLError:
                logger.error("Image failed to load, SSL error")
                meta['status'] = -1
                image = np.zeros((1,1,3), getattr(np, meta['dtype']))
            else:
                img = np.asarray(bytearray(response.content), getattr(np, meta['dtype']))
                del response
                image = cv2.imdecode(img, cv2.IMREAD_COLOR)
        elif (str(meta['mode'])).startswith('image'):
            img = np.frombuffer(buf, getattr(np, meta['dtype']))
            image = cv2.imdecode(img, cv2.IMREAD_COLOR)
        elif meta['mode'] == 'local':
            image = cv2.imread(meta['path'])
        elif meta['mode'] == 'none':
            image = np.zeros((1,1,3))
        else:
            raise NotImplementedError

        meta['image_height'] = image.shape[0]
        meta['image_width'] = image.shape[1]
        meta['image_channels'] = image.shape[2]

        # delete objects to decrement obj store refcount
        # -- this is so the object can be auto-deleted,
        #    so we can reuse the id in the put() below
        del buf
        del payload

        self.put(meta, image)

class Args(XstreamNodeArgs):
    def __init__(self):
        super(Args, self).__init__()

        self.add_input_serve_arg(
            'BUFFER',
            "The buffer may contain a image to be decoded with opencv using " +
                "the np.dtype depending on mode"
        )
        self.add_input_serve_arg(
            'url',
            "(str, situtational): Used if mode=url to specify the path to the image"
        )
        self.add_input_serve_arg(
            'dtype',
            "(str, situational): Used if mode=(url|image) to specify the attribute " +
                "from np to read the data as e.g. uint8 or float32"
        )
        self.add_input_serve_arg(
            'path',
            "(str, situational): A local path on the server to an image"
        )
        self.add_input_serve_arg(
            'mode',
            "(str): Must be one of -- url (get an image from this URL), " +
                "image (the image is encoded in the buffer as a JPEG), " +
                "local (the image is a file on the local server) or " +
                "none (pass forward an array of zeroes)"
        )

        self.add_output_serve_arg(
            'image_height',
            "Height of the image in pixels"
        )
        self.add_output_serve_arg(
            'image_width',
            "Width of the image in pixels"
        )
        self.add_output_serve_arg(
            'image_channels',
            "Channels in the image (usually should be 3)"
        )
        self.add_output_serve_arg(
            'BUFFER',
            "The decoded or grabbed image is placed in the buffer"
        )
