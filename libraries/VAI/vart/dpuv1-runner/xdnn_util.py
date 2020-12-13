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

import time
import re
from functools import wraps
from collections import defaultdict, OrderedDict, Callable
from orderedset import OrderedSet
from six import string_types as _string_types
from ast import literal_eval as _literal_eval




def literal_eval(string):
  if isinstance(string, _string_types):
    try:
      string = _literal_eval(re.sub(r'(?<![\'\"])\b([a-zA-Z_\/]+)\b(?![\'\"])', '\"\g<1>\"', string))
    except:
      sstring = string.split(',')
      if len(sstring) > 1:
        string = [literal_eval(o.strip()) for o in sstring]
  elif isinstance(string, (tuple, list)):
    string = [literal_eval(substring) for substring in string]

  return string


def make_list(obj):
  obj = literal_eval(obj)

  if isinstance(obj, tuple):
    obj = list(obj)
  elif isinstance(obj, dict):
    obj = list(obj.items())
  elif not isinstance(obj, list):
    if obj is None:
      obj = []
    else:
      obj = [obj]

  return obj


def list_depth(L):
    if not isinstance(L, list):
      return 0
    elif not L:
      return 1
    return isinstance(L, list) and max(map(list_depth, L))+1

def nested_list_iter(L):
    if not isinstance(L, list):
      yield L
    elif list_depth(L) == 1:
      yield L
    else:
      for l in L:
        yield from nested_list_iter(l)


######################################################
## timer function
######################################################
def timer(func):
    """Print the runtime of the decorated function"""
    @wraps(func)
    def wrapper_timer(*args, **kwargs):
        start_time = time.perf_counter()
        value = func(*args, **kwargs)
        run_time = time.perf_counter() - start_time
        print(f"Finished {func.__name__!r} in {run_time:.4f} secs")
        return value
    return wrapper_timer


######################################################
## utility functions for Default-Ordered Dictionary
######################################################
class DefaultOrderedDict(OrderedDict):
  def __init__(self, default_factory=None, *a, **kw):
    if (default_factory is not None and
        not isinstance(default_factory, Callable)):
      raise TypeError('first argument must be callable')
    super(DefaultOrderedDict, self).__init__(*a, **kw)
    self.default_factory = default_factory

  def __getitem__(self, key):
    if key in self:
      return super(DefaultOrderedDict, self).__getitem__(key)
    else:
      return self.__missing__(key)

  def __missing__(self, key):
    if isinstance(self.default_factory, Callable):
      value = self.default_factory()
    else:
      value = self.default_factory
    self[key] = value
    return value

  def __repr__(self):
    return 'DefaultOrderedDict({}, {})'.format(self.default_factory, super(DefaultOrderedDict, self).__repr__())


######################################################
## utility functions for union-find structure
######################################################
class UnionFind(object):
  def __init__(self, size):
    self._array = [i for i in range(size)]
    self._weight = [1 for i in range(size)]

  def root(self, i):
    while (self._array[i] != i):
      i = self._array[i]
    return i

  def find(self, i, j):
    return self.root(i) == self.root(j)

  def union(self, i, j):
    root_i = self.root(i)
    root_j = self.root(j)
    if root_i == root_j:
      return
    if self._weight[root_i] < self._weight[root_j]:
      self._array[root_i]   = self._array[root_j]
      self._weight[root_j] += self._weight[root_i]
    else:
      self._array[root_j]   = self._array[root_i]
      self._weight[root_i] += self._weight[root_j]

  def components(self):
    comp = defaultdict(list)
    for i, parent in enumerate(self._array):
      root_i = self.root(parent)
      if i == root_i:
        comp[root_i]
      else:
        comp[root_i].append(i)
    return comp



######################################################
## utility functions to find Lowest Common Scope of nodes
######################################################
class TrieNode(object):
  def __init__(self, key=None):
    self.key = key
    self.children = {}

    # isEOL is True if node represent the end of the list
    self.isEOL = False

  def __contains__(self, child_key):
    return (child_key in self.children)
    # return any([child_key == child.key for child in self.children])

  def __getitem__(self, child_key):
    return self.children.get(child_key, None)
    # if child_key not in self:
    #   return None
    # return [child for child in self.children if child_key == child.key][0]

  def add(self, child_key):
    self.children[child_key] = TrieNode(child_key)
    # self.children.append(TrieNode(child_key))


