#!/usr/bin/python
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
import getpass
import json
import numpy as np
from multiprocessing import Process
import os, subprocess
import signal
import time
import zmq
import traceback
import inspect
import threading
import logging

logger = logging.getLogger(__name__)

g_useRedis = False
if g_useRedis:
  import redis
  g_defaultObjstoreUri = "127.0.0.1"
  g_defaultObjstorePort = int(os.getenv('XSTREAM_REDIS_PORT', 6379))
else:
  import pyarrow.plasma as plasma
  g_defaultObjstoreUri = "/tmp/plasma-xlnx-vitis-%s" % getpass.getuser()
g_defaultComHost = '127.0.0.1'
g_defaultComPort = int(os.getenv('XSTREAM_PORT', 6661)) # this is pub port; sub port is pub port + 1

class Proxy(object):
  def __init__(self, in_port=g_defaultComPort, out_port=(g_defaultComPort+1)):
    self.zcontext = zmq.Context()

    # Socket facing producers
    self.zproducers = self.zcontext.socket(zmq.XSUB)
    try:
      self.zproducers.bind("tcp://*:%d" % in_port)
    except zmq.ZMQError as e:
      logger.error(e, exc_info=1)
      exit(0)

    # Socket facing consumers
    self.zconsumers = self.zcontext.socket(zmq.XPUB)
    self.zconsumers.bind("tcp://*:%d" % out_port)

    zmq.proxy(self.zproducers, self.zconsumers)

def _run_proxy(in_port, out_port):
  signal.signal(signal.SIGINT, signal.SIG_IGN)
  signal.signal(signal.SIGTERM, signal.SIG_IGN)
  Proxy(in_port, out_port)

class Server(object):
  def __init__(self, objStorePath=g_defaultObjstoreUri, in_port=g_defaultComPort,
    start_objstore=True, objstore_size=1000000000, start_proxy=True):
    self.proxy = None
    self.objstore = None

    try:
      getattr(logger, 'status')
    except AttributeError:
      logger.warning("Multiprocess logging isn't configured! This may result in errors!")

    if os.path.exists(objStorePath):
      os.remove(objStorePath)

    if start_proxy:
      self.proxy = Process(target=_run_proxy, args=(in_port, in_port+1))
      self.proxy.start()
      os.setpgid(self.proxy.pid, self.proxy.pid)

    if start_objstore:
      if g_useRedis:
        cmd = "redis-server"
        conf = os.path.dirname(os.path.abspath(__file__)) + "/redis.conf"
        if os.path.exists(conf):
          cmd += " %s" % conf
        cmd += " --port %d" % g_defaultObjstorePort
        self.objstore = subprocess.Popen(cmd, shell=True, preexec_fn=os.setsid)
      else:
        subprocess.call(['pkill', '-9', '-f', objStorePath])
        null = open(os.devnull, "w")
        self.objstore = subprocess.Popen(
          "plasma_store -m %d -s %s" % (objstore_size, objStorePath),
          stdout=null, stderr=null,
          shell=True, preexec_fn=os.setsid)

    # start dummy publisher to block until system is ready
    # having this publisher here, results in some kind of deadlock in Py3 for
    # some reason. For now, it's commented out
    # Publisher()

  def __del__(self):
    time.sleep(1) # give time for transmissions to finish

    try:
      if self.proxy:
        os.killpg(os.getpgid(self.proxy.pid), signal.SIGKILL)
    except:
      pass

    try:
      if self.objstore:
        os.killpg(os.getpgid(self.objstore.pid), signal.SIGKILL)
    except:
      pass

