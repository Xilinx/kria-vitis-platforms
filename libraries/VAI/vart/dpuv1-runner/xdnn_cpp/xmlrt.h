// SPDX-License-Identifier: BSD-3-CLAUSE
//
// (C) Copyright 2018, Xilinx, Inc.
//
/**
 *  @brief Xilinx internal header declarations for cpp files
 *
 *  @author Aaron Ng (aaronn@xilinx.com)
 */

#ifndef XMLRT_H
#define XMLRT_H

#include "xdnn_common.h"
#include <CL/opencl.h>
#include "xblas.h"
#include "xdnn_util.h"
#include <map>
#include <queue>
#include <set>
#include <mutex>
#include <condition_variable>
#include <vector>
#include "xdnn_reg_write_mgr.h"
/**********************************************************
 * XFDNN base classes that we expect all IPs to implement
 **********************************************************/
enum XDNNTensorShapeType { XDNN_TENSOR_1D, XDNN_TENSOR_NCHW };
class XDNNDescriptor;
class XDNNDataDescriptor;
class XPipelinePacket;

class XDNNDispatcher{
  public:
    virtual ~XDNNDispatcher(){ }
    virtual void dispatch ( XDNNDescriptor * d ) = 0;
    virtual void dispatch ( XDNNDataDescriptor * d ) = 0;
};

/* 
 * Flow:
 * 0. scheduler reserves a slot to update "free slot count"
 * 1. fill _memSlot[s] with cl_mem
 * 2. Mark _slotStatus[s] = XCU_SLOT_BUSY when kernel executes job
 * 3. Mark _slotStatus[s] = XCU_SLOT_DONE when kernel done with job
 * 4. Mark _slotStatus[s] = XCU_SLOT_READY when DDR->host xfer done
 */
enum XComputeUnitSlotStatus {
  XCU_SLOT_READY, XCU_SLOT_BUSY, XCU_SLOT_DONE
};
class XBLASHandle;
class XComputeUnit {
public:
  XComputeUnit(XBLASHandle *handle, size_t cuIdx, int ddrBank, int maxNumSlots,
      int maxMemSize);
  ~XComputeUnit();

  unsigned long getNumImagesProcessed() {
    return _numImagesProcessed;
  }

  void setNumImagesProcessed(unsigned long value) {
    _numImagesProcessed = value;
  }

  void incNumImagesProcessed(unsigned long value = 1) {
    this->setNumImagesProcessed(this->getNumImagesProcessed() + value);
  }

  void addCounterSnapShot(float time, int slice) {
    assert(slice < _counterSnapShots.size());
    _counterSnapShots[slice].push_back(std::make_pair(_numImagesProcessed, time));
    if (_counterSnapShots[slice].size() > 10) {
      _counterSnapShots[slice].pop_front();
    }
  }

  double getAverageImageTime(int slice) {
    assert(slice < _counterSnapShots.size());
    if (_counterSnapShots[slice].empty())
      return 0.;

    if (_counterSnapShots[slice].size() < 2)
      return _counterSnapShots[slice].at(0).second / _counterSnapShots[slice].at(0).first;

    int lastIdx = _counterSnapShots[slice].size() - 1;
    double elapsedTime = _counterSnapShots[slice].at(lastIdx).second
        - _counterSnapShots[slice].at(0).second;
    double elapsedImages = _counterSnapShots[slice].at(lastIdx).first
        - _counterSnapShots[slice].at(0).first;
    return elapsedTime / elapsedImages;
  }

  void updateXbarTableCounter(int c) { _xbarTableCounter = c; }
  int getXbarTableCounter() { return _xbarTableCounter; }

  std::pair<cl_mem, int>
  getCreateBuffer(void *hostPtr, int size, cl_mem_flags rwFlags);
  void deleteBuffer(const void *hostPtr);
  int getSlot(void *hostPtr);
  int getNumFreeSlots();

  void saveDdrBase(cl_mem img);
  void saveDdrBase(long long addr);
  void saveScratchDdrBase(cl_mem scratch);
  void saveWeightOffsetMap(std::string offsetMapStr);
  const std::map<std::string, int> &getWeightOffsetMap() {
    return _weightOffsetMap;
  }
  long long getDdrBase() {
    return _ddrBase;
  }
  cl_mem getScratchDdrMem() {
    return _scratchDdrMem;
  }
  unsigned int computeDdrOffset(cl_mem img);
  size_t getCuIdx() { return _cuIdx; }
  XBLASHandle* getHandle(){ return _handle;}

private:
  //void updateSlotStatus(int slot, XComputeUnitSlotStatus newStatus);
  int findFreeSlot();

