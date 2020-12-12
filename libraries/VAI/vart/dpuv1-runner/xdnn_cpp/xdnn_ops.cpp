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
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <sstream>
#include <iterator>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <limits>
#include <string>
#include <vector>
#include "xdnn.h"
#include "xmlrt.h"
#include "xdnn_ops.h"
#include "version.h"
#include <unistd.h>
// Soren Soe's API
#include "CL/cl_ext_xilinx.h"
#include "experimental/xrt++.hpp"
#include <sys/stat.h>


#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#define XDNN_KERNEL_CONTROL                              0x0000
#define XDNN_AXI_ARBITER_WEIGHTS                         0x0130
#define XDNN_AXI_MAX_BURST_SIZE                          0x0134

#define XDNN_USE_COMDQ 1

#define XDNN_KERNEL_CSR_AWIDTH 10 //CSR address width per physical kernel
#define XDNN_MAIN_BLOCK_OFFSET (1 << XDNN_KERNEL_CSR_AWIDTH)

#define XDNN_KERNEL_CONFIGURATION                        0x0004
#define XDNN_IMAGE_EXECUTION_COUNTER                     0x0140

#define XDNN_GLOBAL_CONFIG                                0x001C
#define XDNN_KERNEL_CONFIGURATION_0                      0x0180
#define XDNN_KERNEL_CONFIGURATION_1                      0x0184
#define XDNN_KERNEL_CONFIGURATION_2                      0x0188
#define XDNN_KERNEL_CONFIGURATION_3                      0x018C
#define XDNN_KERNEL_CONFIGURATION_4                      0x0190
#define XDNN_KERNEL_CONFIGURATION_5                      0x0194
#define XDNN_INTERRUPT_STATUS                            0x0010
#define XDNN_INTERRUPT_MASK                              0x0014
#define XDNN_IMAGE_DDR_BASE_ADDR_0                       0x0020
#define XDNN_IMAGE_DDR_BASE_ADDR_1                       0x0024
#define XDNN_FILTER_DDR_BASE_ADDR_0                      0x0028
#define XDNN_FILTER_DDR_BASE_ADDR_1                      0x002C
#define XDNN_DOWNLOAD_DDR_OFFSET                         0x0030
#define XDNN_UPLOAD_DDR_OFFSET                           0x0034
#define XDNN_IMAGE_INDEX                                 0x0038
#define XDNN_COMMAND_QUEUE_CONTROL                       0x003C
#define XDNN_IMAGE_COMPLETION_VECTOR_0                   0x0040
#define XDNN_IMAGE_COMPLETION_VECTOR_1                   0x0044
#define XDNN_IMAGE_COMPLETION_VECTOR_2                   0x0048
#define XDNN_IMAGE_COMPLETION_VECTOR_3                   0x004C
#define XDNN_IMAGE_COMPLETION_VECTOR_4                   0x0050
#define XDNN_IMAGE_COMPLETION_VECTOR_5                   0x0054
#define XDNN_IMAGE_COMPLETION_VECTOR_6                   0x0058
#define XDNN_IMAGE_COMPLETION_VECTOR_7                   0x005C
#define XDNN_CNN_COMMAND                                 0x0060
#define XDNN_CONV2D_GLOBAL_DSP_SCALE_PARAM               0x0098
#define XDNN_CONV2D_GLOBAL_DSP_BIAS_PARAM                0x009C
#define XDNN_INPUT_IMAGE_BASE_ADDR                       0x0070
#define XDNN_INPUT_IMAGE_WIDTH                           0x0064
#define XDNN_INPUT_IMAGE_HEIGHT                          0x0068
#define XDNN_INPUT_IMAGE_CHANNEL                         0x006C
#define XDNN_OUTPUT_IMAGE_BASE_ADDR                      0x00B0
#define XDNN_OUTPUT_IMAGE_WIDTH                          0x00A0
#define XDNN_OUTPUT_IMAGE_HEIGHT                         0x00A4
#define XDNN_OUTPUT_IMAGE_CHANNEL                        0x00A8
#define XDNN_CNN_KERNEL                                  0x00C4
#define XDNN_DEST_CONCAT_INFO                            0x00AC
#define XDNN_SCRIPT_MEMORY_CONTROL                       0x0100

#define XDNN_CROSSBAR_TABLE_SETTING_0_3                  0x0110
#define XDNN_CROSSBAR_TABLE_SETTING_4_7                  0x0114
#define XDNN_CROSSBAR_TABLE_SETTING_8_11                 0x0118
#define XDNN_CROSSBAR_TABLE_SETTING_12_15                0x011C
#define XDNN_CROSSBAR_TABLE_CONTROL                      0x0120
#define XDNN_CROSSBAR_TABLE_POINTER_0                    0x00F4
#define XDNN_CROSSBAR_TABLE_POINTER_1                    0x00F8

#define XDNN_INPUT_FULL_SECTION_NUMBER                   0x0074
#define XDNN_INPUT_REPLICATION_SECTION_NUMBER            0x0078
#define XDNN_INPUT_REPLICATION_UNIT_NUMBER               0x007C
#define XDNN_INPUT_REPLICATION_UNIT_WIDTH                0x0080

#define XDNN_OUTPUT_FULL_SECTION_NUMBER                  0x00B4
#define XDNN_OUTPUT_REPLICATION_SECTION_NUMBER           0x00B8
#define XDNN_OUTPUT_REPLICATION_UNIT_NUMBER              0x00BC
#define XDNN_OUTPUT_REPLICATION_UNIT_WIDTH               0x00C0

#define XDNN_FILTER_DDR_ADDR_OFFSET                      0x00D8
#define XDNN_FILTER_AXI_READ_COUNT                       0x00DC
#define XDNN_FILTER_SRC_ITERATION_COUNT                  0x00E0
#define XDNN_FILTER_DSP_DATA_COUNT                       0x00E4

#define XDNN_ELT_WISE_SEC_SRC_AM_ADDR                    0x0094
#define XDNN_ELT_WISE_SEC_SRC_CHANNEL                    0x0090
#define XDNN_ELT_WISE_SEC_SRC_DDR_ADDR                   0x008C

#define XDNN_TILING_CONTROL_0                            0x00EC
#define XDNN_SRC_AM_BUFFER_0                             0x0084
#define XDNN_SRC_AM_BUFFER_1                             0x0088

#define INLINE_MAXPOOL_OPERATION                         0x00CC
#define INLINE_MAXPOOL_INPUT_WIDTH                       0x00D0
#define INLINE_MAXPOOL_INPUT_HEIGHT                      0x00D4

#define XDNN_DEST_AM_BUFFER_OFFSET_ADDR                  0x00C8
#define XDNN_FILTER_CONCATENATED_OUTPUT_CHANNEL_OFFSET   0x00E8

#define XDNN_GENERAL_COUNTER                             0x0144
#define XDNN_CONVOLUTION_COUNTER                         0x0148
#define XDNN_MISC_COUNTER                                0x014C
#define XDNN_DEBUG_CONTROL                               0x0150
#define XDNN_DEBUG_CONTROL_1                             0x0154
#define XDNN_DEBUG_CONTROL_2                             0x0158
#define XDNN_ERROR_LOG                                   0x0170

#ifdef ENABLE_HBM

#define XDNN_HBM_FILTER_DDR2_BASE_ADDR_0                 0x0190
#define XDNN_HBM_FILTER_DDR2_BASE_ADDR_1                 0x0194
#define XDNN_HBM_FILTER_DDR3_BASE_ADDR_0                 0x0198
#define XDNN_HBM_FILTER_DDR3_BASE_ADDR_1                 0x019C
#define XDNN_HBM_FILTER_DDR4_BASE_ADDR_0                 0x01A0
#define XDNN_HBM_FILTER_DDR4_BASE_ADDR_1                 0x01A4
#define XDNN_HBM_FILTER_DDR5_BASE_ADDR_0                 0x01A8
#define XDNN_HBM_FILTER_DDR5_BASE_ADDR_1                 0x01AC
#define XDNN_HBM_FILTER_DDR6_BASE_ADDR_0                 0x01B0
#define XDNN_HBM_FILTER_DDR6_BASE_ADDR_1                 0x01B4

#endif

#define XDNN_CONV2D_FILTERS_DDR_ADDR_OFFSET              0x007C
#define XDNN_DSP_PRE_SCALE_RIGHT_SHIFT_DDR_ADDR_OFFSET   0x0080
#define XDNN_DSP_SCALE_DDR_ADDR_OFFSET                   0x0084
#define XDNN_DSP_POST_SCALE_RIGHT_SHIFT_DDR_ADDR_OFFSET  0x0088

#define XDNN_CAPABILITY_ID                               0x00B4

/* 
 * Typically 300MHz / 4 = 75MHz.  
 * DATA_CLK is actually the AXI_CLK which will typically be kept at 300MHz, but could be changed.
 * http://confluence.xilinx.com/display/XIP/XDNN+v3+Debug
 */
#define DATA_CLK 75000 // 75MHz

using namespace std;

static boost::mutex g_mtx;

/****************************************
 * KernelInterface (CSR reader/writer)
 ****************************************/
KernelInterface::KernelInterface(XBLASHandle *handle) :
                _handle(handle)
{
    // collect CSR base addr for each CU
    auto &xclbinJsonVals = handle->m_kConfig.m_xclbinJsonVals;
    std::string csrStr;
    if (xclbinJsonVals.find("XDNN_CSR_BASE") != xclbinJsonVals.end())
      csrStr = xclbinJsonVals["XDNN_CSR_BASE"];
    else
    {
     // auto-detect CSR addrs from butler
      std::vector<butler::xCU>& xCUVec = handle->m_xCUVec;
      std::stringstream ss;
      for (int i=0; i < xCUVec.size(); i++)
      {
        if (i > 0)
          ss << ",";
        ss << std::hex << std::showbase << xCUVec[i].getBaseAddr();
      }
      csrStr = ss.str();
    }
    if (getenv("XDNN_CSR_BASE"))
        csrStr = std::string(getenv("XDNN_CSR_BASE"));

    if (!csrStr.empty()) {
        if (XDNN_VERBOSE)
            std::cout << "[XDNN] using custom CSR base " << csrStr << std::endl;

        std::vector < std::string > words = XMLRTUtils::split(csrStr, ",");

        for (int i = 0; i < words.size(); i++) {
            const std::string &word = words[i];
            size_t value = 0;
            size_t valueRWM = 0;

            if (word.find_first_of('x') != std::string::npos) {
                // it's a hex string
                value = strtol(word.c_str(), NULL, 16);
            } else {
                std::cout << "ERROR: " << word << " is not a valid CSR address "
                        << std::endl;
                exit(1);
            }

            std::vector<size_t> addrs;
            std::vector<size_t> addrsRWM;
            {
              // add dedicated block for XDNN_V3
                addrs.push_back(value - XDNN_MAIN_BLOCK_OFFSET);
                addrsRWM.push_back(valueRWM - XDNN_MAIN_BLOCK_OFFSET);
            }
            addrs.push_back(value);
            addrsRWM.push_back(valueRWM);
            _csrBase.push_back(addrs);
#ifdef XRT_20191
            _csrBaseRWM.push_back(addrs);
#else
            _csrBaseRWM.push_back(addrsRWM);
#endif
        }
    } else {
        std::cout
        << "ERROR: CSR mapping not found. Please provide XDNN_CSR_BASE in xclbin.json"
        << std::endl;
        exit(1);
    }

    // collect SLR idx for each CU
    std::string slrStr;
    if (handle->m_kConfig.m_xclbinJsonVals.find("XDNN_SLR_IDX")
            != handle->m_kConfig.m_xclbinJsonVals.end())
        slrStr = handle->m_kConfig.m_xclbinJsonVals["XDNN_SLR_IDX"];
    if (getenv("XDNN_SLR_IDX"))
        slrStr = std::string(getenv("XDNN_SLR_IDX"));

    if (!slrStr.empty()) {
        if (XDNN_VERBOSE)
            std::cout << "[XDNN] using custom SLR idx " << slrStr << std::endl;

        std::vector < std::string > words = XMLRTUtils::split(slrStr, ",");

        for (int i = 0; i < words.size(); i++)
            _cuIdx2SlrIdx.push_back(std::stoi(words[i]));
    } else 
        for (int i=0; i < _csrBase.size(); i++)
          _cuIdx2SlrIdx.push_back(1); // use defaults

    assert(_cuIdx2SlrIdx.size() == _csrBase.size());

    int err;
    _regMap = clCreateBuffer(_handle->context, CL_MEM_REGISTER_MAP,
            sizeof(cl_int), NULL, &err);
    assert(err == CL_SUCCESS);

    // send cmd to clear script memory
    XBLASKernelConfig &kcfg = handle->m_kConfig;
	for (int ci = 0; ci < kcfg.m_ddrBankA.size(); ci++) {

		int cmd = 0x2;

		if (XDNN_VERBOSE)
			printf("Command Buffer Clear 0x%x\n", cmd);
		this->writeIntAllBlocks(ci, XDNN_SCRIPT_MEMORY_CONTROL, cmd);
	}
    this->extractKernelConfig();
}

long KernelInterface::computeCsrBaseAddr(int cuIdx, int blockIdx, RegisterWriteManager *rwm) {
  assert(cuIdx < _csrBase.size());
  if (rwm == nullptr)
	  return _csrBase[cuIdx][blockIdx]
		+ (_cuIdx2SlrIdx[cuIdx] << XDNN_KERNEL_CSR_AWIDTH);
  else
	  return _csrBaseRWM[cuIdx][blockIdx]
		+ (_cuIdx2SlrIdx[cuIdx] << XDNN_KERNEL_CSR_AWIDTH);

}

void KernelInterface::writeRegister(int cuIdx, int blockIdx, 
  size_t offset, size_t size, const void* data, RegisterWriteManager *rwm) {
    boost::lock_guard < boost::mutex > guard(g_mtx);
    if (offset == XDNN_SCRIPT_MEMORY_CONTROL && *(int*)(data) == 1)
    {
      if (_csrBase[cuIdx].size() > 1 && blockIdx == 0)
        _dflInstrCounter[cuIdx]++;
      else
        _instrCounter[cuIdx]++;
    }

    if (XDNN_VERBOSE) {
        if (size == 4) {
          printf("writeRegister %d Offset 0x%llx = %x\n", cuIdx,
              (long long) offset, *(int*) data);
        } else if (size == 8) {
          printf("writeRegister %d Offset 0x%llx = %llx\n", cuIdx,
              (long long) offset, *(long long*) data);
        }
    }
    cl_bool blocking = CL_TRUE;
#ifdef DEBUG
    xdnn::XDebugJournal().logRegWrite("", cuIdx, offset, size, data);
#endif

    int err = CL_SUCCESS;
    if (rwm != nullptr) {
	   int data_i = *((int *)data);
	   rwm->addAddrValuePair(computeCsrBaseAddr(cuIdx, blockIdx, rwm) + offset, data_i);
    } else{
	   err = clEnqueueWriteBuffer(_handle->commands, _regMap, blocking,
		   computeCsrBaseAddr(cuIdx, blockIdx) + offset,
		   size, data, 0, NULL, NULL);
    }
    assert(err == CL_SUCCESS);
}
void KernelInterface::readHardwareCounter() {
    if (XDNN_HARDWARE_GENERAL_COUNTER_TIME_DL
      || XDNN_HARDWARE_GENERAL_COUNTER_TIME_UL
      || XDNN_HARDWARE_GENERAL_COUNTER_TIME_FL
      || XDNN_HARDWARE_GENERAL_COUNTER_TIME_MISC
      || XDNN_HARDWARE_GENERAL_COUNTER_TIME_SYSARR)
    {
      int data=0;
      if (XDNN_HARDWARE_GENERAL_COUNTER_TIME_DL)
        data = 0xa << 16;
      else if (XDNN_HARDWARE_GENERAL_COUNTER_TIME_UL)
        data = 0x9 << 16;
      else if (XDNN_HARDWARE_GENERAL_COUNTER_TIME_FL)
        data = 0x5 << 16;
      else if (XDNN_HARDWARE_GENERAL_COUNTER_TIME_MISC)
        data = 0x7 << 16;
      else if (XDNN_HARDWARE_GENERAL_COUNTER_TIME_SYSARR)
        data = 0x3 << 16;
      int err = clEnqueueWriteBuffer(_handle->commands, _regMap, 
        CL_TRUE, _csrBase[0].back() + XDNN_MAIN_BLOCK_OFFSET + XDNN_DEBUG_CONTROL, 
        sizeof(data), &data, 0, NULL, NULL);
      assert(err==CL_SUCCESS);
    }

    int data=-9;
    int err = clEnqueueReadBuffer(_handle->commands, _regMap, CL_TRUE,
        _csrBase[0].back() + XDNN_MAIN_BLOCK_OFFSET + XDNN_GENERAL_COUNTER,
        sizeof(data), &data, 0, NULL, NULL);
    float elapsedTime = float(data) / float(DATA_CLK);

    printf("V3 Hardware general counter: %d, %f ms\n", data, elapsedTime);
}