class Base(object):
  def __init__(self, path=g_defaultObjstoreUri):
    self.obj_map = {} # id -> buf

    if g_useRedis:
      self.pclient = redis.Redis(path, port=g_defaultObjstorePort, db=0)
      while True:
        try:
          self.pclient.ping()
          break
        except (redis.exceptions.ConnectionError,
                redis.exceptions.BusyLoadingError):
          logger.info("Redis server not available, trying again...")
          time.sleep(2)
    else:
      self.pclient = plasma.connect(path)

  def obj_new(self, size, obj_id=None, meta={}):
    if isinstance(obj_id, (str, int)):
      if isinstance(obj_id, int):
        obj_id = str(obj_id)
      byteStr = obj_id.encode('utf-8')
      extendAmount = 20 - len(byteStr)
      if extendAmount > 0:
        byteStr = b'_' * extendAmount + byteStr
      obj_id = byteStr
    else:
      #obj_id = np.random.bytes(20)
      raise TypeError

    if g_useRedis:
      buf = np.empty((size), dtype=np.uint8, order='C')
      self.obj_map[obj_id] = buf
      self.obj_map[str(obj_id) + "__meta__"] = json.dumps(meta).encode()
      return (self.obj_map[obj_id].data, obj_id)
    else:
      pid = plasma.ObjectID(obj_id)
      if self.pclient.contains(pid):
        self.pclient.delete([pid])
      pobj = self.pclient.create(pid, size, json.dumps(meta).encode())
    return pobj, obj_id

  def obj_delete(self, obj_id):
    if g_useRedis:
      self.pclient.delete(obj_id)
    else:
      pid = plasma.ObjectID(obj_id)
      self.pclient.delete([pid])

  def obj_get(self, obj_id):
    if g_useRedis:
      buf = self.pclient.get(obj_id)
      meta_str = self.pclient.get(str(obj_id) + "__meta__")
      meta = json.loads(meta_str.decode())
    else:
      pid = plasma.ObjectID(obj_id)
      buf = self.pclient.get_buffers([pid])[0]
      metaBuf = self.pclient.get_metadata([pid])[0]
      meta = json.loads(np.frombuffer(metaBuf, dtype="uint8").tostring())
      del metaBuf
    #meta['_id'] = obj_id
    return (meta, buf)

  def obj_seal(self, obj_id):
    if g_useRedis:
      resp1 = self.pclient.set(obj_id, self.obj_map[obj_id].tobytes())
      resp2 = self.pclient.set(str(obj_id) + "__meta__", self.obj_map[str(obj_id) + "__meta__"])
      del self.obj_map[obj_id]
      del self.obj_map[str(obj_id) + "__meta__"]
    else:
      pid = plasma.ObjectID(obj_id)
      self.pclient.seal(pid)

  def obj_list(self, obj_id):
    try:
      pid = plasma.ObjectID(obj_id)
      return self.pclient.list()[pid]
    except:
      return None

  def list(self):
    return self.pclient.list()

class Publisher(Base):
  def __init__(self,
    path=g_defaultObjstoreUri,
    host=g_defaultComHost,
    port=g_defaultComPort,
    use_context = None,
    transport = 'tcp',
    label = 'thread'
  ):
    super(Publisher, self).__init__(path)

    if use_context:
      self.zcontext = use_context
    else:
      self.zcontext = zmq.Context()
    self.zpub = self.zcontext.socket(zmq.PUB)
    self.thread_id = threading.current_thread().ident
    self.pid = os.getpid()
    if transport == 'tcp':
      self.zpub.connect("tcp://%s:%d" % (host, port))
    elif transport == 'inproc':
      self.zpub.bind("inproc://%s" % label)
    self.stats = Statistics()

    time.sleep(0.5) # give subscribers time to connect before transmitting

  def new(self, size, meta={}, obj_id=None):
    return self.obj_new(size, meta=meta, obj_id=obj_id)

  def put_blob(self, channel, nparr, meta, callback=False):
    """
    [summary]

    Args:
        channel (str): Output edge name
        nparr ([type]): [description]
        meta ([type]): [description]
    """
    if not isinstance(meta, dict):
      meta = json.loads(meta)
    assert 'id' in meta

    nbytes = len(nparr.tobytes())
    idStr = str("%s_%s" % (channel, meta['id']))
    (buf, objId) = self.new(nbytes, obj_id=idStr, meta=meta)
    npview = np.frombuffer(buf, dtype=nparr.dtype).reshape(nparr.shape)
    np.copyto(npview, nparr)
    self.put(channel, objId)

    # user/service requested a callback
    # forward the 'meta' to the secret server channel
    # so server can relay it back to user
    if callback and 'callback_id' in meta:
      msg = {
        'callback_id': meta['callback_id'],
        'topic': 'callback',
        'message': json.dumps(meta)
      }
      self.send('__server__', json.dumps(msg))

    if '__heartbeat__' in meta:
      msg = {
        'channel': channel,
        'beat': meta['id'],
        'service': meta['__heartbeat__']
      }
      self.send('__heartbeat__', json.dumps(msg))
      return

    self.stats.record_output(channel, self)

  def put(self, channel, obj_id):
    self.obj_seal(obj_id)
    # print("Putting to channel %s in xstream with id %s" % (channel, str(obj_id)))
    self.send(channel, obj_id)

  def send(self, channel, msg):
    # if threading.current_thread().ident != self.thread_id:
    #   logger.error("Current thread %s doesn't match socket thread %s" %
    #     (threading.current_thread().ident, self.thread_id))
    #   frame = inspect.currentframe()
    #   stack_trace = traceback.format_stack(frame)
    #   logger.error(''.join(stack_trace))
    # if os.getpid() != self.pid:
    #   logger.error("Current PID %s doesn't match socket PID %s" %
    #     (os.getpid(), self.pid))
    #   frame = inspect.currentframe()
    #   stack_trace = traceback.format_stack(frame)
    #   logger.error(''.join(stack_trace))
    if isinstance(msg, str):
      msg = msg.encode()
    self.zpub.send_multipart([channel.encode(), msg])

  def end(self, channel):
    self.send(channel, b'')

