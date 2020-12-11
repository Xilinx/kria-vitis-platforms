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
from os.path import isfile
from collections import OrderedDict
from copy import deepcopy
from six import string_types as _string_types
import dill as pickle

import tensorflow as tf
import numpy as np
from tensorflow.python.ops import script_ops as _script_ops

from vai.dpuv1.rt import xdnn_util
from vai.dpuv1.rt import xdnn_util_tf
from vai.dpuv1.rt.xdnn_rt_base import xdnnRT as _xdnnRT
from vai.dpuv1.rt.device.xdnn_opt import FPGATransform
from cv2 import imread as _imread



## global variables
global_fpga_device    = 'cpu:0'   ## TODO: replace with FPGA:0
global_pyfunc_counter = 0

######################################################
## tensorflow specific utility functions
######################################################
## expanding tf.NodeDef methods
NodeDef = tf.NodeDef        ## TODO: for tf_version > 1.14 use tf.compat.v1.NodeDef
NodeDef.set_name  = xdnn_util_tf.set_name
NodeDef.set_shape = xdnn_util_tf.set_shape
NodeDef.get_shape = xdnn_util_tf.get_shape
NodeDef.get_dtype = xdnn_util_tf.get_dtype

## expanding tf.GraphDef methods
GraphDef = tf.GraphDef        ## TODO: for tf_version > 1.14 use tf.compat.v1.GraphDef
GraphDef.get_node_dict   = xdnn_util_tf.get_node_dict
GraphDef.get_output_dict = xdnn_util_tf.get_output_dict
GraphDef.get_node_index  = xdnn_util_tf.get_node_index
GraphDef.remove_nodes    = xdnn_util_tf.remove_nodes
GraphDef.is_cyclic       = xdnn_util_tf.is_cyclic
GraphDef.all_cycles      = xdnn_util_tf.all_cycles




