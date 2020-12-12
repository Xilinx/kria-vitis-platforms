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

/**
 *  @brief Xilinx BLAS library for FPGA acceleration
 *
 *  @author Aaron Ng (aaronn@xilinx.com)
 */

#include <assert.h>
#include <chrono>
#include <iostream>
#include <limits.h>
#include <math.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <unistd.h>
#include <float.h>
#if defined(MKL)
#include <mkl.h>
#elif defined(CBLAS)
#include <cblas.h>
#endif

#include "CL/cl_ext_xilinx.h"
#include "experimental/xrt++.hpp"
#include "butler_dev.h"

#ifndef CBLAS
#define CblasNoTrans 111
#define CblasTrans 112
#endif

#include "xblas.h"
#include "xmlrt.h"
#include "xdnn_env.h"
#include "xdnn.h"
#include <boost/crc.hpp>  // for boost::crc_32_type
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
//#include "xsimd/xsimd.hpp"

#include <fstream>
#include <algorithm>
#define XBLAS_MAX_NUM_DEVICES 20
#define XBLAS_KERNEL_NAME "kernelSxdnn_0"

/******************************************
 * Load/Unload
 *****************************************/
std::mutex handle_mtx;           // mutex for handle critical section
#define MAX_HANDLES 64
XBLASHandle *myHandles[MAX_HANDLES];

// singleton XBLASHandle instance
static boost::mutex xblas_mtx; // global XBLAS API lock
static boost::mutex xmem_ptr_mtx; // XMemPtr lock
static boost::mutex enqueue_mtx; // enqueue lock

static bool threadDoBeforeWork(XPipelinePacket *packetPtr);
static bool threadDoExecWork(XPipelinePacket *packetPtr);
static void threadDoAfterWork(XPipelinePacket *packetPtr);
//static void threadsWorker(XPipelinePacket *packetPtr);
static void threadsWorker(int id, int cuIdx, XBLASHandle *handle,
    const std::unordered_map <std::string, const float* > in_blob,
    const std::unordered_map <std::string, float* > out_blob,
    XDNNQuantBuf *qbuf, unsigned batch_id, int streamId,
    std::unordered_map<std::string, XDNNLayerQuantParam*> *quantparam,
    XDNNLayerQuantParam *startQp, XDNNLayerQuantParam *lastQp, XDNNDescriptorDB *descDB,
    int startIdx, int dflStartIdx, int stopIdx, int dflStopIdx);

XBLASKernelConfig::XBLASKernelConfig() :
  m_forceRunOnKernelIdx(-1), m_kernelType(XDNN_KERNEL) {
#ifdef ENABLE_HBM
//Manasa Todo:Figure out how to extract number of active ddr banks from a given xclbin. 15 is the number for the current xclbin used for bringup
  for(int i=0;i<15;i++)
  {
    m_ddrBankMap.push_back(i | XCL_MEM_TOPOLOGY);
  }
#else
  // grab mapping from CL/cl_ext.h macros,
  // so we can programmatically select banks
  m_ddrBankMap.push_back(XCL_MEM_DDR_BANK0);
  m_ddrBankMap.push_back(XCL_MEM_DDR_BANK1);
  m_ddrBankMap.push_back(XCL_MEM_DDR_BANK2);
  m_ddrBankMap.push_back(XCL_MEM_DDR_BANK3);
#endif
}
void XBLASKernelConfig::setDDRMap(const std::string &bankStr) {
  m_ddrBankA.clear();
  m_ddrBankB.clear();
  m_ddrBankC.clear();

  std::cout << "[XDNN] using custom DDR banks " << bankStr << std::endl;

  std::vector < std::string > words = XMLRTUtils::split(bankStr, ",");

  std::vector<int> banks;
  for (int i = 0; i < words.size(); i++){
	  m_ddrBankA.push_back(std::stoi(words[i]));
	  m_ddrBankB.push_back(std::stoi(words[i]));
	  m_ddrBankC.push_back(std::stoi(words[i]));
  }
}
int XBLASKernelConfig::getDDRMap(int i) {
  assert(i < m_ddrBankMap.size());
  return m_ddrBankMap[i];
}
void XBLASKernelConfig::print() {
  printf("XBLASKernelConfig:\n");
  //if (m_forceRunOnKernelIdx >= 0)
  //  printf("  (force run on kernel %d only)\n", m_forceRunOnKernelIdx);
  for (unsigned i = 0; i < m_ddrBankA.size(); i++)
	  printf("  DDR Bank A%d: %d\n", i, m_ddrBankA[i]);
  for (unsigned i = 0; i < m_ddrBankB.size(); i++)
	  printf("  DDR Bank B%d: %d\n", i, m_ddrBankB[i]);
  for (unsigned i = 0; i < m_ddrBankC.size(); i++)
	  printf("  DDR Bank C%d: %d\n", i, m_ddrBankC[i]);
}

//////////////////////////////////////////////////////////

XBLASHandle::XBLASHandle(int id) :
    m_isInitialized(false), // this is set to true on xblasCreate()
    m_id(id), xdev(nullptr)
	{
	// init thread pool and future/results
	m_workerPool = new ThreadPool(XDNN_USE_JOB_THREADS_TOTAL_THREADS);
	m_doRelease = true;
	program = nullptr;
}

XBLASHandle::~XBLASHandle() {
  // call xblasDestroy() if user did not call?
  if (!m_isInitialized)
    return;

  // delete thread pool
  delete m_workerPool;
  // delete mgr
  if (m_doRelease){
	  m_mgr.releaseResources(m_resource.myHandle);
  }
	std::vector<butler::xCU>& xCUVec = m_xCUVec;
	for (int i = 0; i < xCUVec.size(); i++){
#ifdef XRT_20191
		xrtcpp::release_cu_context(xCUVec[i].getXDev(), i);
#else
		xrtcpp::release_cu_context(xCUVec[i].getXDev(), xCUVec[i].getCUIdx());
#endif
	}
  // TODO XXX FIXME
  // don't free device objects at end
  // because xilinxopencl's static objects
  // could have been already destroyed
  //destroyDeviceMemPODs();
  if (program != nullptr)
	  clReleaseProgram(program);
  if (commands != nullptr)
	  clReleaseCommandQueue(commands);
  if (context != nullptr)
	  clReleaseContext(context);
  for (int k = 0; k < kernels.size(); k++)
	  if (kernels[k] != nullptr)
		  clReleaseKernel(kernels[k]);

	// release resource
	for (int i = 0; i < MAX_HANDLES; i++){
		if (myHandles[i] == this){
			myHandles[i] = nullptr;
		}
	}
  // delete exec handler
}