int KernelInterface::readImageExecutionCounter(const int cuIdx, int slice) {
    int data = -1;
    int sliceOffset = slice << XDNN_KERNEL_CSR_AWIDTH;

    int err = clEnqueueReadBuffer(_handle->commands, _regMap, CL_TRUE,
        _csrBase[cuIdx].back() + sliceOffset + XDNN_IMAGE_EXECUTION_COUNTER, 
        sizeof(data), &data, 0, NULL, NULL);
    return data;
}
void KernelInterface::extractKernelConfig() {
    int data = 0;

    int err = clEnqueueReadBuffer(_handle->commands, _regMap, CL_TRUE,
        _csrBase[0].back() + XDNN_MAIN_BLOCK_OFFSET + XDNN_KERNEL_CONFIGURATION_0,
        sizeof(data), &data, 0, NULL, NULL);
    int dspArrayWidth = (data & 0xFF);

    err = clEnqueueReadBuffer(_handle->commands, _regMap, CL_TRUE,
        _csrBase[0].back() + XDNN_MAIN_BLOCK_OFFSET + XDNN_KERNEL_CONFIGURATION_1,
        sizeof(data), &data, 0, NULL, NULL);
    const int numCores = data & 0xF;
    const int mode8bit = (data >> 4) & 0x1;
    const int axiDataWidth = (data >> 8) & 0xFF;
    int imgMemSize = (data >> 16) & 0xFF;
    imgMemSize = int(imgMemSize * 0.25);
    const int versionMajor = (data >> 24) & 0xF;
    const int versionMinor = (data >> 28) & 0xF;

    err = clEnqueueReadBuffer(_handle->commands, _regMap, CL_TRUE,
        _csrBase[0].back() + XDNN_MAIN_BLOCK_OFFSET + XDNN_KERNEL_CONFIGURATION_2,
        sizeof(data), &data, 0, NULL, NULL);
    const int maxInstructionNum = (data >> 12) & 0xFFF;
    const int maxCrossbarEntries = pow(2, (data >> 24) & 0xFF);
    printf("\n"
            "[XRT]    git hash                   : %s\n"  
            "[XDNN]   git hash                   : %s\n"  
            "[XDNN] kernel configuration\n"
            "[XDNN]   num cores                  : %d\n"
            "[XDNN]   dsp array width            : %d\n"
            "[XDNN]   axi data width (in 32bits) : %d\n"
            "[XDNN]   img mem size               : %d MB\n"
            "[XDNN]   max instr num              : %d\n"
            "[XDNN]   max xbar entries           : %d\n"
            "[XDNN]   version                    : %d.%d\n"
            "[XDNN]   8-bit mode                 : %d\n", 
            xrt_build_version_hash, git_commit,
            numCores, dspArrayWidth, axiDataWidth,
            imgMemSize, maxInstructionNum, maxCrossbarEntries,
            versionMajor, versionMinor, mode8bit);

    // save kernel config reg values onto XBLASHandle
    XDNNKernelConfigInfoMgr kConf(_handle->m_kConfig.m_configRegInfo);
    kConf.set(XDNNKernelConfigInfoMgr::N_CORES, numCores);
    kConf.set(XDNNKernelConfigInfoMgr::DSP_WIDTH, dspArrayWidth);
    kConf.set(XDNNKernelConfigInfoMgr::AXI_DATA_WIDTH, axiDataWidth);
    kConf.set(XDNNKernelConfigInfoMgr::IMG_MEM, imgMemSize);
    kConf.set(XDNNKernelConfigInfoMgr::VERSION_MAJOR, versionMajor);
    kConf.set(XDNNKernelConfigInfoMgr::VERSION_MINOR, versionMinor);
    kConf.set(XDNNKernelConfigInfoMgr::MODE8BIT, mode8bit);
    kConf.set(XDNNKernelConfigInfoMgr::MAX_INSTR_NUM, maxInstructionNum);
    kConf.set(XDNNKernelConfigInfoMgr::MAX_XBAR_ENTRIES, maxCrossbarEntries);

}

void KernelInterface::readRegister(int cuIdx, size_t offset, size_t size,
        void* data) {
    boost::lock_guard < boost::mutex > guard(g_mtx);
    assert(size == 4); // 4 bytes per address
    const int blockIdx 
      = _csrBase[cuIdx].size() - 1; // always read main block
    int err = clEnqueueReadBuffer(_handle->commands, _regMap, CL_TRUE,
      computeCsrBaseAddr(cuIdx, blockIdx) + offset, size, data, 0,
      NULL, NULL);
    assert(err == CL_SUCCESS);
}

void KernelInterface::writeInt(XComputeUnit * cu, size_t offset, int value) {
    writeInt(cu->getCuIdx(), offset, value);
}

void KernelInterface::writeIntAllBlocks(int cuIdx, size_t offset, int value, RegisterWriteManager *rwm) {
  // write on all processor blocks
  for (int bi=0; bi < _csrBase[cuIdx].size(); bi++)
    writeRegister(cuIdx, bi, offset, sizeof(int), &value, rwm);
}

void KernelInterface::writeInt(int cuIdx, int blockIdx, size_t offset, int value, RegisterWriteManager *rwm) {
  // write only on single processor block
  if (blockIdx == -1)
    blockIdx = _csrBase[cuIdx].size()-1; 
  
  writeRegister(cuIdx, blockIdx, offset, sizeof(int), &value, rwm);
}
void KernelInterface::writeInt(int cuIdx, size_t offset, int value, RegisterWriteManager *rwm) {
  // no blockIdx specified, write to last block
  writeInt(cuIdx, -1, offset, value, rwm);
}

void KernelInterface::writeImgDdrBase(int cuIdx, long long value, RegisterWriteManager *rwm) {
  int halfSize = sizeof(long long) / 2;
  int *intVal = (int*) &value;
  if (XDNN_VERBOSE) {
      printf("[XDNN] writeImgDdrBase (PE%d) 0x%x: 0x%x, 0x%x: 0x%x\n", cuIdx,
              XDNN_IMAGE_DDR_BASE_ADDR_0, *intVal,
              XDNN_IMAGE_DDR_BASE_ADDR_1, *(intVal + 1));
  }

  // write addr to all blocks by default
  for (int bi=0; bi < _csrBase[cuIdx].size(); bi++)
  {
    writeRegister(cuIdx, bi, XDNN_IMAGE_DDR_BASE_ADDR_0, halfSize, intVal);
    writeRegister(cuIdx, bi, XDNN_IMAGE_DDR_BASE_ADDR_1, halfSize, intVal + 1);
  }
}

void KernelInterface::enableXRT(XComputeUnit *cu, bool isThroughput) {
    unsigned int data = 0;
    data |= 16; // XRT FLOW
    if (isThroughput)
    	data |= 32; // ThroughtputMode
    this->writeInt(cu, XDNN_GLOBAL_CONFIG, data);
}

void KernelInterface::writeFilterDdrBase(int cuIdx, long long value, RegisterWriteManager *rwm) {
    int halfSize = sizeof(long long) / 2;
    int *intVal = (int*) &value;
    if (XDNN_VERBOSE) {
        printf("[XDNN] writeFilterDdrBase (PE%d) 0x%x: 0x%x, 0x%x: 0x%x\n",
                cuIdx,
                XDNN_FILTER_DDR_BASE_ADDR_0, *intVal,
                XDNN_FILTER_DDR_BASE_ADDR_1, *(intVal + 1));
    }

    // write addr to all blocks by default
    for (int bi=0; bi < _csrBase[cuIdx].size(); bi++)
    {
      writeRegister(cuIdx, bi, XDNN_FILTER_DDR_BASE_ADDR_0, halfSize, intVal);
      writeRegister(cuIdx, bi, XDNN_FILTER_DDR_BASE_ADDR_1, halfSize, intVal + 1);
    }
  }

void KernelInterface::pushCmd(size_t dl_ofst, size_t ul_ofst, XPipelinePacket *packet) {
	RegisterWriteManager *rwm = packet->rwm;
    const int cuIdx = packet->kernelComputeUnitIdx;

    const int startCmdIdx = packet->xdnnStartIdx;
    const int stopCmdIdx = packet->xdnnStopIdx;
    if (packet->xdnnThroughputInterbuf) {
        // throughput mode
        const size_t imgIdx0 = packet->kernelComputeUnitSlot
                | (packet->xdnnDFLStartIdx << 8)
                | (packet->xdnnDFLStopIdx << 20);
        const size_t imgIdx1 = packet->kernelComputeUnitSlot
                | (startCmdIdx << 8) | (stopCmdIdx << 20);

        const int cmd0 = 0x00000000 // command queue push, NOT in 2019.1+
                | (1 << 15)  // enable "start/stop"
                | (1 << 11);
        // build push command + enable "start/stop"
        const int cmd1 = 0x00000000 // command queue push, NOT in 2019.1+
                | (1 << 15)  // enable "start/stop"
                | (1 << 8);

        // write MB first 
        const int interbufByteOffset 
          = packet->xdnnThroughputInterbuf->getHWSizeInBytes();
        writeInt(cuIdx, 1, XDNN_DOWNLOAD_DDR_OFFSET,
                ul_ofst + interbufByteOffset, rwm);
        writeInt(cuIdx, 1, XDNN_UPLOAD_DDR_OFFSET, ul_ofst, rwm);
        writeInt(cuIdx, 1, XDNN_IMAGE_INDEX, imgIdx1, rwm);
        writeInt(cuIdx, 1, XDNN_COMMAND_QUEUE_CONTROL, cmd1, rwm);

        // write DB last, so we only start executing when handshake is set up
        writeInt(cuIdx, 0, XDNN_DOWNLOAD_DDR_OFFSET, dl_ofst, rwm);
        writeInt(cuIdx, 0, XDNN_UPLOAD_DDR_OFFSET,
                ul_ofst + interbufByteOffset, rwm);
        writeInt(cuIdx, 0, XDNN_IMAGE_INDEX, imgIdx0, rwm);
        writeInt(cuIdx, 0, XDNN_COMMAND_QUEUE_CONTROL, cmd0, rwm);
    } else {
        // build image index + start/stop value
        const size_t imgIdx = packet->kernelComputeUnitSlot 
          | (startCmdIdx << 8)
          | (stopCmdIdx << 20);

        // build push command + enable "start/stop"
		const int cmd = 0x00000000 // command queue push, NOT in 2019.1+
					 |  (1 << 15); // enable "start/stop"
        writeInt(cuIdx, XDNN_DOWNLOAD_DDR_OFFSET, dl_ofst, rwm);
        writeInt(cuIdx, XDNN_UPLOAD_DDR_OFFSET, ul_ofst, rwm);
        writeInt(cuIdx, XDNN_IMAGE_INDEX, imgIdx, rwm);
        writeInt(cuIdx, XDNN_COMMAND_QUEUE_CONTROL, cmd, rwm);
    }
}

static long long readBaseAddrsFromFile(int cuIdx, const std::string &serviceId)
// TODO remove when XRT enhances clGetProgramInfo to give DDR bank base addr
{
  std::string dir = "vitis-" + std::string(std::getenv("USER"));
  ifstream f;
  std::stringstream ss;
  ss << "xdnn_ddr_" << serviceId << "_" << cuIdx;
  std::string fname = ss.str();
  std::replace(fname.begin(), fname.end(), '/', '_');
  f.open("/tmp/" + dir + "/" + fname);
  long long addr;
  f >> addr;
  f.close();
  return addr;
}

static void saveBaseAddrsToFile(int cuIdx, const std::string &serviceId, long long baseAddr)
// TODO remove when XRT enhances clGetProgramInfo to give DDR bank base addr
{
  std::string dir = "vitis-" + std::string(std::getenv("USER"));
  mkdir(dir.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH);
  ofstream f;
  std::stringstream ss;
  ss << "xdnn_ddr_" << serviceId << "_" << cuIdx;
  std::string fname = ss.str();
  std::replace(fname.begin(), fname.end(), '/', '_');
  f.open("/tmp/" + dir + "/" + fname);
  f << baseAddr;
  f.close();
}

int XDNNInitScript(XComputeUnit *cu) {
#ifdef DEBUG
    xdnn::XDebugJournal().log("XDNNScriptClear");
#endif

    if (!cu->getHandle()->m_subscribeId.empty())
    {
      /*
       * read DDR base addr from file written by Publisher
       * this is to share base addrs between workers in a process pool
       * FIXME TODO XXX
       * http://jira.xilinx.com/browse/CR-1047931
       * remove when XRT enhances clGetProgramInfo to give DDR bank base addr
       */
      auto baseAddr = readBaseAddrsFromFile(cu->getCuIdx(),
        cu->getHandle()->m_subscribeId);
      cu->saveDdrBase(baseAddr);
    }

    // write img and filter base addrs to reg
    long long baseAddr = cu->getDdrBase();
    KernelInterface *ki = KernelInterfaceDB::get(cu->getHandle());
    ki->writeImgDdrBase(cu->getCuIdx(), baseAddr);
    ki->writeFilterDdrBase(cu->getCuIdx(), baseAddr);

    if (!cu->getHandle()->m_publishId.empty())
    {
      /*
       * write DDR base addr to file for Subscribers
       * this is to share base addrs between workers in a process pool
       * FIXME TODO XXX
       * http://jira.xilinx.com/browse/CR-1047931
       * remove when XRT enhances clGetProgramInfo to give DDR bank base addr
       */
      saveBaseAddrsToFile(cu->getCuIdx(), cu->getHandle()->m_publishId, baseAddr);
    }

    return (EXIT_SUCCESS);
}

void XDNNScriptExecute(XPipelinePacket *packet) {
#ifdef DEBUG
    xdnn::XDebugJournal().log("XDNNScriptExecute");
#endif
    XBLASHandle &handle = *(packet->xHandle);

    cl_mem input_b = packet->bDevMemPOD->m_devicePtr;
    cl_mem inout_c = packet->cDevMemPOD->m_devicePtr;

    XComputeUnit *cu =
            handle.getComputeUnit(
                    packet->kernelComputeUnitIdx);

    unsigned int dlOffset = cu->computeDdrOffset(input_b);
    unsigned int ulOffset = cu->computeDdrOffset(inout_c);

    KernelInterface *ki = KernelInterfaceDB::get(&handle);
    ki->pushCmd(dlOffset, ulOffset, packet);

    XBLASHandle* xHandle = packet->xHandle;
    std::vector<butler::xCU>& xCUVec = xHandle->m_xCUVec;
#ifdef XRT_20191
    int idx = packet->kernelComputeUnitIdx;
#else
	int idx = xCUVec[packet->kernelComputeUnitIdx].getCUIdx();
#endif
	packet->rwm->execute(idx);
    //packet->rwm->execute(packet->kernelComputeUnitIdx);
    if (XDNN_READ_HARDWARE_GENERAL_COUNTER)
        KernelInterfaceDB::get(&handle)->readHardwareCounter();
}

void XDNNUpdateHardwareCounters(XPipelinePacket* packet) {
    XBLASHandle* xHandle = packet->xHandle;
    XComputeUnit* cu = xHandle->getComputeUnit(packet->kernelComputeUnitIdx);

    std::vector<int> sliceToUpdate;
    if (packet->xdnnDFLStopIdx >= 0)
      sliceToUpdate.push_back(0);
    if (packet->xdnnStopIdx >= 0)
      sliceToUpdate.push_back(1);

    for (int si=0; si < sliceToUpdate.size(); si++)
    {
      int counterValue =
        KernelInterfaceDB::get(xHandle)->readImageExecutionCounter(
          packet->kernelComputeUnitIdx, sliceToUpdate[si]);
      float elapsedTime = float(counterValue) / float(DATA_CLK);

      cu->addCounterSnapShot(elapsedTime, sliceToUpdate[si]);
    }
}

void XDNNUpdateErrorStatus(XPipelinePacket* packet) {
    
    XBLASHandle* xHandle = packet->xHandle;
    
    int data[15]; 
    
    KernelInterfaceDB::get(xHandle)->readRegister(packet->kernelComputeUnitIdx,XDNN_IMAGE_EXECUTION_COUNTER-XDNN_MAIN_BLOCK_OFFSET, sizeof(int),&data[0]);
    KernelInterfaceDB::get(xHandle)->readRegister(packet->kernelComputeUnitIdx,XDNN_IMAGE_EXECUTION_COUNTER, sizeof(int),&data[1]);
    KernelInterfaceDB::get(xHandle)->readRegister(packet->kernelComputeUnitIdx,XDNN_GENERAL_COUNTER, sizeof(int),&data[2]);
    KernelInterfaceDB::get(xHandle)->readRegister(packet->kernelComputeUnitIdx,XDNN_CONVOLUTION_COUNTER, sizeof(int),&data[3]);
    KernelInterfaceDB::get(xHandle)->readRegister(packet->kernelComputeUnitIdx,XDNN_MISC_COUNTER, sizeof(int),&data[4]);
    KernelInterfaceDB::get(xHandle)->readRegister(packet->kernelComputeUnitIdx,XDNN_DEBUG_CONTROL, sizeof(int),&data[5]);
    KernelInterfaceDB::get(xHandle)->readRegister(packet->kernelComputeUnitIdx,XDNN_DEBUG_CONTROL_1, sizeof(int),&data[6]);
    KernelInterfaceDB::get(xHandle)->readRegister(packet->kernelComputeUnitIdx,XDNN_DEBUG_CONTROL_2, sizeof(int),&data[7]);
    KernelInterfaceDB::get(xHandle)->readRegister(packet->kernelComputeUnitIdx,XDNN_ERROR_LOG, sizeof(int),&data[8]);
    KernelInterfaceDB::get(xHandle)->readRegister(packet->kernelComputeUnitIdx,XDNN_KERNEL_CONFIGURATION, sizeof(int),&data[9]);
    KernelInterfaceDB::get(xHandle)->readRegister(packet->kernelComputeUnitIdx,XDNN_KERNEL_CONFIGURATION_1, sizeof(int),&data[10]);
    KernelInterfaceDB::get(xHandle)->readRegister(packet->kernelComputeUnitIdx,XDNN_KERNEL_CONFIGURATION_2, sizeof(int),&data[11]);
    KernelInterfaceDB::get(xHandle)->readRegister(packet->kernelComputeUnitIdx,XDNN_KERNEL_CONFIGURATION_3, sizeof(int),&data[12]);
    KernelInterfaceDB::get(xHandle)->readRegister(packet->kernelComputeUnitIdx,XDNN_KERNEL_CONFIGURATION_4, sizeof(int),&data[13]);
    KernelInterfaceDB::get(xHandle)->readRegister(packet->kernelComputeUnitIdx,XDNN_KERNEL_CONFIGURATION_5, sizeof(int),&data[14]);

    xdnn::XDebugJournal().log("Debugging Registers");
    xdnn::XDebugJournal().logRegRead("XDNN_IMAGE_EXECUTION_COUNTER (DB)",packet->kernelComputeUnitIdx,XDNN_IMAGE_EXECUTION_COUNTER, sizeof(int),&data[0]);
    xdnn::XDebugJournal().logRegRead("XDNN_IMAGE_EXECUTION_COUNTER (MB)",packet->kernelComputeUnitIdx,XDNN_MAIN_BLOCK_OFFSET+XDNN_IMAGE_EXECUTION_COUNTER, sizeof(int),&data[1]);
    xdnn::XDebugJournal().logRegRead("XDNN_GENERAL_COUNTER",packet->kernelComputeUnitIdx,XDNN_GENERAL_COUNTER, sizeof(int),&data[2]);
    xdnn::XDebugJournal().logRegRead("XDNN_CONVOLUTION_COUNTER",packet->kernelComputeUnitIdx,XDNN_CONVOLUTION_COUNTER, sizeof(int),&data[3]);
    xdnn::XDebugJournal().logRegRead("XDNN_MISC_COUNTER",packet->kernelComputeUnitIdx,XDNN_MISC_COUNTER, sizeof(int),&data[4]);
    xdnn::XDebugJournal().logRegRead("XDNN_DEBUG_CONTROL",packet->kernelComputeUnitIdx,XDNN_DEBUG_CONTROL, sizeof(int),&data[5]);
    xdnn::XDebugJournal().logRegRead("XDNN_DEBUG_CONTROL_1",packet->kernelComputeUnitIdx,XDNN_DEBUG_CONTROL_1, sizeof(int),&data[6]);
    xdnn::XDebugJournal().logRegRead("XDNN_DEBUG_CONTROL_2",packet->kernelComputeUnitIdx,XDNN_DEBUG_CONTROL_2, sizeof(int),&data[7]);
    xdnn::XDebugJournal().logRegRead("XDNN_ERROR_LOG",packet->kernelComputeUnitIdx,XDNN_ERROR_LOG, sizeof(int),&data[8]);
    xdnn::XDebugJournal().logRegRead("XDNN_KERNEL_CONFIGURATION",packet->kernelComputeUnitIdx,XDNN_KERNEL_CONFIGURATION, sizeof(int),&data[9]);
    xdnn::XDebugJournal().logRegRead("XDNN_KERNEL_CONFIGURATION_1",packet->kernelComputeUnitIdx,XDNN_KERNEL_CONFIGURATION_1, sizeof(int),&data[10]);
    xdnn::XDebugJournal().logRegRead("XDNN_KERNEL_CONFIGURATION_2",packet->kernelComputeUnitIdx,XDNN_KERNEL_CONFIGURATION_2, sizeof(int),&data[11]);
    xdnn::XDebugJournal().logRegRead("XDNN_KERNEL_CONFIGURATION_3",packet->kernelComputeUnitIdx,XDNN_KERNEL_CONFIGURATION_3, sizeof(int),&data[12]);
    xdnn::XDebugJournal().logRegRead("XDNN_KERNEL_CONFIGURATION_4",packet->kernelComputeUnitIdx,XDNN_KERNEL_CONFIGURATION_4, sizeof(int),&data[13]);
    xdnn::XDebugJournal().logRegRead("XDNN_KERNEL_CONFIGURATION_5",packet->kernelComputeUnitIdx,XDNN_KERNEL_CONFIGURATION_5, sizeof(int),&data[14]);

   return;

}