class TFxdnnRT(_xdnnRT):
    def __init__ (self, args, compilerFunc='SPECULATIVE', **kwargs):
        super(TFxdnnRT, self).__init__(args, compilerFunc, **kwargs)

        if not hasattr(self, 'graph_def'):
          self.graph_def = deepcopy(self._graph)

        if not hasattr(self, 'fpga_pynode_dict'):
          self.fpga_pynode_dict = OrderedDict()

    def __del__(self):
        if hasattr(self, 'fpga_pynode_dict'):
            for pyfunc_name, (partition, input_names, output_dtypes) in self.fpga_pynode_dict.items():
                del partition.layers

    def load_graph(self, args, **kwargs):
        return xdnn_util_tf.load_graph(args, **kwargs)

    def list_inputs_of_graph(self, graph_def):
        return xdnn_util_tf.discover_sourcenodes(graph_def)

    def list_outputs_of_graph(self, graph_def):
        if hasattr(self, 'outputs') and self.outputs is not None:
          return self.outputs
        return xdnn_util_tf.discover_sinknodes(graph_def)

    def extract_subgraph(self, outputs, inputs, inclusive=False, filename=None, session=None):
        destgraph_def = xdnn_util_tf.extract_subgraph(self._graph, outputs, inputs, inclusive)
        destgraph_def, fValidGraph = xdnn_util_tf.freeze_graph(session,
                                                               destgraph_def,
                                                               sinknodes_list=outputs,
                                                               filename=filename,
                                                               freeze_blacklist=[],
                                                               freeze_whitelist=[])
        return destgraph_def

    def device_transforms(self, args, partitions):
        for partition in partitions:
          if not partition.supported:
            continue

          print('Transorm partition index \"{}\"'.format(partition.index))

          device = args.get('device', 'FPGA')
          if device.lower() == "cpu":
            # opt = CPUTransform(TF.time_to_layer_list, TF.layerparameter_dict, args, TF._graph)
            raise RuntimeError('cpu device mode is deprecated. Available decives: \"FPGA\"')
          elif device.lower() == "hwemu":
            # opt = HWEmuTransform(partition.inputs, TF.pydotGraph, TF.compilerSchedule, args)
            raise RuntimeError('hwemu device mode is deprecated. Available decives: \"FPGA\"')
          elif device.lower() == "fpga":
            xclbin = args.get('xclbin', None)
            if not args.xclbin:
              if 'VAI_ALVEO_ROOT' in os.environ and os.path.isdir(os.path.join(os.environ['VAI_ALVEO_ROOT'], 'overlaybins/xdnnv3')):
                xclbin = os.path.join(os.environ['VAI_ALVEO_ROOT'], 'overlaybins/xdnnv3')
              elif 'VAI_ROOT' in os.environ and os.path.isdir(os.path.join(os.environ['VAI_ROOT'], 'overlaybins/xdnnv3')):
                xclbin = os.path.join(os.environ['VAI_ROOT'], 'overlaybins/xdnnv3')
              else:
                raise AttributeError("Must specify path to xclbin when device is FPGA")

            node_dict = self._graph.get_node_dict()
            boundryMap    = {'inputs':  [(name,
                                          xdnn_util_tf.channel_swap(node_dict[name].get_shape()[0],
                                                                    self.channel_index, 1)) for name
                                         in partition.compilerJson_info['inputs']['input_name']
                                        ],
                             'outputs': [(name,
                                          xdnn_util_tf.channel_swap(node_dict[name].get_shape()[0],
                                                                    self.channel_index, 1)) for name
                                         in partition.compilerJson_info['outputs']['previous_layers']
                                        ],
                            }
            # boundryMap    = {'inputs':  [(name, node_dict[name].get_shape()[0]) for name in
            #                              partition.inputs],
            #                  'outputs': [(name, node_dict[name].get_shape()[0]) for name in
            #                              partition.outputs],
            #                 }
            # boundryMap    = {'inputs':  compilerJson.get('inputs', []),
            #                  'outputs': compilerJson.get('outputs', [])}
            opt = FPGATransform(xclbin, args.quant_cfgfile, args.base, boundryMap, partition.compilerJson_info.filename)
          else:
            raise AttributeError("Unsupported device type", args.device)

          #variables hold the inputs/consts of graph
          partition.layers    = opt.getLayers()
          partition.variables = opt.variables

          ## checking the validity of compilation
          if len(partition.outputs) != len(partition.layers[0].output):
            raise RuntimeError('''Compiler output count {} and partitioner output count {}
                               mismatch!'''.format(len(partition.layers[-1].output),
                               len(partition.outputs)))

          ## update partition output and outputMap based on compiler compilation
          partition.outputs = xdnn_util.make_list(partition.layers[-1].output)
          partition.outputMap = {}
          for output in partition.outputs:
            collapse_future = partition.layerparameter_dict[output].collapse_future
            partition.outputMap[output] = collapse_future[-1] if collapse_future else output

          for l in partition.layers:
            l.setup()

    def rebuild_graph(self, partitions):
        py_func_tokens = list(_script_ops._py_funcs._funcs.keys())
        for token in py_func_tokens:
          _script_ops._py_funcs.remove(token)
        _script_ops._py_funcs._unique_id = 0

        self.graph_def = self.rebuild_graph_def(partitions)

    def rebuild_graph_def(self, partitions):
        #############################
        ## helper function
        #############################
        def discover_consumer_nodes(root_name, partition_node_set, graph_output_dict):
            dst_dict = {}
            consumer_nodes = graph_output_dict[root_name]
            for consumer_name, index in consumer_nodes.items():
              if consumer_name not in partition_node_set:
                dst_dict[consumer_name] = index
            return dst_dict

        #def discover_consumer_nodes((root_name, index), partition_outputs, dst_dict):
        #    ## NOTE: This doesn't work if the supported partition is not maximal
        #    if root_name in self.spt_set:
        #      if index != -1 and root_name in partition_outputs:
        #        return
        #      consumer_nodes = graph_output_dict[root_name]
        #      for consumer_item in consumer_nodes.items():
        #        discover_consumer_nodes(consumer_item, partition_outputs, dst_dict)
        #    else:
        #      dst_dict[root_name] = index

        #############################
        ## helper function
        #############################
        def insert_fpga_pynode(graph_def, partition):
            global global_pyfunc_counter

            # def update_partition_boundaries(partition, graph_output_dict):
            #     for extra, layer in partition.spt_set.items():
            #       if layer not in partition.names:
            #         raise RuntimeError('supported layer {} not in partition {}-{}'.format(layer,
            #                                                                               partition.index,
            #                                                                               partition.names))
            #       for output in graph_output_dict[extra].keys():
            #         if output not in partition.spt_set and layer not in partition.outputs:
            #           partition.outputs += [layer]
            #           partition.outputMap.update({layer: extra})


            graph_node_dict, \
            graph_output_dict   = graph_def.get_node_dict(outmap=True)

            input_partitions    = self.connectivity_rev[partition.index]
            # consumer_partitions = self.connectivity[partition.index]

            input_tensors           = []
            placeholder_replace_map = {}

            # update_partition_boundaries(partition, graph_output_dict)

            with tf.Graph().as_default() as fpga_subgraph:
              for idx, inp in enumerate(partition.inputs):
                ## NOTE: index of partition in self.graph_partitions must match partition.index
                input_partition = input_partitions[inp]
                if len(set(input_partition)) > 1:
                  raise RuntimeError('input \"{}\" cannot belong to multiple partitions! {}'.format(inp, set(input_partition)))
                if not partitions[input_partition[0]].supported:
                  inp = partition.inputs[idx] = partition.inputMap[inp]

                ## NOTE: inp shouldn't have ":" except for multiple outputs of py_func
                inp = xdnn_util_tf.strip_node_name(inp)

                ## assuming all py_func inputs are active tensors and FPGA IP data_format is 'NCHW'
                input_nodedef = graph_node_dict[inp]
                input_tensor  = tf.placeholder(input_nodedef.get_dtype(),
                                               shape=input_nodedef.get_shape()[0], name=inp)
                input_tensors += [input_tensor]
                placeholder_replace_map[input_tensor.op.node_def.name] = inp

              with fpga_subgraph.name_scope(xdnn_util.Trie(partition.names).lcs()):
                with fpga_subgraph.name_scope('fpga_func_{}'.format(global_pyfunc_counter)):

                  # print('_names_in_use: ', fpga_subgraph._names_in_use)

                  output_shapes = [graph_node_dict[output].get_shape()[0] for output in partition.outputs]
                  output_dtypes = [graph_node_dict[output].get_dtype() for output in partition.outputs]

                  if self.data_format != 'NCHW':
                    with fpga_subgraph.name_scope('fpga_preproc'):
                      fpga_input_tensors = [tf.transpose(tensor, [0, 3, 1, 2]) if
                                            len(tensor.get_shape()) == 4 else tensor for
                                            tensor in input_tensors]
                  else:
                    fpga_input_tensors = input_tensors

                  # with tf.device(global_fpga_device):
                  fpga_output_tensors = tf.py_func(partition.forward_exec,
                                                   fpga_input_tensors,
                                                   output_dtypes,
                                                   stateful=False)

                  for fpga_output_tensor, output_shape in zip(fpga_output_tensors, output_shapes):
                    if len(output_shape) == 4:
                      fpga_output_tensor.set_shape([output_shape[i] for i in (0, 3, 1, 2)])
                    else:
                      fpga_output_tensor.set_shape(output_shape)

                  # print('graph py_func tokens: {}'.format(_script_ops._py_funcs._funcs.keys()))

                  if self.data_format != 'NCHW':
                    with fpga_subgraph.name_scope('fpga_postproc'):
                      output_tensors = [tf.transpose(tensor, [0, 2, 3, 1]) if
                                        len(tensor.get_shape()) == 4 else tensor for tensor in
                                        fpga_output_tensors]
                  else:
                    output_tensors = fpga_output_tensors

                  # print('graph py_func tokens: {}'.format(_script_ops._py_funcs._funcs.keys()))

                  global_pyfunc_counter += 1

              ## create identity nodes matching the correct output names
              # /wrk/hdstaff/arminb/Anaconda/envs/deephi_tf/lib/python2.7/site-packages/tensorflow/python/framework/op_def_library.py:394
              # /wrk/hdstaff/arminb/Anaconda/envs/deephi_tf/lib/python2.7/site-packages/tensorflow/python/framework/ops.py:6007
              # /wrk/hdstaff/arminb/Anaconda/envs/deephi_tf/lib/python2.7/site-packages/tensorflow/python/framework/ops.py:4117
              output_tensors = [tf.identity(tensor, name=partition.outputMap[name]) for tensor, name
                                in zip(output_tensors, partition.outputs)]

            ## maintain an ordered dictionary of pynodes for loading the graph
            # self.fpga_pynode_dict[fpga_output_tensors[0].op.node_def.attr['token'].s] = partition
            # pickle.detect.trace(True)
            ## remove unpickleable attributes
            del partition.layerparameter_dict, partition.layeroutput_dict, partition.compilerJson_info
            self.fpga_pynode_dict[fpga_output_tensors[0].op.name] = (partition, [inp.name for inp in
                                                                                 fpga_input_tensors],
                                                                     output_dtypes)

            fpga_subgraph_def    = fpga_subgraph.as_graph_def(add_shapes=True)
            # fpga_node_map, \
            # fpga_output_node_map = fpga_subgraph_def.get_node_dict(outmap=True)

            for key, value in placeholder_replace_map.items():
              if key != value:
                raise RuntimeError('real and dummy placeholder names mismatched! {}'.format(placeholder_replace_map))

            ##########################################
            ## NOTE: NOT needed anymore (identity node fixes name mismatches)
            ##########################################
            # ## remove dummy placehoders and connect their consumers to the original inputs
            # fpga_nodes = []
            # for fpga_node_name, fpga_node in fpga_node_map.items():
            #   if fpga_node.op == 'Placeholder':
            #     # replace palceholder with original inputs
            #     placeholder_consumers = fpga_output_node_map[fpga_node_name]
            #     for placeholder_consumer_name, input_index in placeholder_consumers.items():
            #       placeholder_consumer_node = fpga_node_map[placeholder_consumer_name]
            #       del placeholder_consumer_node.input[input_index]
            #       placeholder_consumer_node.input.insert(input_index, placeholder_replace_map[fpga_node_name])
            #   else:
            #     # All nodes except dummy placeholders are to be copied to the main graph
            #     fpga_nodes += [fpga_node]

            # ## connect input of fpga_consumer_nodes to output from py_function
            # for i, outp in enumerate(partition.outputs):
            #   #output_consumer_nodes = {}
            #   #discover_consumer_nodes((outp, -1), set(partition.outputs), output_consumer_nodes)
            #   output_consumer_nodes = discover_consumer_nodes(partition.outputMap[outp],
            #                                                   set(partition.names), graph_output_dict)

            #   ## update the input list for the consumers partitions of outp
            #   for consumerPart in consumer_partitions[outp]:
            #     consumerPart = self.graph_partitions[consumerPart]
            #     # print(outp, output_consumer_nodes, consumerPart.index, consumerPart.supported)
            #     if consumerPart.supported:
            #       temp = self.connectivity_rev[consumerPart.index]
            #       temp[output_tensors[i].name] = temp[outp]
            #       temp.pop(outp)
            #       consumerPart.inputs = [name if name != outp else output_tensors[i].name for name in consumerPart.inputs]
            #       consumerPart.inputMap[output_tensors[i].name] = consumerPart.inputMap[outp]

            #   ## update the input list for the consumer nodes of outp
            #   for output_consumer_node_name, input_index in output_consumer_nodes.items():
            #     output_consumer_node = graph_node_dict[output_consumer_node_name]
            #     del output_consumer_node.input[input_index]
            #     output_consumer_node.input.insert(input_index, output_tensors[i].name)

            ## remove dummy placeholders
            fpga_subgraph_def.remove_nodes([inp.op.name for inp in input_tensors])

            ## remove original output from input graph
            graph_def.remove_nodes([out.op.name for out in output_tensors])

            ## append fpga nodes to original graph
            fpga_nodes = fpga_subgraph_def.node
            graph_def.node.extend(fpga_nodes)

            return graph_def, fpga_nodes

        #############################
        ## function's body
        #############################
        graph_def = deepcopy(self._graph)

        self.fpga_pynode_dict = OrderedDict()

        for t, partition in enumerate(partitions):
          if partition.supported:
            graph_def, __ = insert_fpga_pynode(graph_def, partition)

        # check graph integrity
        isCyclic, cycle = graph_def.is_cyclic()
        if isCyclic:
          print('Graph partitioning resulted in cyclic graph; {}'.format(graph_def.all_cycles()))
          try:
            ## Make sure the modified graph_def is valid
            with tf.Graph().as_default():
              tf.import_graph_def(graph_def, name='')
            print('Tensorflow can handle the cyclic graph. All is fine!')
          except:
            raise RuntimeError('Tensorflow cannot handle the cyclic graph!')

        ## Store pyfunc definitions
        if self._args.get('savePyfunc', False):
          with open(self.file_path('.pickle', name_postfix='-pyfunc'), 'wb') as db:
            pickle.dump(self.fpga_pynode_dict, db, protocol=pickle.HIGHEST_PROTOCOL)

        ## cleanup graph (at this point we do not have variables in graph so no initialization (sess) is needed)
        graph_def, fValidGraph = xdnn_util_tf.freeze_graph(None,
                                                           graph_def,
                                                           sinknodes_list=self.outputs,
                                                           remove_training_nodes=False,
                                                           filename=self.file_path('.pb', name_postfix='-fpga'),
                                                          )

        return graph_def

    def load_partitioned_graph(self):
        print('load partitioned graph')
        ## import the base partitioned graph
        tf.reset_default_graph()
        tf.import_graph_def(self.graph_def, name='')
        graph = tf.get_default_graph()

        ## declare py_functions in graph
        py_func_tokens = list(_script_ops._py_funcs._funcs.keys())
        for token in py_func_tokens:
          _script_ops._py_funcs.remove(token)
        _script_ops._py_funcs._unique_id = 0

        if self._args.get('loadPyfunc', False):
          pickle_file = self.file_path('.pickle', name_postfix='-pyfunc')
          if not isfile(pickle_file):
            raise ValueError(f'missing {pickle_file} to loadPyfunc')
          with open(pickle_file, 'rb') as db:
            self.fpga_pynode_dict = pickle.load(db)

        #with tf.device(global_fpga_device):  ## TODO: add FPGA support for TF
        for pyfunc_name, (partition, input_names, output_dtypes) in self.fpga_pynode_dict.items():
          input_tensors = [graph.get_tensor_by_name(name) for name in input_names]

          fpga_output_tensor = tf.py_func(partition.forward_exec, input_tensors, output_dtypes,
                                          stateful=False, name=pyfunc_name)

          # print('graph py_tunctions:   {}'.format(graph._py_funcs_used_in_graph))
          # print('graph py_func tokens: {}'.format(_script_ops._py_funcs._funcs.keys()))

        return graph


    def forward_exec(self, **kwargs):
        def default_forward_exec(**kwargs):
            if 'sess' in kwargs:
              sess  = kwargs['sess']
              graph = sess.graph
            else:
              #config=tf.ConfigProto(log_device_placement=True)
              graph = self.load_partitioned_graph()
              sess  = tf.Session(graph=graph, config=kwargs.get('config', None))

            ## declare input tensors to network graph
            input_names = kwargs.get('input_names', None)
            if not input_names:
              input_names = self.inputs
            else:
              for inp in input_names:
                if not isinstance(inp, _string_types):
                  raise TypeError('input_names should be flattened list of name strings')
            input_tensors  = [graph.get_operation_by_name(name).outputs[0] for name in input_names]

            ## declare output tensors to network graph
            output_names = kwargs.get('output_names', None)
            if not output_names:
              output_names = self.outputs
            else:
              for out in output_names:
                if not isinstance(out, _string_types):
                  raise TypeError('output_names should be flattened list of name strings')
            output_tensors = [graph.get_operation_by_name(name).outputs[0] for name in output_names]

            ## bound the input tensors to input data
            preprocess   = kwargs.get('preprocess', self.preprocess)
            input_values = xdnn_util.make_list(kwargs.get('input_values', None))

            feed_dict = {inp_tensor: inp_val if len(inp_val.shape) == len(inp_tensor.shape) else
                         inp_val[None,...] for inp_tensor, inp_val in zip(input_tensors, preprocess(input_values))}

            output_values = sess.run(output_tensors, feed_dict=feed_dict if feed_dict else None)

            if 'sess' not in kwargs:
              sess.close()

            return {tensor.op.name: val for val, tensor in zip(output_values, output_tensors)}

        if 'forward_exec' in kwargs:
          return kwargs['forward_exec'](*kwargs.get('argv', None))
        else:
          return default_forward_exec(**kwargs)

    def preprocess(self, inputs):
        ## inputs should be a list
        if not isinstance(inputs, list):
          raise TypeError('inputs to preprocessing should be a list')

        res = []
        for inp in inputs:
          if isinstance(inp, np.ndarray):
            res.append(inp)
          elif isinstance(inp, _string_types):
            res.append(_imread(inp))
        return res


    def debug_finalnode(self, input_values, **kwargs):
        input_values = xdnn_util.make_list(input_values)

        input_names  = self.inputs
        output_names = xdnn_util.make_list(self._args.finalnode)

        ## running the original graph
        with tf.Graph().as_default() as graph_org:
          tf.import_graph_def(self._graph, name='')

          node_dict, \
          output_dict    = graph_org.as_graph_def().get_node_dict(outmap=True)
          input_tensors  = [graph_org.get_operation_by_name(name).outputs[0] for name in input_names]
          output_tensors = [graph_org.get_operation_by_name(name).outputs[0] for name in output_names]

          print('org tensor: {}'.format(output_tensors))
          with tf.Session() as sess:
            feed_dict = {tensor: [value] for tensor, value in zip(input_tensors, input_values)}
            ret = {'org': sess.run(output_tensors, feed_dict)}


        ## running the partitioned graph
        graph          = self.load_partitioned_graph()
        input_tensors  = [graph.get_operation_by_name(name).outputs[0] for name in input_names]
        output_tensors = [graph.get_operation_by_name(name).outputs[0] for name in output_names]

        # output_tensors = []
        # for name in output_names:
        #   consumers = output_dict[name].keys()
        #   name = consumers[0] if consumers else name
        #   inp = [node.name for node in graph.get_operation_by_name(name).inputs if 'transpose' in
        #          node.name]
        #   if inp:
        #     ## use this in case output_names are NOT the last operations in global graph
        #     output_tensors.append(inp[0])
        #   else:
        #     ## use this in case output_names are the last operations in global graph
        #     output_tensors.append(graph.get_operation_by_name(name).outputs[0])
        ##########################################

        print('fpga tensor: {}'.format(output_tensors))
        with tf.Session(graph=graph) as sess:
          feed_dict = {tensor: [value] for tensor, value in zip(input_tensors, input_values)}
          ret.update({'fpga': sess.run(output_tensors, feed_dict)})

        for org, fpga in zip(ret['org'], ret['fpga']):
          print('org  max {} min {} mean {} std {}'.format(org.max(), org.min(), org.mean(), org.std()))
          print('FPGA max {} min {} mean {} std {}'.format(fpga.max(), fpga.min(), fpga.mean(), fpga.std()))
          print('DIFF max {} min {} mean {} std {}'.format((fpga-org).max(), (fpga-org).min(),
                                                           (fpga-org).mean(), (fpga-org).std()))

        return ret







