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

import tensorflow as tf

from vai.dpuv1.rt.xdnn_util import dict2attr
from vai.dpuv1.rt.xdnn_util_tf import load_graph, discover_sourcenodes, discover_sinknodes
from vai.dpuv1.rt.coloring.graph_coloring_base import CGraph as _CGraph



class CGraph_tensorflow(_CGraph):
    def __init__(self, args, **kwargs):
        args = dict2attr(args)
        args.update(kwargs)

        super(CGraph_tensorflow, self).__init__(args)

        self.graph_def, \
        self.startnode, \
        self.finalnode, \
        __ = load_graph(args)

        print('\n#####################################')
        print('adding nodes to graph')
        print('#####################################')
        for node in self.graph_def.node:
          self.add_node(node.name, node.op, node.input[:])










def test_graph():
    from vai.dpuv1.rt.xdnn_util_tf import freeze_graph
    with tf.Graph().as_default() as graph:
        inputshape=[None, 1096, 1024, 3]
        x  = tf.placeholder(tf.float32, shape=inputshape, name="input")
        w  = tf.get_variable('weight', shape=[5, 5, 3, 96])
        y  = tf.nn.conv2d(x, filter=w, strides=[1,1,1,1], padding='SAME', name='conv_1')
        bn = tf.layers.batch_normalization(y, center=True, scale=True, epsilon=0.001,
                                           axis=3, training=False, name='bn-bn',
                                           renorm=True, renorm_momentum=0.99, fused=False)
        ret = tf.nn.relu(bn, name='relu')
    return freeze_graph(None, graph.as_graph_def())[0]


if __name__ == '__main__':
    from argparse import ArgumentParser

    models = \
    ['/proj/xsjhdstaff5/arminb/xilinx_internal_mlsuite/20181114/MLsuite/models/container/tensorflow/inception_v4.pb',
     '/proj/xsjhdstaff5/arminb/tensorflow/uber/image_cnn.v1/image_cnn.v1.1920x512.pb',
     '/proj/xsjhdstaff5/arminb/tensorflow/ssd/ssd_graph.pb',
     '/proj/xsjhdstaff5/arminb/tensorflow/ssd/resnet34_tf.22.1.nhwc.pb',
    ]

    argparser = ArgumentParser(description='Coloring test')
    argparser.add_argument('--networkfile', nargs='+', default=models, help='models path')
    argparser.add_argument('--hardware', type=str, default='xdnn', help='target hardware')
    argparser.add_argument('--loadmode', type=str, default='binary', help='model load mode')
    argparser.add_argument('--startnode', nargs='+', help='start nodes')
    argparser.add_argument('--finalnode', nargs='+', help='final nodes')
    args = argparser.parse_args()

    args.networkfile    = args.networkfile[0]
    args.debug_coloring = True

    cgraph = CGraph_tensorflow(args)
    cgraph.color_graph(cgraph.startnode, cgraph.finalnode)

    cgraph.create_pydot_graph(filename='debug_colored_graph.svg')