/***************************
 *****CrossBar Functions****
 ***************************/
#define XBAR_OP_TBL_SIZE        (8*16)/8
#define XBAR_COND_TBL_SIZE      XBAR_OP_TBL_SIZE/4
#define DESIGN_NUM_DSP_ROWS     96
#define C_PRECISION             8
#define C_ARRAY_COL_NUM         16
#define C_ARRAY_ROW_NUM         96
#define C_ARRAY_XBAR_OCH        32
#define C_ARRAY_OCH_NUM         ((C_PRECISION==8) ? (C_ARRAY_COL_NUM*4) : (C_ARRAY_COL_NUM*2))

class xbar_tbl_data_t;
class xbar_sw_data_t {
public:
    uint32_t mux_arr[XBAR_COND_TBL_SIZE + 1];
    void set_regs(uint32_t reg1, uint32_t reg2, uint32_t reg3, uint32_t reg4,
            uint32_t ctrl_reg) {
        mux_arr[0] = reg1;
        mux_arr[1] = reg2;
        mux_arr[2] = reg3;
        mux_arr[3] = reg4;
        mux_arr[4] = ctrl_reg;
    }
    void operator=(const xbar_tbl_data_t &in);
};

class xbar_tbl_data_t {
public:
    uint8_t mux_arr[XBAR_OP_TBL_SIZE];
    uint8_t sect_offst;
    void operator=(const xbar_sw_data_t &in);
};

bool operator==(const xbar_tbl_data_t& left, const xbar_tbl_data_t& right);
bool operator!=(const xbar_tbl_data_t& left, const xbar_tbl_data_t& right);

bool operator==(const xbar_sw_data_t& left, const xbar_sw_data_t& right);
bool operator!=(const xbar_sw_data_t& left, const xbar_sw_data_t& right);

int32_t comp_xbar_config(std::vector<xbar_sw_data_t>& xbar_cfgVec,
        std::vector<xbar_sw_data_t> *finalXbarCfgVec,
        std::vector<uint64_t> *FinalNumXbarStartPtr,
        std::vector<uint64_t> *FinalNumXbarEndPtr,
        std::vector<uint64_t> *FinalNumXbarIteration,
        std::vector<uint64_t> *FinalNumXbarStartPtr1,
        std::vector<uint64_t> *FinalNumXbarEndPtr1,
        std::vector<uint64_t> *FinalNumXbarIteration1);

int32_t xbar_table_config(int cfg_array_row, int cfg_array_col,
        int cfg_array_o_chnl, int cfg_xbar_i_chnl, int fmap_chnl_num,
        int fmap_start_chnl, int final_fullsect_num, int final_replsect_num,
        int final_replunit_num, int final_replunit_width, bool compression,
        std::vector<xbar_sw_data_t> *FinalXbarCfgVec,
        std::vector<uint64_t> *NumXbarStartPtr,
        std::vector<uint64_t> *NumXbarEndPtr,
        std::vector<uint64_t> *NumXbarIteration,
        std::vector<uint64_t> *NumXbarStartPtr1,
        std::vector<uint64_t> *NumXbarEndPtr1,
        std::vector<uint64_t> *NumXbarIteration1,
        int xbarTableCounterOffset);

int print_comp_setting(std::string str, int op_num,
        std::vector<xbar_sw_data_t> *finalXbarCfgVec,
        std::vector<uint64_t> *FinalNumXbarStartPtr,
        std::vector<uint64_t> *FinalNumXbarEndPtr,
        std::vector<uint64_t> *FinalNumXbarIteration,
        std::vector<uint64_t> *FinalNumXbarStartPtr1,
        std::vector<uint64_t> *FinalNumXbarEndPtr1,
        std::vector<uint64_t> *FinalNumXbarIteration1);

int append_to_final(std::vector<xbar_sw_data_t>& xbar_cfgVec_comp,
        int& startPtr, int& endPtr, std::vector<xbar_sw_data_t> *finXbarCfgVec, int xbarTableCounterOffset);

int single_append(xbar_sw_data_t add_cfg,
        std::vector<xbar_sw_data_t> *FinalXbarVec);

bool find_in_final(std::vector<xbar_sw_data_t>& xbar_cfgVec_comp, int& startPtr,
        int& endPtr, std::vector<xbar_sw_data_t> *finXbarCfgVec);

void compute_find_helper(std::vector<xbar_sw_data_t>& xbar_cfgVec_comp,
        int length, std::vector<int>& helpArr, int startPt);

void print_xbar_sw_data_t(xbar_sw_data_t xbar_entry_val);

void print_fin(std::vector<xbar_sw_data_t> *FinalXbarCfgVec);

int get_vec_fin_size();

void init_xbar();

int single_append(xbar_sw_data_t add_cfg,
        std::vector<xbar_sw_data_t> *FinalXbarVec) {
    add_cfg.mux_arr[4] |= (uint32_t) FinalXbarVec->size();
    FinalXbarVec->push_back(add_cfg);
    return 0;
}

void init_xbar(std::vector<xbar_sw_data_t> *FinalXbarCfgVec) {
    xbar_sw_data_t helper;
    helper.set_regs(0x08040201, 0x0, 0x0, 0x0, 0x80000000);
    single_append(helper, FinalXbarCfgVec);
    helper.set_regs(0x0, 0x08040201, 0x0, 0x0, 0x80000000);
    single_append(helper, FinalXbarCfgVec);
    helper.set_regs(0x0, 0x0, 0x08040201, 0x0, 0x80000000);
    single_append(helper, FinalXbarCfgVec);
    helper.set_regs(0x08040201, 0x0, 0x0, 0x0, 0x80000000);
    single_append(helper, FinalXbarCfgVec);
    helper.set_regs(0x0, 0x08040201, 0x0, 0x0, 0x80000000);
    single_append(helper, FinalXbarCfgVec);
}

void print_xbar_sw_data_t(xbar_sw_data_t xbar_entry_val) {
    printf("%08x ", xbar_entry_val.mux_arr[4]);
    for (int j = XBAR_COND_TBL_SIZE - 1; j >= 0; j--) {
        printf("%08x ", xbar_entry_val.mux_arr[j]);
    }
}

void print_fin(std::vector<xbar_sw_data_t> *FinalXbarCfgVec) {
    for (size_t i = 0; i < FinalXbarCfgVec->size(); i++) {
        printf("Final[%2lu]: ", i);
        print_xbar_sw_data_t((*FinalXbarCfgVec).at(i));

        printf("\n");
    }
}

bool operator==(const xbar_tbl_data_t& left, const xbar_tbl_data_t& right) {
    for (int x = 0; x < XBAR_OP_TBL_SIZE; x++) {
        if (left.mux_arr[x] != right.mux_arr[x]) {
            return false;
        }
    }
    if (left.sect_offst != right.sect_offst) {
        return false;
    }
    return true;
}

bool operator!=(const xbar_tbl_data_t& left, const xbar_tbl_data_t& right) {
    for (int x = 0; x < XBAR_OP_TBL_SIZE; x++) {
        if (left.mux_arr[x] == right.mux_arr[x]) {
            return false;
        }
    }
    if (left.sect_offst == right.sect_offst) {
        return false;
    }
    return true;
}

bool operator==(const xbar_sw_data_t& left, const xbar_sw_data_t& right) {
    for (int x = 0; x < XBAR_COND_TBL_SIZE; x++) {
        if (left.mux_arr[x] != right.mux_arr[x]) {
            return false;
        }
    }
    if ((left.mux_arr[4] & 0x0F000000) != (right.mux_arr[4] & 0x0F000000)) {
        return false;
    }
    return true;
}

bool operator!=(const xbar_sw_data_t& left, const xbar_sw_data_t& right) {
    return !(left == right);
}

void xbar_sw_data_t::operator=(const xbar_tbl_data_t &in) {
    uint32_t* in_arr_ptr = (uint32_t *) (in.mux_arr);
    for (int i = 0; i < XBAR_COND_TBL_SIZE; i++) {
        mux_arr[i] = *(in_arr_ptr + i);
    }
    mux_arr[4] = ((uint32_t)(in.sect_offst) & 0x0F) << 24;
    mux_arr[4] |= 0x80000000;
}

void xbar_tbl_data_t::operator=(const xbar_sw_data_t &in) {
    uint8_t* in_arr_ptr = (uint8_t *) (in.mux_arr);
    for (int i = 0; i < XBAR_OP_TBL_SIZE; i++) {
        mux_arr[i] = *(in_arr_ptr + i);
    }
    sect_offst = (uint8_t)((in.mux_arr[4] >> 24) & 0x0F);
}

int32_t xbar_table_config(int cfg_array_row, int cfg_array_col,
        int cfg_array_o_chnl, int cfg_xbar_i_chnl, int fmap_chnl_num,
        int fmap_start_chnl, int final_fullsect_num, int final_replsect_num,
        int final_replunit_num, int final_replunit_width, bool compression,
        std::vector<xbar_sw_data_t> *FinalXbarCfgVec,
        std::vector<uint64_t> *NumXbarStartPtr,
        std::vector<uint64_t> *NumXbarEndPtr,
        std::vector<uint64_t> *NumXbarIteration,
        std::vector<uint64_t> *NumXbarStartPtr1,
        std::vector<uint64_t> *NumXbarEndPtr1,
        std::vector<uint64_t> *NumXbarIteration1,
        int xbarTableCounterOffset) {
    std::vector<xbar_sw_data_t> xbar_cfgVec;
    xbar_cfgVec.clear();
    int chnl_per_am_bank = 8;
    int actimem_bank_num;
    int xbar_in_bank_num;
    int final_ru_bank_num;
    int array_mux_o_beat;
    int array_start_col;
    int array_start_bank;
    int section_offset;
    int temp_chnl_num;
    int xbar_op_chnl_rmn;
    bool xbar_op_fullsect; //Replaced bit with bool
    int xbar_op_sect_idx;
    int xbar_op_sect_chnl;
    bool xbar_op_sect_crs; //Replaced bit with bool
    int xbar_op_sect_bank;
    int xbar_op_sect_ofst;
    int xbar_op_array_bank;
    xbar_tbl_data_t xbar_op_tbl_data; // uint8_t xbar_op_tbl_data[XBAR_OP_TBL_SIZE];
    xbar_sw_data_t xbar_set_hold;
    uint8_t xbar_op_tbl_mux = 0;
    int xbar_mux_beat_cnt;
    bool xbar_op_sect_ofst_inc; //Replaced bit with bool

    actimem_bank_num = cfg_array_row / chnl_per_am_bank;
    xbar_in_bank_num = cfg_xbar_i_chnl / chnl_per_am_bank;
    final_ru_bank_num = final_replunit_width / chnl_per_am_bank;
    array_mux_o_beat = cfg_array_o_chnl / cfg_xbar_i_chnl;

    array_start_col = fmap_start_chnl % cfg_array_col;
    array_start_bank = array_start_col / chnl_per_am_bank;
    xbar_op_chnl_rmn = fmap_chnl_num;

    if (fmap_start_chnl < (final_fullsect_num * cfg_array_row)) {
        xbar_op_fullsect = true;
        xbar_op_sect_idx = fmap_start_chnl / cfg_array_row;
        xbar_op_sect_chnl = fmap_start_chnl
                - (xbar_op_sect_idx * cfg_array_row);
    } else {
        temp_chnl_num = fmap_start_chnl - (final_fullsect_num * cfg_array_row);
        section_offset = temp_chnl_num / final_replunit_width;
        xbar_op_fullsect = false;
        xbar_op_sect_idx = final_fullsect_num + section_offset;
        xbar_op_sect_chnl = temp_chnl_num
                - (section_offset * final_replunit_width);
    }

    xbar_op_array_bank = array_start_bank;
    xbar_mux_beat_cnt = 0;
    xbar_op_sect_ofst = 0;
    xbar_op_sect_ofst_inc = false;
    //----------------------------------------------------------------------------
    // full-section
    //----------------------------------------------------------------------------
    if (xbar_op_fullsect) {
        while ((xbar_op_sect_idx < final_fullsect_num) && xbar_op_chnl_rmn > 0) {
            xbar_op_sect_crs = 0;
            for (int i = 0; i < XBAR_OP_TBL_SIZE; i++) {
                xbar_op_tbl_data.mux_arr[i] = 0;
            }
            for (int j = 0; j < xbar_in_bank_num; j++) {
                xbar_op_tbl_mux = (1 << xbar_op_array_bank) & 0x0F;
                xbar_op_sect_bank = xbar_op_sect_chnl / chnl_per_am_bank;
                if (xbar_op_fullsect) {
                    xbar_op_tbl_data.mux_arr[xbar_op_sect_bank] =
                            (xbar_op_tbl_mux | (xbar_op_sect_crs << 4)) & 0x1F;
                    xbar_op_sect_chnl += chnl_per_am_bank;
                    if (xbar_op_sect_chnl >= cfg_array_row) {
                        xbar_op_sect_idx++;
                        xbar_op_sect_chnl = 0;
                        xbar_op_sect_crs = 1;
                        xbar_op_sect_ofst_inc = 1;
                        if (xbar_op_sect_idx >= final_fullsect_num) {
                            xbar_op_fullsect = 0;
                        } else {
                            xbar_op_fullsect = 1;
                        }
                    }
                } else {
                    for (int k = 0; k < final_replunit_num; k++) {
                        xbar_op_tbl_data.mux_arr[xbar_op_sect_bank
                                                 + (final_ru_bank_num * k)] = (xbar_op_tbl_mux
                                                         | (xbar_op_sect_crs << 4)) & 0x1F;
                    }
                    xbar_op_sect_chnl += chnl_per_am_bank;
                }
                xbar_op_array_bank++;
                if (xbar_op_array_bank == xbar_in_bank_num)
                    xbar_op_array_bank = 0;
            }
            xbar_op_tbl_data.sect_offst = xbar_op_sect_ofst & 0x0F;
            xbar_set_hold = xbar_op_tbl_data;
            xbar_cfgVec.push_back(xbar_set_hold);
            xbar_op_chnl_rmn -= cfg_xbar_i_chnl;
            if (xbar_op_sect_ofst_inc) {
                xbar_op_sect_ofst++;
                xbar_op_sect_ofst_inc = 0;
            }
            xbar_mux_beat_cnt++;
            if (xbar_mux_beat_cnt == array_mux_o_beat) {
                xbar_op_sect_ofst = 0;
                xbar_mux_beat_cnt = 0;
            }
        }
    }
    //----------------------------------------------------------------------------
    // replication-section
    //----------------------------------------------------------------------------
    while ((xbar_op_sect_idx < (final_fullsect_num + final_replsect_num))
            && (xbar_op_chnl_rmn > 0)) {
        xbar_op_sect_crs = 0;
        for (int i = 0; i < XBAR_OP_TBL_SIZE; i++) {
            xbar_op_tbl_data.mux_arr[i] = 0;
        }
        for (int j = 0; j < xbar_in_bank_num; j++) {
            xbar_op_tbl_mux = (1 << xbar_op_array_bank) & 0x0F;
            xbar_op_sect_bank = xbar_op_sect_chnl / chnl_per_am_bank;
            for (int k = 0; k < final_replunit_num; k++) {
                xbar_op_tbl_data.mux_arr[xbar_op_sect_bank
                                         + (final_ru_bank_num * k)] = (xbar_op_tbl_mux
                                                 | (xbar_op_sect_crs << 4)) & 0x1F;
            }
            xbar_op_sect_chnl += chnl_per_am_bank;
            if (xbar_op_sect_chnl >= final_replunit_width) {
                xbar_op_sect_idx++;
                xbar_op_sect_chnl = 0;
                xbar_op_sect_crs = 1;
                xbar_op_sect_ofst_inc = 1;
            }
            xbar_op_array_bank++;
            if (xbar_op_array_bank == xbar_in_bank_num)
                xbar_op_array_bank = 0;
        }
        xbar_op_tbl_data.sect_offst = xbar_op_sect_ofst & 0x0F;
        xbar_set_hold = xbar_op_tbl_data;
        xbar_cfgVec.push_back(xbar_set_hold);
        xbar_op_chnl_rmn -= cfg_xbar_i_chnl;
        if (xbar_op_sect_ofst_inc) {
            xbar_op_sect_ofst++;
            xbar_op_sect_ofst_inc = 0;
        }
        xbar_mux_beat_cnt++;
        if (xbar_mux_beat_cnt == array_mux_o_beat) {
            xbar_op_sect_ofst = 0;
            xbar_mux_beat_cnt = 0;
        }
    }

    if (compression) {
        /*
         * XXX FIXME WARNING compression does not work for "multi nets on
         * one PE" scenario. Crossbar programming will get messed up 
         * without 'xbarTableCounterOffset'
         */
        comp_xbar_config(xbar_cfgVec, FinalXbarCfgVec, NumXbarStartPtr,
                NumXbarEndPtr, NumXbarIteration, NumXbarStartPtr1, NumXbarEndPtr1,
                NumXbarIteration1);
    }
    else {
        int startPtr = 0;
        int endPtr   = xbar_cfgVec.size() - 1;
        append_to_final(xbar_cfgVec, startPtr, endPtr, FinalXbarCfgVec,
          xbarTableCounterOffset);
        NumXbarStartPtr->push_back((uint64_t) startPtr);
        NumXbarStartPtr1->push_back((uint64_t) 0);
        NumXbarEndPtr->push_back((uint64_t) endPtr);
        NumXbarEndPtr1->push_back((uint64_t) 0);
        NumXbarIteration->push_back((uint64_t) 1);
        NumXbarIteration1->push_back((uint64_t) 0);
    }
    return xbar_cfgVec.size();
}