def getMsgListStr(msg):
  s = ""
  i = 0
  for m in msg:
    try:
      s += "%d:%s " % (i, str(m))
    except:
      s += "%d:? " % (i)

    i += 1

  return s

class Subscribe(Base):
  def __init__(self,
    channel,
    path=g_defaultObjstoreUri,
    host=g_defaultComHost,
    port=g_defaultComPort+1,
    timeout=-1,
    use_context=None,
    transport = 'tcp',
    label = 'thread'
  ):
    super(Subscribe, self).__init__(path)

    if use_context:
      self.zcontext = use_context
    else:
      self.zcontext = zmq.Context()
    self.zsub = self.zcontext.socket(zmq.SUB)
    if transport == 'tcp':
      self.zsub.connect("tcp://%s:%d" % (host, port))
    elif transport == 'inproc':
      self.zsub.connect("inproc://%s" % label)
    self.zsub.setsockopt(zmq.SUBSCRIBE, channel.encode())
    self.zsub.setsockopt(zmq.LINGER, 0)
    self.thread_id = threading.current_thread().ident
    self.pid = os.getpid()
    if timeout > 0:
      self.zsub.setsockopt(zmq.RCVTIMEO, timeout)
    self.timeout = timeout

  def get(self, id=None):
    obj_id = None
    channel = None
    # if threading.current_thread().ident != self.thread_id:
    #   logger.error("Current thread %s doesn't match socket thread %s" % (threading.current_thread().ident, self.thread_id))
    #   frame = inspect.currentframe()
    #   stack_trace = traceback.format_stack(frame)
    #   logger.error(''.join(stack_trace))
    # if os.getpid() != self.pid:
    #   logger.error("Current PID %s doesn't match socket PID %s" % (os.getpid(), self.pid))
    #   frame = inspect.currentframe()
    #   stack_trace = traceback.format_stack(frame)
    #   logger.error(''.join(stack_trace))

    # repeat this loop until a valid message with matching ID is received
    while True:
      # repeat this loop until a valid message is received
      while True:
        retval = None
        try:
          retval = self.zsub.recv_multipart()
          (channel, obj_id) = retval
          break
        except zmq.Again as e:
          if self.timeout > 0:
            return (None, None)
        except TimeoutError:
          raise
        except:
          logger.info(retval)
          logger.exception('xstream get error')
      if obj_id == b'':
        #print("Getting nothing in channel %s in xstream with id %s" % (channel, str(obj_id)))
        return None
      elif obj_id is None:
        continue

      (meta, buf) = self.obj_get(obj_id)

      if id != None and 'id' in meta and meta['id'] != id:
        # user is waiting for a specific id and this is not it
        continue
      else:
        break

    return (meta, buf)

  def get_msg(self):
    # if threading.current_thread().ident != self.thread_id:
    #   logger.error("Current thread %s doesn't match socket thread %s" % (threading.current_thread().ident, self.thread_id))
    #   frame = inspect.currentframe()
    #   stack_trace = traceback.format_stack(frame)
    #   logger.error(''.join(stack_trace))
    # if os.getpid() != self.pid:
    #   logger.error("Current PID %s doesn't match socket PID %s" % (os.getpid(), self.pid))
    #   frame = inspect.currentframe()
    #   stack_trace = traceback.format_stack(frame)
    #   logger.error(''.join(stack_trace))
    while True:
      retval = None
      try:
        retval = self.zsub.recv_multipart()
        (channel, obj_id) = retval
        break
      except zmq.Again as e:
        if self.timeout > 0:
          return (None, None)
      except TimeoutError:
        raise
      except Exception as e:
        logger.info(retval)
        logger.exception('xstream get error')
    if obj_id == b'':
      return None

    return obj_id

  def get_id(self):
    return self.get_msg()

