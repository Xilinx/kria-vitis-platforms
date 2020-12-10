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

from collections import defaultdict, Iterable
from six import string_types as _string_types
from copy import deepcopy

import numpy as np
import tensorflow as tf
from tensorflow.python.platform import gfile as _gfile
from tensorflow.python.framework import ops as _ops
from tensorflow.python.framework import tensor_shape as _tensor_shape
from google.protobuf import text_format

import vai.dpuv1.rt.xdnn_util as _xdnn_util
from vai.dpuv1.rt import graph_editor






graph_util = tf.graph_util    ## TODO: for tf_version > 1.14 use tf.compat.v1.graph_util
GraphDef = tf.GraphDef        ## TODO: for tf_version > 1.14 use tf.compat.v1.GraphDef




def _get_sequence(value, n, channel_index, name):
  if value is None:
    value = [1]
  elif not isinstance(value, Iterable):
    value = [value]

  current_n = len(value)
  if current_n == n + 2:
    return value
  elif current_n == 1:
    value = list((value[0],) * n)
  elif current_n == n:
    value = list(value)
  else:
    raise ValueError("{} should be of length 1, {} or {} but was {}".format(
        name, n, n + 2, current_n))

  if channel_index == 1:
    return [1, 1] + value
  else:
    return [1] + value + [1]



def strip_node_name(node_name):
  """Strips off ports and other decorations to get the underlying node name."""
  if node_name.startswith("^"):
    node_name = node_name[1:]
  node_name = node_name.split(':')[0]
  return node_name



## expanding tf.NodeDef methods
def set_name(self, name):
  self.name = name

def set_shape(self, shape):
  if 'shape' in self.attr:
    if len(self.attr['shape'].shape.dim) == len(shape):
      TensorShapeProto = _tensor_shape.as_shape(shape).as_proto()
      self.attr['shape'].shape.CopyFrom(TensorShapeProto)
    else:
      raise ValueError('shape dimension mismatch')
  else:
    self.attr['_output_shapes'].list.shape.append(_tensor_shape.as_shape(shape).as_proto())

def set_strides(self, strides, channel_index):
  if 'strides' in self.attr and len(self.attr['strides'].list.i) not in [len(strides), len(strides)+2]:
    raise ValueError('stride dimension mismatch')
  if channel_index == 1:
    self.attr['strides'].list.i[:] = [1, 1] + strides
  else:
    self.attr['strides'].list.i[:] = [1] + strides + [1]

def set_dilations(self, dilations, channel_index):
  if 'dilations' in self.attr and len(self.attr['dilations'].list.i) not in [len(dilations), len(dilations)+2]:
    raise ValueError('stride dimension mismatch')
  if channel_index == 1:
    self.attr['dilations'].list.i[:] = [1, 1] + dilations
  else:
    self.attr['dilations'].list.i[:] = [1] + dilations + [1]

def get_strides(self, channel_index):
  ret = []
  if 'strides' in self.attr:
    strides = self.attr['strides'].list.i
    if channel_index == 1:
      ret = strides[2:]
    else:
      ret = strides[1:3]
  return ret

def get_dilations(self, channel_index):
  ret = []
  if 'dilations' in self.attr:
    dilations = self.attr['dilations'].list.i
    if channel_index == 1:
      ret = dilations[2:]
    else:
      ret = dilations[1:3]
  return ret

def get_padding(self):
  ret = None
  if 'padding' in self.attr:
    padding = self.attr['padding']
    if isinstance(padding.s, _string_types):
      ret = padding.s
    else:
      ret = padding.list.i[:]
  return ret

def get_shape(self):
  ret = []
  if 'shape' in self.attr:
    ret.append([dim.size if dim.size>0 else None for dim in self.attr['shape'].shape.dim])
  elif '_output_shapes' in self.attr:
    for output_shape in self.attr['_output_shapes'].list.shape:
      ret.append([dim.size if dim.size>0 else None for dim in output_shape.dim])
  return ret