  unsigned long _numImagesProcessed;
  std::vector<std::deque<std::pair<unsigned long, float>>> _counterSnapShots;
  /////////////////////

  XBLASHandle *_handle;

  size_t _cuIdx;

  const int _ddrBank; // B & C must be same in this scheme
  const int _maxNumSlots;
  const int _maxMemSize;

  std::vector<int> _memSlots; // [idx->size]
  std::vector<cl_mem> _memPtrs;
  std::map<std::string, int> _weightOffsetMap; // layerName -> relative byte offset
  std::vector<XComputeUnitSlotStatus> _slotStatus;
  std::map<const void*, int> _hostPtr2MemSlotMap;
  std::set<void*> _reservedSlots;

  int _totalSizeUsed;
  int _numFreeMemSlots;
  int _numFreeComputeSlots;

  long long _ddrBase;
  cl_mem _scratchDdrMem;

  // this is used in the "multiple nets on one PE" scenario.
  // subsequent xbar programming uses this "offset" to append
  // to the table instead of overwriting existing entries
  int _xbarTableCounter;

  std::mutex _mtx;
};

class XDNNDataDescriptor {
  public:
    XDNNDataDescriptor() = delete;
    virtual ~XDNNDataDescriptor() { }
    XDNNDataDescriptor( const std::string & layerName,
        int dataTypeSize,
        XDNNTensorShapeType shapeType,
        int n, int c, int h, int w, unsigned long long hwOffset, unsigned long long hwSzInBytes,
        bool singleStep);
    size_t getSize() const;
    size_t getSizeInBytes() const;

    unsigned long long getHWSizeInBytes() const
    {
    	return _hwSizeInBytes;
    }

    unsigned long long getHWSize() const{
    	return _hwSizeInBytes / _dataTypeSize;
    }
    unsigned long long getHWAddrOffset() const
    {
    	return _hwAddrOffset;
    }

    std::string getLayer() const {
    	return _layerName;
    }


    virtual int execute(XComputeUnit *cu) = 0;

    virtual void dispatch ( XDNNDispatcher * v) { v->dispatch ( this ); }

    //void *_data;
    XDNNTensorShapeType _shapeType;
    unsigned long long _hwAddrOffset, _hwSizeInBytes;
    std::string _layerName;
    int _n;
    int _c;
    int _h;
    int _w;
    int _dataTypeSize;
    bool _singleStep;
};

template<typename T>
class CLObjs {
public:
  CLObjs() :
      m_objs(0), m_numObjs(0) {
  }
  CLObjs(const CLObjs &obj);
  CLObjs& operator=(const CLObjs &obj);

  ~CLObjs() {
    clear();
  }
  void clear();
  void add(T obj, std::string label = "") {
    if (m_objs.empty()) {
      m_objs.resize(10);
      m_labels.resize(10);
    }

    assert(m_numObjs < 10);
    m_objs[m_numObjs] = obj;
    m_labels[m_numObjs] = label;
    m_numObjs++;
  }
  void extend(const CLObjs &obj) {
    for (int i = 0; i < obj.m_numObjs; i++)
      add(obj.m_objs[i], obj.m_labels[i]);
  }

  std::vector<T> m_objs;
  int m_numObjs;
  std::vector<std::string> m_labels;
};

class XDNNLayerQuantParam;

class XDNNInputDescriptor;
class XDNNOutputDescriptor;
class XDNNThroughputInterbufDescriptor;

#ifdef XDNN_REWRITE
class XPipelinePacket {
public:
  XPipelinePacket();

  void print();
  void saveToJournal();
  void recordEventTimestamp(const char* eventName);
  long getEventTime(const char* eventName);
  void printEventTimestamps();
  void emitProfilingInfo();
  void cleanup();