std::vector<XBLASHandle*>& XBLASHandle::getInstance() {
  static std::vector<XBLASHandle*> *impl = NULL;

  if (impl == NULL)
    impl = new std::vector<XBLASHandle*>(XBLAS_MAX_NUM_DEVICES);

  return *impl;
}

XBLASHandle* XBLASHandle::get(int idx, bool create) {
  static boost::mutex mtx;
  boost::lock_guard<boost::mutex> guard(mtx);

  assert(idx < XBLASHandle::getInstance().size());

  if (create) {
    if (XBLASHandle::getInstance()[idx] == NULL)
      XBLASHandle::getInstance()[idx] = new XBLASHandle(idx);
    handle_mtx.lock();
    myHandles[idx] = XBLASHandle::getInstance()[idx];
	handle_mtx.unlock();
  }

  return XBLASHandle::getInstance()[idx];
}

void XBLASHandle::release(int idx) {
  if (XBLASHandle::getInstance()[idx] != nullptr){
	delete XBLASHandle::getInstance()[idx];
	XBLASHandle::getInstance()[idx] = nullptr;
	handle_mtx.lock();
    myHandles[idx] = nullptr;
    handle_mtx.unlock();
  }
}

//void JobParrallelism(XPipelinePacket *packetPtr) {
void XBLASHandle::JobParrallelism(int id, int cuIdx, XBLASHandle &handle,
	const std::unordered_map <std::string, const float* > in_blob,
	const std::unordered_map <std::string, float* > out_blob,
	std::shared_ptr<XDNNQuantBuf> & qbuf, unsigned batch_id, int streamId,
  std::unordered_map<std::string, XDNNLayerQuantParam*> *quantparam,
  XDNNLayerQuantParam *startQp, XDNNLayerQuantParam *lastQp, XDNNDescriptorDB *descDB,
  int startIdx, int dflStartIdx, int stopIdx, int dflStopIdx) {
	// enqueue job thread
  boost::lock_guard<boost::mutex> guard(m_mtx);
	this->m_workerPoolResult[streamId].emplace_back(this->m_workerPool->enqueue(&threadsWorker, id, cuIdx, &handle, in_blob, out_blob, qbuf.get(), batch_id, streamId, quantparam,
  startQp, lastQp, descDB, startIdx, dflStartIdx, stopIdx, dflStopIdx));
}

XComputeUnit* XBLASHandle::getComputeUnit(int cuIdx)
{
  assert(cuIdx < _computeUnits.size());
  return _computeUnits[cuIdx];
}

int XBLASHandle::getNumComputeUnits()
{
  return _computeUnits.size();
}

std::pair<cl_mem, int> XBLASHandle::getCreateBuffer(int cuIdx,
    void *hostPtr, int size, cl_mem_flags rwFlags) {
  assert(cuIdx < _computeUnits.size());
  return _computeUnits[cuIdx]->getCreateBuffer(hostPtr, size, rwFlags);
}

void XBLASHandle::deleteBuffer(int cuIdx, const void *hostPtr) {
  assert(cuIdx < _computeUnits.size());
  _computeUnits[cuIdx]->deleteBuffer(hostPtr);
}



using boost::property_tree::ptree;
void XBLASHandle::processXclbinJson(const std::string &path) {
  //auto extStart = path.find(".xclbin");
  //assert(extStart != std::string::npos);

  std::string jsonFile = path + ".json";
  std::ifstream file(jsonFile.c_str());
  if (!file.good())
    return; // file does not exist

  std::cout << "[XDNN] loading xclbin settings from " << jsonFile << std::endl;

  ptree pt;
  read_json(file, pt);

  for (ptree::iterator pos = pt.begin(); pos != pt.end(); pos++) {
    std::string key = pos->first;
    std::string val = pos->second.get_value<std::string>();
    if (XDNN_VERBOSE)
      std::cout << "[XDNN]  " << key << ": " << val << std::endl;
    m_kConfig.m_xclbinJsonVals[key] = val;
  }
}

void XBLASHandle::setHost2XMem(const void *src, XMemPtr *x) {
  boost::lock_guard<boost::mutex> guard(m_mtx);
  m_host2XMemMap[src] = x;
}
XMemPtr* XBLASHandle::getHost2XMem(const void *src) {
  //boost::lock_guard<boost::mutex> guard(m_mtx);
  if (m_host2XMemMap.find(src) == m_host2XMemMap.end())
    return NULL;
  else
    return m_host2XMemMap[src];
}

void XBLASHandle::deleteHost2XMem(const void *src) {
  if (src == NULL)
    return;

  XMemPtr *x = getHost2XMem(src);
  if (x == NULL)
    return;

  boost::lock_guard<boost::mutex> guard(m_mtx);
  m_host2XMemMap.erase(src);
}
void XBLASHandle::destroyDeviceMemPODs() {
  // delete device mem objects and device buffers we automatically created.
  // user is reponsible for calling xFree on XMemPtr objects user xMalloc'd
  std::map<const void*, XMemPtr*>::iterator it;
  for (it = m_host2XMemMap.begin(); it != m_host2XMemMap.end(); it++)
    it->second->deleteDeviceMemPODs();
}

