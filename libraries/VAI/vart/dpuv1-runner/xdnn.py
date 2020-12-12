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
from six import itervalues,iteritems

from ctypes import *
import json
import os, sys
import timeit
import numpy as np
from multiprocessing.managers import BaseManager
import time
import h5py

#Parsing JSON directly is easier than passing all the necessary params from C++ to python
#Pybind11 will make passing data between C++/python easier and will remove the need for this class
class CompilerJsonParser:
  def __init__(self, compilerJSONFile):
    self._jsonObj = None
    inName = {}
    outName = {}

    self._inputs = {}
    self._outputs = {}
    with open(compilerJSONFile) as f:
      self._jsonObj = json.load(f)

      for i in self._jsonObj["inputs"]:
        inName[i["input_name"]] = i["input_name"]

      for i in self._jsonObj["outputs"]:
        outName[ i["output_name"] ] = i["previous_tensors"][0]
        #print ( i["previous_tensors"][0] , " -> ", i["output_name"] )

      for i in self._jsonObj["network"]:
        if i["name"] in inName.keys():
          self._inputs[ inName[ i["name"] ] ] = i["outputshapes"]

        elif i["name"] in outName.keys():
          self._outputs[ outName[ i["name"] ] ] = i["outputshapes"]
          #self._outputs[ i["name"] ] = i["outputshapes"]
        # mergedItem
     #   if "merged" in i:
     #     for mergedItem in i["merged"]:
            #print ( "mergeditem ", mergedItem , " -> ", i["name"] )
            #self._hwoutname[mergedItem] = i["name"]


  def getInputs(self):
    return self._inputs

  def getOutputs(self):
    return self._outputs

