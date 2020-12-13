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

import json
from os.path import isfile
from copy import deepcopy
from collections import defaultdict
from six import string_types as _string_types

import numpy as np

from vai.dpuv1.rt import xdnn_util
from vai.dpuv1.rt.coloring.graph_coloring_tensorflow import CGraph_tensorflow
from vai.dpuv1.tools.compile.bin.xfdnn_compiler_tensorflow import TFFrontend



def get_compiler(compilerFunc):
    if compilerFunc.lower() == 'speculative':
      return CGraph_tensorflow
    elif compilerFunc.lower() == 'definitive':
      return TFFrontend
    else:
      raise RuntimeError(f'unrecognized compilerFunc {compilerFunc}. Valid options: \"SPECULATIVE\", \"DEFINITIVE\"')


_g_partition_idx = 0

class LayerPartition(object):
  def __init__(self, index=0, supported=False, time_to_layer_list=None, layerparameter_dict={},
               layeroutput_dict={}, compilerJson_info={}, op_set={}, op_to_const={}):
    self.fValid    = True
    self.index     = index
    self.supported = supported
    if time_to_layer_list:
      z            = list(zip(*time_to_layer_list))
      self.schedule, \
      self.names   = list(z[0]), list(z[1])
    else:
      self.schedule, \
      self.names   = [], []

    self.op_set = {extra: layer for extra, layer in op_set.items() if layer in self.names}

    const_set = [const for op, const in op_to_const.items() if op in self.op_set]
    if const_set:
      const_set = [x for sublist in const_set for x in sublist]
    self.const_set = set(const_set)

    self.inputs,    \
    self.outputs,   \
    self.inputMap,  \
    self.outputMap, \
    self.input_cnt = self.partition_boundaries(layerparameter_dict, layeroutput_dict)

    self.layerparameter_dict = {layer: param for layer, param in layerparameter_dict.items() if
                                layer in self.names+self.inputs}
    self.layeroutput_dict = {layer: param for layer, param in layeroutput_dict.items() if layer in
                             self.names+self.inputs}
    self.compilerJson_info = compilerJson_info

  def size(self):
    return len(self.names)

  def partition_boundaries(self, layerparameter_dict, layeroutput_dict):
    '''
    returns sourcenodes, sinknodes.
    sourcenodes are exclusive, i.e., they do not belong to the layer, except when the sourcenode
    does not have an input itself.
    sinknodes are inclusive, i.e., they are part of the layer.
    sourcenode_map are the last node that was collapsed into the sourcenodes.
    sinknode_map are the last node that was collapsed into the sinknodes.
    '''
    partition_name_set = set(self.names)
    sourcenodes = []
    sinknodes   = []
    bottoms     = []
    for layer in self.names:
      inps = layerparameter_dict[layer].bottoms
      if inps is None or len(inps) == 0:
        sourcenodes += [layer]
      else:
        bottoms += inps

      outs = layeroutput_dict[layer].keys()
      ## NOTE: "not outs" might cause issues
      if not outs or any([output not in partition_name_set for output in outs]):
        sinknodes += [layer]

    sourcenodes += [bottom for bottom in bottoms if bottom not in partition_name_set and bottom not in self.const_set]
    # sinknodes    = [layer for layer in self.names if layer not in set(bottoms)]   ## deprecated

    sourcenodes_cnt = defaultdict(lambda: 0)
    for sourcenode in sourcenodes:
      sourcenodes_cnt[sourcenode] += 1

    sourcenodes = list(sourcenodes_cnt.keys())
    # sinknodes   = list(set(sinknodes))

    sourcenode_map = {}
    for sourcenode in sourcenodes:
      collapse_future = layerparameter_dict[sourcenode].collapse_future
      if not collapse_future or sourcenode in partition_name_set:
        sourcenode_map[sourcenode] = sourcenode
      else:
        #collapse_future = [extra if isinstance(extra, _string_types) else extra.name for extra in collapse_future]
        sourcenode_map[sourcenode] = collapse_future[-1]

    sinknode_map = {}
    for sinknode in sinknodes:
      collapse_future = layerparameter_dict[sinknode].collapse_future
      if not collapse_future:
        sinknode_map[sinknode] = sinknode
      else:
        #collapse_future = [extra if isinstance(extra, _string_types) else extra.name for extra in collapse_future]
        sinknode_map[sinknode] = collapse_future[-1]
    return sourcenodes, sinknodes, sourcenode_map, sinknode_map, sourcenodes_cnt

  def update(self, others):
    if not others:
      return
    layerparameter_dict = self.layerparameter_dict
    layeroutput_dict = self.layeroutput_dict
    if not isinstance(others, list):
      others = [others]
    for other in others:
      if self.compilerJson_info != other.compilerJson_info:
        raise RuntimeError(f'Something is wrong!! NOT supposed to merge partitions from distinct compiler sessions.')
      other.fValid   = False
      self.schedule += other.schedule
      self.names    += other.names
      self.op_set.update(other.op_set)
      self.const_set.update(other.const_set)
      layerparameter_dict.update(other.layerparameter_dict)
      layeroutput_dict.update(other.layeroutput_dict)
    self.inputs,    \
    self.outputs,   \
    self.inputMap,  \
    self.outputMap, \
    self.input_cnt = self.partition_boundaries(layerparameter_dict, layeroutput_dict)
    self.layerparameter_dict = {layer: param for layer, param in layerparameter_dict.items() if
                                layer in self.names+self.inputs}
    self.layeroutput_dict = {layer: param for layer, param in layeroutput_dict.items() if layer in
                             self.names+self.inputs}

  def forward_exec(self, inputs, outputs=None, preprocess=None, save=None):
    if not outputs:
      outputs = self.outputs
    else:
      for output in outputs:
        if isinstance(output, list):
          raise TypeError('outputs should be flattened list of name strings')

    if not isinstance(inputs, list):
      inputs = [inputs]

    # Add network input to variables list
    # self.variables[self.inputs[0]] = preprocess(inputs)
    for name, inp in zip(self.inputs, inputs):
      self.variables[name] = inp

    for layer in self.layers:
      #print('Forward Exec: {}'.format(layer.name))
      layer_inputs = [self.variables[inp] for inp in layer.inputs]
      if layer_inputs:
        layer_outputs = layer.forward_exec( layer_inputs )

        ## FIXME: HACK until all cpu layer outputs have been updated to dictionary
        if not isinstance(layer_outputs, dict):
          if isinstance(layer.output, list) ^ isinstance(layer_outputs, list):
            raise RuntimeError('Expected output of type {} received {}'.format(type(layer.output), type(layer_outputs)))
          elif isinstance(layer.output, list) and isinstance(layer_outputs, list):
            layer_outputs = {name: output for name, output in zip(layer.output, layer_outputs)}
          else:
            layer_outputs = {layer.output: layer_outputs}

        self.variables.update(layer_outputs)
        # for name in layer_outputs.keys():
        #   print('Forward Exec: {} Dimensions: {}'.format(name, layer_outputs[name].shape))
      # elif len(inputs) == 1:
      #   self.variables[layer.output] = inputs[0]
      else:
        raise RuntimeError('layer input mismatch')

    if save is not None:
      for var in self.variables.keys():
        #np.save(save+'/'+'_'.join(var.split('/'))+'.npy', self.variables[var])
        np.savetxt(save+'/'+'_'.join(var.split('/'))+'.txt', self.variables[var].flatten(),fmt="%0.6f")

    return [self.variables[name] for name in outputs]








