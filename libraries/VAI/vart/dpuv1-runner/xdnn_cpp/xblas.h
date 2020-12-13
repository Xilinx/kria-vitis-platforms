// SPDX-License-Identifier: BSD-3-CLAUSE
//
// (C) Copyright 2018, Xilinx, Inc.
//
/**
 *  @brief Xilinx XFDNN library for FPGA acceleration
 *
 *  @author Aaron Ng (aaronn@xilinx.com), Bill Teng (xteng@xilinx.com)
 */

#ifndef XBLAS_H
#define XBLAS_H

#include <chrono>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#include <CL/opencl.h>
#include <boost/thread.hpp>
#include <fstream>
#include <memory>
#include "xdnn_util.h"
#include "thread_pool.h"
#include "xdnn_common.h"
struct xrt_device;

#define XDNN_ONEHACK_NONCE_SCRIPTID 0xBEEF

#include "butler_client.h"

namespace std {
namespace {
// [aaronn] hash function for tuples
// Code from boost
// Reciprocal of the golden ratio helps spread entropy
//     and handles duplicates.
// See Mike Seymour in magic-numbers-in-boosthash-combine:
//     http://stackoverflow.com/questions/4948780

template<class T>
inline void hash_combine(std::size_t& seed, T const& v) {
  seed ^= hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

// Recursive template code derived from Matthieu M.
template<class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
struct HashValueImpl {
  static void apply(size_t& seed, Tuple const& tuple) {
    HashValueImpl<Tuple, Index - 1>::apply(seed, tuple);
    hash_combine(seed, get<Index>(tuple));
  }
};

template<class Tuple>
struct HashValueImpl<Tuple, 0> {
  static void apply(size_t& seed, Tuple const& tuple) {
    hash_combine(seed, get<0>(tuple));
  }
};
}

template<typename ... TT>
struct hash<std::tuple<TT...>> {
  size_t operator()(std::tuple<TT...> const& tt) const {
    size_t seed = 0;
    HashValueImpl<std::tuple<TT...> >::apply(seed, tt);
    return seed;
  }
};
}

class XMemPtr;
class XDNNQuantBuf;
class XDNNLayerQuantParam;
class XDNNDescriptorDB;

enum XBLASKernelType {
  XDNN_KERNEL
};
class XBLASKernelConfig {
public:
  XBLASKernelConfig();

  int m_forceRunOnKernelIdx;
  XBLASKernelType m_kernelType;

  // DDR mappings for each cu
  std::vector<int> m_ddrBankA, m_ddrBankB, m_ddrBankC;

  std::vector<int> m_configRegInfo;
  std::map<std::string, std::string> m_xclbinJsonVals;

  void setDDRMap(const std::string &str);
  int getDDRMap(int i);

  void print();

private:
  std::vector<int> m_ddrBankMap;
};

class XTimer {
public:
  XTimer() :
      beg_(clock_::now()) {
  }
  void reset() {
    beg_ = clock_::now();
  }
  double elapsed() const {
    return std::chrono::duration_cast < second_ > (clock_::now() - beg_).count();
  }

private:
  typedef std::chrono::high_resolution_clock clock_;
  typedef std::chrono::duration<double, std::ratio<1> > second_;
  std::chrono::time_point<clock_> beg_;
};

class XPipelinePacket;
class XDNNPacketExecutor {
public:
  ~XDNNPacketExecutor() {}
  void prepBForDevice(XPipelinePacket &packet);
  void prepCForDevice(XPipelinePacket &packet);
  void unpackCFromDevice(XPipelinePacket &packet);
  void execute(XPipelinePacket &packet) const;
};

class XPipelinePacket;
class XComputeUnit;
class XBLASHandle {
public:
  static XBLASHandle* get(int idx = 0, bool create = true);
  static int getNum() {
    return XBLASHandle::getInstance().size();
  }

  // should not be public
  // but this is ONEHACK life
  cl_context context;
  cl_command_queue commands;
  cl_program program;
  cl_device_id device_id;
  std::vector<cl_kernel> kernels;
  xrt_device* xdev;
  int numcus;
  butler::ButlerClient m_mgr;
  butler::Alloc m_resource;
  std::vector<butler::xCU> m_xCUVec;
  std::string m_publishId;
  std::string m_subscribeId;
  bool m_doRelease;
#ifdef ZMQ
  xdnn::XStream m_xstream;
#endif

  bool m_isInitialized;
  XBLASKernelConfig m_kConfig;
  std::vector<XComputeUnit*> _computeUnits;
  int getId() {
    return m_id;
  }

  void processXclbinJson(const std::string &path);

  // m_host2XMemMap helpers
  void setHost2XMem(const void *src, XMemPtr* dest);
  XMemPtr* getHost2XMem(const void *src);
  void deleteHost2XMem(const void *src);
  void destroyDeviceMemPODs();
  int getNumCUs(){
	 return this->numcus;
  }

  void waitForResults(int streamId = 0, bool recordPipelineTimes = false);

  // multi-handle management
  void release(int idx = 0);

  // JobParrallelism
  //void JobParrallelism(XPipelinePacket *packetPtr);
  void JobParrallelism(int id, int cuIdx, XBLASHandle &handle,
    const std::unordered_map <std::string, const float* > in_blob,
    const std::unordered_map <std::string, float* > out_blob,
    std::shared_ptr<XDNNQuantBuf> & qbuf, unsigned batch_id, int streamId,
    std::unordered_map<std::string, XDNNLayerQuantParam*> *quantparam,
    XDNNLayerQuantParam *startQp, XDNNLayerQuantParam *lastQp, XDNNDescriptorDB *descDB,
    int startIdx, int dflStartIdx, int stopIdx, int dflStopIdx);
  XComputeUnit* getComputeUnit(int cuIdx);
  int getNumComputeUnits();
  std::pair<cl_mem, int> getCreateBuffer(int cuIdx, void *hostPtr, int size, cl_mem_flags rwFlags);
  void deleteBuffer(int cuIdx, const void *hostPtr);

private:
  XBLASHandle(int id = 0); // handled by get()
  ~XBLASHandle(); // handled by release()