if __name__ == '__main__':
    import os
    from argparse import ArgumentParser
    from vai.dpuv1.rt.xdnn_io import default_xdnn_arg_parser

    models = \
    [os.environ['VAI_ALVEO_ROOT']+'/models/container/tensorflow/inception_v1_baseline.pb',
     os.environ['VAI_ALVEO_ROOT']+'/models/container/tensorflow/inception_v4.pb',
     '/proj/xsjhdstaff5/arminb/tensorflow/uber/image_cnn.v1/image_cnn.v1.1920x512.pb',
     '/proj/xsjhdstaff5/arminb/tensorflow/ssd/ssd_graph_frozen.pb',
     '/proj/xsjhdstaff5/arminb/tensorflow/ssd/resnet34_tf.22.1.nhwc.pb',
     '/proj/xsjhdstaff5/arminb/tensorflow/ssd1200/frozen_nhwc.pb',
    ]

    argparser = ArgumentParser(description='partitioning test')
    argparser.add_argument('--networkfile', nargs='+', default=models, help='models path')
    argparser.add_argument('--quant_cfgfile', type=str, default="IGNORE", help='models path')
    argparser.add_argument('--batch_sz', type=int, default=1, help='target hardware')
    argparser.add_argument('--hardware', type=str, default='xdnn', help='target hardware')
    argparser.add_argument('--loadmode', type=str, default='binary', help='model load mode')
    argparser.add_argument('--startnode', type=str, help='start nodes')
    argparser.add_argument('--finalnode', type=str, help='final nodes')
    args = xdnn_util.dict2attr(argparser.parse_args())

    args.networkfile = args.networkfile[1]
    # args.xclbin = os.environ['VAI_ALVEO_ROOT'] + '/overlaybins/xdnnv3/'
    # args.device = 'FPGA'
    # args.bytesperpixels = 1
    # args.xdnnv3 = True
    args.fcinfpga = True
    # args.pingpongsplit = True
    # args.fancyreplications = True
    # args.poolingaround = True
    # args.pipelineconvmaxpool = True
    args.bridges = ['bytype', 'Concat']
    # args.usedeephi = True
    # args.quantz = ''

    # args.savePyfunc = True
    # args.savepickle = True
    # args.loadpickle = True

    # parser = default_xdnn_arg_parser()
    # argv = parser.parse_args([])
    argv = None

    # xdnnTF = TFxdnnRT(argv, 'DEFINITIVE', **args)
    xdnnTF = TFxdnnRT(argv, 'SPECULATIVE', debug_coloring=False, **args)
    xdnnTF.load_partitioned_graph()