/**
 * Takes raw output from xbar settings generated for a specific operation from xbar_table_config
 *      and checks if a pattern is found that can allow for replication, and if that pattern is
 *      present in the final set of settings
 * @returns op_num  -   the index in the (start,end,iteration) vectors where the relevent info
 *                      is located
 */
int32_t comp_xbar_config(std::vector<xbar_sw_data_t>& xbar_cfgVec,
        std::vector<xbar_sw_data_t> *finalXbarCfgVec,
        std::vector<uint64_t> *FinalNumXbarStartPtr,
        std::vector<uint64_t> *FinalNumXbarEndPtr,
        std::vector<uint64_t> *FinalNumXbarIteration,
        std::vector<uint64_t> *FinalNumXbarStartPtr1,
        std::vector<uint64_t> *FinalNumXbarEndPtr1,
        std::vector<uint64_t> *FinalNumXbarIteration1) {
    // should be called when xbar_cfgQ is filled with the next instruction's configs
    int sourceQ_size = xbar_cfgVec.size();
    // int chnl_per_am_bank = 8;
    int endPtr0 = 0;
    int endPtr1 = 0;
    int startPtr0 = 0;
    int startPtr1 = 0;
    int iteration0 = 0;
    int iteration1 = 0;
    int period0 = 0;
    int cycle = 1, x = 1;

    std::vector<xbar_sw_data_t> xbar_cfgVec_comp;

    xbar_sw_data_t first_elem = xbar_cfgVec[0];
    for (x = 1; x < sourceQ_size; x++, cycle++) { //Used to check for repetition
        if (period0 != 0 && (xbar_cfgVec[x] != xbar_cfgVec[cycle - 1])) {
            endPtr0 = (period0 * iteration0) - 1;
            break;
        }
        if (period0 == 0 && xbar_cfgVec[x] == first_elem) {
            endPtr0 = x - 1;
            period0 = (endPtr0 - startPtr0) + 1;
            cycle = 1;
            iteration0++;
        }
        if (period0 == cycle && xbar_cfgVec[x] == xbar_cfgVec[endPtr0]) {
            cycle = 0;
            iteration0++;
        }
    }
    int completed0 = (period0 * iteration0);
    if (iteration0 == 0) {                   //Deals with case where
        endPtr0 = sourceQ_size - 1;
        completed0 = sourceQ_size;
        if (endPtr0 - startPtr0 + 1 > 3) {
            endPtr0 = 2;
            completed0 = 3;
            startPtr1 = 3;
            endPtr1 = sourceQ_size - 1;
            iteration1 = 1;
            // printf("henlo\n");
        }
        iteration0 = 1;
    } else if (completed0 != sourceQ_size) {
        startPtr1 = period0;
        endPtr1 = startPtr1 + (sourceQ_size - 1) - completed0;
        iteration1 = 1;
    }
    for (int i = startPtr0; i <= endPtr0; i++) {
        xbar_cfgVec_comp.push_back(xbar_cfgVec[i]);
    }
    for (int i = completed0; i < sourceQ_size; i++) {
        xbar_cfgVec_comp.push_back(xbar_cfgVec[i]);
    }
    int op_num = FinalNumXbarStartPtr->size();

    bool found = find_in_final(xbar_cfgVec_comp, startPtr0, endPtr0,
            finalXbarCfgVec);
    if (!found) {
        append_to_final(xbar_cfgVec_comp, startPtr0, endPtr0, 
          finalXbarCfgVec, 0);
    }

    found = find_in_final(xbar_cfgVec_comp, startPtr1, endPtr1,
            finalXbarCfgVec);
    if (!found) {
        append_to_final(xbar_cfgVec_comp, startPtr1, endPtr1, 
          finalXbarCfgVec, 0);
    }

    FinalNumXbarStartPtr->push_back((uint64_t) startPtr0);
    FinalNumXbarStartPtr1->push_back((uint64_t) startPtr1);
    FinalNumXbarEndPtr->push_back((uint64_t) endPtr0);
    FinalNumXbarEndPtr1->push_back((uint64_t) endPtr1);
    FinalNumXbarIteration->push_back((uint64_t) iteration0);
    FinalNumXbarIteration1->push_back((uint64_t) iteration1);

    return op_num;
}

/**
 * find_in_final:
 *      Used to determine if the given set of settings is already present in the final vector
 * @param  xbar_cfgVec_comp The recently genereated set of settings
 * @param  startPtr         The start index of the set in the given vector
 * @param  endPtr           The end index of the set in the given vector
 * @return                  true if the set of settings was found in the vector and false if not.
 */
bool find_in_final(std::vector<xbar_sw_data_t>& xbar_cfgVec_comp, int& startPtr,
        int& endPtr, std::vector<xbar_sw_data_t> *finXbarCfgVec) {
    int origStart = startPtr;
    int origEnd = endPtr;
    int srcLength = origEnd - origStart + 1;
    int finLength = finXbarCfgVec->size();
    if (srcLength > finLength) {
        return false;
    }
    std::vector<int> lps(srcLength);
    compute_find_helper(xbar_cfgVec_comp, srcLength, lps, origStart);
    int i = 0; //iterator for final array
    int j = 0; //iterator for source array
    while (i < finLength && j < srcLength) {
        if (xbar_cfgVec_comp[origStart + j] == (*finXbarCfgVec).at(i)) {
            i++;
            j++;
        } else {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                i = i + 1;
            }
        }
    }
    if (j == srcLength) {
        startPtr = i - j;
        endPtr = startPtr + srcLength - 1;
        return true;
    }
    return false;
}

/**
 * compute_find_helper:
 *      A helper function intended to help with the pattern matching invloved in checking if a given
 *
 * @param xbar_cfgVec_comp  The pattern that we are trying to find in the final vector
 * @param length            The length of the pattern
 * @param helpArr           The helper array that we generate to help the pattern matching algorithm
 * @param startPt           The start of the pattern in the given array
 */
void compute_find_helper(std::vector<xbar_sw_data_t>& xbar_cfgVec_comp,
        int length, std::vector<int>& helpArr, int startPt) {
    int len = 0;
    helpArr[0] = 0;
    int i = 1;
    while (i < length) {
        if (xbar_cfgVec_comp[i + startPt] == xbar_cfgVec_comp[len + startPt]) {
            len++;
            helpArr[i] = len;
            i++;
        } else {
            if (len != 0) {
                len = helpArr[len - 1];
            } else {
                helpArr[i] = 0;
                i++;
            }
        }
    }
}

/**
 * append_to_final:
 *      Used to add patterns to the final vector if they are not already present in the vector
 *      also deals with changing the last few bits of the control_register because the index
 *      int the final vector is equivalent to the index in the actual crossbar table
 * @param  xbar_cfgVec_comp The vector from which the pattern to be added will be found
 * @param  startPtr         The start index of the pattern in the source index
 * @param  endPtr           The end index of the pattern in the source index
 * @return                  returns 0 on success
 */
int append_to_final(std::vector<xbar_sw_data_t>& xbar_cfgVec_comp,
        int& startPtr, int& endPtr,
        std::vector<xbar_sw_data_t> *finXbarCfgVec,
        int xbarTableCounterOffset) {
    int finPoint = finXbarCfgVec->size() + xbarTableCounterOffset;
    int initStart = startPtr;

    int counter = finPoint;
    xbar_sw_data_t helper;
    for (int i = startPtr; i <= endPtr; i++, counter++) {
        helper = xbar_cfgVec_comp[i];
        helper.mux_arr[4] |= counter;
        finXbarCfgVec->push_back(helper);
    }
    startPtr += finPoint - initStart;
    endPtr += finPoint - initStart;
    return 0;
}

int print_comp_setting(std::string str, int op_num,
        std::vector<xbar_sw_data_t> *finalXbarCfgVec,
        std::vector<uint64_t> *FinalNumXbarStartPtr,
        std::vector<uint64_t> *FinalNumXbarEndPtr,
        std::vector<uint64_t> *FinalNumXbarIteration,
        std::vector<uint64_t> *FinalNumXbarStartPtr1,
        std::vector<uint64_t> *FinalNumXbarEndPtr1,
        std::vector<uint64_t> *FinalNumXbarIteration1) {
    xbar_sw_data_t xbar_entry_val;
    printf(
            "========================================compressed===============================================\n");
    printf("StartPtr0: %lu endPtr0: %lu iteration0: %lu\n",
            (*FinalNumXbarStartPtr).at(op_num),
            (*FinalNumXbarEndPtr).at(op_num),
            (*FinalNumXbarIteration).at(op_num));
    printf("StartPtr1: %lu endPtr1: %lu iteration1: %lu\n",
            (*FinalNumXbarStartPtr1).at(op_num),
            (*FinalNumXbarEndPtr1).at(op_num),
            (*FinalNumXbarIteration1).at(op_num));
    for (size_t k = 0; k < (*FinalNumXbarIteration).at(op_num); k++) {
        for (uint64_t i = (*FinalNumXbarStartPtr).at(op_num);
                i <= (*FinalNumXbarEndPtr).at(op_num); i++) {
            xbar_entry_val = (*finalXbarCfgVec).at(i);
            printf("%s(%2lu) = ", str.c_str(), i);
            // printf("%x", xbar_entry_val.sect_offst);
            print_xbar_sw_data_t(xbar_entry_val);
            printf("\n");
        }
    }

    for (size_t k = 0; k < (*FinalNumXbarIteration1).at(op_num); k++) {
        for (uint64_t i = (*FinalNumXbarStartPtr1).at(op_num);
                i <= (*FinalNumXbarEndPtr1).at(op_num); i++) {
            xbar_entry_val = (*finalXbarCfgVec).at(i);
            printf("%s(%2lu) = ", str.c_str(), i);
            //
            print_xbar_sw_data_t(xbar_entry_val);
            printf("\n");
        }
    }
    return 0;
}

int XDNNCrossBar(XComputeUnit * cu, std::shared_ptr <XDNNDescriptorDB> descDB,
  const std::string &compilerJson) {
#ifdef DEBUG
    xdnn::XDebugJournal().log("XDNNCrossBar");
#endif
    const int cuIdx = cu->getCuIdx();
    KernelInterface *ki = KernelInterfaceDB::get(cu->getHandle());
    XDNNKernelConfigInfoMgr kConfMgr(cu->getHandle()->m_kConfig.m_configRegInfo);
    const int maxCrossbarEntries 
      = kConfMgr.get(XDNNKernelConfigInfoMgr::MAX_XBAR_ENTRIES);

    bool customCrossbar = false;
    if (!compilerJson.empty())
    {
      using boost::property_tree::ptree;
      ptree pt;
      std::ifstream jsonFile(compilerJson);
      read_json(jsonFile, pt);
      if (pt.get_child_optional("xbar"))
        customCrossbar = true;
    }

    // Switch to enable crossbar table compression from viw
    /* 
     * hard-coded to false for now because there is an issue with compression.
     * need to debug further before enabling compression
     */
    const bool compression = false;

    if (!customCrossbar) {
        int operationNum = 0;
        int total_prev_elements = 0;
        int xbar_entry_num = 0;
        std::vector<xbar_sw_data_t> xbar_cfgVec_fin;
        init_xbar(&xbar_cfgVec_fin);

        std::vector < uint64_t > numXbarStartPtr;
        std::vector < uint64_t > numXbarEndPtr;
        std::vector < uint64_t > numXbarIteration;
        std::vector < uint64_t > numXbarStartPtr1;
        std::vector < uint64_t > numXbarEndPtr1;
        std::vector < uint64_t > numXbarIteration1;

        int concatFullSectNum = 0;
        int concatReplSectNum = 0;
        int concatReplUnitNum = 0;
        int concatReplUnitWidth = 0;
        const std::vector<std::string> &layerNames = descDB->getLayerNames();
        const auto & descs = descDB->getDesc();
        for (const auto & ln : layerNames)
        {
          XDNNDescriptor *desc = descs.find(ln)->second.get();
          if (desc->OpType() != XDNN_CONV
              && desc->OpType() != XDNN_MAXPOOLPIPELINED)
            continue;


            if (desc->_cOp->getInt("concat_full_sect_num") == 0) {
              concatFullSectNum = desc->_cOp->getInt("dst_full_sect_num");
            } else {
              concatFullSectNum = desc->_cOp->getInt("concat_full_sect_num");
            }

            if (desc->_cOp->getInt("concat_repl_sect_num") == 0) {
              concatReplSectNum = desc->_cOp->getInt("dst_repl_sect_num");
            } else {
              concatReplSectNum = desc->_cOp->getInt("concat_repl_sect_num");
            }

            if (desc->_cOp->getInt("concat_repl_unit_num") == 0) {
              concatReplUnitNum = desc->_cOp->getInt("dst_repl_unit_num");
            } else {
              concatReplUnitNum = desc->_cOp->getInt("concat_repl_unit_num");
            }

            if (desc->_cOp->getInt("concat_repl_unit_width") == 0) {
              concatReplUnitWidth = desc->_cOp->getInt("dst_repl_unit_width");
            } else {
              concatReplUnitWidth = desc->_cOp->getInt("concat_repl_unit_width");
            }
            
            // Factoring in half-rate mode
            int mod_array_och_num = desc->_cOp->getInt("en_halfrate_mode", 0) ? 
              C_ARRAY_OCH_NUM / 2 : C_ARRAY_OCH_NUM;

            xbar_entry_num = xbar_table_config(C_ARRAY_ROW_NUM,
                C_ARRAY_COL_NUM, mod_array_och_num /*C_ARRAY_OCH_NUM*/, C_ARRAY_XBAR_OCH,
                desc->_cOp->getInt("outchan"),
                desc->_cOp->getInt("concat_starting_ch"), 
                concatFullSectNum,
                concatReplSectNum, concatReplUnitNum, concatReplUnitWidth, 
                compression, &xbar_cfgVec_fin, &numXbarStartPtr,
                &numXbarEndPtr, &numXbarIteration, &numXbarStartPtr1,
                &numXbarEndPtr1, &numXbarIteration1,
                cu->getXbarTableCounter());
            total_prev_elements += xbar_entry_num;
            operationNum++;

            XDNNConvolutionDescriptor *conv_desc =
              (XDNNConvolutionDescriptor*) desc;
            conv_desc->_startPtr0 = numXbarStartPtr[operationNum - 1];
            conv_desc->_endPtr0 = numXbarEndPtr[operationNum - 1];
            conv_desc->_itnCount0 = numXbarIteration[operationNum - 1];
            conv_desc->_startPtr1 = numXbarStartPtr1[operationNum - 1];
            conv_desc->_endPtr1 = numXbarEndPtr1[operationNum - 1];
            conv_desc->_itnCount1 = numXbarIteration1[operationNum - 1];

            assert(numXbarStartPtr[operationNum - 1] 
              <= numXbarEndPtr[operationNum - 1]);
            assert(numXbarStartPtr1[operationNum - 1] 
              <= numXbarEndPtr1[operationNum - 1]);
            assert(
               ((conv_desc->_endPtr0 - conv_desc->_startPtr0 + 1) 
                  * conv_desc->_itnCount0)
              + ((conv_desc->_endPtr1 - conv_desc->_startPtr1 + 1) 
                  * conv_desc->_itnCount1) 
              == xbar_entry_num);

            if (numXbarEndPtr[operationNum-1] >= maxCrossbarEntries
              || numXbarEndPtr1[operationNum-1] >= maxCrossbarEntries)
            {
              std::cerr << "ERROR: max num crossbar entries (" 
                << maxCrossbarEntries << ") exceeded" << std::endl;
              exit(1);
            }
        }

        for (int i = 0; i < xbar_cfgVec_fin.size(); i++) {

            ki->writeInt(cuIdx, XDNN_CROSSBAR_TABLE_SETTING_0_3,
                    xbar_cfgVec_fin[i].mux_arr[0]);
            ki->writeInt(cuIdx, XDNN_CROSSBAR_TABLE_SETTING_4_7,
                    xbar_cfgVec_fin[i].mux_arr[1]);
            ki->writeInt(cuIdx, XDNN_CROSSBAR_TABLE_SETTING_8_11,
                    xbar_cfgVec_fin[i].mux_arr[2]);
            ki->writeInt(cuIdx, XDNN_CROSSBAR_TABLE_SETTING_12_15,
                    xbar_cfgVec_fin[i].mux_arr[3]);
            ki->writeInt(cuIdx, XDNN_CROSSBAR_TABLE_CONTROL,
                    xbar_cfgVec_fin[i].mux_arr[4]);
        }

        // update the crossbar starting offset counter
        // so if we load another network's crossbar entries,
        // they get appended instead of overwriting existing entries
        cu->updateXbarTableCounter(xbar_cfgVec_fin.size()
          + cu->getXbarTableCounter());
    } else {
        using boost::property_tree::ptree;
        ptree pt;
        std::ifstream jsonFile(compilerJson);
        read_json(jsonFile, pt);

        std::map<std::string, int> addrMap;
        addrMap["setting_0_3"] = XDNN_CROSSBAR_TABLE_SETTING_0_3;
        addrMap["setting_4_7"] = XDNN_CROSSBAR_TABLE_SETTING_4_7;
        addrMap["setting_8_11"] = XDNN_CROSSBAR_TABLE_SETTING_8_11;
        addrMap["setting_12_15"] = XDNN_CROSSBAR_TABLE_SETTING_12_15;
        addrMap["control"] = XDNN_CROSSBAR_TABLE_CONTROL;
        for (auto &entry : pt.get_child("xbar")) {
            int controlVal = 0;
            for (auto &pair : entry.second) {
                std::string key = pair.first;
                int value = strtol(pair.second.data().c_str(), NULL, 16);
                if (key == "control") {
                    controlVal = value;
                    continue;
                }

                ki->writeInt(cuIdx, addrMap[key], value);
            }
            ki->writeInt(cuIdx, addrMap["control"], controlVal);
        }
    }
    return (EXIT_SUCCESS);
}