void XBLASHandle::waitForResults(int streamId, bool recordPipelineTimes) {
  std::vector<std::future<void>> *futures;
  { 
    boost::lock_guard<boost::mutex> guard(m_mtx);
    /*
     * Note: C++11 standard: 23.2.5/8
     * Rehashing invalidates iterators, changes ordering between elements, and changes which buckets elements appear in, but does not invalidate pointers or references to elements. 
     */
    futures = &this->m_workerPoolResult[streamId];
  }

	// wait for each job in stream
	for(auto && result: *futures)
		result.get();

  {
    boost::lock_guard<boost::mutex> guard(m_mtx);
    this->m_workerPoolResult.erase(streamId);
  }
}

XMemPtr::XMemPtr(void *ptr, size_t sz) :
        m_sizeInBytes(sz), m_srcPtr(ptr) {
}

XMemPtr::~XMemPtr() {
}

// return (kernel,cu) DDR location where this ptr is cached
int XMemPtr::getKernelComputeUnitLocation(const void *ptr) {
  //printf("ANDBG getKernelComputeUnitLocation 0x%x 0x%x (%d)\n", this, ptr, m_hostDeviceMemMap.size());
  std::unordered_map<std::tuple<const void*, int>, XDeviceMemPOD<short>*>::iterator it;
  for (auto it = m_hostDeviceMemMap.begin(); it != m_hostDeviceMemMap.end(); it++) {
    const void *cachedPtr = std::get < 0 > (it->first);
    int cIdx = std::get < 1 > (it->first);
    if (cachedPtr == ptr)
      return cIdx;
  }
  return -1;
}

void XMemPtr::deleteDeviceMemPODs() {
  for (auto it = m_hostDeviceMemMap.begin(); it != m_hostDeviceMemMap.end(); it++)
    delete it->second;

  m_hostDeviceMemMap.clear();
}
XDeviceMemPOD<short>* XMemPtr::getDeviceMemPOD(int computeUnitIdx) {
  boost::lock_guard<boost::mutex> guard(xmem_ptr_mtx);
  auto it =
      m_hostDeviceMemMap.find(std::make_tuple(m_srcPtr, computeUnitIdx));

  if (it == m_hostDeviceMemMap.end() )
  {
      XDeviceMemPOD<short> *devMemPOD = new XDeviceMemPOD<short>();
      devMemPOD->_hostsz = m_sizeInBytes / sizeof(short);
      m_hostDeviceMemMap[std::make_tuple(m_srcPtr, computeUnitIdx)] = devMemPOD;
      return devMemPOD;
  }
  else{
      return it->second;
  }
}

#if defined(CBLAS) || defined(MKL)
/*******************************************************************
 * CBLAS GEMM
 *******************************************************************/
void computeFC(float *weight, float *bias, float *data,
    int M, int N, int K, float *output)
{
  // Caffe provides transposed weights
  int lda = K;
  int ldb = K;
  //printf("ANDBG caffe_cpu_gemm %d %d %d | %d %d %d | %f %d | %f %d\n",
  //    CblasRowMajor, CblasNoTrans, CblasTrans,
  //    M, N, K,
  //    1., lda,
  //    0., ldb);
  cblas_sgemm(CblasRowMajor,
      (CBLAS_TRANSPOSE)CblasNoTrans, (CBLAS_TRANSPOSE)CblasTrans,
      M, N, K, 1.,
      data, lda, weight, ldb, 0., output, N);

  // bias is not transposed
  lda = 1;
  ldb = N;
  std::vector<float> bias_multiplier(M, 1.);
  //printf("ANDBG caffe_cpu_gemm %d %d %d | %d %d %d | %f %d | %f %d\n",
  //    CblasRowMajor, CblasNoTrans, CblasNoTrans,
  //    M, N, 1,
  //    1., lda,
  //    1., ldb);
  cblas_sgemm(CblasRowMajor,
      (CBLAS_TRANSPOSE)CblasNoTrans, (CBLAS_TRANSPOSE)CblasNoTrans,
      M, N, 1, 1.,
      &(bias_multiplier[0]), lda, bias, ldb, 1., output, N);
}
#else
void computeFC(float *weight, float *bias, float *data, int M, int N, int K,
    float *output) {
  printf("ERROR: Can't run FC without building xfdnn C++ with CBLAS/MKL\n");
  exit(1);
}
#endif // CBLAS

void computeSoftmax(float * input, size_t bs, size_t num_elem) {
  //float m = std::numeric_limits<float>::min();

  for (size_t b = 0; b < bs; b++) {
    float * d = &input[b * num_elem];
    float m = *std::max_element(d, d + num_elem);
    double sum = 0.0;
    for (size_t i = 0; i < num_elem; i++) {
      d[i] = expf(d[i] - m);
      sum += d[i];
    }

    for (size_t i = 0; i < num_elem; i++)
      d[i] /= sum;
  }
}

static bool writeAToDevice(XBLASHandle &handle, const char* layerMapStr, short * A, size_t cuIdx,  int idxHBM) {
  XBLASKernelConfig &kcfg = handle.m_kConfig;
  XMemPtr *aPtr = handle.getHost2XMem(A);
  assert(aPtr != NULL);
  /*
   * allocate device memory
   */
  XDeviceMemPOD<short>* devMemPOD = aPtr->getDeviceMemPOD( cuIdx);
  const int sizeToAlloc = aPtr->size();
  if (devMemPOD->m_devicePtr == nullptr) {
    cl_mem_ext_ptr_t inputA_buffer_ext;
    inputA_buffer_ext.flags = kcfg.getDDRMap(
        kcfg.m_ddrBankA[cuIdx]+idxHBM);
    inputA_buffer_ext.obj = (void*)A;
    inputA_buffer_ext.param = 0;
#ifdef DEBUG
    xdnn::XDebugJournal().log("clCreateBuffer_A",
        std::to_string(sizeToAlloc));
#endif

    cl_mem_flags memFlags = CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX
        | CL_MEM_USE_HOST_PTR;
    cl_mem mem = clCreateBuffer(handle.context, memFlags, sizeToAlloc,
        &inputA_buffer_ext, NULL);
    if (mem == 0) {
      printf("ERROR: Failed to allocate A device memory\n");
      return false;
    }
    devMemPOD->m_devicePtr = mem;
  }

#ifdef DEBUG
  xdnn::XDebugJournal().logBlob<short>("write_A_to_device",
      A, sizeToAlloc / sizeof(short), 50 );
#endif

  /*
   * write to device
   */
  int err;
  cl_event writeEvent;
  CLObjs<cl_event> writeDependencies; // start writes only after these events
  CLObjs<cl_event> writeEvents;

  err = clEnqueueMigrateMemObjects(handle.commands, 1,
      &(devMemPOD->m_devicePtr),
      0, // do migration from host
      writeDependencies.m_numObjs, &(writeDependencies.m_objs[0]),
      &writeEvent);
  if (err != CL_SUCCESS) {
    printf("ERROR: Failed to write A to device memory\n");
    return false;
  }

  writeEvents.add(writeEvent, "ma");
  clWaitForEvents(writeEvents.m_numObjs,
      &(writeEvents.m_objs[0]));

  handle.getComputeUnit(cuIdx)->saveDdrBase( devMemPOD->m_devicePtr );
  handle.getComputeUnit(cuIdx)->saveWeightOffsetMap( layerMapStr );

  return true;
}