class XDNNFPGAOp:
  def __init__ (self, args, handleList = [0], AcquireID = 0):
    libFile = os.environ["LIBXDNN_PATH"]
    if not libFile or not os.path.isfile(libFile):
      raise AssertionError("XDNN library .so file %s not found" % libFile)

    self._libFile = os.path.abspath(libFile)
    self._lib = cdll.LoadLibrary(self._libFile)

    PE = -1
    if "PE" in args and args["PE"] >= 0:
      # ask butler to auto-acquire one CU
      AcquireID = -1
      PE = 0 # since butler returns one CU, PE idx = 0 always

    self._mpid, self._handles = self.createHandle(args["xclbin"], "kernelSxdnn_0", handleList, AcquireID)
    self._prev_time = 0.0

    self._lib.XDNNMakeScriptExecutorAndLoadWeights.argtypes \
      = [POINTER(c_void_p), c_int,
         c_char_p, c_char_p, c_char_p,
         c_int]
    self._lib.XDNNMakeScriptExecutorAndLoadWeightsFromMem.argtypes \
      = [POINTER(c_void_p), c_int, c_int, POINTER(c_char_p),
         POINTER(POINTER(c_float)), POINTER(c_int),
         POINTER(POINTER(c_float)), POINTER(c_int),
         c_char_p, c_char_p, c_int]
    self._lib.XDNNMakeScriptExecutorAndLoadWeights.restype = c_void_p
    self._lib.XDNNMakeScriptExecutorAndLoadWeightsFromMem.restype = c_void_p

    self._lib.XDNNExecute_2D_float.argtypes = [c_void_p,
                                      POINTER(POINTER( np.ctypeslib.ndpointer(c_float, flags="C_CONTIGUOUS") )),
                                      POINTER(c_char_p),
                                      c_uint,
                                      POINTER( np.ctypeslib.ndpointer(c_float, flags="C_CONTIGUOUS")),
                                      POINTER(c_uint),
                                      POINTER(c_char_p),
                                      c_uint,
                                      c_uint,
                                      c_int,
                                      c_bool]
    self._lib.XDNNWaitForResults.argtypes = [c_void_p, c_int]

    self._lib.XDNNReadHardwareCounter.argtypes = [c_void_p, c_int, c_int]
    self._lib.XDNNReadHardwareCounter.restype = c_float
    self._lib.getNumCUs.argtypes = [c_void_p]
    self._lib.getNumCUs.restype = c_int

    self._lib.xblasCreate.argtypes = [ c_void_p, c_char_p, c_char_p, c_int, c_int, c_char_p, c_char_p]
    self._lib.xblasCreate.restype = c_int

    self._args = args

    numHandles = len(self._handles)
    handlePtrs = (c_void_p*numHandles)()
    for i,h in enumerate(self._handles):
      handlePtrs[i] = h

    if '_layerParams' not in args:
      # load from disk
      self._executor = self._lib.XDNNMakeScriptExecutorAndLoadWeights(\
        handlePtrs, numHandles,
        c_char_p(args['weights'].encode('ascii')),
        args['netcfg'].encode('utf-8'),
        args['quantizecfg'].encode('utf-8'),
        self.getMask(PE))
    else:
      # load directly from mem
      layerParams = self._args['_layerParams']

      weightLayerIndices = []
      for i, lp in enumerate(layerParams):
        if lp["weights"]:
          lp["weights_sz"] = len(lp["weights"])
          lp["weights"] = np.ascontiguousarray(lp["weights"], dtype=np.float32).flatten()
          lp["bias_sz"] = len(lp["bias"])
          lp["bias"] = np.ascontiguousarray(lp["bias"], dtype=np.float32).flatten()
          weightLayerIndices.append(i)

      numWeightLayers = len(weightLayerIndices)
      weightLayerNames = (c_char_p * numWeightLayers)()
      weights = (POINTER(c_float) * numWeightLayers)()
      bias = (POINTER(c_float) * numWeightLayers)()
      weightsSz = (c_int * numWeightLayers)()
      biasSz = (c_int * numWeightLayers)()

      for i, idx in enumerate(weightLayerIndices):
        lp = layerParams[idx]
        weightLayerNames[i] = lp['name']
        weights[i] = lp['weights'].ctypes.data_as(POINTER(c_float))
        weightsSz[i] = lp['weights_sz']
        bias[i] = lp['bias'].ctypes.data_as(POINTER(c_float))
        biasSz[i] = lp['bias_sz']

      self._executor \
        = self._lib.XDNNMakeScriptExecutorAndLoadWeightsFromMem(\
        handlePtrs, numHandles, numWeightLayers,
        weightLayerNames, weights, weightsSz, bias, biasSz,
        args['netcfg'].encode('utf-8'),
        args['quantizecfg'].encode('utf-8'),
        self.getMask(PE))

    self._compilerJSONObj = CompilerJsonParser( args['netcfg'] )
    self._npInputs = {}
    self._npOutputs = {}

    for k, v in self._compilerJSONObj.getInputs().items():
      if isinstance(args['batch_sz'], dict):
        self._bsz = args['batch_sz'][k]
      else:
        self._bsz = args['batch_sz']
      if (self._bsz == -1):
        self._bsz = self.getNumCUs()
      self._npInputs[k] = np.empty(((self._bsz,) + tuple(v[1:])), dtype=np.float32, order='C')

    for k, v in self._compilerJSONObj.getOutputs().items():
      self._npOutputs[k] = np.empty(((self._bsz,) + tuple(v[1:])), dtype=np.float32, order='C')


  def __del__(self):
    self.closeHandle()

  def getBatchSize(self):
    return self._bsz

  def getMPID(self):
    return self._mpid

  def getNumCUs(self):
    return self._lib.getNumCUs(self._handles[0])

  def createHandle(self, xclbin, kernel="kernelSxdnn_0", handleList = [0], AcquireID = 0, subscribeID="", publishID=""):
    """
    Programs a hardware acceleration engine to the FPGA, and initializes communication.

    :param xclbin: Path to binary image (a.k.a. xclbin) to be loaded.
    :type xclbin: str.
    :param kernel: Name of kernel in xclbin. Always use "kernelSxdnn_0". To be deprecated.
    :type kernel: str.
    :param handleList: List of device ids to create handles for
    :type handleList: list.
    :returns: int -- Return Code. Expect 0 for success.
    """
    ret = 0
    self._handles = []
    for i in range(len(handleList)):
      self._handles.append(c_void_p())
      ret |= self._lib.xblasCreate(
        pointer(self._handles[i]),
        xclbin.encode('utf-8'),
        kernel.encode('utf-8'),
        handleList[i],
        AcquireID, subscribeID, publishID)
    return ret, self._handles

  def closeHandle(self):
    """
    Terminates communication by destroying handle. No return value.
    """
    if not self._handles:
      return

    for h in self._handles:
      self._lib.xblasDestroy(h)

  def getInputDescriptors(self):
    return self._compilerJSONObj.getInputs()

  def getOutputDescriptors(self):
    return self._compilerJSONObj.getOutputs()

  def getOutputs(self):
    return self._npOutputs

  def getInputs(self):
    return self._npInputs

  def getMask(self, peList):
    if not isinstance(peList, list): peList = [peList]

    peMask = 0
    for peId in peList:
      if peId == -1: return 0
      peMask = peMask | (1 << peId)
    return peMask

  def execute(self, inputs, outputs, streamId=0, blocking=True ):
    """
    Executes inference on the hardware accelerator. This API call is blocking.

    :param inputs: Array holding the input volume for which to run inference.
    :type inputs: numpy array or array of raw c_short pointers.
    :param outputs: Array holding the result of the inference ran on the hardware accelerator. Shape will be (fpgaoutsz,) where fpgaoutsz is the total number of elements in the final activation ran in HW.
    :type outputs: numpy.ndarray.
    :param streamId: Argument not required.
    :type streamId: int.
    """
    inKeys = inputs.keys()
    outKeys = outputs.keys()

    in_name_arr = (c_char_p * len(inKeys) )(str(*inKeys).encode('ascii'))
    #in_bufsz_arr = ( c_uint * len(inSz))(*inSz)
    out_name_arr = (c_char_p * len(outKeys) )()
    #out_bufsz_arr = ( c_uint * len(outSz))(*outSz)
    in_ptr = {}
    firstInput = next(iter(itervalues(inputs)))
    if isinstance(firstInput,np.ndarray):
      bsz = firstInput.shape[0]
      for key, array in iteritems(inputs):
        in_ptr[key] = []
        for b in range(bsz):
          in_ptr[key].append ( array[b,...] )
    else:
      in_ptr = inputs

    in_batch = next(iter(itervalues(in_ptr)))

    bsz = len(in_batch)
    ptr_inarr_2d = (POINTER( np.ctypeslib.ndpointer(c_float, flags="C_CONTIGUOUS") ) * len(inputs) )()
    i = 0
    for v in itervalues(in_ptr):
      ptr_inarr_2d[i] = ( np.ctypeslib.ndpointer(c_float, flags="C_CONTIGUOUS")  * len(v) )()
      for p, p_val in enumerate(v):
        ptr_inarr_2d[i][p] = p_val.ctypes.data_as( np.ctypeslib.ndpointer(c_float, flags="C_CONTIGUOUS") )
      i += 1

    ptr_outarr_2d = ( np.ctypeslib.ndpointer(c_float, flags="C_CONTIGUOUS") * len(outputs) )()
    out_bufsz_arr = ( c_uint * len(outputs))()

    i = 0
    for k,v in iteritems(outputs):
      ptr_outarr_2d[i] = v.ctypes.data_as( np.ctypeslib.ndpointer(c_float, flags="C_CONTIGUOUS") )
      out_bufsz_arr[i] = np.prod(v.shape[1:])
      out_name_arr[i] = k.encode('ascii')
      i += 1

    self._lib.XDNNExecute_2D_float ( self._executor, ptr_inarr_2d, in_name_arr,
                                     len(inputs), ptr_outarr_2d, out_bufsz_arr, out_name_arr, len(outputs), bsz, streamId, blocking)

  def exec_async(self, inputs, output, streamId=0):
    """
    Executes inference on the hardware accelerator. This API call is non-blocking. The result of execution can be fetched using xdnn.get_result.

    :param inputs: Array holding the input volume for which to run inference.
    :type inputs: <class 'xdnn.LP_c_short_Array_1'>.
    :param outputs: Array holding the result of the inference ran on the hardware accelerator. Shape will be (fpgaoutsz,) where fpgaoutsz is the total number of elements in the final activation ran in HW.
    :type outputs: numpy.ndarray.
    :param streamId: Stream ID used to recover result at a later time.
    :type streamId: int.
    :returns: int -- Return Code. Expect 0 for success.
    """
    return self.execute(inputs, output, streamId, False)

  def get_result(self, streamId=0):
    """
    Get result of execution for a given PE, and a given stream. This API is used in conjuntion with xdnn.exec_async.
    :param streamId: Stream ID to recover result from.
    :type streamId: int.
    :returns: int -- Return Code. Expect 0 for success.
    """
    return self._lib.XDNNWaitForResults( self._executor, streamId )

  def get_exec_time(self, devIdx=0,cuIdx=0):
    curr_time = self._lib.XDNNReadHardwareCounter(self._executor, devIdx, cuIdx)
    elapsed = curr_time - self._prev_time
    self._prev_time = curr_time
    return elapsed

