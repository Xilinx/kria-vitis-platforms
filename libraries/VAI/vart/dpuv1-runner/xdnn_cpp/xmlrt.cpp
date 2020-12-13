// Copyright 2019 Xilinx Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <climits>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include "xblas.h"
#include "xmlrt.h"
#include "xdnn_env.h"
#include <boost/algorithm/string.hpp>
#include <limits.h>

static boost::mutex c_post_print_mtx; // C post print lock

#if defined(MKL)
#include <mkl.h>
#elif defined(CBLAS)
#include <cblas.h>
#endif

///////////////////////////////////////////////////////////

XComputeUnit::XComputeUnit(XBLASHandle *handle, size_t cuIdx, int ddrBank,
    int maxNumSlots, int maxMemSize) :
    _handle(handle), _cuIdx(cuIdx), _ddrBank(ddrBank), _maxNumSlots(
        maxNumSlots), _maxMemSize(maxMemSize), _totalSizeUsed(0), _numFreeMemSlots(
        maxNumSlots), _numFreeComputeSlots(maxNumSlots), _ddrBase(
        std::numeric_limits<long long>::max()),
        _scratchDdrMem(0), _numImagesProcessed(0),
   _xbarTableCounter(0) 
{
  _memSlots.resize(maxNumSlots, 0);
  _memPtrs.resize(maxNumSlots, 0);
  _slotStatus.resize(maxNumSlots, XCU_SLOT_READY);
  _counterSnapShots.resize(2);
}

XComputeUnit::~XComputeUnit() {
  for (int i = 0; i < _memPtrs.size(); i++)
    if (_memPtrs[i])
      clReleaseMemObject(_memPtrs[i]);
}

std::pair<cl_mem, int> // (mem, slotIdx)
XComputeUnit::getCreateBuffer(void *hostPtr, int size, cl_mem_flags rwFlags) {
  std::lock_guard < std::mutex > guard(_mtx);
  /* 
   * [aaronn] use original size below, instead of 
   * inflating to 4K. Because, clCreateBuffer will
   * memcpy & duplicated unaligned buffers. If we give an inflated
   * size to clCreateBuffer, it will memcpy more bytes
   * than allocated in hostPtr and cause memory corruption.
   */
  const int realSize = size; //XMLRTUtils::roundUp4k(size);
  auto it = _hostPtr2MemSlotMap.find(hostPtr);
  if (it != _hostPtr2MemSlotMap.end()) {
    // reuse slot/buffer
    int slotIdx = it->second;
    assert(realSize <= _memSlots[slotIdx]);
    assert(_slotStatus[slotIdx] == XCU_SLOT_READY);
    return std::make_pair(_memPtrs[slotIdx], slotIdx);
  }

  // alloc new slot
  if(_numFreeMemSlots <= 0 || _totalSizeUsed + realSize >= _maxMemSize) {
    printf("ERROR: Unable to allocate new buffers.  Free Mem Slots = %d, Total Size = %d, MaxMemSize = %d\n",
           _numFreeMemSlots, (_totalSizeUsed + realSize), _maxMemSize);
    exit(1);
  }

  int freeSlotIdx = findFreeSlot();

  assert(freeSlotIdx >= 0);
  assert(_slotStatus[freeSlotIdx] == XCU_SLOT_READY);

#ifdef DEBUG
  xdnn::XDebugJournal().log("clCreateBuffer_getCreateBuffer",
      std::to_string(realSize));
#endif

  cl_mem_ext_ptr_t bufferExt;
  bufferExt.obj = hostPtr;
  bufferExt.param = 0;
  bufferExt.flags = _ddrBank;
  cl_mem_flags memFlags = CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | rwFlags;
  cl_int err;
  cl_mem mem = clCreateBuffer(_handle->context, memFlags, realSize, &bufferExt,
      &err);
  assert(mem);

  saveDdrBase(mem);

  _memSlots[freeSlotIdx] = realSize;
  _memPtrs[freeSlotIdx] = mem;
  _hostPtr2MemSlotMap[hostPtr] = freeSlotIdx;

  _totalSizeUsed += realSize;
  _numFreeMemSlots--;

  return std::make_pair(mem, freeSlotIdx);
}