def get_dtype(self):
  if 'T' in self.attr:        ## if op
    dtype_enum = self.attr['T'].type
  elif 'dtype' in self.attr:  ## if tensors
    dtype_enum = self.attr['dtype'].type
  elif 'output_types' in self.attr:       ## if IteratorGetNext
    dtype_enum = self.attr['output_types'].list.type[0]
  else:
    raise AttributeError('NodeDef not supperted')
  return tf.DType(dtype_enum)

def get_dtype_enum(self):
  dtype = get_dtype(self)
  return dtype.as_datatype_enum



## expanding tf.GraphDef methods
def get_node_dict(self, outmap=False):
  node_dict = {}
  output_dict = defaultdict(lambda: {})
  for node in self.node:
    node_name = strip_node_name(node.name)

    if node_name in node_dict:
      raise KeyError('{} already exist in node dictionary'.format(node_name))
    else:
      node_dict[node_name] = node

    output_dict[node_name]
    for input_index, input_name in enumerate(node.input):
      input_name = strip_node_name(input_name)
      output_dict[input_name][node_name] = input_index

  if outmap:
    return node_dict, output_dict
  return node_dict

def get_output_dict(self):
  output_dict = defaultdict(lambda: {})
  for node in self.node:
    node_name = strip_node_name(node.name)
    output_dict[node_name]
    for input_index, input_name in enumerate(node.input):
      input_name = strip_node_name(input_name)
      output_dict[input_name][node_name] = input_index
  return output_dict

def get_node_index(self, node_name):
  for idx, node in enumerate(self.node):
    if node.name == node_name:
      return idx

def remove_nodes(self, node_names):
  for name in _xdnn_util.make_list(node_names):
    del self.node[self.get_node_index(name)]

def is_cyclic(self):
  return _xdnn_util.is_cyclic(get_node_dict(self), {node_name: desc_dict.keys() for node_name, desc_dict in get_output_dict(self).items()})

def all_cycles(self):
  return _xdnn_util.all_cycles(get_node_dict(self), {node_name: desc_dict.keys() for node_name, desc_dict in get_output_dict(self).items()})



## discover global sink nodes in a graph
def discover_sinknodes(graph_def):
  ## this alorithm work for the global graph (not subgraphs)
  ## find nodes that are not input to anyother node
  inp_set = set([inp.split(':')[0] for node in graph_def.node for inp in node.input])
  sinknodes = [node.name for node in graph_def.node if node.name not in inp_set]
  return sinknodes


def discover_sourcenodes(graph_def):
  return [node.name for node in graph_def.node if node.op == 'Placeholder']


def freeze_graph(sess, graph_def, remove_training_nodes=True, remove_redundant_nodes=True,
                 sinknodes_list=[], freeze_blacklist=[], freeze_whitelist=[], filename=None):
  ## freezing a graph_def by removing freeze_blacklist, training nodes(if remove_training_nodes), nodes not
  ## contributing in sinknode_list computation(if remove_redundant_nodes), while keeping
  ## freeze_blacklist (which includes the specified sinknodes_list)

  print('freeze model')

  sinknodes_list   = _xdnn_util.make_list(sinknodes_list)
  freeze_blacklist = _xdnn_util.make_list(freeze_blacklist)
  freeze_whitelist = _xdnn_util.make_list(freeze_whitelist)

  # if sess is not None:
  #   graph_def = sess.graph.as_graph_def(add_shapes=True)

  ## convert variables to constants for inference model
  if not sinknodes_list:
    sinknodes_list = discover_sinknodes(graph_def)

  if sess is not None:
    print('.... convert variables to constants')
    graph_def = graph_util.convert_variables_to_constants(sess, graph_def, sinknodes_list)

  freeze_whitelist += sinknodes_list
  print('.... node count {}'.format(len(graph_def.node)))
  if remove_training_nodes:
    graph_def = graph_util.remove_training_nodes(graph_def, protected_nodes=freeze_whitelist)
    print('.... node count after removing training nodes {}'.format(len(graph_def.node)))

  if remove_redundant_nodes:
    graph_def = graph_util.extract_sub_graph(graph_def, sinknodes_list)
    print('.... node count after removing redundant nodes {}'.format(len(graph_def.node)))

  ## remove freeze_balcklist nodes
  # add summary nodes to freeze_balcklist
  freeze_blacklist += ['Summaries', 'MergeSummary']

  graph_def_frozen = GraphDef()

  for node in graph_def.node:
    pass_cnd = np.array([blocked not in node.name for blocked in freeze_blacklist])
    if pass_cnd.all():
      graph_def_frozen.node.extend([node])
  print('.... node count after removing blacklisted nodes {}'.format(len(graph_def_frozen.node)))

  try:
    fValidGraph = True
    ## fill in all output shapes
    with tf.Graph().as_default() as temp_graph:
      tf.import_graph_def(graph_def_frozen, name='')
      graph_def_frozen = temp_graph.as_graph_def(add_shapes=True)

  except Exception as e:
    fValidGraph = False
    print(e)
    print(type(graph_def_frozen))
    #assert(False, "invalid graph_def")

  if filename is not None:
    print('save graph at {}'.format(filename))
    with tf.gfile.GFile(filename, "wb") as f:
      f.write(graph_def_frozen.SerializeToString())

  return graph_def_frozen, fValidGraph