static std::pair<cl_mem, int> createDdrBuffer(XPipelinePacket &packet,
    int computeUnitIdx, void *hostPtr, int size, cl_mem_flags rwFlags) {
    XBLASHandle &handle = *(packet.xHandle);
    XBLASKernelConfig &kcfg = handle.m_kConfig;

    cl_mem mem;
    int slot = 0;

    // return slot as well
    // (B sets the packet's kernelComputeUnitSlot)
    std::pair<cl_mem, int> memSlot =
        handle.getCreateBuffer(computeUnitIdx,
            hostPtr, size, rwFlags);

    mem = memSlot.first;
    slot = memSlot.second;

    return std::make_pair(mem, slot);
}

static bool writeBToDevice(XPipelinePacket &packet) {
  XBLASHandle &handle = *(packet.xHandle);
  XBLASKernelConfig &kcfg = handle.m_kConfig;
  int computeUnitIdx = packet.kernelComputeUnitIdx;

  XDeviceMemPOD<short> *devMemPOD = packet.bDevMemPOD;

  packet.executor->prepBForDevice(packet);

  /*
   * allocate device memory
   */
  void *memToTransfer = nullptr;
  int sizeToAllocate = 0;
  if ( devMemPOD->m_data.get() == nullptr) {
      memToTransfer = packet.B->get();
      sizeToAllocate = packet.B->size();
#ifdef DEBUG
  xdnn::XDebugJournal().logBlob<short>("write_B_to_device",
      (short*)memToTransfer, packet.B->size() / sizeof(short), 50);
#endif
  } else{
      memToTransfer = devMemPOD->m_data.get();
      sizeToAllocate = devMemPOD->m_data.sizeInBytes();
#ifdef DEBUG
  xdnn::XDebugJournal().logBlob<short>("write_B_to_device",
      (short*)memToTransfer, devMemPOD->m_data.numelem(), 50);
#endif
  }
  if (devMemPOD->m_devicePtr == nullptr) {
    int cuBankIdx = computeUnitIdx;
    assert(cuBankIdx < kcfg.m_ddrBankB.size());
#ifdef ENABLE_HBM
    int ddrBank = kcfg.getDDRMap(4);
#else
    int ddrBank = kcfg.getDDRMap(kcfg.m_ddrBankB[cuBankIdx]);
#endif
    cl_mem_flags rwFlags = CL_MEM_READ_ONLY;

    const bool useComputeUnitMgr =
        (kcfg.m_kernelType == XDNN_KERNEL) ? true : false;

    std::pair<cl_mem, int> memSlot = createDdrBuffer(packet,
        cuBankIdx, memToTransfer, sizeToAllocate, rwFlags);

    cl_mem mem = memSlot.first;
    // B slot determines packet's slot
    packet.kernelComputeUnitSlot = memSlot.second;

    devMemPOD->m_devicePtr = mem;
    if (!devMemPOD->m_devicePtr) {
      printf("ERROR: Failed to allocate B device memory\n");
      return false;
    }
  } else {
    int slot = handle.getComputeUnit(
        computeUnitIdx)->getSlot(memToTransfer);
    assert(slot >= 0);
    packet.kernelComputeUnitSlot = slot;
  }

  {
    int err;
    cl_event writeEvent;

    err = clEnqueueMigrateMemObjects(handle.commands, 1,
        &(devMemPOD->m_devicePtr),
        0, // do migration from host
        packet.writeDependencies.m_numObjs,
        &(packet.writeDependencies.m_objs[0]), &writeEvent);

    if (err != CL_SUCCESS) {
      printf("ERROR: Failed to write B to device memory\n");
      return false;
    }
    packet.writeEvents.add(writeEvent, "mb" + std::to_string(0));
  }

  return true;
}

static XDeviceMemPOD<short>* prepCForDevice(XPipelinePacket &packet) {
  XMemPtr *cPtr = packet.C;

  /*
   * Get pointer to start of blob
   */

  XDeviceMemPOD<short> *devMemPOD = cPtr->getDeviceMemPOD(packet.kernelComputeUnitIdx);
  /*
   * format C blob
   */
  if (devMemPOD->m_devicePtr == nullptr) {
      devMemPOD->m_data.reset(packet.outbufsz);
  }

  return devMemPOD;
}