  int id;
  class XBLASHandle *xHandle;
  std::shared_ptr<XDNNHWFormatters> hwFormat;
  XBLASConfig cfg;
  float qp_threshIn, qp_threshOut;
  int   qp_bitWidth;
  const float * float_in[2];
  float * float_out[2];
  std::shared_ptr<XPacketExecutor> executor;
  int xdnnStartIdx, xdnnDFLStartIdx;
  int xdnnStopIdx, xdnnDFLStopIdx;
  const std::unordered_map< std::string, XDNNLayerQuantParam* > * quantParam;
  const std::map< std::string, std::shared_ptr <XDNNInputDescriptor> > * xdnnInput;
  const XDNNThroughputInterbufDescriptor *xdnnThroughputInterbuf;
  const std::map< std::string, std::shared_ptr <XDNNOutputDescriptor> > * xdnnOutput;
  // kernel scheduling
  int kernelIdx;
  int kernelComputeUnitIdx;
  int slotIdx;
  int streamId;
  // exec stage
  CLObjs<cl_event> writeDependencies; // start writes only after these events
  CLObjs<cl_event> readDependencies; // start reads only after these events
  CLObjs<cl_event> execDependencies; // start execs only after these events
  CLObjs<cl_event> writeEvents;
  CLObjs<cl_event> execEvents;
  CLObjs<cl_event> readEvents;

  CLObjs<cl_event> eventsToRelease; // events to release after done with packet

  xdnn::XTimer kernelExecTimer;
  double kernelExecTime;

  // std::chrono::high_res_clock microsec stamps
  std::vector<std::pair<std::string, long> > eventTimestamps;
};
#endif

class XPipelinePacket {
public:
  XPipelinePacket();

  // Soren's API for a packet
  xdnn::RegisterWriteManager *rwm;
  void print();
  void saveToJournal();
  void recordEventTimestamp(const char* eventName);
  long getEventTime(const char* eventName);
  std::vector<std::pair<std::string, float> > getEventTimestamps();
  void printEventTimestamps();
  void emitProfilingInfo();
  void cleanup();

  unsigned id;
  class XBLASHandle *xHandle;
  XMemPtr *A;
  XMemPtr *B;
  XMemPtr *C;

  std::unordered_map <std::string, const float*> float_in;
  std::unordered_map <std::string, float*> float_out;
  std::shared_ptr<XDNNPacketExecutor> executor;
  int xdnnStartIdx, xdnnDFLStartIdx;
  int xdnnStopIdx, xdnnDFLStopIdx;
  XDNNLayerQuantParam * startQp, *lastQp;
  const std::unordered_map< std::string, XDNNLayerQuantParam* > * quantParam;
  const std::map< std::string, std::shared_ptr <XDNNInputDescriptor> > * xdnnInput;
  const XDNNThroughputInterbufDescriptor *xdnnThroughputInterbuf;
  const std::map< std::string, std::shared_ptr <XDNNOutputDescriptor> > * xdnnOutput;
  void * inbuf, *outbuf;
  size_t inbufsz, outbufsz;

  // kernel scheduling
  int kernelComputeUnitIdx;
  int kernelComputeUnitSlot;
  int streamId;
  // write stage
  XDeviceMemPOD<short> *aDevMemPOD;
  XDeviceMemPOD<short> *bDevMemPOD;
  XDeviceMemPOD<short> *cDevMemPOD;
  // exec stage
  CLObjs<cl_event> writeDependencies; // start writes only after these events
  CLObjs<cl_event> readDependencies; // start reads only after these events
  CLObjs<cl_event> execDependencies; // start execs only after these events
  CLObjs<cl_event> writeEvents;
  CLObjs<cl_event> execEvents;
  CLObjs<cl_event> readEvents;

  CLObjs<cl_event> eventsToRelease; // events to release after done with packet

  xdnn::XTimer kernelExecTimer;
  double kernelExecTime;

  // std::chrono::high_res_clock microsec stamps
  std::vector<std::pair<std::string, long> > eventTimestamps;
};

class XMLRTUtils {
public:
  static std::vector<std::string> split(std::string str, std::string delim) {
    std::vector < std::string > ret;

    auto start = 0U;
    auto end = str.find(delim);
    while (end != std::string::npos) {
      std::string word = str.substr(start, end - start);
      start = end + delim.length();
      end = str.find(delim, start);

      ret.push_back(word);
    }
    ret.push_back(str.substr(start, end));

    return ret;
  }
};

#endif // XMLRT_H
