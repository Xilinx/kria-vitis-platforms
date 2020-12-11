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





class hw_opTree(object):
    class hw_opNode(object):
        def __init__(self, key):
            self.key = key
            self.before = []
            self.after = []

        def in_before(self, other):
            return other in self.before

        def in_after(self, other):
            return other in self.after

    def __init__(self, core, after_dict):
        self.nodeMap = {}
        self.root = self.get_opNode()

        for op in core:
          node = self.get_opNode(op)
          x = self.get_opNode(None)
          node.before.append(x)
          x.after.append(node)

        for op, after in after_dict.items():
          op = self.get_opNode(op)
          for x in after:
            x = self.get_opNode(x)
            op.after.append(x)
            x.before.append(op)

    def get_opNode(self, key=None):
        if key not in self.nodeMap:
          self.nodeMap[key] = hw_opTree.hw_opNode(key)
        return self.nodeMap[key]

    def is_atom_op(self, opNode):
        return opNode.in_before(self.root)





class hw_xdnn(object):
    def __init__(self, version=3, platform='tensorflow'):
        self.version = version
        self._opTree = hw_xdnn.xdnn_opTree()
        if platform.lower() == 'tensorflow':
          self._validOps = hw_xdnn.xdnn2tf_opMap()
        self._opMap = self.platform2hw_opMap(self._validOps)

    def get_op(self, op):
        return self._opMap.get(op, None)

    def is_supported(self, op):
        return (op in self._validOps)

    def platform2hw_opMap(self, mapping):
        ret = {}
        for xdnn_op, tf_ops in mapping.items():
          ret.update({op: xdnn_op for op in tf_ops})
        return ret

    ### platform specific methods
    @classmethod
    def xdnn2tf_opMap(cls):
        return {'Convolution':    ['Conv2D','Convolution','SeparableConv2D', 'DepthwiseConv2dNative'],
                'Deconvolution':  ['Deconvolution','Conv2DBackpropInput'],
                'MatrixMultiply': ['MatMul'],
                'Pooling':        ['Pooling','Pool', 'MaxPool','AvgPool','Mean'],
                'Scale':          ['Add', 'AddN', 'Mul', 'Eltwise', 'BiasAdd', 'FusedBatchNorm','BatchNorm','BatchNormWithGlobalNormalization'],
                'ReLU':           ['Relu','Prelu','Relu6'],
                'Padding':        ['Pad'],
                'Shape':          ['Shape', 'Reshape'],
                'Slice':          ['Slice', 'StridedSlice'],
                'Maximum':        ['Maximum'],
                'NOP':            ['Concat', 'ConcatV2', 'Flatten', 'Pack'],
               }

    ### hardware specific methods
    @classmethod
    def xdnn_opTree(cls):
        core = ['Convolution', 'Deconvolution', 'Scale', 'Pooling']
        after_dict = {'Convolution':    ['ReLU', 'Scale', 'Pooling', 'NOP'],
                      'Deconvolution':  ['ReLU', 'Scale', 'Pooling', 'NOP'],
                      'MatrixMultiply': ['Scale'],
                      'Scale':          ['ReLU', 'Scale','Maximum'],
                      'Padding':        ['Convolution', 'Deconvolution', 'Pooling'],
                      'ReLU':           ['NOP', 'Pooling'],
                      'Shape':          ['Convolution', 'Deconvolution', 'MatrixMultiply', 'Slice'],
                      'Slice':          ['NOP'],
                      'NOP':            ['NOP', 'Shape'],
                     }
        return hw_opTree(core, after_dict)






if __name__ == '__main__':
  x = hw_xdnn(version=3, platform='tensorflow')
  print('valid ops:\n{}'.format(x._validOps))
  print('op map:\n{}'.format(x._opMap))
  print('op tree:\n{}'.format(x._opTree))
