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
import os
import json
from functools import reduce

from collections import OrderedDict
import numpy as np

from vai.dpuv1.rt.device.layer import layer as _layer
from vai.dpuv1.rt.xdnn import XDNNFPGAOp
from vai.dpuv1.rt.xdnn_util import timer

class layer_fpga_pyfunc(_layer):
  def __init__(self, XDNNFPGAargs, boundryMap):
    self._execute = None
    self._XDNNFPGAargs = XDNNFPGAargs
    self._boundryMap = boundryMap

  def forward_exec(self, inputs):
    if 'inputs' not in self._boundryMap or 'outputs' not in self._boundryMap:
      raise RuntimeError('''Run compiler with \"cpulayermustgo\" flag to produce \"inputs\" and
                         \"outputs\" fields in compilerJson.''')

    batch_size = None
    inputMap = {}
    batchSizeMap = {}
    inShapeMap = {}
    for (name, shape), val in zip(self._boundryMap['inputs'], inputs):
      inp = np.empty(val.shape, dtype=np.float32, order='C_CONTIGUOUS')
      np.copyto(inp, val) # assuming input data format is nchw

      if not batch_size:
        batch_size = inp.shape[0]

      if batch_size != inp.shape[0]:
        raise RuntimeError('inputs batch sizes do not match: Expected {}, Received {}'.format(batch_size, inp.shape[0]))

      inputMap[name] = inp
      batchSizeMap[name] = inp.shape[0]
      inShapeMap[name] = tuple(inp.shape[1:])

    self._XDNNFPGAargs.update({
      'batch_sz':       batchSizeMap,
      #'in_shape':       inShapeMap,
    })

    outputMap = {}
    outShapeMap = {}
    for (name, shape) in self._boundryMap['outputs']:
      fpgaOutSize = reduce((lambda x, y: x * y), shape[1:])
      outputMap[name] = np.empty((batch_size, fpgaOutSize,), dtype=np.float32, order='C_CONTIGUOUS')
      outShapeMap[name] = tuple(shape[1:])

    if self._execute is None:
      self._execute = timer(XDNNFPGAOp(self._XDNNFPGAargs).execute)

    self._execute(inputMap, outputMap)

    return {name: output.reshape((batch_size,) + outShapeMap[name]) for name, output in outputMap.items()}