static int _computeSrcAddrConfig(const XDNNCompilerOp *op) {
  if (op->getStr("memory_description_src", "ddr") != "ddr")
    return 0;

  const int srcConfig = op->getInt("srcAddrReadFromImgQ", 0)? 0x3: 0x2;
  return srcConfig << 16;
}

static int _computeDestAddrConfig(const XDNNCompilerOp *op) {
  if (op->getStr("memory_description_dest", "ddr") != "ddr")
    return 0;

  const int destConfig = op->getInt("destAddrReadFromImgQ", 0)? 0x3: 0x2;
  return destConfig << 18;
}

XDNNScatterDescriptor::XDNNScatterDescriptor(const XDNNCompilerOp &op) :
  XDNNDescriptor(op)
{
}

int XDNNScatterDescriptor::execute(XComputeUnit * cu) {
    if (XDNN_VERBOSE)
        printf("XDNNScatterDescriptor (%s): %dx%dx%d (AM 0x%x -> DDR 0x%x)\n",
          _cOp->getStr("name").c_str(),
          _cOp->getInt("insize_w"), _cOp->getInt("insize_h"),
          _cOp->getInt("inchan"), 
          _cOp->getInt("uram_src"), _cOp->getInt("ddr_dest"));

#ifdef DEBUG
    xdnn::XDebugJournal().log("XDNNScatter");
#endif
    const int cuIdx = cu->getCuIdx();
    long long scratchOffset = 0;
    assert(scratchOffset >= 0);
    assert(scratchOffset < numeric_limits<int>::max());
    int ddroffset = _cOp->getInt("ddr_dest") + (int) scratchOffset;

    KernelInterface *ki = KernelInterfaceDB::get(cu->getHandle());

    XDNNKernelConfigInfoMgr kConfMgr(cu->getHandle()->m_kConfig.m_configRegInfo);
    const int dspW = kConfMgr.get(XDNNKernelConfigInfoMgr::DSP_WIDTH);
    const int UL = 0x0;
    const int _waitDL = 1;
    const int _waitUL = 1;
    const int _waitConv2D = 1;
    const int _waitPool = 1;
    const int _waitEW = 1;
    const int _waitUpSmpl = 1;
    const int _srcAddrConfig = 0x0;
    // destAddrConfig depends on whether output 
    // is from Paolo's DDR src or ImgQ DDR src
    // 0x3 = ImqQ DDR
    // 0x2 = Paolo's DDR
    const int _destAddrConfig = _cOp->getInt("destAddrReadFromImgQ")? 0x3 : 0x2;
    const int xdnnCnnCmd = (_destAddrConfig << 18) | (_srcAddrConfig << 16) 
      | (_waitUpSmpl << 9) | (_waitEW << 8)
      | (_waitPool << 7) | (_waitConv2D << 6) | (_waitUL << 5)
      | (_waitDL << 4) | UL;

    ki->writeInt(cuIdx, XDNN_INPUT_IMAGE_WIDTH, _cOp->getInt("insize_w"));
    ki->writeInt(cuIdx, XDNN_INPUT_IMAGE_HEIGHT, _cOp->getInt("insize_h"));
    ki->writeInt(cuIdx, XDNN_INPUT_IMAGE_CHANNEL, _cOp->getInt("inchan"));
    ki->writeInt(cuIdx, XDNN_CNN_COMMAND, xdnnCnnCmd); //0x803f0);
    ki->writeInt(cuIdx, XDNN_TILING_CONTROL_0, 0x0);
    ki->writeInt(cuIdx, XDNN_DEST_CONCAT_INFO, 0);

    int fullSectNum = ceil(float(_cOp->getInt("inchan")) / float(dspW));
    if (_cOp->hasKey("full_sect_num"))
      fullSectNum = _cOp->getInt("full_sect_num");

    int replSectNum = 0x0;
    if (_cOp->hasKey("repl_sect_num"))
      replSectNum = _cOp->getInt("repl_sect_num");

    int replUnitNum = 0x0;
    if (_cOp->hasKey("repl_unit_num"))
      replUnitNum = _cOp->getInt("repl_unit_num");

    int replUnitWidth = 0x20;
    if (_cOp->hasKey("repl_unit_width"))
      replUnitWidth = _cOp->getInt("repl_unit_width");

    ki->writeInt(cuIdx, XDNN_INPUT_FULL_SECTION_NUMBER, fullSectNum);
    ki->writeInt(cuIdx, XDNN_INPUT_REPLICATION_SECTION_NUMBER, replSectNum);
    ki->writeInt(cuIdx, XDNN_INPUT_REPLICATION_UNIT_NUMBER, replUnitNum);
    ki->writeInt(cuIdx, XDNN_INPUT_REPLICATION_UNIT_WIDTH, replUnitWidth);
    ki->writeInt(cuIdx, XDNN_DOWNLOAD_DDR_OFFSET, ddroffset);

    ki->writeInt(cuIdx, XDNN_OUTPUT_FULL_SECTION_NUMBER,
            ceil(float(_cOp->getInt("inchan")) / float(dspW)));
    ki->writeInt(cuIdx, XDNN_OUTPUT_REPLICATION_SECTION_NUMBER, 0x0);
    ki->writeInt(cuIdx, XDNN_OUTPUT_REPLICATION_UNIT_NUMBER, 0x0);
    ki->writeInt(cuIdx, XDNN_OUTPUT_REPLICATION_UNIT_WIDTH, 0x20);

    ki->writeInt(cuIdx, XDNN_INPUT_IMAGE_BASE_ADDR, _cOp->getInt("uram_src"));
    ki->writeInt(cuIdx, XDNN_OUTPUT_IMAGE_BASE_ADDR, _cOp->getInt("ddr_dest"));
    ki->writeInt(cuIdx, XDNN_SCRIPT_MEMORY_CONTROL, 0x1);

    return (EXIT_SUCCESS);
}

XDNNGatherDescriptor::XDNNGatherDescriptor(const XDNNCompilerOp &op) :
  XDNNDescriptor ( op ) {
}

int XDNNGatherDescriptor::execute(XComputeUnit * cu) {
    if (XDNN_VERBOSE)
        printf("XDNNGatherDescriptor: %dx%dx%d (DDR 0x%x -> AM 0x%x)\n", 
          _cOp->getInt("insize_w"), _cOp->getInt("insize_h"),
          _cOp->getInt("inchan"), 
          _cOp->getInt("ddr_src"), _cOp->getInt("uram_dest"));

#ifdef DEBUG
    xdnn::XDebugJournal().log("XDNNGather");
#endif
    const int cuIdx = cu->getCuIdx();
    long long scratchOffset = 0;
    assert(scratchOffset >= 0);
    assert(scratchOffset < numeric_limits<int>::max());
    int ddroffset = _cOp->getInt("ddr_src") + (int) scratchOffset;

    KernelInterface *ki = KernelInterfaceDB::get(cu->getHandle());
    XDNNKernelConfigInfoMgr kConfMgr(cu->getHandle()->m_kConfig.m_configRegInfo);
    const int dspW = kConfMgr.get(XDNNKernelConfigInfoMgr::DSP_WIDTH);

    const int DL = 0x0;
    const int _waitDL = 1;
    const int _waitUL = 1;
    const int _waitConv2D = 1;
    const int _waitPool = 1;
    const int _waitEW = 1;
    const int _waitUpSmpl = 1;
    // srcAddrConfig depends on whether input 
    // is from Paolo's DDR src or ImgQ DDR src
    // 0x3 = ImqQ DDR
    // 0x2 = Paolo's DDR
    const int _srcAddrConfig = _cOp->getInt("srcAddrReadFromImgQ", 0)? 0x3 : 0x2;
    const int _destAddrConfig = 0x0; 
    const int _srcXDNNReshapeDL = 0;
    const int xdnnCnnCmd = (_srcXDNNReshapeDL << 20) | (_destAddrConfig << 18)
      | (_srcAddrConfig << 16) | (_waitUpSmpl << 9) | (_waitEW << 8)
      | (_waitPool << 7) | (_waitConv2D << 6) | (_waitUL << 5)
      | (_waitDL << 4) | DL;

    ki->writeInt(cuIdx, XDNN_INPUT_IMAGE_WIDTH, _cOp->getInt("insize_w"));
    ki->writeInt(cuIdx, XDNN_INPUT_IMAGE_HEIGHT, _cOp->getInt("insize_h"));
    ki->writeInt(cuIdx, XDNN_INPUT_IMAGE_CHANNEL, _cOp->getInt("inchan"));
    ki->writeInt(cuIdx, XDNN_CNN_COMMAND, xdnnCnnCmd); //0x203f0
    ki->writeInt(cuIdx, XDNN_TILING_CONTROL_0, 0x0);
    ki->writeInt(cuIdx, XDNN_DEST_CONCAT_INFO, 0);

    ki->writeInt(cuIdx, XDNN_INPUT_FULL_SECTION_NUMBER, 
      ceil(float(_cOp->getInt("inchan")) / float(dspW)));
    ki->writeInt(cuIdx, XDNN_INPUT_REPLICATION_SECTION_NUMBER, 0x0);
    ki->writeInt(cuIdx, XDNN_INPUT_REPLICATION_UNIT_NUMBER, 0x0);
    ki->writeInt(cuIdx, XDNN_INPUT_REPLICATION_UNIT_WIDTH, 0x20);
    ki->writeInt(cuIdx, XDNN_DOWNLOAD_DDR_OFFSET, ddroffset);

    int fullSectNum = ceil(float(_cOp->getInt("inchan")) / float(dspW));
    if (_cOp->hasKey("full_sect_num"))
      fullSectNum = _cOp->getInt("full_sect_num");

    int replSectNum = 0x0;
    if (_cOp->hasKey("repl_sect_num"))
      replSectNum = _cOp->getInt("repl_sect_num");

    int replUnitNum = 0x0;
    if (_cOp->hasKey("repl_unit_num"))
      replUnitNum = _cOp->getInt("repl_unit_num");

    int replUnitWidth = 0x20;
    if (_cOp->hasKey("repl_unit_width"))
      replUnitWidth = _cOp->getInt("repl_unit_width");

    ki->writeInt(cuIdx, XDNN_OUTPUT_FULL_SECTION_NUMBER, fullSectNum);
    ki->writeInt(cuIdx, XDNN_OUTPUT_REPLICATION_SECTION_NUMBER, replSectNum);
    ki->writeInt(cuIdx, XDNN_OUTPUT_REPLICATION_UNIT_NUMBER, replUnitNum);
    ki->writeInt(cuIdx, XDNN_OUTPUT_REPLICATION_UNIT_WIDTH, replUnitWidth);

    ki->writeInt(cuIdx, XDNN_INPUT_IMAGE_BASE_ADDR, _cOp->getInt("ddr_src")); 
    ki->writeInt(cuIdx, XDNN_OUTPUT_IMAGE_BASE_ADDR, _cOp->getInt("uram_dest"));
    ki->writeInt(cuIdx, XDNN_SCRIPT_MEMORY_CONTROL, 0x1);

    return (EXIT_SUCCESS);
}

XDNNEltwiseDescriptor::XDNNEltwiseDescriptor(const void *A, const XDNNCompilerOp &op) :
  XDNNDescriptor(op), _A(A), _useQuantParams(false) {
}

int XDNNEltwiseDescriptor::execute(XComputeUnit * cu) {
#ifdef DEBUG
    xdnn::XDebugJournal().log("XDNNEltwise");
#endif

    const bool _add = _cOp->getInt("add") > 0; 
    const bool _bn = _cOp->getInt("bn") > 0;
    const bool _relu = _cOp->getInt("relu") > 0;

    int op = 0;
    if (_bn && !_add) {
        op = 1;
    } else if (_add && _bn) {
        op = 0;
    } else {
        assert(_add || _bn);
    }
    const int _parallelRead = _cOp->getInt("parallel_read");
    const int _waitUpsmpl = _cOp->getInt("wait_upsmpl");
    const int _waitEw = _cOp->getInt("wait_ew");
    const int _waitPool = _cOp->getInt("wait_pool");
    const int _waitConv = _cOp->getInt("wait_conv");
    const int _waitUpload = _cOp->getInt("wait_upload");
    const int _waitDownload = _cOp->getInt("wait_download");
    int xdnnCommand = _parallelRead << 27 | _relu << 26 | op << 21
            | _waitUpsmpl << 9 | _waitEw << 8 | _waitPool << 7 | _waitConv << 6
            | _waitUpload << 5 | _waitDownload << 4 | 0x3;

    const int _inImgSizex = _cOp->getInt("insize_w");
    const int _inImgSizey = _cOp->getInt("insize_h");
    const int _inImgChannels = _cOp->getInt("inchan");
    const int _inBaseAddr = _cOp->getInt("inaddrA");
    const int _inBaseAddrB = _cOp->getInt("inaddrB");
    const int _outBaseAddr = _cOp->getInt("outaddr");

    int imgSize_elt = _inImgSizex | _inImgSizey << 16;
    if (XDNN_VERBOSE)
        printf("Eltwise (add:%d, bn:%d, relu:%d): %dx%dx%d -> %dx%dx%d\n", _add,
                _bn, _relu, _inImgSizex, _inImgSizey, _inImgChannels,
                _inImgSizex, _inImgSizey, _inImgChannels);

    const int cuIdx = cu->getCuIdx();

    KernelInterface *ki = KernelInterfaceDB::get(cu->getHandle());

    const int _prelu = _cOp->getInt("prelu", 0);
    const int _HAT = _cOp->getInt("HAT");

    xdnnCommand = _prelu<<28 | xdnnCommand;
    
    if(_HAT==1)
    {
       const int _tileHeight = _cOp->getInt("tile_height");
       const int _tileWidth = _cOp->getInt("tile_width");
       const int _SRCAM_Buffer_0 = _cOp->getInt("SRCAM-Buffer_0");
       const int _SRCAM_Buffer_1 = _cOp->getInt("SRCAM-Buffer_1");
       const int _DEST_AM_Buffer_Offset = _cOp->getInt("DEST_AM-Buffer_Offset");
       const int _EWA_2nd_Src_AM = _cOp->getInt("EWA_2nd-Src_AM");
       int tilingCmd = (_HAT<<31) | (_tileHeight << 16) | _tileWidth;
       const int _streamToDdr = 0;// (_cOp->getStr("memory_description_dest", "ddr") == "ddr")? 1 : 0;
       xdnnCommand |= (_streamToDdr << 15);
       xdnnCommand |= _computeSrcAddrConfig(_cOp.get());
       xdnnCommand |= _computeDestAddrConfig(_cOp.get());
       // xdnnCommand = xdnnCommand | (1<<17) | (1<<19);
       ki->writeInt(cuIdx, XDNN_TILING_CONTROL_0, tilingCmd); 
       ki->writeInt(cuIdx, XDNN_SRC_AM_BUFFER_0, _SRCAM_Buffer_0);
       ki->writeInt(cuIdx,XDNN_SRC_AM_BUFFER_1, _SRCAM_Buffer_1);
       ki->writeInt(cuIdx, XDNN_DEST_AM_BUFFER_OFFSET_ADDR, _DEST_AM_Buffer_Offset);
       ki->writeInt(cuIdx, XDNN_ELT_WISE_SEC_SRC_AM_ADDR, _EWA_2nd_Src_AM);
       ki->writeInt(cuIdx, XDNN_ELT_WISE_SEC_SRC_DDR_ADDR, _inBaseAddrB);
    }
    else
    {
      ki->writeInt(cuIdx, XDNN_TILING_CONTROL_0, 0x0);

      ki->writeInt(cuIdx, XDNN_ELT_WISE_SEC_SRC_AM_ADDR, _inBaseAddrB);
    }

    // by default, make global quantization parameters to identity values so that they are not in effect
    ki->writeInt(cuIdx, XDNN_CONV2D_GLOBAL_DSP_SCALE_PARAM, 1 << 0 | 0 << 24);
    ki->writeInt(cuIdx, XDNN_CONV2D_GLOBAL_DSP_BIAS_PARAM, 0);
 
    if (_useQuantParams) {
        if (_scale.size() == 1) {
            ///Assuming global qp's for a given elt-wise layer have json format as only one scale, postShift and preShift value in the file
            // suitable for basic feature map scaling.
            int qparams = _postShift[0] << 24 | _scale[0];
            ki->writeInt(cuIdx, XDNN_CONV2D_GLOBAL_DSP_SCALE_PARAM, qparams);
            ki->writeInt(cuIdx, XDNN_CNN_COMMAND, xdnnCommand);
        } else {
            //per-channel quantization
            XMemPtr *aPtr = cu->getHandle()->getHost2XMem(_A);
            assert(aPtr != NULL);
            XDeviceMemPOD<short> *devMemPOD = aPtr->getDeviceMemPOD(cu->getCuIdx());
            assert(devMemPOD && devMemPOD->m_devicePtr);
            cl_mem input_a = devMemPOD->m_devicePtr;

            int _perChannel = 1;
            xdnnCommand = _perChannel << 23 | xdnnCommand;
            ki->writeInt(cuIdx, XDNN_CNN_COMMAND, xdnnCommand);

            const std::string uniqueLayerName = getUniqueLayerName(_name);

            ki->writeInt(cuIdx, XDNN_CONV2D_GLOBAL_DSP_SCALE_PARAM, 0);
            auto weightIt = cu->getWeightOffsetMap().find(uniqueLayerName);
            if (weightIt != cu->getWeightOffsetMap().end()) {
              int offset = cu->getWeightOffsetMap().at(uniqueLayerName);
              int physBase = cu->computeDdrOffset(input_a);
              int physOffset = physBase + offset;
              ki->writeInt(cuIdx, XDNN_FILTER_DDR_ADDR_OFFSET, physOffset);
            } else if (_bn) {
              std::cout << "[XDNN] cfg params offset for layer "
                      << uniqueLayerName << " not found" << std::endl;
              assert(0);
            } 
        }
    }
    else
      ki->writeInt(cuIdx, XDNN_CNN_COMMAND, xdnnCommand);
     
    ki->writeInt(cuIdx, XDNN_CNN_KERNEL, 0x1111); //hardware requires this to be set to 0x1111 always for any elt-wise

    const int _srcFullSectNum = _cOp->getInt("src_full_sect_num");
    const int _srcReplSectNum = _cOp->getInt("src_repl_sect_num");
    const int _srcReplUnitNum = _cOp->getInt("src_repl_unit_num");
    const int _srcReplUnitWidth = _cOp->getInt("src_repl_unit_width");
    const int _dstFullSectNum = _cOp->getInt("dst_full_sect_num");
    const int _dstReplSectNum = _cOp->getInt("dst_repl_sect_num");
    const int _dstReplUnitNum = _cOp->getInt("dst_repl_unit_num");
    const int _dstReplUnitWidth = _cOp->getInt("dst_repl_unit_width");

    ki->writeInt(cuIdx, XDNN_DEST_CONCAT_INFO, 0);
    ki->writeInt(cuIdx, XDNN_INPUT_FULL_SECTION_NUMBER, _srcFullSectNum);
    ki->writeInt(cuIdx, XDNN_INPUT_REPLICATION_SECTION_NUMBER, _srcReplSectNum);
    ki->writeInt(cuIdx, XDNN_INPUT_REPLICATION_UNIT_NUMBER, _srcReplUnitNum);
    ki->writeInt(cuIdx, XDNN_INPUT_REPLICATION_UNIT_WIDTH, _srcReplUnitWidth);
    ki->writeInt(cuIdx, XDNN_OUTPUT_FULL_SECTION_NUMBER, _dstFullSectNum);
    ki->writeInt(cuIdx, XDNN_OUTPUT_REPLICATION_SECTION_NUMBER,
            _dstReplSectNum);
    ki->writeInt(cuIdx, XDNN_OUTPUT_REPLICATION_UNIT_NUMBER, _dstReplUnitNum);
    ki->writeInt(cuIdx, XDNN_OUTPUT_REPLICATION_UNIT_WIDTH, _dstReplUnitWidth);
    ki->writeInt(cuIdx, XDNN_OUTPUT_IMAGE_BASE_ADDR, _outBaseAddr);
    ki->writeInt(cuIdx, XDNN_INPUT_IMAGE_WIDTH, _inImgSizex);
    ki->writeInt(cuIdx, XDNN_INPUT_IMAGE_HEIGHT, _inImgSizey);
    ki->writeInt(cuIdx, XDNN_OUTPUT_IMAGE_WIDTH, _inImgSizex);
    ki->writeInt(cuIdx, XDNN_OUTPUT_IMAGE_HEIGHT, _inImgSizey);
    ki->writeInt(cuIdx, XDNN_INPUT_IMAGE_CHANNEL, _inImgChannels);
    ki->writeInt(cuIdx, XDNN_OUTPUT_IMAGE_CHANNEL, _inImgChannels);
    ki->writeInt(cuIdx, XDNN_INPUT_IMAGE_BASE_ADDR, _inBaseAddr);
    ki->writeInt(cuIdx, XDNN_SCRIPT_MEMORY_CONTROL, 0x1); // push

    return (EXIT_SUCCESS);
}
int XDNNInputDescriptor::execute(XComputeUnit *cu) {
	assert(0);
  // deprecated
}


