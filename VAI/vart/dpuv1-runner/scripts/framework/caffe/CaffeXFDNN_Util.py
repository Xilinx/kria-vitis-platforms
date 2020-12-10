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
import google.protobuf.text_format as pbtf
import caffe.proto.caffe_pb2 as caffe_pb2
import json

from vai.dpuv1.rt.scripts.framework.base.quantize_controls import quantize_controls
#from vai.dpuv1.rt.scripts.framework.caffe.CaffeXFDNN_Quantize import CaffeXFDNN_Quantize

def get_subgraph_nodes(proto) :
    quantized_inputs = set()
    res = []
    for layer in proto.layer:
        if layer.type == "Python" and layer.python_param.module == "vai.dpuv1.rt.scripts.framework.caffe.CaffeXFDNN_UnQuantize":
            continue
        if layer.type == 'Python' and layer.python_param.module == "vai.dpuv1.rt.scripts.framework.caffe.CaffeXFDNN_Quantize":
            for t_name in layer.top :
                quantized_inputs.add(t_name)
        all_inps_quantized = True
        if len(layer.bottom) == 0 :
            all_inps_quantized = False
        for inp in layer.bottom :
            if inp not in quantized_inputs :
                all_inps_quantized = False
                break
        if all_inps_quantized :
            if layer.type in ["Convolution", "Scale"]:
                res.append(layer.name)
            for t_name in layer.top :
                quantized_inputs.add(t_name)
    return res


def quantize_weights(net, model_def, q_cfg):
    quantize_cfg = None
    with open(q_cfg) as qfile :
        quantize_cfg = json.load(qfile)
    proto = caffe_pb2.NetParameter()
    with open(model_def) as pfile :
        pbtf.Parse(pfile.read(), proto)
    node_names = get_subgraph_nodes(proto)
    env = quantize_controls(q_cfg)
    for name in node_names :
        env.quantize_wts(net.params[name][0].data, name)
        env.quantize_bias(net.params[name][1].data, name)