  static std::vector<XBLASHandle*>& getInstance(); // singleton

  // store a mapping of all xMemcpy (src, dest) ptrs
  // this is so we can map src ptrs to XMemPtrs.
  std::map<const void*, XMemPtr*> m_host2XMemMap;

  // make one command queue for each thread
  std::map<unsigned long, cl_command_queue> m_commands;

  int m_id;
  boost::mutex m_mtx;
  int m_refCnt;
  //JobParrallelism
  ThreadPool *m_workerPool;
  // first dimension the stream
  // second dimension the job (one job per image, size is batch size)
  std::unordered_map<long, std::vector<std::future<void>> > m_workerPoolResult;
};


template <typename DType>
class XDeviceMemPOD {
public:
  XDeviceMemPOD(): _hostsz(0), _hostptr(nullptr), m_devicePtr(nullptr){}
  ~XDeviceMemPOD(){}
  size_t numelem() { return _hostsz; }
  //size_t deviceSizeInBytes() { return sizeof(DType) * _deviceSz; }
  xdnn::XAlignedBlob<DType> & getData() { return m_data; }

  size_t _hostsz;
  void * _hostptr;
  xdnn::XAlignedBlob<DType> m_data; // (optional) can be used to store transformed data
                             //            (e.g., for padding)
  cl_mem m_devicePtr;
};

// Class to manage host<->device memory alloc and transfer
// Note: 
// - XMemPtr maps 1-to-1 to hostPtr
// - in multi-kernel/multi-cu mode, 
//   XMemPtr maps 1 hostPtr to many XDeviceMemPOD<short> objects
class XMemPtr {
public:
  XMemPtr(void * ptr, size_t sz);
  ~XMemPtr();

  int getKernelComputeUnitLocation(const void*);
  XDeviceMemPOD<short>* getDeviceMemPOD(int cuIdx);
  void deleteDeviceMemPODs();
  size_t size() const { return m_sizeInBytes; }
  void * get() const { return m_srcPtr; }
private:
  // populated by use r
  size_t m_sizeInBytes;
  void *m_srcPtr; // pointer to original data, in case we need lookup
  // auto-populated when data is written from host to device
  // this holds the data objects prepped for the FPGA
  // m_hostDeviceMemMap helpers
  // Note: 
  // - each kernel's CU can have its own DDRs
  // - one host ptr can map to multiple CUs' DDRs
  //   (e.g. A blob is loaded into all CUs' DDRs) 
  std::unordered_map<std::tuple<const void*, int>, XDeviceMemPOD<short> * > m_hostDeviceMemMap;
  XMemPtr();
};

int xdnnv3ImgSizeReq(int, int, int, int, int, int);
template<class DestT>
void std2xdnnv3(short*, DestT*, int, int, int, int, int, int);
template<class DdrT>
void xdnnv32std(DdrT*, short*, int, int, int, int, int, int);
/******************************************************************* 
 * XBLAS Client API
 *******************************************************************/

class XPipelinePacket;
//void xblasEnqueueJob(XBLASHandle &handle, XPipelinePacket *pkt);
void xblasEnqueueJob(XBLASHandle &handle,
    const std::unordered_map <std::string, const float* > in_blob,
    const std::unordered_map <std::string, float* > out_blob,
    std::shared_ptr<XDNNQuantBuf> & qbuf, unsigned batch_id, int streamId,
    std::unordered_map<std::string, XDNNLayerQuantParam*> *quantparam,
    XDNNLayerQuantParam *startQp, XDNNLayerQuantParam *lastQp, XDNNDescriptorDB *descDB,
    int startIdx, int dflStartIdx, int stopIdx, int dflStopIdx);

template<typename DType>
void XDNNQuantize(const float *in, DType *out, const int sz, const float thresh, const int bitWidth, bool doRounding=false);

extern "C" {

int getNumCUs( XBLASHandle * handle );

int xblasCreate(XBLASHandle *&handle, const char *xclbin,
    const char *kernelName, int deviceIdx = 0, int AcquireID = 0, const char *subscribeID=NULL, const char *publishID=NULL);

void xFree(XBLASHandle &handle, XMemPtr *memPtr, bool freeMemPtr = true);

XMemPtr* xRegMem(XBLASHandle &handle, void *hostMemPtr, size_t sz, bool copy = true);

void xblasDestroy(XBLASHandle *handle);

// special functions to preload matrices to DDR
void xblasLoadA(XBLASHandle &handle,  short *A, const char *layerMapStr, int prefCuIdx = -1);

#ifdef ENABLE_HBM
void xblasHBMLoadA(XBLASHandle &handle,  std::vector<std::vector<int> > &dataBlobsHBM, const char *layerMapStr, int prefCuIdx = -1);
#endif

void computeFC(float *weight, float *bias, float *data, int M, int N, int K,
    float *output);
void computeSoftmax(float * input, size_t bs, size_t num_elem);

void __attribute__ ((constructor)) my_init(void);
void __attribute__ ((destructor)) my_fini(void);

} // extern "C" {

#endif /*XBLAS_H*/