void XComputeUnit::deleteBuffer(const void *hostPtr)
// remove from this Compute Unit's "memory"
// if delMemObj==true, release cl_mem
    {
  std::lock_guard < std::mutex > guard(_mtx);

  auto it = _hostPtr2MemSlotMap.find(hostPtr);
  if (it == _hostPtr2MemSlotMap.end())
    return;

  int slotIdx = it->second;
  int realSize = _memSlots[slotIdx];

  clReleaseMemObject(_memPtrs[slotIdx]);

  _hostPtr2MemSlotMap.erase(it);
  _memSlots[slotIdx] = 0;
  _memPtrs[slotIdx] = 0;

  _totalSizeUsed -= realSize;
  _numFreeMemSlots++;
}

int XComputeUnit::getSlot(void *hostPtr) {
  std::lock_guard < std::mutex > guard(_mtx);
  auto it = _hostPtr2MemSlotMap.find(hostPtr);
  if (it == _hostPtr2MemSlotMap.end())
    return -1;

  return it->second;
}

void XComputeUnit::saveWeightOffsetMap(std::string offsetMapStr) {
  if (offsetMapStr.empty())
    return;

  _weightOffsetMap.clear();

  // convert string to map
  std::vector < std::string > words;
  boost::split(words, offsetMapStr, boost::is_any_of(","));

  for (int i = 0; i < words.size(); i++) {
    std::vector < std::string > pair;
    boost::split(pair, words[i], boost::is_any_of(":"));
    assert(pair.size() == 2);

    _weightOffsetMap[pair[0]] = std::stoi(pair[1]) * sizeof(short);
  }
}

void XComputeUnit::saveDdrBase(cl_mem img) {
  long long imageInputMemPhys = std::numeric_limits<long long>::max();
  xclGetMemObjDeviceAddress(img, _handle->device_id, sizeof(long long),
      &imageInputMemPhys);

  _ddrBase = std::min(_ddrBase, imageInputMemPhys);
  //printf("ANDBG saveImgDdrBase %llx\n", _ddrBase);
}
void XComputeUnit::saveDdrBase(long long addr) {
  _ddrBase = std::min(_ddrBase, addr);
}
//To-do saveScratchDdrBase
void XComputeUnit::saveScratchDdrBase(cl_mem scratch) {
  saveDdrBase(scratch);
  _scratchDdrMem = scratch;
}

unsigned int XComputeUnit::computeDdrOffset(cl_mem img) {
  assert(_ddrBase != std::numeric_limits<long long>::max());

  long long imageMemPhys;
  xclGetMemObjDeviceAddress(img, _handle->device_id, sizeof(long long),
      &imageMemPhys);

  assert(imageMemPhys - _ddrBase < std::numeric_limits<unsigned int>::max());
  return imageMemPhys - _ddrBase;
}

int XComputeUnit::findFreeSlot() {
  for (int i = 0; i < _memSlots.size(); i++)
    if (_memSlots[i] == 0)
      return i;

  return -1;
}

/******************************************************************
 * CLObjs
 *******************************************************************/

template<typename T>
void CLObjs<T>::clear() {
  //if (m_objs != NULL)
  //  delete[] m_objs;

  m_objs.clear();
  m_numObjs = 0;
  m_labels.clear();
}

template<typename T>
CLObjs<T>::CLObjs(const CLObjs &obj) {
  clear();
  for (int i = 0; i < obj.m_numObjs; i++)
    add(obj.m_objs[i], obj.m_labels[i]);
}

template<typename T>
CLObjs<T>& CLObjs<T>::operator=(const CLObjs &obj) {
  clear();
  for (int i = 0; i < obj.m_numObjs; i++)
    add(obj.m_objs[i], obj.m_labels[i]);

  return *this;
}

/******************************************************************
 * Pipeline packet
 *******************************************************************/