static bool writeCToDevice(XPipelinePacket &packet, bool doMemWrite) {
  XBLASHandle &handle = *(packet.xHandle);
  XBLASKernelConfig &kcfg = handle.m_kConfig;
  int computeUnitIdx = packet.kernelComputeUnitIdx;

  /*
   * allocate device memory
   */
  XDeviceMemPOD<short> *devMemPOD = packet.cDevMemPOD;
  packet.executor->prepCForDevice(packet);

  void *memToTransfer = devMemPOD->m_data.get();
  int sizeToAllocate = devMemPOD->m_data.sizeInBytes();

  if (devMemPOD->m_devicePtr == 0) {
#ifdef ENABLE_HBM
    int ddrBank = kcfg.getDDRMap(11);
#else
    int ddrBank = kcfg.getDDRMap(kcfg.m_ddrBankC[computeUnitIdx]);
#endif
    cl_mem_flags rwFlags = CL_MEM_READ_WRITE;

    std::pair<cl_mem, int> memSlot = createDdrBuffer(packet, computeUnitIdx, memToTransfer, sizeToAllocate, rwFlags);

    cl_mem mem = memSlot.first;
    devMemPOD->m_devicePtr = mem;
    if (!devMemPOD->m_devicePtr) {
      printf("ERROR: Failed to allocate C device memory\n");
      return false;
    }

    /*
     * Need to init & write C or we will get ECC error
     */
    cl_event writeDone;
    int err = clEnqueueMigrateMemObjects(handle.commands, 1,
        &(devMemPOD->m_devicePtr),
        CL_MIGRATE_MEM_OBJECT_CONTENT_UNDEFINED, 0, NULL, &writeDone);
    assert(err == CL_SUCCESS);
    clWaitForEvents(1, &writeDone);
    err = clEnqueueWriteBuffer(handle.commands, devMemPOD->m_devicePtr, CL_TRUE,
        0, sizeToAllocate, memToTransfer, 0, NULL, &writeDone);
    assert(err == CL_SUCCESS);
    clWaitForEvents(1, &writeDone);
  }

  return true;
}

static int schedulePacket(XBLASHandle &handle) {
  int kernelIdx = -1;
  int cuIdx = -1;
  int maxFreeSlots = 0;

  // round-robin scheduler
  static std::map<XBLASHandle*, std::vector<int>*> cuIdxCounters;
  const int numKernels = 1;
  static std::map<XBLASHandle*, int> kernelIdxCounters;
  if (cuIdxCounters.find(&handle) == cuIdxCounters.end()) {
    cuIdxCounters[&handle] = new std::vector<int>(numKernels, 0);
    kernelIdxCounters[&handle] = 0;
  }
  std::vector<int> &cuIdxCounter = *(cuIdxCounters[&handle]);
  int &kernelIdxCounter = kernelIdxCounters[&handle];

  kernelIdx = kernelIdxCounter;
  cuIdx = cuIdxCounter[kernelIdxCounter];

  cuIdxCounter[kernelIdxCounter] = ++cuIdxCounter[kernelIdxCounter]
      % handle.getNumComputeUnits();

  kernelIdxCounter = ++kernelIdxCounter % numKernels;

  assert(kernelIdx >= 0 && cuIdx >= 0); // no free slot
  // TODO: if no free CU, evict to make a free slot
  // TODO: or block until a slot becomes free

  if (XDNN_VERBOSE)
    std::cout << "[XDNN] xdnnSchedulerAssignPacketToComputeUnit: "
        << handle.getId() << "," << kernelIdx << "," << cuIdx << " "
        << maxFreeSlots << std::endl;

  // Assign CU
  return cuIdx;
}

template
void XDNNQuantize(const float *in, short *out, int sz, const float thresh, const int bitWidth, bool doRounding=false);

template
void XDNNQuantize(const float *in, char *out, int sz, const float thresh, const int bitWidth, bool doRounding=false);


template<typename DType>
void XDNNQuantize(const float *in, DType *out, const int sz, const float thresh,
    const int bitWidth, bool doRounding)
{
  //XTimer t;
  const int maxRange = 1 << bitWidth;
  const float halfRange = (maxRange / 2) - 1;
  const float quantFactor = halfRange / thresh;
  assert ( thresh > FLT_MIN);
  for (int i = 0; i < sz; i++) {
    float val = in[i];
    val = std::max( std::min ( val, thresh ), -thresh );

    // apply quantization formula
    val *= quantFactor;
    if (doRounding)
      val = std::round(val);

    out[i] = DType(val);
  }
}

// Can be multi-threaded
bool threadDoBeforeWork(XPipelinePacket *packetPtr /*XComputeUnit * cu, float* in, short * hwInBuf, short * hwOutBuf*/) {
	bool rtn = true;
    XPipelinePacket &packet = *packetPtr;
    XBLASKernelConfig &kcfg = packet.xHandle->m_kConfig;

	for ( const auto & kv : *packet.xdnnInput)
	{
		size_t inputOffset = kv.second->getHWAddrOffset();
		char * qbuf = (char*)packet.inbuf + inputOffset;
		XDNNQuantize( packet.float_in[kv.first],  (short*)qbuf, kv.second->getSize(), packet.startQp->threshIn, packet.startQp->bitWidth);
	}
    packet.recordEventTimestamp("quant");

    // look up mapping for bPtr, cPtr and copy data to device
    XMemPtr *bPtr = xRegMem(*packet.xHandle, packet.inbuf, packet.inbufsz);
    XMemPtr *cPtr = xRegMem(*packet.xHandle, packet.outbuf, packet.outbufsz);

    // A must be pre-loaded
    assert(packet.A == NULL);
    packet.bDevMemPOD = bPtr->getDeviceMemPOD(packet.kernelComputeUnitIdx);

    if (packet.cDevMemPOD == NULL)
      packet.cDevMemPOD = prepCForDevice(packet);
    if (packet.cDevMemPOD == NULL)
      return false;

    // enqueue writes to DDR in this stage
    // TODO verify B DDR bank map for this kernel
    assert(packet.bDevMemPOD != NULL);
    bool ret = writeBToDevice(packet);
    ret = writeCToDevice(packet, /*doMemWrite*/true);

    packet.recordEventTimestamp("prep");

	cl_event prepEvent = 0;
	//assert(packet.writeEvents.m_numObjs == 1);
	if (packet.writeEvents.m_numObjs == 1) {
	  prepEvent = packet.writeEvents.m_objs[0];
	  clWaitForEvents(1, &prepEvent);
	  packetPtr->recordEventTimestamp("ddr_wr");
	}

    return rtn;
}

bool threadDoExecWork(XPipelinePacket *packetPtr) {
    XPipelinePacket &packet = *packetPtr;
    packet.executor->execute(packet);
    // start timer to measure kernel exec time
    packet.recordEventTimestamp("submit");
    packet.kernelExecTimer.reset();
    packetPtr->rwm->wait();
    return true;
}