class XDNNCPUOp:
  def __init__(self, weights_path, libFile=None):
    if not libFile and "LIBXDNN_PATH" in os.environ:
      libFile = os.environ["LIBXDNN_PATH"]
    if not libFile or not os.path.isfile(libFile):
      raise AssertionError("XDNN library .so file %s not found" % libFile)

    self._libFile = os.path.abspath(libFile)
    self._handles = None
    self._wgt = weights_path
    self._lib = cdll.LoadLibrary(self._libFile)
    self._lib.computeFC.argtypes \
      = [np.ctypeslib.ndpointer(c_float, flags="C_CONTIGUOUS"),
         np.ctypeslib.ndpointer(c_float, flags="C_CONTIGUOUS"),
         np.ctypeslib.ndpointer(c_float, flags="C_CONTIGUOUS"),
         c_int, c_int, c_int,
         np.ctypeslib.ndpointer(c_float, flags="C_CONTIGUOUS")]
    self._lib.computeSoftmax.argtypes = [np.ctypeslib.ndpointer(c_float, flags="C_CONTIGUOUS"), c_uint, c_uint ]
    self._weight, self._bias = self.loadFCWeightsBias()

  def loadFCWeightsBias(self, index = 0): 
    data_dir = self._wgt
    if ".h5" in data_dir: 
      with h5py.File(data_dir,'r') as f:  
        keys = list(f.keys()) 
        fcKeys = [k for k in keys if k.startswith("fc_")]
        fcBiasKeys = [k for k in keys if k.startswith("fc_bias_")]

        if fcKeys and fcBiasKeys:
          weight = list(np.array(f[fcKeys[0]]).flatten())
          bias = list(np.array(f[fcBiasKeys[0]]).flatten())
        else:
          print("No FC layers found in weight file {:s}".format(data_dir))
          return (None, None)
    else:
      fname = "%s/fc" % data_dir
      if not os.path.exists(fname):
        nearMatch = getNearFileMatchWithPrefix(data_dir, "fc", index)
        if nearMatch:
          fname = nearMatch
      if os.path.exists(fname):

        with open(fname, 'r') as f:
          line = f.read()
          vals = line.strip().split(' ')
          weight = [float(v) for v in vals]
      else:
        print("No FC layers found in {:s}".format(data_dir))
        return (None, None)

      fname = "%s/fc_bias" % data_dir
      if not os.path.exists(fname):
        nearMatch = getNearFileMatchWithPrefix(data_dir, "fc_bias", index)
        if nearMatch:
          fname = nearMatch
      with open(fname, 'r') as f:
        line = f.read()
        vals = line.strip().split(' ')
        bias = [float(v) for v in vals]


    return (np.asarray(weight, dtype=np.float32), np.asarray(bias, dtype=np.float32))

  def computeSoftmax(self, data):
    """
    Compute the softmax of a given activation or a set of activations.

    :param data: Activation or a set of activations corresponding to multiple images stored as a 1D Array.
    :type data: numpy.ndarray.
    :param num: Number of images processed.
    :returns: numpy.ndarray -- Softmax Activation.
    """
    for i in range(data.shape[0]):
      self._lib.computeSoftmax(data[i,:], 1, np.prod(data.shape[1:]))
    return data

  def computeFC(self, data,out):
    """
    Compute the inner product layer for a given activation or a set of activations. WX+B.

    :param weight: Weights corresponding to the inner product layer. These weights are extracted by the xdnn_io.loadWeights API.
    :type weight: numpy.ndarray
    :param bias: Biases corresponding to the inner product layer. These biases are extracted by the xdnn_io.loadWeights API.
    :type bias: numpy.ndarray
    :param data: Activation or a set of activations corresponding to multiple images stored as a 1D Array.
    :type data: numpy.ndarray.
    :param out: Inner Product result (output volume)
    :type out: numpy.ndarray.
    """
    M = int(data.shape[0])
    N = int(out.shape[1])
    K = np.product ( data.shape[1:])
    #print(len(self._weight))
    #print(len(self._bias)) 
    if len(self._weight) != K*N:
      raise Exception('FC weight dim mismatch')
    if np.size(data) != M*K:
      raise Exception('FC input dim mismatch')
    if len(self._bias) != N:
      raise Exception('FC bias dim mismatch')

    self._lib.computeFC(self._weight, self._bias, data, M, N, K, out)

# class XDNNMPManager(BaseManager):
#   pass
#
# XDNNMPManager.register('XDNNManager', XDNNManager)