_load_graph_args_keys = ['networkfile', 'loadmode', 'startnode', 'finalnode', 'inclusive', 'batch_sz', 'fixinputnames', 'placeholdershape', 'remove_training_nodes', 'remove_redundant_nodes', 'freeze_blacklist', 'freeze_whitelist', 'graph_savepath']

def load_graph(args, **kwargs):
    ## Loads the graph at args.networkfile. args can be either namedspace, namedtuple. or dictionary
    ## parameters:
    ##    networkfile:    tf.Graph, tf.GraphDef, or path to the network file
    ##    loadmode:       saving protocol of the network file
    ##    startnode:      list of source nodes of the graph. (optional. Defaults to all placehoders)
    ##    finalnode:      list of sink nodes of the graph. (optional. Defaults to all sinknodes)
    ##    inclusive:      include the starnodes. (optional. Defaults to True)
    ##    fixinputnames:  fix the input placeholder name. otherwise their names starts with geph__ and ends with a index. (optional. Defaults to True)
    ##    placeholdershape:         Dictionary mapping of placehoder shapes to new shapes
    ##    remove_training_nodes:    Limits the network to inference nodes if True (optional. Defaults to True)
    ##    remove_redundant_nodes:   Limits the network to nodes involved in computing the sinknodes (optional. Defaults to True)
    ##    freeze_blacklist:         list of nodes to keep in the graph (optional)
    ##    freeze_whitelist:         list of nodes to remove in the graph (optional)
    ##    graph_savepath:           path to save the updated graph (optional)
    ## return:
    ##    imported and modified graph_def
    ##    inputs to graph_def
    ##    outputs from graph_def

    sess = node_dict = None
    args = _xdnn_util.dict2attr(args)
    args.update(kwargs)

    ## load graph_def
    if not args.networkfile:
      raise AttributeError('networkfile is not specified!')

    print('\n######### load_graph arguments #############')
    print('{: <25} {}'.format('networkfile', args.networkfile if isinstance(args.networkfile, _string_types)
          else 'ALREADY LOADED'))
    for key in _load_graph_args_keys[1:]:
      print('{: <25} {}'.format(key, args[key]))
    print('#############################################\n')

    if isinstance(args.networkfile, tf.Graph):
      graph_def = args.networkfile.as_graph_def(add_shapes=True)
    elif isinstance(args.networkfile, GraphDef):
      graph_def = deepcopy(args.networkfile)
    elif isinstance(args.networkfile, _string_types):
      loadmode = args.get('loadmode', 'binary')

      graph_def = GraphDef()
      with tf.Graph().as_default() as temp_graph:
        if loadmode.lower() in ['chkpt', 'checkpoint']:
          saver = tf.train.Saver()
          sess = tf.Session(graph=temp_graph)
          saver.restore(sess, args.networkfile)
          graph_def = sess.graph.as_graph_def(add_shapes=True)
        if loadmode.lower() in ['savedmodel', 'saved_model']:
          sess = tf.Session(graph=temp_graph)
          tf.saved_model.loader.load(sess, [tf.saved_model.tag_constants.SERVING], args.networkfile)
          graph_def = sess.graph.as_graph_def(add_shapes=True)
        elif loadmode.lower() in ['txt', 'text']:
          with open(args.networkfile) as f:
            graph_def = text_format.Parse(f.read(), GraphDef())
        elif loadmode.lower() in ['pb', 'binary']:
          with _gfile.FastGFile(args.networkfile, 'rb') as f:
          # with tf.io.gfile.GFile(args.networkfile, 'rb') as f:
            graph_def.ParseFromString(f.read())
        else:
          raise ValueError('unsupported loadmode parameter: \"{}\"'.format(loadmode))
    else:
      raise ValueError('unrecognise networkfile format: {}'.format(args.networkfile))

      ## fill in all output shapes (Not necessary, will be performed in freeze_graph)
      # tf.import_graph_def(graph_def, name='')
      # graph_def = temp_graph.as_graph_def(add_shapes=True)

    placeholdershape = args.get('placeholdershape', {})
    startnode        = _xdnn_util.make_list(args.startnode)
    finalnode        = _xdnn_util.make_list(args.finalnode)
    inputs           = startnode if startnode else discover_sourcenodes(graph_def)
    outputs          = finalnode if finalnode else discover_sinknodes(graph_def)

    ## Apply batch size correction
    batch_sz = args.get('batch_sz', -1)
    if batch_sz > 0:
      node_dict = node_dict if node_dict else get_node_dict(graph_def)
      for name in inputs:
        shapes = get_shape(node_dict[name])
        for shape in shapes:
          if shape[0] == None:
            if len(shape) == 4:
              shape[0] = batch_sz
              placeholdershape[name] = shape
            else:
              print(f'Please specify placeholdershape explicitly. Unknown batch_size {batch_sz} for input shape {shape}')

    ## Apply placeholder shape corrections
    if placeholdershape:
      node_dict = node_dict if node_dict else get_node_dict(graph_def)
      for name, shape in placeholdershape.items():
        print('change palceholder {} shape to {}'.format(name, shape))
        set_shape(node_dict[name], shape)

    ## Extract the subgraph of interest
    if startnode or finalnode:
      graph_def = extract_subgraph(graph_def, outputs, inputs,
                                   inclusive=args.get('inclusive', True),
                                   fixinputnames=args.get('fixinputnames', True),
                                   fixnodeshapes=args.get('fixnodeshapes', True)
                                  )

      inputs  = discover_sourcenodes(graph_def)
      outputs = discover_sinknodes(graph_def)

    graph_def, fValidGraph = freeze_graph(sess, graph_def,
                                          sinknodes_list=outputs,
                                          remove_training_nodes=args.get('remove_training_nodes', True),
                                          remove_redundant_nodes=args.get('remove_redundant_nodes', True),
                                          freeze_blacklist=args.get('freeze_blacklist', []),
                                          freeze_whitelist=args.get('freeze_whitelist', []),
                                          filename=args.graph_savepath)

    if sess:
      sess.close

    return graph_def, inputs, outputs, fValidGraph