class Runner(object):
  def __init__(self, timeout=-1, path=g_defaultObjstoreUri):
    self.path = path
    self.timeout = timeout
    self.publisher = {}
    self.subscribe = {}

  def _get_pub(self):
    thread_id = threading.current_thread().ident
    if thread_id not in self.publisher:
      self.publisher[thread_id] = Publisher(path=self.path)
    return self.publisher[thread_id]

  def _get_sub(self, chan):
    thread_id = threading.current_thread().ident
    # initially, each run() would instantiate its own Subscribe. However,
    # there was an issue with all these subscribers trying to access Plasma
    # where you get an error about a bad file descriptor
    key = "%d:%s" % (thread_id, chan)
    if key not in self.subscribe:
      self.subscribe[key] = Subscribe(chan, timeout=self.timeout, path=self.path)
    return self.subscribe[key]

  def run(self, startchan, endchan, nparr, meta):
    #print("Endchan is %s" % endchan)
    #print("entering runner.run with id: %s" % meta['id'])
    xsResult = self._get_sub(endchan)
    xsPub = self._get_pub()
    xsPub.put_blob(startchan, nparr, meta)
    #print("put blob in runner with id: %s" % meta['id'])
    payload = None
    #print("Runner meta id is %s" % str(meta['id']))
    # ignore nodes propagating termination signals from previous runs
    while payload is None:
      payload = xsResult.get(meta['id'])
    #print("got payload in runner with id: %s" % meta['id'])
    (meta, buf) = payload
    if '_id' in meta:
      del meta['_id']
    payload = (meta, buf)
    return payload

  def send(self, startchan, nparr, meta):
    xsPub = self._get_pub()
    xsPub.put_blob(startchan, nparr, meta)

class Statistics(object):
  class __Impl(object):
    """ Implementation of the singleton interface """
    def __init__(self):
      self._output_stats = {}
      self._last_update = time.time()

    def record_output(self, outname, pub):
      if outname not in self._output_stats:
        self._output_stats[outname] = []

      # only store most recent entries
      self._output_stats[outname].append(time.time())
      if len(self._output_stats[outname]) > 100:
        self._output_stats[outname].pop(0)

      self.report_throughput(pub)

    def report_throughput(self, pub):
      now = time.time()
      if now - self._last_update < 10:
        return
      self._last_update = now

      if len(self._output_stats) == 0:
        return

      report = {}
      for outname, stat in self._output_stats.items():
        n = float(len(stat))
        if n < 2:
          report[outname] = 0
          continue

        t = stat[-1] - stat[0]
        report[outname] = n / t

      msg = {
        'topic': 'xs_throughput',
        'message': json.dumps(report)
      }
      pub.send('__server__', json.dumps(msg))

  # storage for the instance reference
  __instance = None

  def __init__(self):
      """ Create singleton instance """
      # Check whether we already have an instance
      if Statistics.__instance is None:
          # Create and remember instance
          Statistics.__instance = Statistics.__Impl()

      # Store instance reference as the only member in the handle
      self.__dict__['_Statistics__instance'] = Statistics.__instance

  def __getattr__(self, attr):
      """ Delegate access to implementation """
      return getattr(self.__instance, attr)

  def __setattr__(self, attr, value):
      """ Delegate access to implementation """
      return setattr(self.__instance, attr, value)

  def _drop(self):
    "Drop the instance (for testing purposes)."
    Statistics.__instance = None
    del self._Statistics__instance
