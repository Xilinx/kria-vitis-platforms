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

import pydot
from collections import defaultdict

from vai.dpuv1.rt.xdnn_util import dict2attr, make_list, topological_ordering, is_cyclic, all_cycles, DefaultOrderedDict
from vai.dpuv1.rt.xdnn_util_tf import strip_node_name
from vai.dpuv1.rt.coloring.hw_definitions import hw_xdnn

class Node(object):
  def __init__(self, name, op, inputs, consumers=None, color=0, parameters=None):
    self.name = name
    self.op = op
    self.inputs = make_list(inputs)
    self.consumers = make_list(consumers)
    self.color = color
    self.paramters = parameters

  def add_children(self, inputs):
    self.inputs.extend(make_list(inputs))

  def add_parents(self, consumers):
    self.consumers.extend(make_list(consumers))


class CGraph(object):
    def __init__(self, args, **kwargs):
        args = dict2attr(args)
        args.update(kwargs)

        ## identify hardware definitions
        hw_def = args.get('hardware', 'xdnn')
        if hw_def.lower() == 'xdnn':
          self.hw_def = hw_xdnn()
        else:
          raise TypeError('hardware {} is not supported!'.format(args.hardware))

        self._args            = args
        self.nodeMap          = {}
        self.colorMap_ops     = {}    # 0x<2:counter><3:cpu><3:fpga>
        self.colorMap_consts  = {}    # 0x<2:counter><3:cpu><3:fpga>
        self.adjacencyMap     = defaultdict(list)
        self.adjacencyMap_rev = defaultdict(list)
        self.collectionMap    = DefaultOrderedDict(list)

    def boundaries_by_scope(self, scope=None):
        '''
        returns sourcenodes, sinknodes.
        sourcenodes are exclusive, i.e., they do not belong to the layer, except when the sourcenode
        does not have an input itself.
        sinknodes are inclusive, i.e., they are part of the layer.
        sourcenode_map are the last node that was collapsed into the sourcenodes.
        sinknode_map are the last node that was collapsed into the sinknodes.
        '''
        scoped_nodes = [name for name in self.nodeMap if scope in name] if scope else self.nodeMap.keys()
        scope_name_set = set(scoped_nodes)
        sourcenodes = []
        sinknodes   = []
        inputs      = []
        for name in scope_name_set:
          inps = [x.name for x in self.nodeMap[name].inputs]
          if len(inps) == 0:
            if name not in self.colorMap_consts:
              sourcenodes += [name]
          else:
            inputs += inps

          outs = [x.name for x in self.nodeMap[name].consumers]
          ## NOTE: "not outs" might cause issues
          if not outs or any([output not in scope_name_set for output in outs]):
            sinknodes += [name]

        sourcenodes += [inp for inp in set(inputs) if inp not in scope_name_set]
        return sourcenodes, sinknodes

    def add_node(self, name, op, inputs, parameters=None):
        _op = self.hw_def.get_op(op)
        if not _op:
          _op = op
        # print(name, op, _op)
        name = strip_node_name(name)
        node = Node(name, _op, inputs, parameters=parameters)
        self.nodeMap[name] = node

    def get_fpga_nodes(self):
        ret = {}
        ret.update({'ops': {name: color for name, color in self.colorMap_ops.items() if color & 0x7}})
        ret.update({'constants': {name: color for name, color in self.colorMap_consts.items() if color & 0x7}})

        ret['op_to_const'] = {}
        for op, __ in ret['ops'].items():
          i = 0
          nodes = [op]
          while i < len(nodes):
            nodes += [name for name in self.adjacencyMap[nodes[i]] if name in ret['constants']]
            i += 1
          if len(nodes) > 1:
            ret['op_to_const'][op] = nodes[1:]
        return ret

    def get_cpu_nodes(self):
        ret = {}
        ret.update({'ops': {name: color for name, color in self.colorMap_ops.items() if color & 0x38}})
        ret.update({'constants': {name: color for name, color in self.colorMap_consts.items() if color & 0x38}})

        ret['op_to_const'] = {}
        for op, __ in ret['ops'].items():
          i = 0
          nodes = [op]
          while i < len(nodes):
            nodes += [name for name in self.adjacencyMap[nodes[i]] if name in ret['constants']]
            i += 1
          if len(nodes) > 1:
            ret['op_to_const'][op] = nodes[1:]
        return ret

    def color_op_node(self, node, color, decolor=0x00):
        if node.color & decolor != decolor:
          decolor &= node.color
          #print('WARNING: decoloring would set a color bit instead or erasing all. node {}, current color {}, decoloring {}'.format(node.name, node.color, decolor))

        node.color ^= decolor
        node.color |= color
        self.colorMap_ops[node.name] = node.color

    def color_extraneous_node(self, node, color, decolor=0x00):
        if node.color & decolor == decolor:
          node.color ^= decolor
        else:
          raise RuntimeError('decoloring is setting a color instead or erasing. current color {}, decoloring {}'.format(node.color, decolor))
        node.color |= color
        self.colorMap_consts[node.name] = node.color

    def build_graph_connections(self):
        print('\nbuild graph connections ....')
        if not self.nodeMap:
          raise RuntimeError('Please add nodes to graph first using add_node.')

        for name, node in self.nodeMap.items():
          ## name is already stripped
          self.adjacencyMap[name]
          self.adjacencyMap_rev[name]
          inputs = []
          for child in node.inputs:
            ## child is stripped
            child = strip_node_name(child)
            self.adjacencyMap[name].append(child)
            self.adjacencyMap_rev[child].append(name)

            child = self.nodeMap[child]
            child.add_parents(node)
            inputs.append(child)
          node.inputs = inputs

        isCyclic, cycle = self.is_cyclic()
        if isCyclic:
          cycles = self.all_cycles()
          self.break_cycles(cycles)
          print(self.is_cyclic())
          # raise RuntimeError('Graph is cyclic; {}'.format(cycles))
        print('build graph connections [DONE]\n')

    def is_cyclic(self):
        return is_cyclic(self.nodeMap, self.adjacencyMap)

    def all_cycles(self):
        return all_cycles(self.nodeMap, self.adjacencyMap)

    def break_cycles(self, cycles):
        for cycle in cycles:
          print('.... breaking cycle {}'.format(cycle))
          parent, child = cycle[-2:]
          idx = self.adjacencyMap[parent].index(child)
          self.adjacencyMap[parent].pop(idx)
          self.adjacencyMap_rev[child].remove(parent)
          self.nodeMap[parent].inputs.pop(idx)

    def color_compute_path(self, startnodes):
        print('\ncolor input path ....')
        for startnode in make_list(startnodes):
          nodes = [self.nodeMap[startnode]]
          while nodes:
            node = nodes.pop()
            if node.name not in self.colorMap_ops:
              self.color_op_node(node, 0x08)
              # print(node.name, node.op)
              nodes.extend(node.consumers)
        print('color input path [DONE]\n')

    def color_pertinent_path(self, finalnodes):
        print('\ncolor output path ....')
        colored = {x: False for x in self.colorMap_ops}
        opTree  = self.hw_def._opTree

        def fpga_subgraph(root, _is_fpga):
            path = []
            if colored.get(root.name, True):
              ## root is colored or
              ## not an op on both input and output paths
              return path

            # print('visiting:   {}'.format(root.name))
            colored[root.name] = True

            op = opTree.get_opNode(root.op)

            if opTree.is_atom_op(op):
              _is_fpga[0] = True

            for input in root.inputs:
              if op.in_before(opTree.get_opNode(input.op)):
                _path = fpga_subgraph(input, _is_fpga)
                if _is_fpga[0]:
                  path += _path

            if _is_fpga[0]:
              path.append(root)

            return path

        # def is_fpga(node, visited):
        #     def _return(ret, color=None, decolor=None, cause=''):
        #         if color or decolor:
        #           colored[node.name] = True
        #           self.color_op_node(node, color, decolor)
        #         # print('{}: {} --> {}'.format(cause, node.name, ret))
        #         return ret

        #     if node.name not in colored:
        #       ## node is not an op on both input and output paths
        #       return _return(False, cause='CONSTANT')
        #     elif colored[node.name]:
        #       ## op is already colored
        #       if node.color & 0x7:
        #         return _return(True, cause='COLORED')
        #       else:
        #         return _return(False, cause='COLORED')

        #     visited.add(node.name)

        #     op = opTree.get_opNode(node.op)

        #     for input in node.inputs:
        #       if (input.name not in visited and
        #           op.in_before(opTree.get_opNode(input.op)) and
        #           is_fpga(input, visited)):
        #         return _return(True, 0x01, 0x08, 'INPUT')

        #     if opTree.is_atom_op(op):
        #       return _return(True, 0x01, 0x08, 'ATOM')

        #     for consumer in node.consumers:
        #       if (consumer.name not in visited and
        #           op.in_after(opTree.get_opNode(consumer.op)) and
        #           is_fpga(consumer, visited)):
        #         return _return(True, 0x01, 0x08, 'CONSUMER')

        nodes = set()
        for finalnode in make_list(finalnodes):
          nodes.add(self.nodeMap[finalnode])
          while nodes:
            root = nodes.pop()
            # print('start here: {}'.format(root.name))

            path = fpga_subgraph(root, [False])
            if path:
              for node in path:
                # print('coloring:   {}'.format(node.name))
                self.color_op_node(node, 0x01, 0x08)
                self.collectionMap[root.name].append(node.name)

                nodes.update(set([inp for inp in node.inputs if not colored.get(inp.name, True)]))

              # is_fpga(root, set())
            else:
              if root.name in colored:
                nodes.update(set([inp for inp in root.inputs if not colored.get(inp.name, True)]))
        print('color output path [DONE]\n')

    def color_extraneous_path(self):
        print('\ncolor constants ....')
        for name, color in self.colorMap_ops.items():
          nodes = [self.nodeMap[name]]
          while nodes:
            node = nodes.pop()
            inputs = [inp for inp in node.inputs if inp.name not in self.colorMap_ops]
            for inp in inputs:
              self.color_extraneous_node(inp, node.color)
            nodes += inputs
        print('color constants [DONE]\n')

    def color_graph(self, startnodes, finalnodes):
        self.build_graph_connections()
        self.color_compute_path(startnodes)
        self.color_pertinent_path(finalnodes)
        self.color_extraneous_path()

        if self._args.get('debug_coloring', False):
          print('\nDEBUG PRINTS:\n')
          print('\nFPGA OPERATIONS:')
          fpga_nodes = self.get_fpga_nodes()
          for op in [name for name, color in fpga_nodes['ops'].items()]:
            print('   {}'.format(op))
          print('FPGA CONSTANTS:')
          for op in [name for name, color in fpga_nodes['constants'].items()]:
            print('   {}'.format(op))

          print('\nCPU OPERATIONS:')
          cpu_nodes = self.get_cpu_nodes()
          for op in [name for name, color in cpu_nodes['ops'].items()]:
            print('   {}'.format(op))
          print('CPU CONSTANTS:')
          for op in [name for name, color in cpu_nodes['constants'].items()]:
            print('   {}'.format(op))

          # print('\nMIXED OPERATIONS:')
          # for op in [name for name, color in ops['ops'].items() if color == 9]:
          #   print('   {}'.format(op))
          # print('MIXED CONSTANTS:')
          # for op in [name for name, color in ops['constants'].items() if color == 9]:
          #   print('   {}'.format(op))

        if self._args.get('debug_coloring', False):
          import dill as pickle
          with open('debug_colored_graph.pickle', 'wb') as db:
            pickle.dump(self.colorMap_ops, db, protocol=pickle.HIGHEST_PROTOCOL)
            pickle.dump(self.colorMap_consts, db, protocol=pickle.HIGHEST_PROTOCOL)

        return self.colorMap_ops, self.colorMap_consts

    def create_pydot_graph(self, filename=None):
        print('\ncreate pydot graph ....')
        def mod_name(name):
            return "_".join(name.split('/'))

        pydot_graph = pydot.Dot(graph_type='digraph')
        pydot_nodes = {}
        fillMap = {
                   0x00: 'red',     # redundant
                   0x01: 'blue',    # FPGA
                   0x08: 'cyan4',   # CPU
                   0x09: 'yellow',  # mix
                  }

        for name, node in self.nodeMap.items():
          name = mod_name(name)
          _node = pydot.Node(name, style='filled', fillcolor=fillMap[node.color])
          pydot_graph.add_node(_node)
          pydot_nodes[name] = _node

        for name, children in self.adjacencyMap.items():
          name = mod_name(name)
          for child in children:
            child = mod_name(child)
            pydot_graph.add_edge(pydot.Edge(pydot_nodes[child], pydot_nodes[name]))

        if filename:
          frmt = filename.rsplit('.', 1)[-1]
          pydot_graph.write(filename, prog=None, format=frmt, encoding=None)
        print('create pydot graph [DONE]\n')

    ###########################################
    ### XDNN compiler compatibility methods
    ###########################################
    def get_nodes(self):
        nodes = []
        for name, node in self.nodeMap.items():
          _node = dict2attr()
          _node.LayerParameter = dict2attr({'name': name,
                                            'bottoms': [inp.name for inp in node.inputs],
                                            'extras_and_future': None,
                                            'collapse_future': None,
                                            'collapse_past': None,
                                           })
          nodes.append(_node)
        return nodes

    def compile(self):
        def compilerJson():
            sourcenodes, sinknodes = self.boundaries_by_scope()
            cpu_nodes = self.get_cpu_nodes()
            unsupported = {}
            unsupported.update(cpu_nodes['ops'])
            unsupported.update(cpu_nodes['constants'])
            ret = {}
            ret.update({'arguments': {'networkfile': ''}})
            ret.update({'unsupported': {'list': unsupported}})
            # ret.update({'inputs': [{'input_name': name} for name in sourcenodes]})
            # ret.update({'outputs': [{'output_name': None, 'previous_layers': name} for name in sinknodes]})
            ret.update({'inputs': [{'input_name': None} for name in sourcenodes]})
            ret.update({'outputs': [{'output_name': None, 'previous_layers': None} for name in sinknodes]})
            return ret

        def compilerSchedule():
            connectivity = defaultdict(lambda: defaultdict(list))
            for name, connections in self.adjacencyMap_rev.items():
              connectivity[name][name] += connections

            top_order = topological_ordering(connectivity, seed=self.startnode)

            ret = dict2attr()
            ret.time_to_layer = {i: [op] for i, op in enumerate(top_order[::-1])}
            ret.op_to_const = dict2attr()
            ret.op_to_const.supported   = self.get_fpga_nodes()['op_to_const']
            ret.op_to_const.unsupported = self.get_cpu_nodes()['op_to_const']
            return ret

        self.color_graph(self.startnode, self.finalnode)

        if self._args.get('debug_coloring', False):
          self.create_pydot_graph('debug_colored_graph.svg')

        return (self.graph_def,
                self.startnode,
                self.finalnode,
                self,
                compilerSchedule(),
                None,
                compilerJson()
               )