## finding ops between a set of seed_ops and boundry_ops
def make_list_of_op(ops):
  if isinstance(ops, _ops.Graph):
    return ops.get_operations()
  else:
    if not isinstance(ops, Iterable):
      ops = [ops]
    if not ops:
      return []
    return [op for op in ops if isinstance(op, _ops.Operation)]

def get_within_boundary_ops(ops, seed_ops, boundary_ops=(), inclusive=False):
  ops          = make_list_of_op(ops)
  seed_ops     = make_list_of_op(seed_ops)
  boundary_ops = set(make_list_of_op(boundary_ops))
  res          = set(seed_ops)
  #if boundary_ops & res:
  #  raise ValueError("Boundary is intersecting with the seeds.")
  wave = set() if res == boundary_ops else set(seed_ops)
  while wave:
    new_wave = set()
    ops_io = [inp.op for op in wave for inp in op.inputs]
    for op in ops_io:
      if op in res:
        continue
      if op in boundary_ops:
        if inclusive:
          res.add(op)
      else:
        new_wave.add(op)
    res.update(new_wave)
    wave = new_wave
  return [op for op in ops if op in res]

def extract_subgraph(graph_def, outputs, inputs, inclusive=False, fixinputnames=True,
                     fixnodeshapes=False):
    node_dict = org_node_dict = None
    with tf.Graph().as_default() as graph:
      tf.import_graph_def(graph_def, name='')
      seed_ops      = [graph.get_operation_by_name(output) for output in outputs]
      boundary_ops  = [graph.get_operation_by_name(input)  for input  in inputs ]
      ops = get_within_boundary_ops(graph, seed_ops, boundary_ops, inclusive)

      sgv = graph_editor.make_view(ops)

      subgraph = tf.Graph()
      graph_editor.copy(sgv, subgraph, reuse_dst_scope=True)

      subgraph_def = subgraph.as_graph_def()

      if fixinputnames:
        ## renaming the placeholders
        graph_inputs  = [inp.op.name for inp in sgv.inputs]
        org_node_dict = get_node_dict(graph_def)
        node_dict, \
        output_dict   = get_node_dict(subgraph_def, outmap=True)
        ph_names      = [name for name in node_dict.keys() if graph_editor.util._DEFAULT_PLACEHOLDER_PREFIX in name]

        for name in ph_names:
          consumer, index = list(output_dict[name].items())[0]
          org_name = org_node_dict[consumer].input[index]

          ## rename the node itself
          node = node_dict.pop(name)
          node.name = org_name
          node_dict[org_name] = node


          ## rename associated consumer node inputs
          ## NOTE: we do NOT fix this dictionary key
          for cns_name, index in output_dict[name].items():
            cns_node = node_dict[cns_name]
            cns_node.input[index] = org_name

      if fixnodeshapes:
        org_node_dict = org_node_dict if org_node_dict else get_node_dict(graph_def)
        node_dict     = node_dict if node_dict else get_node_dict(subgraph_def)
        for name, node in node_dict.items():
          org_shapes = get_shape(org_node_dict[name])
          if not get_shape(node) and org_shapes:
            for org_shape in org_shapes:
              set_shape(node, org_shape)

    return subgraph_def