int XDNNOutputDescriptor::execute(XComputeUnit *cu) {
  assert(0);
  // deprecated
}

XDNNConvolutionDepthDescriptor::XDNNConvolutionDepthDescriptor(const void *A,
    int Aoffset, int biasOffset, const XDNNCompilerOp &op) :
    XDNNConvolutionDescriptor(A, Aoffset, biasOffset, op)
{
}

int XDNNConvolutionDepthDescriptor::execute(XComputeUnit *cu) {
#ifdef DEBUG
    xdnn::XDebugJournal().log("XDNNDepthConv");
#endif
    //cl_kernel kernel = handle.kernels[packet->kernelIdx];
    assert(OpType() == XDNN_CONVDEPTH);

    const int cuIdx = cu->getCuIdx();
    KernelInterface *ki = KernelInterfaceDB::get(cu->getHandle());
    // A needs to already have been written to DDR (e.g., xblasLoadA())
    XMemPtr *aPtr = cu->getHandle()->getHost2XMem(_A);
    assert(aPtr != NULL);
    XDeviceMemPOD<short> *devMemPOD = aPtr->getDeviceMemPOD(cu->getCuIdx());
    assert(devMemPOD && devMemPOD->m_devicePtr);
    cl_mem input_a = devMemPOD->m_devicePtr;
    const int kernSizex = _cOp->getInt("kernel_w");
    const int kernSizey = _cOp->getInt("kernel_h");
    const int kernStridex = _cOp->getInt("strides_w");
    const int kernStridey = _cOp->getInt("strides_h");
    const int inImgSize = _cOp->getInt("insize_w") | _cOp->getInt("insize_h") << 16;
    const int outImgSize = _cOp->getInt("outsize_w") | _cOp->getInt("outsize_h") << 16;
    const int paddingx = _cOp->getInt("padding_w");
    const int paddingy = _cOp->getInt("padding_h");
    const int dilx = _cOp->getInt("dilation_w") - 1;
    const int dily = _cOp->getInt("dilation_h") - 1;
    const int kernControl = (dily << 28) | (dilx << 24) | (paddingy << 20)
                    | (paddingx << 16) | (kernStridey << 12) | (kernStridex << 8)
                    | (kernSizey << 4) | kernSizex;
    const std::string uniqueLayerName = getUniqueLayerName(_name);
    if (cu->getWeightOffsetMap().find(uniqueLayerName)
            == cu->getWeightOffsetMap().end()) {
        std::cout << "[XDNN] weight offset for layer " << uniqueLayerName
                << " not found" << std::endl;
        assert(0);
    }
    int layerWeightByteOffset = cu->getWeightOffsetMap().at(uniqueLayerName);
    int offset = _Aoffset + layerWeightByteOffset;
    int biasOffset = _biasOffset + layerWeightByteOffset;
    int preShift = _preShift;
    int postShift = _postShift;
    int scaleVal = _scaleVal;
    bool perChannelScale = _perChannelScale;
    if (perChannelScale) {
        preShift += layerWeightByteOffset;
        postShift += layerWeightByteOffset;
        scaleVal += layerWeightByteOffset;
    }
    const bool relu = _cOp->getInt("relu") > 0;
    const int bias = _cOp->getInt("bias");
    const int inBaseAddr = _cOp->getInt("inaddr");
    const int inImgChannels = _cOp->getInt("inchan");
    const int outBaseAddr = _cOp->getInt("outaddr");
    const int outImgChannels = _cOp->getInt("outchan");
    int enableConvDepth = 1;
    int xdnnCommand = (enableConvDepth << 9) | (relu << 8) | (bias << 7)
                    | (perChannelScale << 6) | (perChannelScale << 5)
                    | (perChannelScale << 4) | 0x02; // 0x02 for Convolution

    int ddr_cache_mode = 0;
    int bypass_perf = 0;
    int ddr_cache = (ddr_cache_mode << 31) | (bypass_perf << 28);
    ki->writeInt(cuIdx, XDNN_CNN_COMMAND, xdnnCommand);
    ki->writeInt(cuIdx, XDNN_INPUT_IMAGE_CHANNEL, inImgChannels);
    ki->writeInt(cuIdx, XDNN_OUTPUT_IMAGE_CHANNEL, outImgChannels);
    ki->writeInt(cuIdx, XDNN_INPUT_IMAGE_BASE_ADDR, inBaseAddr);
    ki->writeInt(cuIdx, XDNN_OUTPUT_IMAGE_BASE_ADDR, outBaseAddr);

    ki->writeInt(cuIdx, XDNN_CNN_KERNEL, kernControl);
    int physBase = cu->computeDdrOffset(input_a);
    int physOffset = physBase + offset;
    int physBiasOffset = physBase + biasOffset;

    ki->writeInt(cuIdx, XDNN_DEST_CONCAT_INFO, 0);

    if (perChannelScale) {
        int physPreShiftOffset = physBase + preShift;
        int physScaleOffset = physBase + scaleVal;
        int physPostShiftOffset = physBase + postShift;

        ki->writeInt(cuIdx,
                XDNN_DSP_PRE_SCALE_RIGHT_SHIFT_DDR_ADDR_OFFSET, physPreShiftOffset);
        ki->writeInt(cuIdx, XDNN_DSP_SCALE_DDR_ADDR_OFFSET, physScaleOffset);
        ki->writeInt(cuIdx,
                XDNN_DSP_POST_SCALE_RIGHT_SHIFT_DDR_ADDR_OFFSET, physPostShiftOffset);
        // force global scale values to 0 making sure it isn't in effect
        ki->writeInt(cuIdx,
                XDNN_CONV2D_GLOBAL_DSP_SCALE_PARAM, 0 << 0 | 0 << 8 | 0 << 16);
    } else
        ki->writeInt(cuIdx, XDNN_CONV2D_GLOBAL_DSP_SCALE_PARAM,
                preShift << 0 | scaleVal << 8 | postShift << 16);
    ki->writeInt(cuIdx, XDNN_CONV2D_FILTERS_DDR_ADDR_OFFSET, physOffset);

    ki->writeInt(cuIdx, XDNN_SCRIPT_MEMORY_CONTROL, 0x1); // push
    return (EXIT_SUCCESS);
}

static int int_ceil(int num, int den) {
    int result = ceil(float(num) / float(den));
    return result;
}

XDNNMaxPoolPipelinedDescriptor::XDNNMaxPoolPipelinedDescriptor(const void *A,
    int Aoffset, int biasOffset, const XDNNCompilerOp &op) :
    XDNNConvolutionDescriptor(A, Aoffset, biasOffset, op)
{
  _name = _cOp->getStr("conv_name");
}


static void computeFilterAxiDataCounts(XDNNCompilerOp *cOp, 
  int _kernSizex, int _kernSizey, bool is8Bit, 
  int &filterDspDataCount, int &filterAxiReadCount)
{
  const int _srcFullSectNum = cOp->getInt("src_full_sect_num");
  const int _srcReplSectNum = cOp->getInt("src_repl_sect_num");
  const int _srcReplUnitNum = cOp->getInt("src_repl_unit_num");
  const int _enHalfrateMode = cOp->getInt("en_halfrate_mode");
  const int slice = cOp->getInt("slice");

  filterDspDataCount = (_srcReplSectNum==0) ?
     (_srcFullSectNum*_kernSizex*_kernSizey) 
   : (_srcFullSectNum*_kernSizex*_kernSizey
      + int_ceil((_kernSizex*_kernSizey*_srcReplSectNum), _srcReplUnitNum));
  filterDspDataCount = (_enHalfrateMode) ? 
    (filterDspDataCount + 1) / 2 : filterDspDataCount;

  int C_ARRAY_ROW_NUM_ARR[2]= {21,96};
  int C_ARRAY_KMEM_WIDTH=32;

  int C_ARRAY_SIMUL_OPS = (is8Bit)?2:1;
  int C_SA_CFG_INDEXES=8;
  int C_ARRAY_COL_NUM_ARR[2]= {8,16};
  int C_AXI_DATA_WIDTH=512;
  int max_conv_och_per_column = 2*C_ARRAY_SIMUL_OPS;
  int max_conv_och_per_iteration = C_ARRAY_COL_NUM_ARR[slice]*max_conv_och_per_column;

  int num_axi_words_per_array_rows 
    = ceil(float(C_ARRAY_ROW_NUM_ARR[slice]*C_ARRAY_KMEM_WIDTH)/float(C_AXI_DATA_WIDTH));

  int convAxiFilterPerIter = (((max_conv_och_per_iteration*num_axi_words_per_array_rows)
                              / max_conv_och_per_column)*filterDspDataCount);
  int convAxiMacCfgPerIter = ceil(float(max_conv_och_per_iteration*C_SA_CFG_INDEXES*8)
                                  /float(C_AXI_DATA_WIDTH));

  filterAxiReadCount = convAxiMacCfgPerIter + convAxiFilterPerIter;

#ifdef ENABLE_HBM
  //Todo: check how this varies for slice 0 
  if(slice!=0)
    {
     int numberofDdrBanks = C_ARRAY_ROW_NUM_ARR[slice]/C_ARRAY_COL_NUM_ARR[slice];
     filterAxiReadCount = filterAxiReadCount / numberofDdrBanks;
    }

#endif

}

