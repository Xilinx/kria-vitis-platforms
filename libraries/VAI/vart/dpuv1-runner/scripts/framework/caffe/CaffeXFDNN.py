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

import caffe
import time

from vai.dpuv1.rt import xdnn, xdnn_io

# Our custom FPGA One-shot layer
class CaffeXFDNN(caffe.Layer):

  # Called once when the network is initialized
  def setup(self, bottom, top):
    self.param_dict = eval(self.param_str) # Get args from prototxt
    self._args = xdnn_io.make_dict_args(self.param_dict)
    self._numPE = self._args["batch_sz"] # Bryan hack to detremine number of PEs in FPGA
    # Establish FPGA Communication, Load bitstream
    self._args["PE"] = -1
    # Instantiate runtime interface object
    self.fpgaRT = xdnn.XDNNFPGAOp(self._args)
    self._indictnames = self._args["input_names"]
    self._outdictnames =  self._args["output_names"]
    self._parser = xdnn.CompilerJsonParser(self._args["netcfg"])

  # Called before every forward
  def reshape(self, bottom, top):
    bsz = bottom[0].num
    for i,n in enumerate( self._indictnames ):
      dim = self._parser.getInputs()[n]
      dim[0] = bsz
      #print ( "INPUT NAME: ", n, "SHAPE: ", dim)
      t = tuple(dim)
      bottom[i].reshape (*t)

    for i,n in enumerate( self._outdictnames ):
      dim = self._parser.getOutputs()[ n ]
      dim[0] = bsz
      #print ( "OUTPUT NAME: ", n, "SHAPE: ", dim)
      t = tuple ( dim )
      top[i].reshape(*t)

  # Called for every batch
  def forward(self, bottom, top):
    indict = {}
    outdict = {}
    for i,n in enumerate(self._indictnames):
      indict[ n ] = bottom[i].data

    for i,n in enumerate(self._outdictnames):
      outdict[ n ] = top[i].data

    self.fpgaRT.execute(indict, outdict)

  def backward(self, top, propagate_down, bottom):
    raise Exception("Can't do backward propagation... yet")