def channel_swap(shape, source_channel_index, target_channel_index):
    if len(shape) == 4 and source_channel_index != target_channel_index:
     channels = shape.pop(source_channel_index)
     shape.insert(target_channel_index, channels)
    return shape

def graph_channel_swap(graph_def, source_channel_index, target_channel_index):
    node_dict, \
    output_dict = get_node_dict(graph_def, outmap=True)

    visited = set()
    for name in discover_sourcenodes(graph_def):
      names = [name]
      visited.add(name)

      while names:
        name = names.pop()
        node = node_dict[name]
        for consumer in output_dict[name].keys():
          if consumer not in visited:
            names.append(consumer)
            visited.add(consumer)

        shapes = get_shape(node)
        if shapes:
          for shape in shapes:
            channels = shape.pop(source_channel_index)
            shape.insert(target_channel_index, channels)
            set_shape(node, shape)

        if 'data_format' in node.attr:
          print(node.name)
          if target_channel_index == 1:
            node.attr['data_format'].s = 'NCHW'
          else:
            node.attr['data_format'].s = 'NHWC'

        strides = get_strides(node, source_channel_index)
        if strides:
          set_strides(node, strides, target_channel_index)

        dilations = get_dilations(node, source_channel_index)
        if dilations:
          set_dilations(node, dilations, target_channel_index)

        padding = get_padding(node)
        if padding and not isinstance(padding, _string_types):
          pass

    node_dict, \
    output_dict = get_node_dict(graph_def, outmap=True)

    return graph_def