int XDNNMaxPoolPipelinedDescriptor::execute(XComputeUnit * cu) {
#ifdef DEBUG
    xdnn::XDebugJournal().log("XDNNMaxPoolPipelined");
#endif
    XBLASHandle & handle = *cu->getHandle();
    assert(OpType() == XDNN_MAXPOOLPIPELINED);

    const int cuIdx = cu->getCuIdx();

    // A needs to already have been written to DDR (e.g., xblasLoadA())
    XMemPtr *aPtr = handle.getHost2XMem(_A);
    assert(aPtr != NULL);
    XDeviceMemPOD<short> *devMemPOD = aPtr->getDeviceMemPOD( cu->getCuIdx());
    assert(devMemPOD && devMemPOD->m_devicePtr);
    cl_mem input_a = devMemPOD->m_devicePtr;

    const int _convRelu = _cOp->getInt("conv_relu");
    const int _convBias = _cOp->getInt("conv_bias");
    const int _convPRelu = _cOp->getInt("conv_prelu",0);
    const int _enInlineMaxpool = _cOp->getInt("en_inlinemaxpool");
    assert(_enInlineMaxpool);
    const int _preRelu = _cOp->getInt("prerelu");
    const int _enHalfrateMode = _cOp->getInt("en_halfrate_mode");
    const int _enPingpongWeight = _cOp->getInt("en_pingpong_weight");
    const int _waitUpsmpl = _cOp->getInt("wait_upsmpl");
    const int _waitEw = _cOp->getInt("wait_ew");
    const int _waitPool = _cOp->getInt("wait_pool");
    const int _waitConv = _cOp->getInt("wait_conv");
    const int _waitUpload = _cOp->getInt("wait_upload");
    const int _waitDownload = _cOp->getInt("wait_download");

    const int _postRelu = _convRelu;
    const int _conv2D = 1;
    int xdnnCnnCmd = (_convPRelu << 28) |(_enInlineMaxpool << 27) | (_postRelu << 26)
      | (_preRelu << 22) | (_enHalfrateMode << 21)
      | (_enPingpongWeight << 20) | (_waitUpsmpl << 9) | (_waitEw << 8)
      | (_waitPool << 7) | (_waitConv << 6) | (_waitUpload << 5)
      | (_waitDownload << 4) | _conv2D;

    const int kernSizex = _cOp->getInt("conv_kernel_w");
    const int kernSizey = _cOp->getInt("conv_kernel_h");
    const int kernStridex = _cOp->getInt("conv_strides_w");
    const int kernStridey = _cOp->getInt("conv_strides_h");
    const int paddingx = _cOp->getInt("conv_padding_w");
    const int paddingy = _cOp->getInt("conv_padding_h");
    const int _inImgSizex = _cOp->getInt("insize_w");
    const int _inImgSizey = _cOp->getInt("insize_h");
    const int _outImgSizex = _cOp->getInt("outsize_w");
    const int _outImgSizey = _cOp->getInt("outsize_h");
    const int inImgSize = _inImgSizex | _inImgSizey << 16;
    const int outImgSize = _outImgSizex | _outImgSizey << 16;
    const int dilx = _cOp->getInt("conv_dilation_w") - 1;
    const int dily = _cOp->getInt("conv_dilation_h") - 1;

    int kernControl = (dily << 28) | (dilx << 24) | (paddingy << 20)
                    | (paddingx << 16) | (kernStridey << 12) | (kernStridex << 8)
                    | (kernSizey << 4) | kernSizex;

    const int _poolPadW = _cOp->getInt("paddings_w");
    const int _poolPadH = _cOp->getInt("paddings_h");
    const int _poolStrideW = _cOp->getInt("strides_w");
    const int _poolStrideH = _cOp->getInt("strides_h");
    const int _poolKernW = _cOp->getInt("kernel_w");
    const int _poolKernH = _cOp->getInt("kernel_h");

    int poolKernControl = (_poolPadH << 20) | (_poolPadW << 16)
                    | (_poolStrideH << 12) | (_poolStrideW << 8) | (_poolKernH << 4)
                    | _poolKernW;

    const std::string uniqueLayerName 
      = getUniqueLayerName(_cOp->getStr("conv_name"));
    if (cu->getWeightOffsetMap().find(uniqueLayerName)
            == cu->getWeightOffsetMap().end()) {
        std::cout << "[XDNN] weight offset for layer " << uniqueLayerName
                << " not found" << std::endl;
        assert(0);
    }
    int layerWeightByteOffset = cu->getWeightOffsetMap().at(uniqueLayerName);
    int offset = _Aoffset + layerWeightByteOffset;
    int biasOffset = _biasOffset + layerWeightByteOffset;
    bool perChannelScale = _perChannelScale;
    bool relu = _convRelu;
    int bias = _convBias;
    int _inBaseAddr = _cOp->getInt("inaddr");
    int _inImgChannels = _cOp->getInt("inchan");
    int _outBaseAddr = _cOp->getInt("outaddr");
    int _outImgChannels = _cOp->getInt("pool_inchan");

    int xdnnCommand = (relu << 8) | (bias << 7) | (perChannelScale << 6)
                    | (perChannelScale << 5) | (perChannelScale << 4) | 0x02; // 0x02 for Convolution

    KernelInterface *ki = KernelInterfaceDB::get(&handle);

    XDNNKernelConfigInfoMgr kConfMgr(handle.m_kConfig.m_configRegInfo);
    const bool is8Bit = kConfMgr.get(XDNNKernelConfigInfoMgr::MODE8BIT);

    const int _srcFullSectNum = _cOp->getInt("src_full_sect_num");
    const int _srcReplSectNum = _cOp->getInt("src_repl_sect_num");
    const int _srcReplUnitNum = _cOp->getInt("src_repl_unit_num");
    const int _srcReplUnitWidth = _cOp->getInt("src_repl_unit_width");
    const int _dstFullSectNum = _cOp->getInt("dst_full_sect_num");
    const int _dstReplSectNum = _cOp->getInt("dst_repl_sect_num");
    const int _dstReplUnitNum = _cOp->getInt("dst_repl_unit_num");
    const int _dstReplUnitWidth = _cOp->getInt("dst_repl_unit_width");
    const int slice = _cOp->getInt("slice");

    int filterDspDataCount = 0;
    int filterAxiReadCount = 0;
    computeFilterAxiDataCounts(_cOp.get(), kernSizex, kernSizey, 
      is8Bit, filterDspDataCount, filterAxiReadCount);

    /********************************************
     *********AutoProgrammed CSR Registers*******
     *******************************************/
    int physBaseMaxPool = cu->computeDdrOffset(input_a);
    int physOffsetMaxPool = physBaseMaxPool + offset;
    const int _HAT = _cOp->getInt("HAT");
    const int _tileWidth = _cOp->getInt("tile_width");
    const int _tileHeight = _cOp->getInt("tile_height");
    const int _SRCAM_Buffer_0 = _cOp->getInt("SRCAM-Buffer_0");
    const int _SRCAM_Buffer_1 = _cOp->getInt("SRCAM-Buffer_1");

    if (_HAT == 1) {
        const int _DEST_AM_Buffer_Offset = _cOp->getInt("DEST_AM-Buffer_Offset");
        const int _streamToDdr = (_cOp->getStr("memory_description_dest", "ddr") == "ddr")? 1 : 0;
        xdnnCnnCmd |= (_streamToDdr << 15);
        xdnnCnnCmd |= _computeSrcAddrConfig(_cOp.get());
        xdnnCnnCmd |= _computeDestAddrConfig(_cOp.get());
        int tilingControlCmd = (_HAT << 31) | (_tileHeight << 16) | (_tileWidth);
        ki->writeInt(cuIdx, slice, XDNN_SRC_AM_BUFFER_0, _SRCAM_Buffer_0);
        ki->writeInt(cuIdx, slice, XDNN_SRC_AM_BUFFER_1, _SRCAM_Buffer_1);
        ki->writeInt(cuIdx, slice, XDNN_DEST_AM_BUFFER_OFFSET_ADDR, _DEST_AM_Buffer_Offset);
        ki->writeInt(cuIdx, slice, XDNN_TILING_CONTROL_0, tilingControlCmd);
    } else {
        ki->writeInt(cuIdx, slice, (size_t) XDNN_TILING_CONTROL_0, 0x0);
    }
    ki->writeInt(cuIdx, slice, XDNN_CNN_COMMAND, xdnnCnnCmd);
    ki->writeInt(cuIdx, slice, XDNN_INPUT_FULL_SECTION_NUMBER, _srcFullSectNum);
    ki->writeInt(cuIdx, slice, XDNN_INPUT_REPLICATION_SECTION_NUMBER, _srcReplSectNum);
    ki->writeInt(cuIdx, slice, XDNN_INPUT_REPLICATION_UNIT_NUMBER, _srcReplUnitNum);
    ki->writeInt(cuIdx, slice, XDNN_INPUT_REPLICATION_UNIT_WIDTH, _srcReplUnitWidth);
    ki->writeInt(cuIdx, slice, XDNN_OUTPUT_FULL_SECTION_NUMBER, _dstFullSectNum);
    ki->writeInt(cuIdx, slice, XDNN_OUTPUT_REPLICATION_SECTION_NUMBER, _dstReplSectNum);
    ki->writeInt(cuIdx, slice, XDNN_OUTPUT_REPLICATION_UNIT_NUMBER, _dstReplUnitNum);
    ki->writeInt(cuIdx, slice, XDNN_OUTPUT_REPLICATION_UNIT_WIDTH, _dstReplUnitWidth);
    ki->writeInt(cuIdx, slice, XDNN_OUTPUT_IMAGE_BASE_ADDR, _outBaseAddr);
    ki->writeInt(cuIdx, slice, XDNN_INPUT_IMAGE_WIDTH, _inImgSizex);
    ki->writeInt(cuIdx, slice, XDNN_INPUT_IMAGE_HEIGHT, _inImgSizey);
    ki->writeInt(cuIdx, slice, XDNN_OUTPUT_IMAGE_WIDTH, _outImgSizex);
    ki->writeInt(cuIdx, slice, XDNN_OUTPUT_IMAGE_HEIGHT, _outImgSizey);
    ki->writeInt(cuIdx, slice, (size_t) XDNN_DEST_CONCAT_INFO, 0);
    ki->writeInt(cuIdx, slice, XDNN_INPUT_IMAGE_CHANNEL, _inImgChannels);
    ki->writeInt(cuIdx, slice, XDNN_OUTPUT_IMAGE_CHANNEL, _outImgChannels);
    ki->writeInt(cuIdx, slice, XDNN_INPUT_IMAGE_BASE_ADDR, _inBaseAddr);
    ki->writeInt(cuIdx, slice, XDNN_CNN_KERNEL, kernControl);
    ki->writeInt(cuIdx, slice, INLINE_MAXPOOL_OPERATION, poolKernControl);
    ki->writeInt(cuIdx, slice, INLINE_MAXPOOL_INPUT_WIDTH, _cOp->getInt("pool_insize_w"));
    ki->writeInt(cuIdx, slice, INLINE_MAXPOOL_INPUT_HEIGHT, _cOp->getInt("pool_insize_h"));
    int physBase = cu->computeDdrOffset(input_a);
    int physOffset = physBase + offset;

    ki->writeInt(cuIdx, slice, XDNN_FILTER_DDR_ADDR_OFFSET, physOffset);
    ki->writeInt(cuIdx, slice, XDNN_FILTER_AXI_READ_COUNT, filterAxiReadCount);
    ki->writeInt(cuIdx, slice, XDNN_FILTER_SRC_ITERATION_COUNT, 0x1);
    ki->writeInt(cuIdx, slice, XDNN_FILTER_DSP_DATA_COUNT, filterDspDataCount);

    if (!_cOp->hasKey("override_xbar_table_ptr_0")) {
        int tablePtr0 = (_itnCount0 << 24) | (_endPtr0 << 12) | (_startPtr0);
        int tablePtr1 = (_itnCount1 << 24) | (_endPtr1 << 12) | (_startPtr1);
        ki->writeInt(cuIdx, slice, XDNN_CROSSBAR_TABLE_POINTER_0, tablePtr0);
        ki->writeInt(cuIdx, slice, XDNN_CROSSBAR_TABLE_POINTER_1, tablePtr1);
    } else {
      ki->writeInt(cuIdx, slice, XDNN_CROSSBAR_TABLE_POINTER_0, 
        _cOp->getInt("override_xbar_table_ptr_0"));
      ki->writeInt(cuIdx, slice, XDNN_CROSSBAR_TABLE_POINTER_1,
        _cOp->getInt("override_xbar_table_ptr_1"));
    }
    ki->writeInt(cuIdx, slice, XDNN_SCRIPT_MEMORY_CONTROL, 0x1);
    /******************
     *******end********
     *****************/

    return (EXIT_SUCCESS);
}

XDNNConvolutionDescriptor::XDNNConvolutionDescriptor(const void *A, int Aoffset,
  int biasOffset, const XDNNCompilerOp &op) :
  XDNNDescriptor(op),
  _A(A), _Aoffset(Aoffset), _biasOffset(biasOffset),
  _preShift(op.getInt("preshift", 0)), 
  _postShift(op.getInt("postshift", 0)), 
  _scaleVal(op.getInt("scale", 0)), _perChannelScale(false),
  _startPtr0(-1), _startPtr1(-1), _endPtr0(-1), _endPtr1(-1), 
  _itnCount0(-1), _itnCount1(-1)
  {
}

int XDNNConvolutionDescriptor::execute(XComputeUnit *cu) {
#ifdef DEBUG
    xdnn::XDebugJournal().log("XDNNConv");
#endif
    XBLASHandle &handle = *cu->getHandle();
    assert(OpType() == XDNN_CONV);

    const int cuIdx = cu->getCuIdx();

    // A needs to already have been written to DDR (e.g., xblasLoadA())
    XMemPtr *aPtr = handle.getHost2XMem(_A);
    assert(aPtr != NULL);
    XDeviceMemPOD<short> *devMemPOD = aPtr->getDeviceMemPOD(cuIdx);
    assert(devMemPOD && devMemPOD->m_devicePtr);
    cl_mem input_a = devMemPOD->m_devicePtr;

    const bool _relu = _cOp->getInt("relu") > 0;

    const int _preRelu = _cOp->getInt("prerelu", 0);
    const int _postRelu=_relu;
    const int _conv2D=1;
    const int _prelu = _cOp->getInt("prelu", 0);
    const int _enInlineMaxpool = _cOp->getInt("en_inlinemaxpool");
    const int _enHalfrateMode = _cOp->getInt("en_halfrate_mode");
    const int _enPingpongWeight = _cOp->getInt("en_pingpong_weight");
    const int _waitUpsmpl = _cOp->getInt("wait_upsmpl");
    const int _waitEw = _cOp->getInt("wait_ew");
    const int _waitPool = _cOp->getInt("wait_pool");
    const int _waitConv = _cOp->getInt("wait_conv");
    const int _waitUpload = _cOp->getInt("wait_upload");
    const int _waitDownload = _cOp->getInt("wait_download");
    int xdnnCnnCmd= (_prelu<<28) | (_enInlineMaxpool<<27) | (_postRelu<<26) | (_preRelu<<22) | (_enHalfrateMode<<21) | (_enPingpongWeight<<20) | (_waitUpsmpl<<9) | (_waitEw<<8) | (_waitPool<<7) | (_waitConv<<6) | (_waitUpload<<5) | (_waitDownload<<4) | _conv2D;

    const int kernSizex = _cOp->getInt("kernel_w");
    const int kernSizey = _cOp->getInt("kernel_h");
    const int kernStridex = _cOp->getInt("strides_w");
    const int kernStridey = _cOp->getInt("strides_h");
    const int inImgSize = _cOp->getInt("insize_w") | _cOp->getInt("insize_h") << 16;
    const int outImgSize = _cOp->getInt("outsize_w") | _cOp->getInt("outsize_h") << 16;
    const int paddingx = _cOp->getInt("padding_w");
    const int paddingy = _cOp->getInt("padding_h");
    const int dilx = _cOp->getInt("dilation_w") - 1;
    const int dily = _cOp->getInt("dilation_h") - 1;

    int kernControl = (dily<<28)|(dilx<<24)|(paddingy<<20)|(paddingx<<16)|(kernStridey<<12)|(kernStridex<<8)|(kernSizey<<4)| kernSizex;

    const std::string uniqueLayerName = getUniqueLayerName(_name);
    if (cu->getWeightOffsetMap().find(uniqueLayerName)
            == cu->getWeightOffsetMap().end()) {
        std::cout << "[XDNN] weight offset for layer " << uniqueLayerName
                << " not found" << std::endl;
        assert(0);
    }
    int layerWeightByteOffset = cu->getWeightOffsetMap().at(uniqueLayerName);
    int offset = _Aoffset + layerWeightByteOffset;
    int biasOffset = _biasOffset + layerWeightByteOffset;
    int preShift = _preShift;
    int postShift = _postShift;
    int scaleVal = _scaleVal;
    bool perChannelScale = _perChannelScale;
    if (perChannelScale) {
        preShift += layerWeightByteOffset;
        postShift += layerWeightByteOffset;
        scaleVal += layerWeightByteOffset;
    }
    const bool relu = _relu;
    const int _bias = _cOp->getInt("bias");
    const int _inBaseAddr = _cOp->getInt("inaddr");
    const int _inImgSizex = _cOp->getInt("insize_w");
    const int _inImgSizey = _cOp->getInt("insize_h");
    const int _inImgChannels = _cOp->getInt("inchan");
    const int _outBaseAddr = _cOp->getInt("outaddr");
    const int _outImgSizex = _cOp->getInt("outsize_w");
    const int _outImgSizey = _cOp->getInt("outsize_h");
    const int _outImgChannels = _cOp->getInt("outchan");

    int xdnnCommand = (relu << 8) | (_bias << 7) | (perChannelScale << 6)
              | (perChannelScale << 5) | (perChannelScale << 4) | 0x02; // 0x02 for Convolution

    KernelInterface *ki = KernelInterfaceDB::get(cu->getHandle());

    const int _srcFullSectNum = _cOp->getInt("src_full_sect_num");
    const int _srcReplSectNum = _cOp->getInt("src_repl_sect_num");
    const int _srcReplUnitNum = _cOp->getInt("src_repl_unit_num");
    const int _srcReplUnitWidth = _cOp->getInt("src_repl_unit_width");
    const int _dstFullSectNum = _cOp->getInt("dst_full_sect_num");
    const int _dstReplSectNum = _cOp->getInt("dst_repl_sect_num");
    const int _dstReplUnitNum = _cOp->getInt("dst_repl_unit_num");
    const int _dstReplUnitWidth = _cOp->getInt("dst_repl_unit_width");
    const int slice = _cOp->getInt("slice");

    XDNNKernelConfigInfoMgr kConfMgr(cu->getHandle()->m_kConfig.m_configRegInfo);
    const bool is8Bit = kConfMgr.get(XDNNKernelConfigInfoMgr::MODE8BIT);

    int filterDspDataCount = 0;
    int filterAxiReadCount = 0;
    computeFilterAxiDataCounts(_cOp.get(), kernSizex, kernSizey, 
      is8Bit, filterDspDataCount, filterAxiReadCount);

    /********************************************
     *********AutoProgrammed CSR Registers*******
     *******************************************/
    int physBaseIf=cu->computeDdrOffset(input_a);
    int physOffsetIf=physBaseIf+offset;

    const int _HAT = _cOp->getInt("HAT");

    if(_HAT==1)
    {
      const int _tileWidth = _cOp->getInt("tile_width");
      const int _tileHeight = _cOp->getInt("tile_height");
      const int _SRCAM_Buffer_0 = _cOp->getInt("SRCAM-Buffer_0");
      const int _SRCAM_Buffer_1 = _cOp->getInt("SRCAM-Buffer_1");
      const int _streamToDdr = (_cOp->getStr("memory_description_dest", "ddr") == "ddr")? 1 : 0;
      xdnnCnnCmd |= (_streamToDdr << 15);
      xdnnCnnCmd |= _computeSrcAddrConfig(_cOp.get());
      xdnnCnnCmd |= _computeDestAddrConfig(_cOp.get());
      int tilingControlCmd = (_HAT<<31) | (_tileHeight<<16) | (_tileWidth);
      ki->writeInt(cuIdx, slice, XDNN_TILING_CONTROL_0, tilingControlCmd);
      ki->writeInt(cuIdx, slice, XDNN_SRC_AM_BUFFER_0, _SRCAM_Buffer_0);
      ki->writeInt(cuIdx, slice, XDNN_SRC_AM_BUFFER_1, _SRCAM_Buffer_1);
    }
    else
    {
      ki->writeInt(cuIdx, XDNN_TILING_CONTROL_0, 0x0);
    }

    ki->writeInt(cuIdx, slice, XDNN_CNN_COMMAND, xdnnCnnCmd);
    ki->writeInt(cuIdx, slice, XDNN_INPUT_FULL_SECTION_NUMBER,_srcFullSectNum);
    ki->writeInt(cuIdx, slice, XDNN_INPUT_REPLICATION_SECTION_NUMBER, _srcReplSectNum);
    ki->writeInt(cuIdx, slice, XDNN_INPUT_REPLICATION_UNIT_NUMBER, _srcReplUnitNum);
    ki->writeInt(cuIdx, slice, XDNN_INPUT_REPLICATION_UNIT_WIDTH, _srcReplUnitWidth);
    ki->writeInt(cuIdx, slice, XDNN_OUTPUT_FULL_SECTION_NUMBER,_dstFullSectNum);
    ki->writeInt(cuIdx, slice, XDNN_OUTPUT_REPLICATION_SECTION_NUMBER,_dstReplSectNum);
    ki->writeInt(cuIdx, slice, XDNN_OUTPUT_REPLICATION_UNIT_NUMBER, _dstReplUnitNum);
    ki->writeInt(cuIdx, slice, XDNN_OUTPUT_REPLICATION_UNIT_WIDTH, _dstReplUnitWidth);
    ki->writeInt(cuIdx, slice, XDNN_OUTPUT_IMAGE_BASE_ADDR, _outBaseAddr);
    ki->writeInt(cuIdx, slice, XDNN_INPUT_IMAGE_WIDTH, _inImgSizex);
    ki->writeInt(cuIdx, slice, XDNN_INPUT_IMAGE_HEIGHT, _inImgSizey);
    ki->writeInt(cuIdx, slice, XDNN_OUTPUT_IMAGE_WIDTH,_outImgSizex);
    ki->writeInt(cuIdx, slice, XDNN_OUTPUT_IMAGE_HEIGHT, _outImgSizey);
    ki->writeInt(cuIdx, slice, (size_t) XDNN_DEST_CONCAT_INFO, 0);
    ki->writeInt(cuIdx, slice, XDNN_INPUT_IMAGE_CHANNEL, _inImgChannels);
    ki->writeInt(cuIdx, slice, XDNN_OUTPUT_IMAGE_CHANNEL, _outImgChannels);
    ki->writeInt(cuIdx, slice, XDNN_INPUT_IMAGE_BASE_ADDR, _inBaseAddr);
    ki->writeInt(cuIdx, slice, XDNN_CNN_KERNEL,kernControl);
    int physBase=cu->computeDdrOffset(input_a);
    int physOffset=physBase+offset;

    ki->writeInt(cuIdx, slice, XDNN_FILTER_DDR_ADDR_OFFSET,physOffset);
    ki->writeInt(cuIdx, slice, XDNN_FILTER_AXI_READ_COUNT,filterAxiReadCount);
    ki->writeInt(cuIdx, slice, XDNN_FILTER_SRC_ITERATION_COUNT, 0x1);
    ki->writeInt(cuIdx, slice, XDNN_FILTER_DSP_DATA_COUNT, filterDspDataCount);

    if (!_cOp->hasKey("override_xbar_table_ptr_0"))
    {
      int tablePtr0 =(_itnCount0<<24) | (_endPtr0<<12) | (_startPtr0);
      int tablePtr1 =(_itnCount1<<24) | (_endPtr1<<12) | (_startPtr1);
      ki->writeInt(cuIdx, slice, XDNN_CROSSBAR_TABLE_POINTER_0, tablePtr0);
      ki->writeInt(cuIdx, slice, XDNN_CROSSBAR_TABLE_POINTER_1, tablePtr1);
    }
    else
    {
      ki->writeInt(cuIdx, slice, XDNN_CROSSBAR_TABLE_POINTER_0, 
          _cOp->getInt("override_xbar_table_ptr_0"));
      ki->writeInt(cuIdx, slice, XDNN_CROSSBAR_TABLE_POINTER_1, 
          _cOp->getInt("override_xbar_table_ptr_1"));
    }

    ki->writeInt(cuIdx, slice, XDNN_SCRIPT_MEMORY_CONTROL, 0x1);
    return (EXIT_SUCCESS);
}

