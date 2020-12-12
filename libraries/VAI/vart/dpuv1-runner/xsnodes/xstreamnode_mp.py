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
import logging
import multiprocessing as mp
import os
import zmq

logger = logging.getLogger(__name__)

def run(self):
    self._processes = []
    handle = get_ipc_handle(self._name)

    for i in range(self._processes_num):
        p = mp.Process(
            target=self.init_process,
            args=(handle,)
        )
        self._processes.append(p)
        p.start()

    sender = get_socket(self.context, handle)
    while True:
        try:
            payload = self.get_id()
            if payload is None:
                break
            sender.send(payload)
        except Exception:
            logger.exception("Node run_mp error")
    os.remove(handle)
    for p in self._processes:
        p.terminate()
        p.join()
    self.finish()

def init_process(self, handle):
    pub = self.get_pub()
    context = self.context
    receiver = context.socket(zmq.PULL)
    receiver.connect("ipc://%s" % handle)
    while True:
        try:
            obj_id = receiver.recv()
            # not passing in a process-local sub/pub results in a plasma error
            payload = self.get_obj(obj_id, pub)
            self.loop(payload, pub)
        except Exception:
            logger.exception("mp init process")

def get_ipc_handle(name):
    ipc_directory = '/tmp/xsnodes/ipc_handles'
    if not os.path.exists(ipc_directory):
        os.makedirs(ipc_directory)

    ipc_handle_format = ipc_directory + "/%s_" % name
    ipc_handle_format += "%s"
    while True:
        next_handle = next_path(ipc_handle_format)
        try:
            # atomic file existence check and creation
            # https://stackoverflow.com/a/33223732
            os.open(next_handle,  os.O_CREAT | os.O_EXCL)
            break
        except FileExistsError:
            continue

    return next_handle

def next_path(path_pattern):
    """
    Finds the next free path in an sequentially named list of files

    e.g. path_pattern = 'file-%s.txt':

    file-1.txt
    file-2.txt
    file-3.txt

    Runs in log(n) time where n is the number of existing files in sequence
    https://stackoverflow.com/a/47087513
    """
    i = 1

    # First do an exponential search
    while os.path.exists(path_pattern % i):
        i = i * 2

    # Result lies somewhere in the interval (i/2..i]
    # We call this interval (a..b] and narrow it down until a + 1 = b
    a, b = (i // 2, i)
    while a + 1 < b:
        c = (a + b) // 2 # interval midpoint
        a, b = (c, b) if os.path.exists(path_pattern % c) else (a, c)

    return path_pattern % b

def get_socket(context, port):
    sender = context.socket(zmq.PUSH)
    sender.bind("ipc://%s" % port)
    return sender