void threadDoAfterWork(XPipelinePacket *packetPtr /*, XComputeUnit * cu, short * hwOutBuf*/) {
    XPipelinePacket &packet = *packetPtr;
    XBLASHandle &handle = *(packet.xHandle);
    XComputeUnit* cu = handle.getComputeUnit(packet.kernelComputeUnitIdx);
    cu->incNumImagesProcessed();

    packetPtr->recordEventTimestamp("exec");
    packetPtr->kernelExecTime = packetPtr->kernelExecTimer.elapsed();

    // Read back results from the device
    const int cBytes = packetPtr->cDevMemPOD->m_data.sizeInBytes();
    cl_mem inout_c = packetPtr->cDevMemPOD->m_devicePtr;
    cl_event readEvent;
    int err = clEnqueueMigrateMemObjects(handle.commands, 1, &(inout_c),
        CL_MIGRATE_MEM_OBJECT_HOST, 0, NULL, &readEvent);
    assert(err == CL_SUCCESS);
    packetPtr->readEvents.add(readEvent);
    clWaitForEvents(1, &readEvent);
    packetPtr->recordEventTimestamp("ddr_rd");
#ifdef DEBUG // cKernel
    void *C = packet.cDevMemPOD->m_data.get();
    xdnn::XDebugJournal().logBlob<short>("read_C_from_device",
        (short*)C, packet.cDevMemPOD->m_data.numelem(), 50);
#endif

    packet.executor->unpackCFromDevice(packet);

    /*
     * C result in each batch is in shorts,
     * straight from FPGA. Convert short->float.
     * Don't need << 16 like older kernel because
     * last step always does scale which converts
     * 32 bit accumulator result to 16 bit final result
     */
	assert( packet.float_out.size() == packet.xdnnOutput->size());
	short * qbuf = (short*)packet.outbuf;
	assert (qbuf);
	for (const auto & kv: *(packet.xdnnOutput)){
	  //assert(output.find(kv.first) != output.end());
	  float * fout = packet.float_out[kv.first];
	  assert(fout);
	  unsigned outSize = kv.second->getSize();
#ifdef DEBUG // cKernel
	  std::string n = "unquantize_C_" + kv.first;
	  xdnn::XDebugJournal().logBlob<short>(n.c_str() ,
		  qbuf, outSize, outSize);
#endif

	  std::string qparam_name = kv.second->getLayer();
	  if ( packet.quantParam->find(qparam_name) != packet.quantParam->end() ){
		const auto & qp = packet.quantParam->find(qparam_name)->second;
		XDNNUnQuantize(qbuf, fout, outSize, qp->threshOut, qp->bitWidth);
	  } else {
		if (XDNN_VERBOSE)
			std::cout << LOCATION << "WARNING: Quantization parameters for layer " << qparam_name << " not found!" << std::endl;
	    if (packet.lastQp)
			XDNNUnQuantize(qbuf, fout, outSize, packet.lastQp->threshOut, packet.lastQp->bitWidth);
	  }
	  qbuf += outSize;
	}

    packet.recordEventTimestamp("post");

}

/*******************************************************************
 * FPGA XBLAS/XDNN
 *******************************************************************/

void xblasEnqueueJob(XBLASHandle &handle,
    const std::unordered_map <std::string, const float* > in_blob,
    const std::unordered_map <std::string, float* > out_blob,
    std::shared_ptr<XDNNQuantBuf> & qbuf, unsigned batch_id, int streamId,
    std::unordered_map<std::string, XDNNLayerQuantParam*> *quantparam,
    XDNNLayerQuantParam *startQp, XDNNLayerQuantParam *lastQp, XDNNDescriptorDB *descDB,
    int startIdx, int dflStartIdx, int stopIdx, int dflStopIdx)
{
  boost::lock_guard<boost::mutex> guard(enqueue_mtx);
  static unsigned packetCounter = 0;

  handle.JobParrallelism(packetCounter++, schedulePacket(handle), handle,
    in_blob, out_blob, qbuf, batch_id, streamId, quantparam,
    startQp, lastQp, descDB, startIdx, dflStartIdx, stopIdx, dflStopIdx);
}

// Job Parrallelism Task
//void threadsWorker(XPipelinePacket *packetPtr){
static void threadsWorker(int id, int cuIdx, XBLASHandle *handle,
    const std::unordered_map <std::string, const float* > in_blob,
    const std::unordered_map <std::string, float* > out_blob,
    XDNNQuantBuf *qbuf, unsigned batch_id, int streamId,
    std::unordered_map<std::string, XDNNLayerQuantParam*> *quantparam,
    XDNNLayerQuantParam *startQp, XDNNLayerQuantParam *lastQp, XDNNDescriptorDB *descDB,
    int startIdx, int dflStartIdx, int stopIdx, int dflStopIdx)
{
  XPipelinePacket packet;
  std::unique_ptr<xdnn::RegisterWriteManager> rwm(
    new xdnn::RegisterWriteManager(handle->xdev)); // TODO create memory pool

  packet.id = id;
  packet.kernelComputeUnitIdx = cuIdx;
  packet.rwm = rwm.get();
  packet.recordEventTimestamp("start");
  packet.saveToJournal(); // ANDBG JOURNAL

  packet.float_in = in_blob;
  packet.float_out = out_blob;

  packet.quantParam = quantparam;
  packet.startQp = startQp;
  packet.lastQp = lastQp;
  packet.xHandle = handle;
  packet.xdnnInput = &descDB->getInputDesc();
  packet.xdnnThroughputInterbuf = descDB->getThroughputInterbufDesc();
  packet.xdnnOutput = &descDB->getOutputDesc();
  packet.inbuf = qbuf->getInBuf(batch_id);
  packet.outbuf = qbuf->getOutBuf(batch_id);
  packet.inbufsz = qbuf->getInSize() * sizeof(short);
  packet.outbufsz = qbuf->getOutSize() * sizeof(short);
  packet.xdnnStartIdx = startIdx;
  packet.xdnnDFLStartIdx = dflStartIdx;
  packet.xdnnStopIdx = stopIdx;
  packet.xdnnDFLStopIdx = dflStopIdx;

  if (XDNN_VERBOSE)
  {
    printf("[XDNN] fpgaXdnn run instructions dfl:[%d, %d] mb:[%d, %d]\n",
      packet.xdnnDFLStartIdx, packet.xdnnDFLStopIdx,
      packet.xdnnStartIdx, packet.xdnnStopIdx);
  }

  packet.executor.reset(new XDNNPacketExecutor());
  packet.streamId = streamId;
  packet.B = xRegMem(*packet.xHandle, packet.inbuf, packet.inbufsz, false  );
  packet.C = xRegMem(*packet.xHandle, packet.outbuf, packet.outbufsz, false  );

	if (!threadDoBeforeWork(&packet))
		return;
	if (!threadDoExecWork(&packet))
	  return;
	threadDoAfterWork(&packet);
  packet.recordEventTimestamp("done");
  packet.emitProfilingInfo();
  packet.cleanup();
}

