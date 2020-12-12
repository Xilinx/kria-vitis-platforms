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
"""
This module defines the base class for all xsnodes: XstreamNode. For new nodes,
extend this class. It also defines the XstreamNodeArgs class. This object defines
what arguments the node expects and what it adds. It can be optionally defined
per node to document the arguments of the node.
"""

import logging
import signal
import six
import time
from types import MethodType

import numpy as np
import zmq

from vai.dpuv1.rt import xstream
from vai.dpuv1.rt.xsnodes import xstreamnode_mp

logger = logging.getLogger(__name__)

class XstreamNode(object):
    """
    The base class for xstreamNodes defines the structure and basic methods to
    ensure consistency in the structure of nodes so they can communicate with
    each other. For the simplest nodes, the child class only needs to define
    the loop() method. More advanced nodes may override other methods as needed.

    If you choose to adhere to the structure provided here, it goes something
    like this:
    1. __init__()
    2. initialize()
    3. run() varies based on the arguments to the node
        a) Get payload with get_payload().
        b) If timeouts are enabled, call timeout()
        c) call loop()
    4. loop()
    5. finish()
    """
    def __init__(self, name, inputs, outputs, args):
        """
        This method is called when a service containing this node starts. It
        initializes the node instance and calls the initialize() method at the
        end.

        Your node may optionally include more arguments but the four above are
        required.

        This method will not be needed in your nodes unless you need
        to handle additional input arguments beyond the ones listed above.
        Usually, you should use initialize() to initialize your node beyond
        what the base class provides.

        Args:
            name (str): Name of the node
            inputs (list): List of strs of input channels to sub to
            outputs (list): List of strs of output channels to pub to
            args (dict): Arguments for this node
        """
        signal.signal(signal.SIGINT, signal.SIG_IGN)
        signal.signal(signal.SIGTERM, signal.SIG_IGN)

        self._name = name
        self._args = args
        self._inputs = inputs[:]
        self._outputs = outputs[:]

        # share a common ZMQ context for this node. This is needed to allow
        # inter-thread zmq inproc communication through the create_<pub,sub>
        # methods.
        self.context = zmq.Context()

        # if heartbeats are present in the service, heartbeats override the
        # regular zmq timeouts when nodes call get(). To allow timeouts again,
        # this keeps track of time and calls a timeout if a real non-heartbeat
        # message doesn't arrive by the timeout value of the node.
        self._last_seen = time.time()

        # defines the timeout for ZMQ in milliseconds
        self._timeout = args['timeout'] if 'timeout' in args else -1

        # callback is used to indicate that the node will communicate back to
        # the client through a websocket, not necessarily through standard
        # REST
        self._callback = args['callback'] if 'callback' in args else False

        # used by some nodes to set the frequency of repeating some operation
        self._frequency = args['frequency'] if 'frequency' in args else -1

        self._processes_num = args['processes'] if 'processes' in args else 0

        # these constants can be used to control execution if some methods below
        # are overridden by inherting classes.
        self.BREAK = -1
        self.PASS = 0
        self.CONTINUE = 1

        # subscribe to all the input channels
        self._in_streams = []
        for _i, input_name in enumerate(self._inputs):
            self._in_streams.append(
                xstream.Subscribe(input_name, timeout=self._timeout)
            )

        self._out_stream = xstream.Publisher()

        # if multiple processes are requested, override the default run method
        # by binding the multiprocess run method instead
        if self._processes_num > 0:
            if six.PY3:
                self.run = MethodType(xstreamnode_mp.run, self)
                self.init_process = MethodType(xstreamnode_mp.init_process, self)
            else:
                self.run = MethodType(xstreamnode_mp.run, self, XstreamNode)
                self.init_process = MethodType(xstreamnode_mp.init_process, self, XstreamNode)
        elif self._processes_num == 0:
            pass

        self.initialize(args)

    def initialize(self, args):
        """
        By default, initializing a node automatically calls this method, which
        simply starts the main loop of node (where it waits for incoming data).
        On more complex nodes, this behavior may not be desired and so this method
        can be overridden. If the method is overridden, then make sure to call
        self.run() or call this base method as the last call. In normal cases,
        nodes only need to override this method to provide custom initialization
        (e.g. to add more attributes) for the node.

        Args:
            args (dict): Arguments for this node are available here if needed
        """
        self.run()

    # The following four methods, create_pub, create_sub, get_pub, get_sub
    # are needed to provide thread-local publishers and subscribers. The return
    # values from these methods are then passed as arguments to the get/put
    # function calls. If your node spawns threads/processes, then it should
    # call these methods and use their pub/subs for communication. ZMQ sockets
    # are not thread-safe and sharing sockets between threads can result in
    # corrupted data.
    #
    # Note, for now, create_sub and create_pub are not quite fully usable and
    # should be avoided. If you want to communicate between threads, use your
    # own queues for now.

    def create_pub(self, label):
        """
        Returns a new thread-local intra-process publisher for communication
        between threads.

        Args:
            label (str): A string identifier of where to publish. Subs should
                listen on the same label

        Returns:
            Publisher: An xstream Publisher object to use with get/put
        """
        return xstream.Publisher(
            use_context=self.context,
            transport='inproc',
            label=label
        )

    def create_sub(self, label):
        """
        Returns a new thread-local intra-process subscriber for communication
        between threads.

        Args:
            label (str): A string identifier of where to listen. Pubs should
                publish to the same label

        Returns:
            Subscriber: An xstream Subscriber object to use with get/put
        """
        return xstream.Subscribe(
            label,
            use_context=self.context,
            transport='inproc',
            label=label
        )

    def get_pub(self):
        """
        Returns a thread-local copy of the node's publisher. This allows a
        thread to publish to the next node safely.

        Returns:
            Publisher: An xstream Publisher object to use with get/put
        """
        return xstream.Publisher(use_context=self.context)

    def get_sub(self, idx):
        """
        Returns a thread-local copy of a node's subscriber using the index.
        This allows a thread to subscribe to the previous node safely.

        Returns:
            Subscriber: An xstream Subscriber object to use with get/put
        """
        return xstream.Subscribe(
            self._inputs[idx],
            timeout=self._timeout,
            use_context=self.context
        )

    def get(self, inIdx=0, sub=None, pub=None, suppress_hbeat=None):
        """
        Get data from one of the subscribed channels.

        If this method is called from the main node thread, the inIdx value
        defines which subscribed channel to get data from. If using inside a
        new thread, then the sub and pub arguments MUST be used. In this case,
        inIdx is used only to suppress heartbeats for non-zero indices.

        Args:
            inIdx (int, optional): The index corresponding to the channel to
                read from. Defaults to 0.
            sub (Subscriber, optional): An xstream Subscriber object to use
                instead of inIdx. Defaults to None.
            pub (Publisher, optional): An xstream Publisher object to use
                instead of inIdex. Defaults to None.
            suppress_hbeat (Boolean, optional): Disables writing heartbeats to
                the next node. Defaults to None.

        Returns:
            Tuple: The payload as a tuple for the node
        """
        if sub is None:
            sub = self._in_streams[inIdx]
        if pub is None:
            pub = self._out_stream

        # Multiple nodes may subscribe to the output of a single node, which
        # can create cycles in the graph. Normally, all nodes simply get and put
        # received heartbeats on to the next node, which doesn't work well if
        # there are cycles. Therefore, we must be able to suppress them. By
        # default, it uses the index to determine whether or not to push
        # heartbeats forward. You can also explicitly enable/disable this by
        # passing in a boolean value.
        if suppress_hbeat is None:
            suppress_hbeat = True if inIdx != 0 else False
        payload = None
        while True:
            payload = sub.get()
            if not payload:
                break

            (meta, _buf) = payload
            now = time.time()

            # if it's a heartbeat, push it forward and continue waiting for
            # non-heartbeat message.
            if meta is not None and '__heartbeat__' in meta:
                if not suppress_hbeat:
                    self.put(meta, pub=pub)
                timed_out = (self._last_seen > 0) and \
                    (now - self._last_seen > self._timeout / 1000)
                if self._timeout > 0 and timed_out:
                    self._last_seen = now
                    return (None, None) # force timeout if no valid message comes
            else:
                self._last_seen = now
                break
        return payload

    def get_payload(self):
        """
        This method defines how to get the incoming payload for the node.

        By default, a node will use get() to get a payload from the first indexed
        xstream. This method will usually not need to be overridden.

        Returns:
            Tuple: Payload for the node
        """
        return self.get()

    def get_id(self, inIdx=0, sub=None, pub=None, suppress_hbeat=None):
        """
        In contrast to get(), which returns the payload, this method returns
        the obj_id of an incoming message. Then, with the ID, the object itself
        can be retrieved later by the node.

        If this method is called from the main node thread, the inIdx value
        defines which subscribed channel to get data from. If using inside a
        new thread, then the sub and pub arguments MUST be used. In this case,
        inIdx is used only to suppress heartbeats for non-zero indices.

        Args:
            inIdx (int, optional): The index corresponding to the channel to
                read from. Defaults to 0.
            sub (Subscriber, optional): An xstream Subscriber object to use
                instead of inIdx. Defaults to None.
            pub (Publisher, optional): An xstream Publisher object to use
                instead of inIdex. Defaults to None.
            suppress_hbeat (Boolean, optional): Disables writing heartbeats to
                the next node. Defaults to None.

        Returns:
            str: The ID of payload that can be retrieved with an Xstream Base,
                Publisher or Subscriber object using the obj_get function.
        """
        if sub is None:
            sub = self._in_streams[inIdx]
        if pub is None:
            pub = self._out_stream
        if suppress_hbeat is None:
            suppress_hbeat = True if inIdx != 0 else False
        obj_id = None
        while True:
            obj_id = sub.get_id()
            if 'hbeat' in str(obj_id): # heartbeat message
                if not suppress_hbeat:
                    (meta, _buf) = sub.obj_get(obj_id)
                    self.put(meta, pub=pub)
            else:
                break
        return obj_id

    def get_obj(self, obj_id, sub=None):
        """
        Gets the object using the ID. Use after get_id() returns the object ID

        Args:
            obj_id (str): ID of object to get
            sub (Subscriber/Publisher, optional): Sub/Pub to use. Defaults to None.

        Returns:
            Tuple: Payload from the object_id
        """
        if sub is None:
            sub = self._in_streams[0]

        return sub.obj_get(obj_id)

    def run(self):
        """
        This function executes the main loop of an xsnode. In an infinite loop,
        until termination, the node will wait for payload, and then take action
        on the received payload.

        self.timeout() can be used to take action if the node times out.
        self.loop() MUST be defined by the child class and it defines the work
            that will be done.

        The base class provides this standard structure for nodes to use. More
        complex nodes may choose to override this method. Usually, use the
        loop() method, combined with the methods below that are called in the
        base run(), to specify the work to be done.
        """
        while True:
            try:
                payload = self.get_payload()
                if payload is None:
                    break
                elif payload == (None, None):
                    retval = self.timeout()
                    if retval == self.BREAK:
                        break
                    elif retval == self.CONTINUE:
                        continue
                self.loop(payload)
            except Exception:
                logger.exception("Node run error")
        self.finish()

    def timeout(self):
        """
        This method gets called when the node receives a message indicating
        timeout of the get() function.

        By default, timeouts are ignored. If timeouts should result in termination,
        return self.BREAK. To return to get_payload(), return self.CONTINUE.
        To ignore and pass the message to loop(), return self.PASS

        Returns:
            int: Constant (one of PASS, CONTINUE, BREAK) to say what should
                occur after this method returns.
        """
        return self.PASS

    # TODO for some reason, the multiprocess loop needs a publisher passed in...?
    # Inside the new process, I do a get_pub() and then pass that in. It should
    # be the same as not doing at all since the class does the same thing
    # but for now this is here to be investigated
    def loop(self, payload, pub=None):
        """
        This method is the only required one that the node must implement. It
        describes what work the node must perform. Of course, if run() is
        overridden, then this method may not be called and is then not needed.

        The payload should be interpreted as the node may expect it. Usually,
        it will be (meta, buf) where buf is a buffered array, and meta is a dict.

        At the end, the buf and payload may be deleted and the node should
        write to the next node

        Args:
            payload (tuple): Incoming payload
        """
        raise NotImplementedError

    def finish(self):
        """
        This method runs after the node receives a terminate signal. Perform
        cleanup here.

        If overriden, your method should call self.end(0) to propagate the
        termination condition to downstream nodes.
        """
        logger.status("%s is ending" % self._name)

        # forward the termination signal to downstream nodes
        self.end()

    def put(self, meta, nparr=np.zeros(1), outIdx=0, pub=None):
        """
        Publish a dictionary and buffer to a channel. If this function is called
        from a separate thread/process, a thread-local publisher MUST be passed
        in.

        Args:
            meta (dict): Contains values for the next node to use
            nparr (Buffer, optional): A buffer type (probably a numpy array).
                Defaults to np.zeros(1).
            outIdx (int, optional): Index of output channel to use (usually,
                there should be just one). Defaults to 0.
            pub (Publisher, optional): An xstream Publisher object. Defaults
                to None.
        """
        if pub is None:
            pub = self._out_stream
        try:
            out_name = self._outputs[outIdx]
            pub.put_blob(out_name, nparr, meta, self._callback)
        except Exception:
            logger.exception("put error %s %s" % (self._outputs[outIdx], meta['id']))

    def end(self, outIdx=0, pub=None):
        """
        Forwards the termination signal to downstream nodes. If this function
        is called from a separate thread/process, a thread-local publisher MUST
        be passed in.

        Args:
            outIdx (int, optional): Index of output channels to use. Defaults to 0.
            pub (Publisher, optional): An xstream Publisher object. Defaults to None.
        """
        if pub is None:
            pub = self._out_stream
        out_name = self._outputs[outIdx]
        pub.end(out_name)