XPipelinePacket::XPipelinePacket() :
    id(-1), xHandle(NULL), A(NULL), B(NULL), C(NULL), 
    xdnnStartIdx(-1), xdnnDFLStartIdx(-1), xdnnStopIdx(-1), xdnnDFLStopIdx(-1),
    kernelComputeUnitIdx(0), kernelComputeUnitSlot(0), streamId(0), 
    aDevMemPOD(NULL), bDevMemPOD(NULL), cDevMemPOD(NULL), kernelExecTime(0.) {
  startQp = nullptr;
  lastQp = nullptr;
  inbuf = nullptr;
  outbuf = nullptr;
  inbufsz = 0;
  outbufsz = 0;
  xdnnInput = nullptr;
  xdnnThroughputInterbuf = nullptr;
  xdnnOutput = nullptr;
  executor = nullptr;
}

void XPipelinePacket::print() {
  printf("XPipelinePacket:\n");
  printf("  kernelIdx:%d/%d\n", kernelComputeUnitIdx, kernelComputeUnitSlot);
}

void XPipelinePacket::recordEventTimestamp(const char *eventName) {
  std::chrono::high_resolution_clock clock;
  eventTimestamps.push_back(
      std::make_pair(eventName,
          std::chrono::duration_cast < std::chrono::microseconds
              > (clock.now().time_since_epoch()).count()));
}

long XPipelinePacket::getEventTime(const char* eventName) {
  for (int i = 1; i < eventTimestamps.size(); i++) {
    if (eventTimestamps[i].first != eventName)
      continue;

    return (eventTimestamps[i].second - eventTimestamps[i - 1].second);
  }
  return 0;
}

void XPipelinePacket::saveToJournal() {
#if 0 // [aaronn] TODO engine-refactor
  static int i = 0;
  if (XDNN_VERBOSE) {
    if (xdnnDescriptorsPtr == NULL) {
      printf("[XBLAS] XJ: %d %s %d %d %d | %d/%d/%d\n", i++,
          cfg.m_taskName.c_str(), size, aOffset, bOffset, kernelIdx,
          kernelComputeUnitIdx, kernelComputeUnitSlot);
    } else {
      printf(
          "[XDNN] XJ: %d %d %s | %d/%d/%d | %d %d %d %d %x | %d %d %d %d %x | %d\n",
          i++, id, cfg.m_taskName.c_str(), kernelIdx, kernelComputeUnitIdx,
          kernelComputeUnitSlot, xdnnInput._shape[0], xdnnInput._shape[1],
          xdnnInput._shape[2], xdnnInput._shape[3], xdnnInput._baseAddr,
          xdnnOutput._shape[0], xdnnOutput._shape[1], xdnnOutput._shape[2],
          xdnnOutput._shape[3], xdnnOutput._baseAddr,
          int(xdnnDescriptorsPtr->size()));

      for (int di = 0; di < xdnnDescriptorsPtr->size(); di++) {
        XDNNDescriptor &desc = *((*xdnnDescriptorsPtr)[di].get());
        printf("[XDNN] XJ_%d %d | %d %d | %d %d -> %d %d | %x -> %x\n",
            di, desc.OpType(),
            desc._opSizex, desc._opStridex,
            desc._inImgSizex, desc._inImgChannels,
            desc._outImgSizex, desc._outImgChannels,
            desc._inBaseAddr, desc._outBaseAddr);

      }
    }
  }
#endif
}