/*******************************************************************
 * XBLAS Client API
 *******************************************************************/

int getNumCUs( XBLASHandle * handle )
{
	int numcus = 0;
	std::vector<butler::xCU>& xCUVec = handle->m_xCUVec;
	numcus = xCUVec.size();
	return numcus;
}

int acquireCUs( XBLASHandle * handle )
{
	int numcus = 0;

	std::vector<butler::xCU>& xCUVec = handle->m_xCUVec;
	for (int i = 0; i < xCUVec.size(); i++){
#ifdef XRT_20191
		xrtcpp::acquire_cu_context(xCUVec[i].getXDev(), i);
#else
		xrtcpp::acquire_cu_context(xCUVec[i].getXDev(), xCUVec[i].getCUIdx());
#endif
	}
	numcus = getNumCUs(handle);
	return numcus;
}

static int getDDRBankFromButlerBitmask(unsigned bitmask)
{
  const int numBits = sizeof(bitmask) * CHAR_BIT;
  for (int i=0; i < numBits; i++)
    if (bitmask & (1 << i))
      return i;
  return -1;
}

int xblasCreate(XBLASHandle *&handle, const char *xclbinInput,
    const char *kernelName, int deviceIdx, int AcquireID,
    const char *subscribeID, const char *publishID) {
  boost::lock_guard<boost::mutex> guard(xblas_mtx);
  int err;
  handle = XBLASHandle::get(deviceIdx);
  if (handle->m_isInitialized && AcquireID == 0)
    return 0;

  int numTries = 0;
	while(handle->m_resource.myErrCode != butler::errCode::SUCCESS){
		std::cout << "-------------------" << std::endl;
		std::cout << "Speaking to Butler " << std::endl;
		std::pair<butler::Alloc, std::vector<butler::xCU>> temp;
		handle->m_xCUVec.clear();
    if (AcquireID > 0)
			temp = handle->m_mgr.acquireID(AcquireID);
    else if (AcquireID < 0)
			temp = handle->m_mgr.acquireCU(kernelName, xclbinInput);
    else if (subscribeID && !std::string(subscribeID).empty())
    {
      temp = handle->m_mgr.subscribeService(subscribeID);
      handle->m_subscribeId = std::string(subscribeID);
    }
    else
    {
    	temp = handle->m_mgr.acquireFPGA(xclbinInput);
      if (publishID && !std::string(publishID).empty())
      {
        handle->m_mgr.publishService(publishID, {temp.first.myHandle});
        handle->m_publishId = std::string(publishID);
      }
    }

		handle->m_resource = temp.first;
		handle->m_xCUVec = temp.second;
		if (handle->m_resource.myErrCode == butler::errCode::CLIENT_CONNECTION_ERROR){
			std::cout << "client connection error" << std::endl;
		}
		std::cout << "Response from Butler is: " << std::endl;
		std::cout << handle->m_resource << std::endl;

    if (numTries >= 10)
    {
      std::cerr << "Butler connection failed, exiting after too many errors" << std::endl;
      exit(1);
    }

    numTries++;
	}
	// get CU for xdnn kernel
	std::vector<butler::xCU>& xCUVec = handle->m_xCUVec;
	auto it = xCUVec.begin();
	while (it != xCUVec.end()){
		if (it->getKernelName().compare(XBLAS_KERNEL_NAME) != 0)
			it = xCUVec.erase(it);
		else
			it++;
	}
	assert (xCUVec.size() != 0);
	std::string str = xCUVec[0].getXCLBINPath();
	const char *xclbin = str.c_str();

	if (AcquireID > 0) handle->m_doRelease = false;

  handle->device_id = xCUVec[0].getOCLDev();

  /*
   * Create compute context
   */
  handle->context = clCreateContext(0, 1, &handle->device_id, NULL, NULL, &err);
  if (!handle->context) {
    printf("ERROR: Failed to create a compute context\n");
    return -1;
  };

  int commandQueueProperties = CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE;
  if (XBLAS_EMIT_PROFILING_INFO)
    commandQueueProperties |= CL_QUEUE_PROFILING_ENABLE;
  handle->commands = clCreateCommandQueue(handle->context, handle->device_id,
      commandQueueProperties, &err);
  if (!handle->commands) {
    printf("ERROR: Failed to create command queue\n");
    printf("ERROR: code %i\n", err);
    return -1;
  }

  /*
   * ONEHACK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   */
  // Read xclbin and create program
    std::string fnm = std::string(xclbin);
    std::ifstream stream(fnm);
    stream.seekg(0,stream.end);
    size_t size = stream.tellg();
    stream.seekg(0,stream.beg);
    std::vector<char> xclbinChar(size);
    stream.read(xclbinChar.data(),size);
    auto data = reinterpret_cast<const unsigned char*>(xclbinChar.data());
    cl_int status = CL_SUCCESS;
    handle->program = clCreateProgramWithBinary(handle->context,1,&handle->device_id,&size,&data,&status,&err);
  /*
   * load xclbin settings from xclbin json file (if exists)
   */
  handle->processXclbinJson(xclbin);

  XBLASKernelConfig &kcfg = handle->m_kConfig;

  std::string ddrStr;
  if (kcfg.m_xclbinJsonVals.find("XDNN_DDR_BANK") != kcfg.m_xclbinJsonVals.end())
  {
    // extract DDR info from xclbinJson
    ddrStr = kcfg.m_xclbinJsonVals["XDNN_DDR_BANK"];
  }
  else
  {
    // auto-detect DDR bank from butler info
    std::vector<butler::xCU>& xCUVec = handle->m_xCUVec;
    for (int i=0; i < xCUVec.size(); i++)
    {
      int ddrBank
        = getDDRBankFromButlerBitmask(xCUVec[i].getKernelArgMemIdxMapAt(0));
      assert(ddrBank >= 0);
      if (!ddrStr.empty())
        ddrStr += ",";
      ddrStr += std::to_string(ddrBank);
    }
  }
  if (getenv("XDNN_DDR_BANK"))
    ddrStr = std::string(getenv("XDNN_DDR_BANK"));
  assert(!ddrStr.empty());
  kcfg.setDDRMap(ddrStr);

	handle->xdev = xclGetXrtDevice(handle->device_id,&err);
	if (err){
		std::runtime_error("xclGetXrtDevice error in XBLASHandle");
	}

	/*
	 * Create compute unit manager
	 * (to manage memory buffers and compute load)
	 */
	std::vector <int> ddrBanks;
	int numComputeUnits = kcfg.m_ddrBankB.size();
	assert(numComputeUnits == xCUVec.size());
	for (size_t cu = 0; cu < numComputeUnits; cu++){
		handle->_computeUnits.push_back(
			  new XComputeUnit(handle, cu, kcfg.getDDRMap(kcfg.m_ddrBankB[cu]),
			  /*maxNumSlots=*/256, /*maxMemSize=*/std::numeric_limits<int>::max()));
	   //ddrBanks.push_back(kcfg.getDDRMap(kcfg.m_ddrBankB[cu]));
	}


	int numcus = 0;
	numcus = acquireCUs(handle);
	handle->numcus = numcus;
	handle->m_isInitialized = true;
	butler::Alloc temp = handle->m_resource;
	return temp.myHandle;
}