class XstreamNodeArgs(object):
    def __init__(self):
        self._start_args = {
            'timeout': "(int, optional): The node's get will timeout after x " +
                "ms without received messages. Defaults to -1 (no timeout).",
            'callback': "(bool, optional): Setting callback to True, and " +
                "providing a callback_id, sends the meta over the __server__ " +
                "channel to be sent over a websocket to the callback_id from " +
                "the current node. Defaults to False.",
            'frequency': "(int, optional): Some nodes continue to perform work " +
                "after the initial request. The frequency determines the rate. " +
                "If used, the node should override this message ",
            'processes': "(int, optional): Spawn n processes to perform the " +
                "work in loop(). If 0, spawn no additional processes. In " +
                "addition, if run() is overridden, this value must be 0. " +
                "Defaults to 0."
        }

        self._serve_args = {
            'input': {
                'BUFFER': "This value should be overridden in the node to say " +
                    "whether this node uses the contents of the buffer",
                'callback_id': "(str, situational): if callback is True, use this " +
                    "id for the websocket to send the meta data to when this node " +
                    "writes publishes"
            },
            'output': {
                'BUFFER': "This value should be overridden in the node to say" +
                    "whether this node puts any contents into the buffer",
                'status': "This value should be overridden to indicate what " +
                    "different status values mean and to indicate error states " +
                    "to downstream nodes"
            }
        }

    def add_start_arg(self, key, desc=None):
        """
        Adds an argument + description for a start-time argument

        Args:
            key (str): Name of the argument
            desc (str, optional): Description of argument. Defaults to None.
        """
        self._start_args[key] = desc

    def add_input_serve_arg(self, key, desc=None):
        """
        Adds an argument + description for a serve-time argument

        Args:
            key (str): Name of the argument
            desc (str, optional): Description of argument. Defaults to None.
        """
        self._serve_args['input'][key] = desc

    def add_output_serve_arg(self, key, desc=None):
        """
        Adds an argument + description for something this node adds for downstream
        nodes

        Args:
            key (str): Name of the argument
            desc (str, optional): Description of argument. Defaults to None.
        """
        self._serve_args['output'][key] = desc

    def to_dict(self):
        """
        Returns the dictionary representation of the object for returning over
        REST

        Returns:
            dict: Arguments and descriptions for the node
        """
        args = {}
        args['start_args'] = self._start_args
        args['serve_args'] = self._serve_args
        return args
