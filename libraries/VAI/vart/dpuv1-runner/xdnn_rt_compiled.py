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

import json
from os import mkdir as _mkdir
from os.path import exists as _exists
from collections import defaultdict, OrderedDict
from copy import deepcopy

import tensorflow as tf
import caffe
import numpy as np
from tensorflow.python.ops import script_ops as _script_ops


from xdnn_rt import xdnnRT
import xdnn_util
from xdnn_opt import CPUTransform, HWEmuTransform, FPGATransform


class compilerxdnnRT(xdnnRT):
    def __init__ (self, compilerjson, weightdir, device, inps = None, outs = None, isV3 = True, save = None):
        self.inputs = inps
        self.outputs = outs
        self.device_transforms(compilerjson, weightdir, device, isV3)
        self.save = save

    def list_inputs_of_graph(self, networkjson):
        inps = []
        for layer in networkjson['network'] :
            if 'bottoms' in layer and len(layer['bottoms']) == 0 :
                inps.append(layer['name'])
        self.inputs = inps

    #def list_outputs_of_graph(self, graph):
    #    return ['prob']
    #    raise notimplementederror('''list_outputs_of_graph method not implemented for caffexdnnrt.
    #                              (remove *.pickle to fix this issue for now!!!!)''')

    def list_outputs_of_graph(self, networkjson):
        outsdic = set()
        for layer in networkjson['network'] :
            if 'bottoms' in layer :
                outsdic.add(layer['name'])
                for inp_name in layer['bottoms'] :
                    if inp_name in outsdic :
                        outsdic.remove(inp_name)
        self.outputs = list(outsdic)

    def device_transforms(self, compilerjson, weightdir, device, isV3):
        print(device)
        jobj = None

        with open(compilerjson, 'r') as jsr :
            jobj = json.load(jsr)
        if not self.outputs :
          self.list_outputs_of_graph(jobj)
        if not self.inputs :
          self.list_inputs_of_graph(jobj)
        print("Inputs  : ", self.inputs)
        print("Outputs : ", self.outputs)
        if device=="CPU":
            opt = CPUTransform(networkjson = jobj, weightdir = weightdir, inps = self.inputs, outs = self.outputs)
        elif device == "HWEmu":
            opt = HWEmuTransform(networkjson = jobj, weightdir = weightdir, inps = self.inputs, outs=self.outputs, isV3=isV3)
        self.layers = opt.getLayers()
        self.variables = opt.variables
#        for partition in self.graph_partitions:
#          time_to_layer_list = zip(partition.schedule, partition.names)
#          print partition.supported
#          if partition.supported:
#            if args.device == "CPU":
#              opt = CPUTransform(networkjson=args.)
#            elif args.device == "HWEmu":
#              opt = HWEmuTransform(time_to_layer_list, self.layerparameter_dict, args, self._graph)
#              #raise RuntimeError('not implemented yet')
#              #opt = HWEmuTransform(partition.inputs, pydotGraph, compilerSchedule, args)
#            elif args.device == "FPGA":
#                if not args.fpga_recipe:
#                  args.fpga_recipe = {'start': [time_to_layer_list[0][1]], 'end': partition.outputs}
#                if args.xclbin:
#                  opt = FPGATransform(time_to_layer_list, self.layerparameter_dict, self.compilerJson, args, self._graph)
#                else:
#                  raise AttributeError("Must specify path to xclbin when device = FPGA")
#            else:
#              raise AttributeError("Unsupported device type", args.device)
#          else:
#            ## default back to CPU implementation
#            opt = CPUTransform(time_to_layer_list, self.layerparameter_dict, args, self._graph)
#
#          #variables hold the inputs/consts of graph
#          partition.layers    = opt.getLayers()
#          partition.variables = opt.variables
#          for l in partition.layers:
#            l.setup()

    def rebuild_graph(self):
        pass

    def forward_exec(self, inputs, outputs=None, preprocess=None, **kwargs):
        if not outputs:
          outputs = self.outputs
        else:
          for output in outputs:
            if isinstance(output, list):
              raise TypeError('outputs should be flattened list of name strings')
        print(outputs)
        #self.save = None if 'save' not in kwargs else kwargs['save']
        if not preprocess:
          data = inputs
        else :
          data = preprocess(inputs, **kwargs)
        res = {}
        print(self.save)
        for i in range(len(inputs)) :
          self.variables[self.inputs[i]] = data[i]
        for layer in self.layers :
          #import pdb; pdb.set_trace()
          inps = [self.variables[inp] for inp in layer.inputs]
          self.variables[layer.name] = layer.forward_exec(inps)
          print("Running : ",layer.name, self.variables[layer.name].shape)
          if self.save:
            np.save(self.save+'/'+'_'.join(layer.name.split('/'))+'.npy', self.variables[layer.name])

        if self.save:
          with open(self.save+'/layers.txt', 'w') as f:
            for layer in self.layers:
                f.write('_'.join(layer.name.split('/'))+'\n')
          self.save = None
        return res
