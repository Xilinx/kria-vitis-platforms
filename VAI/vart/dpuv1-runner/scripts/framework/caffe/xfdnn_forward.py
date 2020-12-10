#!/usr/bin/env python
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
from __future__ import print_function

import caffe
import argparse

# Need to create derived class to clean up properly
class Net(caffe.Net):
  def __del__(self):
    for layer in self.layer_dict:
      if hasattr(self.layer_dict[layer],"fpgaRT"):
        del self.layer_dict[layer].fpgaRT

def forward(prototxt,caffemodel,numBatches):
  net = Net(prototxt,caffemodel,caffe.TEST)
  inputKey = list(net.blobs.keys())[0]
  ptxtShape = net.blobs[inputKey].data.shape
  print("Running with shape of: ",ptxtShape)
  accum = {}
  for i in range(1,numBatches+1): # 1000 iterations w/ batchSize 50 will yield 50,000 images
    out = net.forward()
    for k in out:
      if out[k].size != 1:
        continue
      if k not in accum:
        accum[k] = 0.0
      accum[k] += out[k]
      print(k, " -- This Batch: ",out[k]," Average: ",accum[k]/i," Batch#: ",i)

if __name__ == "__main__":
  parser = argparse.ArgumentParser(description='pyXDNN')
  parser.add_argument('--prototxt', default="", help='User must provide the prototxt')
  parser.add_argument('--caffemodel', default="", help='User must provide the caffemodel')
  parser.add_argument('--numBatches', type=int, default=1000, help='User must provide the caffemodel')
  args = vars(parser.parse_args())
  forward(args["prototxt"],args["caffemodel"],args["numBatches"])