void XPipelinePacket::emitProfilingInfo() {
  if (!XBLAS_EMIT_PROFILING_INFO)
    return;

  boost::lock_guard<boost::mutex> guard(c_post_print_mtx);
  // t_0 is reset at every kernel0's first event.
  // all times are then offset from t_0.
  static size_t t_0 = 0;
  static size_t t_lastEmit = 0;
  const size_t minTimeBetweenBatchEmits = 500000000; // 500 ms

  size_t t_bStart = 0;
  size_t t_bEnd = 0;
  std::string bLabel;
  size_t t_kStart = 0;
  size_t t_kEnd = 0;
  size_t t_cStart = 0;
  size_t t_cEnd = 0;

  if (kernelComputeUnitSlot != 0)
    return; // only emit the first slot's info for each CU

  for (int m = 0; m < writeEvents.m_numObjs; ++m) {
    if (writeEvents.m_labels[m].find("mb") == std::string::npos)
      continue; // only looking for Bs now

    bLabel = writeEvents.m_labels[m];

    clGetEventProfilingInfo(writeEvents.m_objs[m],
    CL_PROFILING_COMMAND_START, sizeof(t_bStart), &t_bStart, NULL);
    clGetEventProfilingInfo(writeEvents.m_objs[m],
    CL_PROFILING_COMMAND_END, sizeof(t_bEnd), &t_bEnd, NULL);

    if (xHandle->getId() == 0 && kernelComputeUnitIdx == 0
        && kernelComputeUnitSlot == 0)
      t_0 = t_bStart; // reset batch epoch

    break; // TODO handle other DDR reads
  }

  if (readEvents.m_numObjs) {
    clGetEventProfilingInfo(readEvents.m_objs[0],
    CL_PROFILING_COMMAND_START, sizeof(t_cStart), &t_cStart, NULL);
    clGetEventProfilingInfo(readEvents.m_objs[0],
    CL_PROFILING_COMMAND_END, sizeof(t_cEnd), &t_cEnd, NULL);
  } else
    t_cStart = t_cEnd = t_0;

  if (execEvents.m_numObjs) {
    clGetEventProfilingInfo(execEvents.m_objs[0],
    CL_PROFILING_COMMAND_START, sizeof(t_kStart), &t_kStart, NULL);
    clGetEventProfilingInfo(execEvents.m_objs[0],
    CL_PROFILING_COMMAND_END, sizeof(t_kEnd), &t_kEnd, NULL);
  } else {
    // use write/read times to infer exec time
    t_kStart = t_bEnd;
    t_kEnd = t_cStart;
  }

  XComputeUnit* cu = this->xHandle->getComputeUnit(this->kernelComputeUnitIdx);

  std::vector<double> hwTimePerImage;
  if (xdnnDFLStopIdx >= 0)
    hwTimePerImage.push_back(cu->getAverageImageTime(0));
  else
    hwTimePerImage.push_back(0.);
  if (xdnnStopIdx >= 0)
    hwTimePerImage.push_back(cu->getAverageImageTime(1));
  else
    hwTimePerImage.push_back(0.);

  std::map<std::string, float> statsMap;
  auto et = getEventTimestamps();
  for (int i=0; i < et.size(); i++) 
    statsMap[et[i].first] = et[i].second;
  statsMap["device_id"] = xHandle->getId();
  statsMap["t"] = (eventTimestamps[0].second/1000) % 1000000; // in ms
  statsMap["id"] = id;
  statsMap["pe"] = kernelComputeUnitIdx;
  statsMap["slot"] = kernelComputeUnitSlot;
  statsMap["ddr_wr"] = (t_bEnd - t_bStart) / 1000000.;
  statsMap["exec"] = kernelExecTime*1000;
  statsMap["ddr_rd"] = (t_cEnd - t_cStart) / 1000000.;
  statsMap["hw_counter_0"] = hwTimePerImage[0];
  statsMap["hw_counter_1"] = hwTimePerImage[1];

  if (XDNN_VERBOSE) {
    printf("[XDNN] FPGA metrics (%d/%d/%d)\n"
      "[XDNN]   packet ID      : %d\n"
      "[XDNN]   write_to_fpga  : %.2f ms\n"
      "[XDNN]   exec_xdnn      : %.2f ms\n"
      "[XDNN]   read_from_fpga : %.2f ms\n"
      "[XDNN]   hw_counter_0   : %.4f ms\n"
      "[XDNN]   hw_counter_1   : %.4f ms\n",
      int(statsMap["device_id"]), int(statsMap["pe"]), int(statsMap["slot"]), 
      int(statsMap["id"]), statsMap["ddr_wr"], statsMap["exec"], statsMap["ddr_rd"],
      statsMap["hw_counter_0"], statsMap["hw_counter_1"]);

    printEventTimestamps();
  }
#ifdef ZMQ
  xHandle->m_xstream.sendDataToServer("speedodata", statsMap);
#endif

#if 0
  if (t_0 > t_lastEmit
      && t_0 - t_lastEmit < minTimeBetweenBatchEmits)
  return; // too soon; don't broadcast so much
  t_lastEmit = t_0;

  //int batchSize = cfg.m_batchSize;
  const int batchSize = 1;

  XBLASKernelConfig &kcfg = xHandle->m_kConfig;
  assert(kcfg.m_kernelType == XDNN_KERNEL);
  int coreIdx = kernelComputeUnitIdx;

  // publish info

  std::stringstream ss;
  ss << "id " << id << ","
  << "b " << batchSize << ","
  << "k " << coreIdx << ","
  << "h " << xHandle->getId() << ",";
  //  << "w_start " << t_bStart - t_0 << ","
  //  << "w_end " << t_bEnd - t_0 << ","
  //  << "k_start " << t_kStart - t_0 << ","
  //  << "k_end " << t_kEnd - t_0 << ","
  //  << "r_start " << t_cStart - t_0 << ","
  //  << "r_end " << t_cEnd - t_0;
  size_t writeStart = t_bStart - t_0;
  size_t writeEnd = t_bEnd - t_0;
  size_t execStart = writeEnd + (getEventTime("submit")*1000);
  size_t execEnd = 0;
  double maxBlockTime = 0;
  for (int bi=0; bi < hwTimePerImage.size(); bi++)
    maxBlockTime = std::max(maxBlockTime, hwTimePerImage[bi]);
  if (maxBlockTime > 0)
    execEnd = execStart + (maxBlockTime*1000000);
  else
    execEnd = execStart + (kernelExecTime*1000000000);
  size_t readStart = execEnd;
  size_t readEnd = readStart + t_cEnd - t_cStart;
  ss << "w_start " << writeStart << ","
  << "w_end " << writeEnd << ","
  << "k_start " << execStart << ","
  << "k_end " << execEnd << ","
  << "r_start " << readStart << ","
  << "r_end " << readEnd;
  zmq::message_t request(ss.str().length()+1);
  memcpy(request.data(), ss.str().c_str(), ss.str().length()+1);
  zmq::socket_t *g_zmqsocket =  this->xHandle->g_zmqsocket;
  g_zmqsocket->send(request);
#endif
}

