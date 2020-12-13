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
import multiprocessing as mp
import os
import signal
import time
from vai.dpuv1.rt import xstream

class SignalWatcher(object):
    kill_now = False
    def __init__(self):
        signal.signal(signal.SIGINT, self.exit_gracefully)
        signal.signal(signal.SIGTERM, self.exit_gracefully)

    def exit_gracefully(self, sig, iframe):
        self.kill_now = True

class GraphNode(object):
    def __init__(self, name, xsnode, args=None, friendly_name=None):
        self._name = name
        self._xsnode = xsnode
        self._args = args
        self.friendly_name = friendly_name
        self._inputs = []
        self._outputs = []
        self._proc = None

    def add_input(self, name):
        if name not in self._inputs:
            self._inputs.append(name)

    def add_output(self, name):
        if name not in self._outputs:
            self._outputs.append(name)

    def start(self, args={}):
        # override default args with runtime args if available
        if 'add' in args:
            for key, value in args['add'].items():
                self._args[key] = value
        if 'remove' in args:
            for key, _value in args['remove'].items():
                if key in self._args:
                    del self._args[key]

        # TODO add support for Docker
        self._proc = mp.Process(
            target=self._xsnode,
            args=(self._name, self._inputs, self._outputs, self._args,)
        )
        self._proc.start()
        os.setpgid(self._proc.pid, self._proc.pid)

    def kill(self):
        if self._proc is not None:
            os.killpg(os.getpgid(self._proc.pid), signal.SIGKILL)
        self.join()

    def join(self):
        # with misbehaved nodes, things may close weirdly. Check here to make
        # sure it's not actually joined yet before joining
        if self._proc is not None:
            self._proc.join()
            self._proc = None

class Graph(object):
    def __init__(self, prefix):
        self._prefix = prefix
        self._nodes = {}
        self._in = []
        self._out = []

    def node(self, name, xsnode, args, friendly_name=None):
        self._nodes[name] = GraphNode(name, xsnode, args, friendly_name)

    def edge(self, edge_name, driver_name, load_name):
        assert (driver_name is not None) or (load_name is not None)
        assert (driver_name is None) or (driver_name in self._nodes)
        assert (load_name is None) or (load_name in self._nodes)

        edge_name = "%s.%s" % (self._prefix, edge_name)

        if driver_name:
            self._nodes[driver_name].add_output(edge_name)
        elif edge_name not in self._in:
            self._in.append(edge_name)

        if load_name:
            self._nodes[load_name].add_input(edge_name)
        elif edge_name not in self._out:
            self._out.append(edge_name)

    def serve(self, args={}, background=False):
        # these arrays should be non-empty
        assert self._in
        assert self._out

        for _name, node in self._nodes.items():
            if node.friendly_name in args:
                node.start(args[node.friendly_name])
            else:
                node.start()

        if not background:
            # run in 'foreground' and block until CTRL-C
            watcher = SignalWatcher()
            while not watcher.kill_now:
                time.sleep(2)
            self.stop()

    def stop(self, kill=True):
        xs = xstream.Publisher()
        for i in self._in:
            xs.end(i)

        # let nodes shutdown gracefully if possible
        time.sleep(5)

        for _name, node in self._nodes.items():
            if kill:
                # forceful exit in case nodes are not well-behaved
                # warning, this can leave behind plasma objects
                # (tee hee we're not using plasma anymore!)
                node.kill()
            else:
                # wait and trust that all nodes politely exit
                node.join()