XDNNUpsampleDescriptor::XDNNUpsampleDescriptor(const XDNNCompilerOp &op) 
  : XDNNDescriptor(op)
{
}

int XDNNUpsampleDescriptor::execute(XComputeUnit * cu) {
    const int winSizex = 1;
    const int winStridex = 1;
    const int upsacale_x = _cOp->getInt("kernel_w");
    const int winSizey = 1;
    const int winStridey = 1;
    const int upsacale_y = _cOp->getInt("kernel_h");
    const int inImgSizex = _cOp->getInt("insize_w");
    const int inImgSizey = _cOp->getInt("insize_h");
    const int inImgChannels = _cOp->getInt("inchan");
    const int outImgSizex = _cOp->getInt("outsize_w");
    const int outImgSizey = _cOp->getInt("outsize_h");
    const int inBaseAddr = _cOp->getInt("inaddr");
    const int outBaseAddr = _cOp->getInt("outaddr");
    const int dilx = 0;
    const int dily = 0;
    const int paddingy = 0;
    const int paddingx = 0;
    const int kernControl = (dily << 28) | (dilx << 24) | (paddingy << 20)
                    | (paddingx << 16) | (winStridey << 12) | (winStridex << 8)
                    | (winSizey << 4) | winSizex;

    double p_KernelTimeMs = 0;

#ifdef DEBUG
    xdnn::XDebugJournal().log("XDNNUpsample");
#endif

    XBLASHandle &handle = *cu->getHandle();
    int xdnnCommand =  0x4; // Upsample

    const std::string upsample_mode = _cOp->getStr("method"); 
    if(upsample_mode == "NearestNeighbor")
    {
       xdnnCommand = xdnnCommand | (1 << 20);
    }
    int log_2_scale_x = log2(upsacale_x);
    int log_2_scale_y = log2(upsacale_y);
    xdnnCommand = xdnnCommand | (log_2_scale_x << 22) | (log_2_scale_y << 25);

    const int cuIdx = cu->getCuIdx();

    KernelInterface *ki = KernelInterfaceDB::get(&handle);
    const int _waitDL = _cOp->getInt("wait_download");
    const int _waitUL = _cOp->getInt("wait_upload");
    const int _waitConv2D = _cOp->getInt("wait_conv");
    const int _waitPool = _cOp->getInt("wait_pool");
    const int _waitEW = _cOp->getInt("wait_ew");
    const int _waitUpSmpl = _cOp->getInt("wait_upsmpl");
    const int _srcFullSectNum = _cOp->getInt("src_full_sect_num");
    const int _srcReplSectNum = _cOp->getInt("src_repl_sect_num");
    const int _srcReplUnitNum = _cOp->getInt("src_repl_unit_num");
    const int _srcReplUnitWidth = _cOp->getInt("src_repl_unit_width");
    const int _dstFullSectNum = _cOp->getInt("dst_full_sect_num");
    const int _dstReplSectNum = _cOp->getInt("dst_repl_sect_num");
    const int _dstReplUnitNum = _cOp->getInt("dst_repl_unit_num");
    const int _dstReplUnitWidth = _cOp->getInt("dst_repl_unit_width");
    const int _HAT = _cOp->getInt("HAT");

    xdnnCommand = xdnnCommand | (_waitUpSmpl << 9) | (_waitEW << 8)
                    | (_waitPool << 7) | (_waitConv2D << 6) | (_waitUL << 5)
                    | (_waitDL << 4) ;

    ki->writeInt(cuIdx, XDNN_INPUT_FULL_SECTION_NUMBER, _srcFullSectNum);
    ki->writeInt(cuIdx, XDNN_INPUT_REPLICATION_SECTION_NUMBER, _srcReplSectNum);

    ki->writeInt(cuIdx, XDNN_INPUT_REPLICATION_UNIT_NUMBER, _srcReplUnitNum);
    ki->writeInt(cuIdx, XDNN_INPUT_REPLICATION_UNIT_WIDTH, _srcReplUnitWidth);

    ki->writeInt(cuIdx, XDNN_OUTPUT_FULL_SECTION_NUMBER, _dstFullSectNum);
    ki->writeInt(cuIdx, XDNN_OUTPUT_REPLICATION_SECTION_NUMBER,
            _dstReplSectNum);
    ki->writeInt(cuIdx, XDNN_OUTPUT_REPLICATION_UNIT_NUMBER, _dstReplUnitNum);
    ki->writeInt(cuIdx, XDNN_OUTPUT_REPLICATION_UNIT_WIDTH, _dstReplUnitWidth);

    ki->writeInt(cuIdx, XDNN_INPUT_IMAGE_WIDTH, inImgSizex);
    ki->writeInt(cuIdx, XDNN_INPUT_IMAGE_HEIGHT, inImgSizey);
    ki->writeInt(cuIdx, XDNN_OUTPUT_IMAGE_WIDTH, outImgSizex);
    ki->writeInt(cuIdx, XDNN_DEST_CONCAT_INFO, 0);
    ki->writeInt(cuIdx, XDNN_OUTPUT_IMAGE_HEIGHT, outImgSizey);
    ki->writeInt(cuIdx, XDNN_INPUT_IMAGE_CHANNEL, inImgChannels);
    ki->writeInt(cuIdx, XDNN_OUTPUT_IMAGE_CHANNEL, inImgChannels);
    ki->writeInt(cuIdx, XDNN_INPUT_IMAGE_BASE_ADDR, inBaseAddr);
    ki->writeInt(cuIdx, XDNN_OUTPUT_IMAGE_BASE_ADDR, outBaseAddr);
    ki->writeInt(cuIdx, XDNN_CNN_KERNEL, kernControl);
    if (_HAT == 1) {
        const int _tileWidth = _cOp->getInt("tile_width");
        const int _tileHeight = _cOp->getInt("tile_height");
        const int _SRCAM_Buffer_0 = _cOp->getInt("SRCAM-Buffer_0");
        const int _SRCAM_Buffer_1 = _cOp->getInt("SRCAM-Buffer_1");
        const int _DESTAMBufferOffset = _cOp->getInt("DEST_AM-Buffer_Offset");

        int tilingCmd = (_HAT << 31) | (_tileHeight << 16) | _tileWidth;
        xdnnCommand = xdnnCommand | (1 << 17) | (1 << 19); //src 10 and es t 10, because assuming addrs passed by compiler
        ki->writeInt(cuIdx, XDNN_TILING_CONTROL_0, tilingCmd);
        ki->writeInt(cuIdx, XDNN_SRC_AM_BUFFER_0, _SRCAM_Buffer_0);
        ki->writeInt(cuIdx, XDNN_SRC_AM_BUFFER_1, _SRCAM_Buffer_1);
        ki->writeInt(cuIdx, XDNN_DEST_AM_BUFFER_OFFSET_ADDR, _DESTAMBufferOffset);
    } else {
        ki->writeInt(cuIdx, XDNN_TILING_CONTROL_0, 0x0);
    }
    ki->writeInt(cuIdx, XDNN_CNN_COMMAND, xdnnCommand);

    ki->writeInt(cuIdx, XDNN_SCRIPT_MEMORY_CONTROL, 0x1); // push

    return (EXIT_SUCCESS);
}

XDNNPoolDescriptor::XDNNPoolDescriptor(const XDNNCompilerOp &op) 
  : XDNNDescriptor(op)
{
}

int XDNNPoolDescriptor::execute(XComputeUnit * cu) {
    const int winSizex = _cOp->getInt("kernel_w");
    const int winSizey = _cOp->getInt("kernel_h");
    const int winStridey = _cOp->getInt("strides_h");
    const int winStridex = _cOp->getInt("strides_w");
    const int inImgSizex = _cOp->getInt("insize_w");
    const int inImgSizey = _cOp->getInt("insize_h");
    const int inImgChannels = _cOp->getInt("inchan");
    const int outImgSizex = _cOp->getInt("outsize_w");
    const int outImgSizey = _cOp->getInt("outsize_h");
    const int paddingx = _cOp->getInt("paddings_w");
    const int paddingy = _cOp->getInt("paddings_h");
    const int inBaseAddr = _cOp->getInt("inaddr");
    const int outBaseAddr = _cOp->getInt("outaddr");
    const int dilx = 0;
    const int dily = 0;
    const int kernControl = (dily << 28) | (dilx << 24) | (paddingy << 20)
                    | (paddingx << 16) | (winStridey << 12) | (winStridex << 8)
                    | (winSizey << 4) | winSizex;

    bool fcMode = _cOp->hasKey("fcmode") ? _cOp->getInt("fcmode") == 1 : false;
    const bool avgPool = (OpType() == XDNN_AVGPOOL);
    double p_KernelTimeMs = 0;

#ifdef DEBUG
    xdnn::XDebugJournal().log("XDNNPool");
#endif

    XBLASHandle &handle = *cu->getHandle();
    int xdnnCommand = fcMode << 5 | avgPool << 4 | 0x3; // maxpool (or avgPool)

    const int cuIdx = cu->getCuIdx();

    KernelInterface *ki = KernelInterfaceDB::get(&handle);
    const int _waitDL = _cOp->getInt("wait_download");
    const int _waitUL = _cOp->getInt("wait_upload");
    const int _waitConv2D = _cOp->getInt("wait_conv");
    const int _waitPool = _cOp->getInt("wait_pool");
    const int _waitEW = _cOp->getInt("wait_ew");
    const int _waitUpSmpl = _cOp->getInt("wait_upsmpl");
    const int _srcFullSectNum = _cOp->getInt("src_full_sect_num");
    const int _srcReplSectNum = _cOp->getInt("src_repl_sect_num");
    const int _srcReplUnitNum = _cOp->getInt("src_repl_unit_num");
    const int _srcReplUnitWidth = _cOp->getInt("src_repl_unit_width");
    const int _dstFullSectNum = _cOp->getInt("dst_full_sect_num");
    const int _dstReplSectNum = _cOp->getInt("dst_repl_sect_num");
    const int _dstReplUnitNum = _cOp->getInt("dst_repl_unit_num");
    const int _dstReplUnitWidth = _cOp->getInt("dst_repl_unit_width");
    const int _HAT = _cOp->getInt("HAT");

    xdnnCommand = (avgPool << 20) | (_waitUpSmpl << 9) | (_waitEW << 8)
                    | (_waitPool << 7) | (_waitConv2D << 6) | (_waitUL << 5)
                    | (_waitDL << 4) | 0x2;
    if (avgPool && _postShift.size() && _scale.size()) {
      // const int postScaleShift = _postShift.at(0);
      // const int scale = _scale.at(0);

      // const int dspScaleParamVal = (postScaleShift << 24) | scale;
      // ki->writeInt(cuIdx, XDNN_CONV2D_GLOBAL_DSP_SCALE_PARAM, dspScaleParamVal); 
      // ki->writeInt(cuIdx, XDNN_CONV2D_GLOBAL_DSP_BIAS_PARAM, 0x0);
      XMemPtr *aPtr = cu->getHandle()->getHost2XMem(_A);
      assert(aPtr != NULL);
      XDeviceMemPOD<short> *devMemPOD = aPtr->getDeviceMemPOD(cu->getCuIdx());
      assert(devMemPOD && devMemPOD->m_devicePtr);
      cl_mem input_a = devMemPOD->m_devicePtr;

      int _perChannel = 1;
      xdnnCommand = _perChannel << 23 | xdnnCommand;

      const std::string uniqueLayerName = getUniqueLayerName(_name);
      if (cu->getWeightOffsetMap().find(uniqueLayerName)
              == cu->getWeightOffsetMap().end()) {
          std::cout << "[XDNN] cfg params offset for layer "
                  << uniqueLayerName << " not found" << std::endl;
          assert(0);
      }

      int offset = cu->getWeightOffsetMap().at(uniqueLayerName);
      int physBase = cu->computeDdrOffset(input_a);
      int physOffset = physBase + offset;
      ki->writeInt(cuIdx, XDNN_FILTER_DDR_ADDR_OFFSET, physOffset);
      ki->writeInt(cuIdx, XDNN_CONV2D_GLOBAL_DSP_SCALE_PARAM, 0);
    }
    ki->writeInt(cuIdx, XDNN_INPUT_FULL_SECTION_NUMBER, _srcFullSectNum);
    ki->writeInt(cuIdx, XDNN_INPUT_REPLICATION_SECTION_NUMBER, _srcReplSectNum); 
    ki->writeInt(cuIdx, XDNN_INPUT_REPLICATION_UNIT_NUMBER, _srcReplUnitNum);
    ki->writeInt(cuIdx, XDNN_INPUT_REPLICATION_UNIT_WIDTH, _srcReplUnitWidth);

    ki->writeInt(cuIdx, XDNN_OUTPUT_FULL_SECTION_NUMBER, _dstFullSectNum);
    ki->writeInt(cuIdx, XDNN_OUTPUT_REPLICATION_SECTION_NUMBER,
            _dstReplSectNum);
    ki->writeInt(cuIdx, XDNN_OUTPUT_REPLICATION_UNIT_NUMBER, _dstReplUnitNum);
    ki->writeInt(cuIdx, XDNN_OUTPUT_REPLICATION_UNIT_WIDTH, _dstReplUnitWidth);

    ki->writeInt(cuIdx, XDNN_INPUT_IMAGE_WIDTH, inImgSizex);
    ki->writeInt(cuIdx, XDNN_INPUT_IMAGE_HEIGHT, inImgSizey);
    ki->writeInt(cuIdx, XDNN_OUTPUT_IMAGE_WIDTH, outImgSizex);
    ki->writeInt(cuIdx, XDNN_DEST_CONCAT_INFO, 0);
    ki->writeInt(cuIdx, XDNN_OUTPUT_IMAGE_HEIGHT, outImgSizey);
    ki->writeInt(cuIdx, XDNN_INPUT_IMAGE_CHANNEL, inImgChannels);
    ki->writeInt(cuIdx, XDNN_OUTPUT_IMAGE_CHANNEL, inImgChannels);
    ki->writeInt(cuIdx, XDNN_INPUT_IMAGE_BASE_ADDR, inBaseAddr);
    ki->writeInt(cuIdx, XDNN_OUTPUT_IMAGE_BASE_ADDR, outBaseAddr);
    ki->writeInt(cuIdx, XDNN_CNN_KERNEL, kernControl);
    if (_HAT == 1) {
        const int _tileWidth = _cOp->getInt("tile_width");
        const int _tileHeight = _cOp->getInt("tile_height");
        const int _SRCAM_Buffer_0 = _cOp->getInt("SRCAM-Buffer_0");
        const int _SRCAM_Buffer_1 = _cOp->getInt("SRCAM-Buffer_1");
        const int _DESTAMBufferOffset = _cOp->getInt("DEST_AM-Buffer_Offset");

        int tilingCmd = (_HAT << 31) | (_tileHeight << 16) | _tileWidth;
        xdnnCommand = xdnnCommand | (1 << 17) | (1 << 19); //src 10 and es t 10, because assuming addrs passed by compiler
        ki->writeInt(cuIdx, XDNN_TILING_CONTROL_0, tilingCmd);
        ki->writeInt(cuIdx, XDNN_SRC_AM_BUFFER_0, _SRCAM_Buffer_0);
        ki->writeInt(cuIdx, XDNN_SRC_AM_BUFFER_1, _SRCAM_Buffer_1);
        ki->writeInt(cuIdx, XDNN_DEST_AM_BUFFER_OFFSET_ADDR, _DESTAMBufferOffset);
    } else {
        ki->writeInt(cuIdx, XDNN_TILING_CONTROL_0, 0x0);
    }
    ki->writeInt(cuIdx, XDNN_CNN_COMMAND, xdnnCommand);

    ki->writeInt(cuIdx, XDNN_SCRIPT_MEMORY_CONTROL, 0x1); // push
    return (EXIT_SUCCESS);
}

XDNNDataDescriptor::XDNNDataDescriptor(const std::string & layerName, int dataTypeSize,
        XDNNTensorShapeType shapeType, int n, int c, int h, int w, unsigned long long hwOffset, unsigned long long hwSizeInBytes,
        bool singleStep) :
                _shapeType(shapeType), _dataTypeSize(dataTypeSize), _hwAddrOffset(hwOffset), _hwSizeInBytes(hwSizeInBytes), _singleStep( singleStep) {
    // only 1 type supported now:
    assert(shapeType == XDNN_TENSOR_NCHW);
    _n = n;
    _c = c;
    _h = h;
    _w = w;
    _layerName = layerName;
}

size_t XDNNDataDescriptor::getSize() const{
    return _n * _c * _h * _w;;
}

size_t XDNNDataDescriptor::getSizeInBytes() const {
    return getSize() * _dataTypeSize;
}