void xblasDestroy(XBLASHandle *handle) {
  handle->release(handle->getId());
}

// delete XMemPtr object & deviceMem object (which also frees device buffer)
void xFree(XBLASHandle &handle, XMemPtr *memPtr, bool freeMemPtr) {
  if (!memPtr)
    return;

  int numCUs = handle.getNumComputeUnits();
  for (int ci = 0; ci < numCUs; ci++)
	  handle.deleteBuffer(ci, memPtr->get());
  handle.deleteHost2XMem(memPtr->get());
  memPtr->deleteDeviceMemPODs();

  if (freeMemPtr)
    delete memPtr;
}

void xblasLoadA(XBLASHandle &handle, short *A,
    const char *layerMapStr, int prefCuIdx) {

	// Prepare A blob & write to DDR
	assert(prefCuIdx < handle.getNumComputeUnits());
	XBLASKernelConfig &kcfg = handle.m_kConfig;

	// load data into DDR of all kernels' compute units
	// seems like assumption is, each CU is mapped to a different ddr bank
	// ki is always 0 and numKernels always 1 for now,
	// so we are looping through compute units of first kernel/xclbin
	// we are figuring out number of compute units from number of ddr banks
	for (int ci = 0; ci < kcfg.m_ddrBankA.size(); ci++) {
	  if (prefCuIdx != -1 && prefCuIdx != ci) {
		//std::cout << "xblasLoadA skip CU " << ci << std::endl;
		continue;
	  }
	  assert(writeAToDevice(handle, layerMapStr, A, ci, 0 ));
	}
}

void xblasHBMLoadA(XBLASHandle &handle, std::vector<std::vector<int> > &dataBlobsHBM, const char *layerMapStr, int prefCuIdx) {
  // Prepare A blob & write to DDR
  //std::cout<<prefCuIdx<<" "<<handle.m_computeUnitManagers[0]->getNumComputeUnits()<<std::endl;
  XBLASKernelConfig &kcfg = handle.m_kConfig;

    for (int ci = 0; ci < kcfg.m_ddrBankA.size(); ci++) {
      if (prefCuIdx != -1 && prefCuIdx != ci) {
        //std::cout << "xblasLoadA skip CU " << ci << std::endl;
        continue;
      }

      //loading weights across multiple ddr banks for HBM mode
      int numberOfDdrBanks = 6;
      int *blobToLoad = NULL;
      short *blobToLoadInShorts = NULL;
      for (int i=0; i<numberOfDdrBanks; i++)
      {
        blobToLoad = &(dataBlobsHBM[i][0]);//2D vectors do not store data in contiguous blocks, if we go past first vectors values we do not go into second vectors values
        blobToLoadInShorts = (short*)blobToLoad;
        assert(writeAToDevice(handle, layerMapStr, blobToLoadInShorts, ci,i));
      }

    }
}


XMemPtr* xRegMem(XBLASHandle &handle, void *hostMemPtr, size_t sz, bool copy) {
  XMemPtr * devPtr = handle.getHost2XMem(hostMemPtr);
  if (!devPtr) {
    devPtr = new XMemPtr(hostMemPtr, sz);
    handle.setHost2XMem(hostMemPtr, devPtr);
    //std::cerr << "Mapped " << hostMemPtr << " to " << devPtr << std::endl;
  }
  return devPtr;
}


/***************************************************************
 * Manage load/unload of handles
 **************************************************************/
void my_init(void){
	for (int i = 0; i < MAX_HANDLES; i++){
		myHandles[i] = nullptr;
	}
}

void my_fini(void){
	// release resource
	for (int i = 0; i < MAX_HANDLES; i++){
		if (myHandles[i] != nullptr){
            //myHandles[i]->release(i);
            std::cout << "[XBLAS] Warning: XBLASHandle # " << i << " was not destroyed." << std::endl;
		}
	}
}