std::vector<std::pair<std::string, float> > 
XPipelinePacket::getEventTimestamps() {
  std::vector<std::pair<std::string, float> > stats; 

  long startTime = eventTimestamps[0].second;
  float total = 0.;
  for (int i = 1; i < eventTimestamps.size(); i++) {
    long time = eventTimestamps[i].second;
    long prev = eventTimestamps[i - 1].second;
    float val = float(time - prev) / 1000.;
    total += val;
    stats.push_back(std::make_pair(eventTimestamps[i].first, val));
  }
  stats.push_back(std::make_pair("total", total));
  return stats;
}

void XPipelinePacket::printEventTimestamps() {
  if (!XBLAS_EMIT_PROFILING_INFO)
    return;

  auto et = getEventTimestamps();
  if (et.empty())
    return;

  long startTime = eventTimestamps[0].second;
  printf("[XDNN]  Packet %d: %ld\n", id, startTime);
  for (int i=0; i < et.size(); i++)
    printf("[XDNN]  Packet %d: %8s %.2f\n", id, et[i].first.c_str(), et[i].second);
}

void XPipelinePacket::cleanup() {
  eventsToRelease.extend(writeEvents);
  eventsToRelease.extend(execEvents);
  eventsToRelease.extend(readEvents);

  for (int ei = 0; ei < eventsToRelease.m_numObjs; ei++)
    clReleaseEvent(eventsToRelease.m_objs[ei]);

}

template class CLObjs<cl_mem> ;
template class CLObjs<cl_event> ;
