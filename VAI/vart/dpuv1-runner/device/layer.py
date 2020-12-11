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
class layer(object):
  def setOp(self, operator):
    self.op = operator

  def setName(self, name):
    self.name = name

  def setInput(self, inputs):
    self.inputs = inputs

  def setType(self, typename):
    self.type = typename

  def setOutput(self, output):
    if not hasattr(self, 'name'):
      self.name = output
    self.output = output

  def setShape(self, shape):
    self.shape = shape

  def set_layer_params(self, params, weights=None):
    self.setInput(params['bottoms'])
    self.setOutput(params['name'])
    self.outshape = params['outputshapes']
    if 'deeppy' in params :
        self.deephi_quantizations = params['deeppy']

  def setup(self):
    pass

  def forward_exec(self, inputs):
    pass