class xdnnRT(object):
    def __init__(self, args, compilerFunc, **kwargs):
        args = xdnn_util.dict2attr(args)
        args.update(kwargs)

        self.networkfilename     = args.get('networkfilename', args.networkfile)
        self.data_format         = args.data_format
        self.channel_index       = 3 if args.data_format == 'NHWC' else 1
        self.save2modeldir       = args.save2modeldir   ## whether to save to model directory
        self.savedir             = args.savedir

        if args.get('loadPyfunc', False):
          modified_networkfile = self.file_path('.pb', name_postfix='-fpga')
          if isfile(modified_networkfile):
            args.networkfile = modified_networkfile
          else:
            ## turn off "loadPyfunc"
            args['loadPyfunc'] = False

        self._args = args

        self._graph, \
        self.inputs, \
        self.outputs, \
        self.fValidGraph \
            = self.load_graph(args,
                              startnode=None,
                              finalnode=None,
                              inclusive=True,
                              remove_training_nodes=True,
                              remove_redundant_nodes=True
                             )

        args.networkfile = self._graph   ## optimization: overload networkfile name with loaded graph_def for

        if args.get('loadPyfunc', False):
          return

        partitions = self.partition_graph(args, compilerFunc)

        # self.fpga_partition_cnt = 0
        # for partition in partitions:
        #   if partition.supported:
        #     self.fpga_partition_cnt += 1

        ## device transformation for supported layers
        self.device_transforms(args, partitions)

        self.rebuild_graph(partitions)


    def file_path(self, file_extension, name_prefix='', name_postfix=''):
        path_split = self.networkfilename.rsplit('/', 1)
        savepath = self.savedir if self.savedir else '.'
        savepath = path_split[0] if self.save2modeldir and len(path_split) > 1 else savepath
        networkname = path_split[-1].rsplit('.', 1)[0]
        return '/'.join([savepath, name_prefix + networkname + name_postfix + file_extension])

    def load_graph(self, args, **kwargs):
        raise NotImplementedError('load_graph method not implemented for xdnnRT')

    def list_inputs_of_graph(self, pydotGraph):
        raise NotImplementedError('list_inputs_of_graph method not implemented for xdnnRT')

    def list_outputs_of_graph(self, pydotGraph):
        if hasattr(self, 'outputs') and self.outputs is not None:
          return self.outputs

        sinknodes = []
        for node in pydotGraph.get_nodes():
            param = node.get('LayerParameter')
            if param.tops is None :
                sinknodes += param.bottoms

        return sinknodes

    def partition_graph(self, args, compilerFunc, **kwargs):
        def partition_graph_iter(args, compilerFunc, **kwargs):
            args = xdnn_util.dict2attr(args)
            args.update(kwargs)

            _partitions = []
            startnode = xdnn_util.make_list(args.startnode)
            finalnode = xdnn_util.make_list(args.finalnode)
            startnode_depth = xdnn_util.list_depth(startnode)
            finalnode_depth = xdnn_util.list_depth(finalnode)

            if startnode_depth != finalnode_depth:
              raise ValueError(f'startnode indicate {startnode_depth} partitions whereas finalnode indicate {finalnode_depth} partitions')

            for idx, (startnode, finalnode) in enumerate(zip(xdnn_util.nested_list_iter(startnode),
                                                             xdnn_util.nested_list_iter(finalnode))):
              # save pydotGraph and compilerJson
              name_postfix = args.get('name_postfix', f'_orgPartition_{idx}')
              filename = self.file_path('.pb', name_postfix=name_postfix)
              self.compiler_picklefile = self.file_path('.pickle', name_postfix=name_postfix)

              ## run compiler
              compiler_args = xdnn_util.dict2attr(args)
              compiler_args.update({'startnode': startnode,
                                    'finalnode': finalnode,
                                    'weights': filename,
                                    'savepickle': self.compiler_picklefile if args.savepickle else None,
                                    'loadpickle': self.compiler_picklefile if args.loadpickle else None,
                                   })
              compiler = get_compiler(compilerFunc)(None, **compiler_args)

              __, \
              __, \
              __, \
              pydotGraph, \
              compilerSchedule, \
              __, \
              compilerJson \
                  = compiler.compile()

              if compilerJson is None:
                raise RuntimeError('Compiler failed to produce valid schedule')

              ## restore compilerJson['arguments']['networkfile'] in case modified
              if 'arguments' in compilerJson and \
                 'networkfile' in compilerJson['arguments'] and \
                 not isinstance(compilerJson['arguments']['networkfile'], _string_types):
                compilerJson['arguments']['networkfile'] = self.file_path('')

              ## store compiler json
              # with open(self.file_path('.json', name_postfix=name_postfix+'-xdlfCompiler'), "w") as f:
              #   json.dump(compilerJson, f, sort_keys=True, indent=4, separators=(',',': '))

              # if not args.get('fPartition', True):
              #   return [(None, compilerJson)]

              compilerJson_info  = xdnn_util.dict2attr({'inputs': {'input_name': [name for itm in compilerJson['inputs'] for name in xdnn_util.make_list(itm['input_name'])]},
                                                        'outputs': {'previous_layers': [name for itm in compilerJson['outputs'] for name in xdnn_util.make_list(itm['previous_layers'])]},
                                                        'unsupported_list': list(compilerJson['unsupported']['list'].keys()),
                                                        'fake_nodes': [name for itm in compilerJson['inputs'] for name in xdnn_util.make_list(itm['input_name'])] + [name for itm in compilerJson['outputs'] for name in xdnn_util.make_list(itm['output_name'])],
                                                        'filename': filename, })

              time_to_layer_list = []
              for time, layers in compilerSchedule.time_to_layer.items():
                for layer in layers:
                  time_to_layer_list.append((time, layer))

              spt_set, \
              unspt_set, \
              layerparameter_dict, \
              layeroutput_dict, \
              layer_spt_dict = self.parse_compiler_output(pydotGraph.get_nodes(),
                                                          compilerJson_info['unsupported_list'] +
                                                          compilerJson_info['fake_nodes'],
                                                          time_to_layer_list)

              ## partitioning the graph
              ## Discover partitions based on compiler schedule
              op_to_const = compilerSchedule.op_to_const if hasattr(compilerSchedule, 'op_to_const') else xdnn_util.dict2attr({'supported': {}, 'unsupported': {}})

              _partitions += self.partitions_from_compiler_schedule(spt_set,
                                                                    unspt_set,
                                                                    layer_spt_dict,
                                                                    layerparameter_dict,
                                                                    layeroutput_dict,
                                                                    compilerJson_info,
                                                                    op_to_const)

            return _partitions

        partitions = partition_graph_iter(args, compilerFunc, **kwargs)
        ## optimize partitions
        partitions = self.refine_graph_partitions(partitions)


        added_partitions = []
        for index, partition in enumerate(partitions):
          ## extract supported subgraph
          if partition.supported:
            print('Re-compile partition_index \"{}\"'.format(partition.index))

            partitions.pop(index)
            name_postfix = '_partition_{:02d}'.format(partition.index)
            filename = self.file_path('.pb', name_postfix=name_postfix)

            partitioner_args = xdnn_util.dict2attr(args)
            partitioner_args.update({
                                     'fPartition':        False,
                                     'startnode':         list(partition.inputMap.values()),
                                     'finalnode':         list(partition.outputMap.values()),
                                     'outputz':           list(partition.outputMap.values()),
                                     'inclusive':         False,
                                     'networkfilename':   self.networkfilename,
                                     'generatefile':      filename,
                                     # 'graph_savepath':  filename,
                                     'name_postfix':      name_postfix,
                                     'cpulayermustgo':    True,
                                    })
            ## NOTE: hack to enable mp_classify style batching
            if args.get('mpClassifyStyle_batching', False):
              partitioner_args.networkfile = self.networkfilename
              partitioner_args.batch_sz = 1

            added_partitions += partition_graph_iter(partitioner_args, 'DEFINITIVE')
          else:
            partition.layers    = list(partition.names)
            partition.variables = {}
        ## optimize partitions
        partitions += added_partitions
        partitions = self.refine_graph_partitions(partitions)
        return partitions

    def parse_compiler_output(self, pydotGraph_nodes, unsupported_list, time_to_layer_list):
        spt_set              = {}
        unspt_set            = {layer: layer for layer in unsupported_list}
        layerparameter_dict  = {}
        layeroutput_dict     = defaultdict(lambda: {})
        layer_spt_dict       = defaultdict(lambda: [0, []])    ## format: {grp_idx: (spt_flag, t2l_list)}

        for node in pydotGraph_nodes:
          LayerParameter = node.get('LayerParameter')
          layer_name = LayerParameter.name
          layerparameter_dict[layer_name] = LayerParameter
          layeroutput_dict[layer_name]
          if ('blob' not in layer_name and    ## FIXME: hack to remove output blobs from output list
              LayerParameter.bottoms):
            for input_index, input_name in enumerate(LayerParameter.bottoms):
              layeroutput_dict[input_name][layer_name] = input_index

        ## partition consecutive supported layers in zip(*list(partition.next()[1]))[1]
        print('.... is_supported, layer_index, layer_name')
        partition_idx   = -1
        prev_spt        = True if time_to_layer_list[0][1] in unspt_set else False
        for time, layer in time_to_layer_list:
          spt = False if layer in unspt_set else True
          if spt ^ prev_spt:
            partition_idx += 1
            layer_spt_dict[partition_idx][0] = spt
          layer_spt_dict[partition_idx][1] += [(time, layer)]
          prev_spt = spt

          ## build supported layer set
          if spt:
            spt_set[layer] = layer
            extras_and_future = layerparameter_dict[layer].extras_and_future
            if extras_and_future is not None:
              extras_names = [extra if isinstance(extra, _string_types) else extra.name for extra in extras_and_future]
              spt_set.update({name: layer for name in extras_names if name not in unspt_set})

          print('.... {0:5s}, {1:3d}, {2:s}'.format(str(spt), time, layer))

        return spt_set, unspt_set, layerparameter_dict, layeroutput_dict, layer_spt_dict

    def partitions_from_compiler_schedule(self, spt_set, unspt_set, layer_spt_dict,
                                          layerparameter_dict, layeroutput_dict, compilerJson_info,
                                          op_to_const={}):
        global _g_partition_idx

        print('\nPartition FPGA (un)supported layers from compiler schedule ....')

        partitions = [LayerPartition(grp_idx+_g_partition_idx,
                                     spt,
                                     t2l_list,
                                     layerparameter_dict,
                                     layeroutput_dict,
                                     compilerJson_info,
                                     spt_set if spt else unspt_set,
                                     op_to_const.supported if spt else op_to_const.unsupported)
                      for grp_idx, (spt, t2l_list) in layer_spt_dict.items()]

        _g_partition_idx += len(layer_spt_dict)
        print('Partition FPGA (un)supported layers from compiler schedule [DONE]')
        return partitions


    def refine_graph_partitions(self, partitions):
        def _print_connectivity(partitions, connectivity, forward=True):
            for partition in partitions:
              print(f'''.... partition ({partition.index:3d}, {str(partition.supported):5s}) {"-->"
                    if forward else "<--"} {[(partitions[index].index, partitions[index].supported)
                    for index_list in connectivity[partition.index].values() for index in
                    index_list]}''')

        def partitions_connectivity(partitions, debugprint=False):
            # print('partition connectivity:')
            connectivity     = defaultdict(lambda: defaultdict(list))
            connectivity_rev = defaultdict(lambda: defaultdict(list))
            for index, partition in enumerate(partitions):
              connectivity[partition.index]
              connectivity_rev[partition.index]
              other_partitions = list(partitions)
              other_partitions.pop(index)
              for other in other_partitions:
                for name in set(other.inputs).intersection(set(partition.op_set)):
                  connectivity[partition.index][name] += [other.index for i in
                                                           range(other.input_cnt[name])]

                for name in set(partition.inputs).intersection(set(other.op_set)):
                  connectivity_rev[partition.index][name] += [other.index for i in
                                                                range(partition.input_cnt[name])]

            if debugprint:
              _print_connectivity(partitions, connectivity, forward=True)
              print('....')
              _print_connectivity(partitions, connectivity_rev, forward=False)
              print('....')
            return connectivity, connectivity_rev

        def merge_partitions(partitions, matchings):
            for src_idx, dst_idxs in matchings:
              src_partition = partitions[src_idx]
              dst_partitions = [partitions[dst_idx] for dst_idx in dst_idxs]
              src_partition.update(dst_partitions)

            ## filter invalid partitions
            partitions = [partition for partition in partitions if partition.fValid]

            ## fix the indices
            for idx, partition in enumerate(partitions):
              partition.index = idx

            return partitions

        def acyclic_matching(partitions):
            def is_contraction_valid(connectivity, connectivity_rev, ordering, src, dst):
                def affected_set(ordering, src, dst):
                    end_idx   = ordering.index(src)
                    start_idx = ordering.index(dst)
                    return set(ordering[start_idx:end_idx+1])

                af_set = affected_set(ordering, src, dst)

                af_forward = af_set.copy(); af_forward.remove(dst)
                fDFS = xdnn_util.topological_ordering(connectivity, subset_view=af_forward, seed=[src])

                af_backward = af_set.copy(); af_backward.remove(src)
                bDFS = xdnn_util.topological_ordering(connectivity_rev, subset_view=af_backward, seed=[dst])

                if set(fDFS).intersection(set(bDFS)):
                  return False
                print('contract src {} <-- dst {}'.format(src, dst))
                return True

            def summarize_matchings(matching):
                def helper(src, dst_list):
                    for dst in dst_list:
                      if dst in matching:
                        helper(dst, matching[dst])
                        matching[src] += matching[dst]
                        matching.pop(dst)

                for src in sorted(matching.keys()):
                  helper(src, matching[src])

            #############################
            ## acyclic_matching body
            #############################
            connectivity, \
            connectivity_rev = partitions_connectivity(partitions, debugprint=False)

            topOrder = xdnn_util.topological_ordering(connectivity)

            matching = xdnn_util.UnionFind(len(topOrder))
            for src_v in topOrder[::-1]:
              src_support = partitions[src_v].supported
              unzipped = list(zip(*connectivity[src_v].items()))
              v_list = unzipped[1] if len(unzipped) > 1 else []
              v_list = set([item for sublist in v_list for item in sublist])
              for dst_v in v_list:
                dst_support = partitions[dst_v].supported
                if ((src_support == dst_support) and
                    is_contraction_valid(connectivity, connectivity_rev, topOrder, src_v, dst_v)):
                  matching.union(src_v, dst_v)
            merge_matches = matching.components()

            partitions = merge_partitions(partitions, list(merge_matches.items()))
            return partitions


        #############################
        ## refine_graph_partitions body
        #############################
        print('\nRefine Graph Partitions ....')

        _partitions = list(partitions)
        idx, partitions = 0, []
        for partition in _partitions:
          ## filter fake partitions
          is_fake = [True for name in partition.names if name in partition.compilerJson_info.fake_nodes]
          if is_fake and all(is_fake):
            partition.fValid = False
          ## filter invalid partitions
          if partition.fValid:
            partition.index = idx
            partitions.append(partition)
            idx += 1

        partitions = acyclic_matching(partitions)

        self.connectivity, \
        self.connectivity_rev = partitions_connectivity(partitions, debugprint=True)

        #for partition in partitions:
          ## NOTE: Heuristic. Don't use fpga for small partitions
          #if partition.size() < 4:
          #  partition.supported = False

        print('\nSUMMARY:')
        for partition in partitions:
          print(f'.... partition_index \"{partition.index}\" - SUPPORTED: {partition.supported}')
          print(f'........ inputs:          {list(partition.inputMap.keys())}')
          print(f'........ inputs actual:   {list(partition.inputMap.values())}')
          print(f'........ outputs:         {list(partition.outputMap.keys())}')
          print(f'........ outputs actual:  {list(partition.outputMap.values())}')
        print('Refine Graph Partitions [DONE]')

        return partitions

    def device_transforms(self, args):
        raise NotImplementedError('device_transforms method not implemented for xdnnRT')

    def rebuild_graph(self):
        raise NotImplementedError('rebuild_graph method not implemented for xdnnRT')

    def preprocess(self,  inputs):
        raise NotImplementedError('preprocess method not implemented for xdnnRT')

    def batch_classify(self, img_list, batch, preprocess):
        bctr = 0
        ictr = 0
        pred = None
        prepdata = {}
        prep = self.inputs[0]
        ctr = 0
        pred = {}
        while ctr < len(img_list):
            ctrmax = min(ctr+batch, len(img_list))
            print("processing", img_list[ctr:ctrmax])
            res = self.forward_exec(img_list[ctr:ctrmax], preprocess=preprocess)
            for k, v in res.items() :
                pred.setdefault(k, []).append(v)
            ctr = ctrmax
        if len(pred) == 0: return {}
        for k, v in pred.items() :
            if len(pred[k]) == 1 :
                pred[k] = v[0]
            else :
                pred[k] = np.concatenate(v)
        return pred
