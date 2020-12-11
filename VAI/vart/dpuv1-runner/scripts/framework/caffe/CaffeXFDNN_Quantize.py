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

from vai.dpuv1.rt.scripts.framework.base.quantize_controls import quantize_controls

class CaffeXFDNN_Quantize(caffe.Layer):
  def setup(self, bottom, top):
    _args = eval(self.param_str) # Get args from prototxt
    self._xdnn_env = quantize_controls(qcfg=_args['quantizecfg'], xdnn_lib=_args['xdnn_lib'])
    params = self._xdnn_env.get_params()
    self.name = _args["name"]

  def reshape(self, bottom, top):
    for i in range(len(bottom)):
      dim = bottom[i].data.shape
      top[i].reshape(*dim)

  def forward(self, bottom, top):
    inps = [bottom[i].data for i in range(len(bottom))]
    outs = self._xdnn_env.quantize_inputs(inps, self.name)
    for i in range(len(outs)) :
      top[i].data[...] = outs[i]


  def backward(self, top, propagate_down, bottom):
    raise Exception("Can't do backward propagation... yet")