class Trie(object):
  def __init__(self, name_list=[], name_sep='/'):
    self.name_sep = name_sep
    self.root = self.newNode()
    if name_list:
      for name in name_list:
        self.insert(name)

  def newNode(self, key=None):
    # Returns new trie node (initialized to NULLs)
    return TrieNode(key)

  def insert(self, name):
    # If not present, inserts name scopes into trie
    # If the scope is prefix of trie node,
    # just marks leaf node
    root = self.root
    for scope in name.split(self.name_sep):
      # if current character is not present
      if scope not in root:
        root.add(scope)
      root = root[scope]

    # mark last node as leaf
    root.isEOL = True

  def search(self, name):
    # Search name in the trie
    # Returns true if name presents
    # in trie, else false
    root = self.root
    for scope in name.split(self.name_sep):
      if scope not in root:
        return False
      root = root[scope]

    return (root != None and root.isEOL)

  def lcs(self):
    ## find lowest common scope
    root = self.root
    ret = []
    while len(root.children) == 1:
      ret.append(root.key)
      root = next(iter(root.children.values()))
    if not root.isEOL:
      ## if the lcs is not one of names in Trie
      ret.append(root.key)
    return self.name_sep.join(ret[1:])


######################################################
## utility functions to convert between dict and attr
######################################################
class dict2attr(dict):
    DEFAULT = None

    def __init__(self, mapping=None):
        if mapping is None:
          return
        elif isinstance(mapping, dict):
          items = mapping.items
        elif hasattr(mapping, '__dict__'):
          items = mapping.__dict__.items
        else:
          raise TypeError('expected dict')
        for key, value in items():
          self[key] = literal_eval(value)

    def __setitem__(self, key, value):
        if isinstance(value, dict) and not isinstance(value, dict2attr):
          value = dict2attr(value)
        super(dict2attr, self).__setitem__(key, value)

    def __getitem__(self, key):
        found = super(dict2attr, self).get(key, dict2attr.DEFAULT)
        #found = self.get(key, dict2attr.DEFAULT)
        # if found is dict2attr.DEFAULT:
        #   found = dict2attr()
        #   super(dict2attr, self).__setitem__(key, found)
        return found

    def get(self, key, default=None):
        val = self[key]
        return val if val is not dict2attr.DEFAULT else default

    def update(self, other):
        other = dict2attr(other)
        super(dict2attr, self).update(other)

    def copy(self):
        return dict2attr(self)

    __setattr__, __getattr__ = __setitem__, __getitem__



######################################################
## DAG utilities
######################################################
def topological_ordering(connectivity_map, subset_view=None, seed=None):
    #################
    ## input formats:
    ## connectivity_map[component_i][element_x in component_i] = [list of component_j]
    #################

    vertex_support = subset_view if subset_view else connectivity_map
    vertex_seed    = seed if seed else vertex_support

    def helper(vertex, ordering):
      if vertex not in vertex_support or visited[vertex] == 1:   ## processing vertex is done
        return
      elif visited[vertex] == 2: ## processing vertex is incomplete
        raise RuntimeError('graph is not an DAG')
      visited[vertex] = 2
      for __, v_list in connectivity_map[vertex].items():
        for v in v_list:
          helper(v, ordering)
      visited[vertex] = 1
      ordering.append(vertex)

    visited = {v: 0 for v in vertex_support}   ## 0 if not visited, 1 if done, 2 if inprocess
    ordering = []
    for v in vertex_seed:
      if not visited[v]:
        helper(v, ordering)
    return ordering

def topological_level(ordering, connectivity):
    ordering   = list(ordering)
    vertex_cnt = len(ordering)
    levels     = {v: vertex_cnt for v in ordering}
    vertex     = ordering.pop()
    levels[vertex] = 0
    while ordering:
      for __, v_list in connectivity[vertex].items():
        for v in v_list:
          levels[v] = min(levels[v], levels[vertex]+1)
      vertex = ordering.pop()

    level2vertex = defaultdict(list)
    for v, l in levels.items():
      level2vertex[l].append(v)

    return levels, level2vertex

def is_cyclic(nodes, adjacencyMap):
    cycles = []

    def util(node_name, visited_set, cur_path):
        if node_name in cur_path:
          temp = list(cur_path)
          idx  = temp.index(node_name)
          cycles.append(temp[idx:]+[node_name])
          return True

        cur_path.add(node_name)
        if node_name not in visited_set:
          visited_set.add(node_name)
          if node_name in adjacencyMap:
            for desc_name in adjacencyMap[node_name]:
              desc_name = desc_name.split(':')[0]
              if util(desc_name, visited_set, cur_path):
                return True

        cur_path.pop()
        return False

    visited_set = set()
    cur_path    = OrderedSet()
    for node_name in nodes:
      if util(node_name, visited_set, cur_path):
        return True, cycles[0]
    return False, cycles

def all_cycles(nodes, adjacencyMap):
    cycles = []

    def util(node_name, visited_set, cur_path):
        if node_name in cur_path:
          temp = list(cur_path)
          idx  = temp.index(node_name)
          cycles.append(temp[idx:]+[node_name])
          return

        cur_path.add(node_name)
        if node_name not in visited_set:
          visited_set.add(node_name)
          for desc_name in adjacencyMap[node_name]:
            desc_name = desc_name.split(':')[0]
            util(desc_name, visited_set, cur_path)

        cur_path.pop()
        return

    visited_set = set()
    cur_path    = OrderedSet()
    for node_name in nodes:
      util(node_name, visited_set, cur_path)

    return cycles
