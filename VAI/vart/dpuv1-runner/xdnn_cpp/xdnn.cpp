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
 *  @brief Xilinx XDNN library for FPGA acceleration
 *
 *  @author Aaron Ng (aaronn@xilinx.com)
 */

#include <cctype>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <iterator>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <vector>
#include <map>
#include <future>
#include "xblas.h"
#include "xdnn.h"
#include "xmlrt.h"
#include "xdnn_ops.h"
#include <boost/algorithm/clamp.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/crc.hpp>  // for boost::crc_32_type
#include "H5Cpp.h"

using namespace std;

short* XDNNMakeWeightsBiasQuantBlob(int totalSize);
void XDNNQuantizeBiasV3(const float *in, int* out, int sz, float threshIn, std::vector<float>* threshparams, int bitWidth, bool doRounding);
void XDNNQuantizeBiasV3_scale(const float *in, int *out, int sz, float threshOut, std::vector<int>* postShift, int bitWidth, bool doRounding);

static int int_ceil(int num, int den) {
  int result = ceil(float(num) / float(den));
  return result;
}

template void XDNNdbgDumpToFile(const short *data, int size, std::string fname);
template void XDNNdbgDumpToFile(const float *data, int size, std::string fname);
template void XDNNdbgDumpToFile(const double *data, int size, std::string fname);
template void XDNNdbgReadFromFile(short *data, int size, std::string fname);
template void XDNNdbgReadFromFile(float *data, int size, std::string fname);
template void XDNNdbgReadFromFile(double *data, int size, std::string fname);

template
int XDNNV3FillWeightsBiasQuantBlob(short *blob, int offset,
    std::string layerName, std::string fpgaCfgFile, const short *weight,
    unsigned int weight_sz, const short *bias,
    unsigned int bias_sz, unsigned short kern_w,
    unsigned short kern_h, unsigned int inChans, unsigned int outChans,
    int srcFullSectNum, int srcReplSectNum, int srcReplUnitNum,
    int srcReplUnitWidth, bool convHalfRateMode, std::string opType, 
    int slice, std::vector<int> &cfgWtEachItnDemarkIdx);
template
int XDNNV3FillWeightsBiasQuantBlob(short *blob, int offset,
    std::string layerName, std::string fpgaCfgFile, const int *weight,
    unsigned int weight_sz, const int *bias,
    unsigned int bias_sz, unsigned short kern_w,
    unsigned short kern_h, unsigned int inChans, unsigned int outChans,
    int srcFullSectNum, int srcReplSectNum, int srcReplUnitNum,
    int srcReplUnitWidth, bool convHalfRateMode, std::string opType,
    int slice, std::vector<int> &cfgWtEachItnDemarkIdx);
template
int XDNNV3FillWeightsBiasQuantBlob(short *blob, int offset,
    std::string layerName, std::string fpgaCfgFile, const float *weight,
    unsigned int weight_sz, const float *bias,
    unsigned int bias_sz, unsigned short kern_w,
    unsigned short kern_h, unsigned int inChans, unsigned int outChans,
    int srcFullSectNum, int srcReplSectNum, int srcReplUnitNum,
    int srcReplUnitWidth, bool convHalfRateMode, std::string opType,
    int slice, std::vector<int> &cfgWtEachItnDemarkIdx);
template
int XDNNV3FillWeightsBiasQuantBlob(short *blob, int offset,
    std::string layerName, std::string fpgaCfgFile, const double *weight,
    unsigned int weight_sz, const double *bias,
    unsigned int bias_sz, unsigned short kern_w,
    unsigned short kern_h, unsigned int inChans, unsigned int outChans,
    int srcFullSectNum, int srcReplSectNum, int srcReplUnitNum,
    int srcReplUnitWidth, bool convHalfRateMode, std::string opType,
    int slice, std::vector<int> &cfgWtEachItnDemarkIdx);

template
int XDNNFillWeightsBiasQuantBlob(short *blob, int offset, std::string layerName,
    std::string fpgaCfgFile, const short *weight, unsigned int weight_sz,
    const short *bias, unsigned int bias_sz,
    unsigned short kern_w, unsigned short kern_h, unsigned int inChans,
    unsigned int outChans, XDNNOperation op);
template
int XDNNFillWeightsBiasQuantBlob(short *blob, int offset, std::string layerName,
    std::string fpgaCfgFile, const int *weight, unsigned int weight_sz,
    const int *bias, unsigned int bias_sz,
    unsigned short kern_w, unsigned short kern_h, unsigned int inChans,
    unsigned int outChans, XDNNOperation op);
template
int XDNNFillWeightsBiasQuantBlob(short *blob, int offset, std::string layerName,
    std::string fpgaCfgFile, const float *weight, unsigned int weight_sz,
    const float *bias, unsigned int bias_sz,
    unsigned short kern_w, unsigned short kern_h, unsigned int inChans,
    unsigned int outChans, XDNNOperation op);
template
int XDNNFillWeightsBiasQuantBlob(short *blob, int offset, std::string layerName,
    std::string fpgaCfgFile, const double *weight, unsigned int weight_sz,
    const double *bias, unsigned int bias_sz,
    unsigned short kern_w, unsigned short kern_h, unsigned int inChans,
    unsigned int outChans, XDNNOperation op);


template
short* XDNNScriptExecutor<float>::XDNNLoadWeights(const std::vector<XBLASHandle*> & xblas_handles,
    const std::string &weightsPath,
    const std::string &xdnnNetFile, const std::string & quantizeCfgFile,
    unsigned int cuMask);

template
short* XDNNScriptExecutor<float>::XDNNLoadWeights(const std::vector<XBLASHandle*> & xblas_handles,
    const std::map<std::string, XDNNWeightParam<float> > & m,
    const std::string &xdnnNetFile, const std::string & quantizeCfgFile,
    unsigned int cuMask);

template
short* XDNNScriptExecutor<double>::XDNNLoadWeights(const std::vector<XBLASHandle*> & xblas_handles,
    const std::map<std::string, XDNNWeightParam<double> > & m,
    const std::string &xdnnNetFile, const std::string & quantizeCfgFile,
    unsigned int cuMask);

template int XDNNScriptExecutor<float>::fpgaXdnn(XBLASHandle &handle,
                                                 const std::unordered_map <std::string, const float*> & in_blob,
                                                 const std::unordered_map <std::string, float*> & out_blob,
                                                 std::shared_ptr<XDNNQuantBuf> & qbuf, unsigned idx,
                                                 int streamId);

template void XDNNScriptExecutor<float>::execute ( const std::unordered_map <std::string, std::vector< const float*> > & input_ptrs,
                                                   std::unordered_map< std::string, std::vector<float*> > & output, int streamId);
template void XDNNScriptExecutor<float>::execute_async ( const  std::unordered_map <std::string, std::vector<const float*> > input_ptrs,
                                                         std::unordered_map< std::string, std::vector<float*>> output, int streamId);

template XDNNScriptExecutor<float>::XDNNScriptExecutor(
    const std::vector<XBLASHandle*> & xblas_handles,
    const std::map<std::string, XDNNWeightParam<float> > &m,
    const std::string & xdnn_net_file, const std::string & quantize_CfgFile,
    unsigned int cu_mask);

const char *fpga_clerror(int errorCode) {
  switch (errorCode) {
  case CL_SUCCESS:
    return "CL_SUCCESS";
    break;
  case CL_DEVICE_NOT_FOUND:
    return "CL_DEVICE_NOT_FOUND";
    break;
  case CL_DEVICE_NOT_AVAILABLE:
    return "CL_DEVICE_NOT_AVAILABLE";
    break;
  case CL_COMPILER_NOT_AVAILABLE:
    return "CL_COMPILER_NOT_AVAILABLE";
    break;
  case CL_MEM_OBJECT_ALLOCATION_FAILURE:
    return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
    break;
  case CL_OUT_OF_RESOURCES:
    return "CL_OUT_OF_RESOURCES";
    break;
  case CL_OUT_OF_HOST_MEMORY:
    return "CL_OUT_OF_HOST_MEMORY";
    break;
  case CL_PROFILING_INFO_NOT_AVAILABLE:
    return "CL_PROFILING_INFO_NOT_AVAILABLE";
    break;
  case CL_MEM_COPY_OVERLAP:
    return "CL_MEM_COPY_OVERLAP";
    break;
  case CL_IMAGE_FORMAT_MISMATCH:
    return "CL_IMAGE_FORMAT_MISMATCH";
    break;
  case CL_IMAGE_FORMAT_NOT_SUPPORTED:
    return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
    break;
  case CL_BUILD_PROGRAM_FAILURE:
    return "CL_BUILD_PROGRAM_FAILURE";
    break;
  case CL_MAP_FAILURE:
    return "CL_MAP_FAILURE";
    break;
  case CL_MISALIGNED_SUB_BUFFER_OFFSET:
    return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
    break;
  case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
    return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
    break;
  case CL_COMPILE_PROGRAM_FAILURE:
    return "CL_COMPILE_PROGRAM_FAILURE";
    break;
  case CL_LINKER_NOT_AVAILABLE:
    return "CL_LINKER_NOT_AVAILABLE";
    break;
  case CL_LINK_PROGRAM_FAILURE:
    return "CL_LINK_PROGRAM_FAILURE";
    break;
  case CL_DEVICE_PARTITION_FAILED:
    return "CL_DEVICE_PARTITION_FAILED";
    break;
  case CL_KERNEL_ARG_INFO_NOT_AVAILABLE:
    return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";
    break;
  case CL_INVALID_VALUE:
    return "CL_INVALID_VALUE";
    break;
  case CL_INVALID_DEVICE_TYPE:
    return "CL_INVALID_DEVICE_TYPE";
    break;
  case CL_INVALID_PLATFORM:
    return "CL_INVALID_PLATFORM";
    break;
  case CL_INVALID_DEVICE:
    return "CL_INVALID_DEVICE";
    break;
  case CL_INVALID_CONTEXT:
    return "CL_INVALID_CONTEXT";
    break;
  case CL_INVALID_QUEUE_PROPERTIES:
    return "CL_INVALID_QUEUE_PROPERTIES";
    break;
  case CL_INVALID_COMMAND_QUEUE:
    return "CL_INVALID_COMMAND_QUEUE";
    break;
  case CL_INVALID_HOST_PTR:
    return "CL_INVALID_HOST_PTR";
    break;
  case CL_INVALID_MEM_OBJECT:
    return "CL_INVALID_MEM_OBJECT";
    break;
  case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
    return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
    break;
  case CL_INVALID_IMAGE_SIZE:
    return "CL_INVALID_IMAGE_SIZE";
    break;
  case CL_INVALID_SAMPLER:
    return "CL_INVALID_SAMPLER";
    break;
  case CL_INVALID_BINARY:
    return "CL_INVALID_BINARY";
    break;
  case CL_INVALID_BUILD_OPTIONS:
    return "CL_INVALID_BUILD_OPTIONS";
    break;
  case CL_INVALID_PROGRAM:
    return "CL_INVALID_PROGRAM";
    break;
  case CL_INVALID_PROGRAM_EXECUTABLE:
    return "CL_INVALID_PROGRAM_EXECUTABLE";
    break;
  case CL_INVALID_KERNEL_NAME:
    return "CL_INVALID_KERNEL_NAME";
    break;
  case CL_INVALID_KERNEL_DEFINITION:
    return "CL_INVALID_KERNEL_DEFINITION";
    break;
  case CL_INVALID_KERNEL:
    return "CL_INVALID_KERNEL";
    break;
  case CL_INVALID_ARG_INDEX:
    return "CL_INVALID_ARG_INDEX";
    break;
  case CL_INVALID_ARG_VALUE:
    return "CL_INVALID_ARG_VALUE";
    break;
  case CL_INVALID_ARG_SIZE:
    return "CL_INVALID_ARG_SIZE";
    break;
  case CL_INVALID_KERNEL_ARGS:
    return "CL_INVALID_KERNEL_ARGS";
    break;
  case CL_INVALID_WORK_DIMENSION:
    return "CL_INVALID_WORK_DIMENSION";
    break;
  case CL_INVALID_WORK_GROUP_SIZE:
    return "CL_INVALID_WORK_GROUP_SIZE";
    break;
  case CL_INVALID_WORK_ITEM_SIZE:
    return "CL_INVALID_WORK_ITEM_SIZE";
    break;
  case CL_INVALID_GLOBAL_OFFSET:
    return "CL_INVALID_GLOBAL_OFFSET";
    break;
  case CL_INVALID_EVENT_WAIT_LIST:
    return "CL_INVALID_EVENT_WAIT_LIST";
    break;
  case CL_INVALID_EVENT:
    return "CL_INVALID_EVENT";
    break;
  case CL_INVALID_OPERATION:
    return "CL_INVALID_OPERATION";
    break;
  case CL_INVALID_GL_OBJECT:
    return "CL_INVALID_GL_OBJECT";
    break;
  case CL_INVALID_BUFFER_SIZE:
    return "CL_INVALID_BUFFER_SIZE";
    break;
  case CL_INVALID_MIP_LEVEL:
    return "CL_INVALID_MIP_LEVEL";
    break;
  case CL_INVALID_GLOBAL_WORK_SIZE:
    return "CL_INVALID_GLOBAL_WORK_SIZE";
    break;
  case CL_INVALID_PROPERTY:
    return "CL_INVALID_PROPERTY";
    break;
  case CL_INVALID_IMAGE_DESCRIPTOR:
    return "CL_INVALID_IMAGE_DESCRIPTOR";
    break;
  case CL_INVALID_COMPILER_OPTIONS:
    return "CL_INVALID_COMPILER_OPTIONS";
    break;
  case CL_INVALID_LINKER_OPTIONS:
    return "CL_INVALID_LINKER_OPTIONS";
    break;
  case CL_INVALID_DEVICE_PARTITION_COUNT:
    return "CL_INVALID_DEVICE_PARTITION_COUNT";
    break;
  default:
    return "UNKNOWN ERROR";
  }
}
bool is_power_of_2(int x) {

  return x > 0 && (x << 1 == (x | (x - 1)) + 1);
}
bool ddrFormatNeeded(int rows, int dspWdth) {
  if (dspWdth != 28 && dspWdth != 56) {
    std::cout
        << "XDNN V2 DSP PADDING IMPLEMENTED FOR 28 and 56, CURRENT DSP IS "
        << dspWdth << std::endl;
  } else {
    if (rows >= dspWdth) {
      if (rows % dspWdth == 0) {
        return false;
      } else {
        return true;
      }
    } else {
      if (dspWdth % rows == 0) {
        if (!is_power_of_2(dspWdth / rows)) {
          return true;
        } else {
          return false;
        }

      } else {
        return true;
      }
    }
  }
}

/********************************
 * V3 Channel Config
 ********************************/

class V3ChannelConfig {
public:
  void setPreRightShift(int val) {
    uint64_t actualVal = uint64_t(val);
    uint64_t valToAdd = actualVal & 0x3F;
    config |= valToAdd;
  }

  void setEnabled(bool en) {
    uint64_t actualVal = uint64_t(en);
    config |= actualVal << 7;
  }

  void setScale(int val) {
    uint64_t actualVal = uint64_t(val);
    uint64_t valToAdd = (actualVal << 8);
    valToAdd = valToAdd & 0xFFFFFF;
    config |= valToAdd;
  }

  void setBias(int val) {
    uint64_t actualVal = uint64_t(val);
    uint64_t valToAdd = (actualVal << 24);
    valToAdd = valToAdd & 0xFFFFFFFFFFFFFF;
    config |= valToAdd;
  }

  void setPostRightShift(int val) {
    uint64_t actualVal = uint64_t(val);
    uint64_t valToAdd = (actualVal & 0x3F) << 56;
    config |= valToAdd;
  }

  void printout() {
    std::bitset < 64 > printx(config);
    std::cout << printx << std::endl;
  }

  void debugDump(int preShift, bool en, int scale, short bias, int postShift) {
    std::bitset < 6 > preShiftx(preShift);
    std::bitset < 1 > enx(en);
    std::bitset < 16 > scalex(scale);
    std::bitset < 32 > biasx(bias);
    std::bitset < 6 > postShiftx(postShift);

    std::cout << preShiftx << std::endl;
    std::cout << enx << std::endl;
    std::cout << scalex << std::endl;
    std::cout << biasx << std::endl;
    std::cout << postShiftx << std::endl;
  }
  uint64_t getConfig() {
    return config;
  }

  void setConfig(int preShift, bool en, int scale, int bias, int postShift) {
    setPreRightShift(preShift);
    setEnabled(en);
    setScale(scale);
    setBias(bias);
    setPostRightShift(postShift);
  }

private:
  std::uint64_t config;
};


#if 0
/***************************************
 * Binary Format Weights Read/Write
 ***************************************/

class WeightsFileReaderWriter {
public:
  char _layerName[100];
  int _width;
  int _height;
  int _inChans;
  int _outChans;

  WeightsFileReaderWriter(std::string layerName, int width, int height,
      int inChans, int outChans) :
      _width(width), _height(height), _inChans(inChans), _outChans(outChans) {
    strcpy(_layerName, layerName.c_str());
  }

  int getSizeOfHeader() {
    return sizeof(_layerName) + sizeof(_width) + sizeof(_height)
        + sizeof(_inChans) + sizeof(_outChans) + sizeof(int); // extra space to store numVals
  }

  void writeToFile(std::string fname, const std::vector<float>& vals) {
    const int numVals = vals.size();
    const int totalSize = getSizeOfHeader() + (sizeof(float) * vals.size());

    std::vector<char> buffer(totalSize);
    char *ptr = &(buffer[0]);

    strcpy(ptr, _layerName);
    ptr += sizeof(_layerName);
    memcpy(ptr, &_width, sizeof(_width));
    ptr += sizeof(_width);
    memcpy(ptr, &_height, sizeof(_height));
    ptr += sizeof(_height);
    memcpy(ptr, &_inChans, sizeof(_inChans));
    ptr += sizeof(_inChans);
    memcpy(ptr, &_outChans, sizeof(_outChans));
    ptr += sizeof(_outChans);
    memcpy(ptr, &numVals, sizeof(numVals));
    ptr += sizeof(numVals);
    memcpy(ptr, &(vals[0]), sizeof(float) * vals.size());

    std::ofstream ofile(fname.c_str(), std::ios::binary);
    ofile.write(&(buffer[0]), totalSize);
    ofile.close();
  }

  void readFromFile(std::string fname, std::vector<float>& vals) {
    int sizeToRead = getSizeOfHeader();
    std::vector<char> buffer(sizeToRead);
    char *ptr = &(buffer[0]);

    std::ifstream ifile(fname.c_str(), std::ios::binary);
    ifile.read(ptr, sizeToRead);

    int numVals = 0;
    strcpy(_layerName, ptr);
    ptr += sizeof(_layerName);
    memcpy(&_width, ptr, sizeof(_width));
    ptr += sizeof(_width);
    memcpy(&_height, ptr, sizeof(_height));
    ptr += sizeof(_height);
    memcpy(&_inChans, ptr, sizeof(_inChans));
    ptr += sizeof(_inChans);
    memcpy(&_outChans, ptr, sizeof(_outChans));
    ptr += sizeof(_outChans);
    memcpy(&numVals, ptr, sizeof(numVals));
    ptr += sizeof(numVals);

    sizeToRead += sizeof(float) * numVals;
    buffer.resize(sizeToRead);
    ptr = &(buffer[0]);

    ifile.seekg(0, std::ios::beg);
    ifile.read(ptr, sizeToRead);
    ifile.close();

    ptr += getSizeOfHeader();
    vals.resize(numVals);
    memcpy(&(vals[0]), ptr, sizeof(float) * numVals);
  }
};

#endif

/****************************************
 * Quant cfg DB with cache
 ****************************************/
XDNNLayerQuantParam* XDNNLayerQuantParamDB::getLayerQuantParam(
    const std::string &cfgFile, const std::string &layerName) {
  static std::map<std::string, std::vector<XDNNLayerQuantParam> > xlnxParams;
  static std::map<std::string, XDNNLayerQuantParam*> qpMap;

  if(XDNN_VERBOSE) std::cout << LOCATION << "Quantize Params Queried : " << cfgFile << " " << layerName << std::endl;

  if (cfgFile.empty())
    return nullptr;
  if (layerName.empty())
    return nullptr;

  if (xlnxParams.find(cfgFile) == xlnxParams.end()) {
    if(XDNN_VERBOSE) std::cout << LOCATION << "Quantize Params Creating : " << cfgFile << " " << layerName << std::endl;
    // load new quant cfg file
    xlnxParams[cfgFile] = XDNNloadLayerQuantizationParams(cfgFile);
    std::vector<XDNNLayerQuantParam> &params = xlnxParams[cfgFile];
    assert(!params.empty());

    for (int i = 0; i < params.size(); i++)
      qpMap[cfgFile + ":" + params[i].name] = &(params[i]);
  }

  auto it = qpMap.find(cfgFile + ":" + layerName);
  if (it == qpMap.end())
    return NULL;

  if(XDNN_VERBOSE) std::cout << LOCATION << "Quantize Params Returning : " << cfgFile << " " << layerName << std::endl;
  return it->second;
}

/****************************************
 * XDNN 
 ****************************************/

void XDNNdbgDumpToFileCfgParamsRTLFormat(int *preShift, int *scale,
    int *postShift, short *fpgaBias, int size, string fname) {
  ofstream file;
  file.open(fname.c_str());
  file
      << "Per Output Channel, PreShift(a.k.a. rsh, integer), Scale (integer), PostShift(a.k.a. rsh2, integer), Bias (short, 16bit) values"
      << "\n";
  for (int i = 0; i < size; i++) {
    file << preShift[i] << "\n";
    file << scale[i] << "\n";
    file << postShift[i] << "\n";
    file << fpgaBias[i] << "\n";
  }
  file.close();
}
void XDNNdbgDumpChar(char *data, int size, string fname) {
  ofstream file;
  file.open(fname.c_str());
  file
      << "Weights, already quantized, in char(8bits) values. Format: OcIcHW where Oc being OutputChans, Ic being InputChans, H being Kernel EHeight and W being kernel Width"
      << "\n";
  for (int i = 0; i < size; i++) {
    std::bitset < 8 > datax(data[i]);
    file << hex << int(data[i]) << "\n";
  }
  file.close();
}
void XDNNdbgDumpToFileRTLFormat(const short *data, int size, string fname) {
  ofstream file;
  file.open(fname.c_str());
  file
      << "Weights, already quantized, in short(16bits) values. Format: OcIcHW where Oc being OutputChans, Ic being InputChans, H being Kernel EHeight and W being kernel Width"
      << "\n";
  for (int i = 0; i < size; i++) {
    file << data[i] << "\n";
  }
  file.close();
}
template<typename T>
void XDNNdbgDumpToFile(const T *data, int size, string fname) {
  ofstream file;
  file.open(fname.c_str());
  for (int i = 0; i < size; i++) {
    //if (i < 20)
    // cout << "ANDBG  " << fname << "_" << i << " " << data[i] << endl;
    file << i << " " << data[i] << "\n";
  }
  file.close();
}
template<typename T>
void XDNNdbgReadFromFile(T *data, int size, string fname) {
  cout << "ANDBG readFromFile " << fname << " " << size << endl;
  ifstream file(fname.c_str());

  if (!file.is_open())
    return;

  int i = 0;
  while (!file.eof() && i < size)
    file >> data[i++];

  file.close();
}

template<typename Dtype>
int XDNNScriptExecutor<Dtype>::fpgaXdnn(XBLASHandle &handle,
		 	 	 	 	 	 	 	 	const std::unordered_map <std::string, const Dtype* > & in_blob,
		 	 	 	 	 	 	 	 	const std::unordered_map <std::string, Dtype* > & out_blob,
										std::shared_ptr<XDNNQuantBuf> & qbuf, unsigned batch_id,
                                        int streamId)
{
  XDNNKernelConfigInfoMgr kConfMgr(handle.m_kConfig.m_configRegInfo);
  const int maxInstructionNum = kConfMgr.get(XDNNKernelConfigInfoMgr::MAX_INSTR_NUM);
  if (_descDB->num_desc() > maxInstructionNum)
  {
    std::cerr << "ERROR: number of Compiler instructions (" << _descDB->num_desc() << ") exceeds FPGA command queue size (" << maxInstructionNum << ")" << std::endl;
    exit(1);
  }

  if (XDNN_VERBOSE)
  {
    printf("[XDNN] fpgaXdnn run instructions dfl:[%d, %d] mb:[%d, %d]\n", 
      _dflStartIdx, _dflStopIdx, _startIdx, _stopIdx);
  }

  xblasEnqueueJob(handle, in_blob, out_blob, qbuf, batch_id, streamId, &_quantparam,
    _startQp, _lastQp, _descDB.get(), _startIdx, _dflStartIdx, _stopIdx, _dflStopIdx);

  return 0;
}

static void transpose(short *inmat, short *outmat, int inrows, int incols,
    int outrows, int outcols) {
  assert(inrows == outcols);
  assert(incols == outrows);
  for (int r = 0; r < inrows; ++r)
    for (int c = 0; c < incols; ++c)
      outmat[c * outcols + r] = inmat[r * incols + c];
}

// align mallocs for Xilinx runtime performance
static void* mallocAligned(size_t size, size_t alignment = 0) {
  if (alignment == 0)
    alignment = getpagesize();

  void* buffer = NULL;
  if (posix_memalign(&buffer, alignment, size))
    throw std::bad_alloc();

  return buffer;
}

void XDNNGetMatToDDRSize(unsigned short kern_w, unsigned short kern_h,
    unsigned int inChans, unsigned int outChans, unsigned int &outRows,
    unsigned int &outCols) {
  const int chanPerBlock = 32;

  unsigned int tmpInChans = inChans;
  unsigned int tmpOutChans = outChans;

  // inchannels has to be even
  if (tmpInChans % 2 != 0)
    ++tmpInChans;

  // outChans has to multiple of chanPerBlock
  if (tmpOutChans % chanPerBlock != 0) {
    tmpOutChans = ((outChans / chanPerBlock) + 1) * chanPerBlock;
  }

  unsigned int ddrSize = (tmpOutChans * kern_w * kern_h * tmpInChans);
  assert(ddrSize % 64 == 0);
  outRows = 64;
  outCols = ddrSize / outRows;
}

int XDNNComputeBiasQuantSize(int outChans) {
  const int biasSize = ceil((float) outChans / 32.) * 32;
  return biasSize;
}

// temporary -- just for getting data to test pyXDNN
int XDNNComputeWeightsBiasQuantSize(int kernWidth, int kernHeight, int inChans,
    int outChans, bool doQuant, int op) {
  if (XLNX_DUMP_XDNN_STANDALONE_DATA) {
    static int dumpIter = 0;
    ofstream file;
    file.open("xdnn/wbq_size_" + std::to_string(dumpIter++));
    file << kernWidth << " " << inChans << " " << outChans;
    file.close();
  }

  //printf("ANDBG XDNNComputeWeightsBiasQuantSize %d %d %d %d %d\n",
  //  kernWidth, kernHeight, inChans, outChans, int(doQuant));
  int totalDdrSize = 0;

  if(op == XDNN_ELTWISE_SCALE) {
    const int wtSize = ceil((float) outChans / 32.) * 32;
    totalDdrSize += wtSize;
    const int biasSize = ceil((float) outChans / 32.) * 32;
    totalDdrSize += biasSize;
  } else if (op == XDNN_ELTWISE) {
    const int biasSize = ceil((float) outChans / 32.) * 32;
    totalDdrSize += biasSize;
  } else {
    unsigned int ddrRows = 0, ddrCols = 0;
    XDNNGetMatToDDRSize(kernWidth, kernHeight, inChans, outChans, ddrRows, ddrCols);
    totalDdrSize += ddrRows * ddrCols;
    const int biasSize = ceil((float) outChans / 32.) * 32;
    totalDdrSize += biasSize;
  }

  //std::cout << "ANDBG compute size " << weightLayers[i]->layer_param().name() << " " << ddrRows*ddrCols << std::endl;

  const int biasSize = ceil((float) outChans / 32.) * 32;
  if (doQuant)
    totalDdrSize += biasSize * 3; // for pre-shift, scale, post-shift

  return totalDdrSize;
}

static int XDNNV3ComputeWeightsBiasQuantSize(int kernWidth, int kernHeight,
    int outChans, int srcFullSectNum, int srcReplSectNum,
    int srcReplSectUnitNum, bool is8bit, bool convHalfRateMode) {
  /****************************
   returns size required in ints
   *****************************/
  bool precision8Bit = is8bit;

  int C_AXI_DATA_WIDTH = 512; //Currently this is 512 for VCU1525, 128/256 may also be possible depending on the architecture.
  int C_ARRAY_SIMUL_OPS = (precision8Bit) ? 2 : 1;
  int C_SA_CFG_INDEXES = 8; //8-bytes of configuration for each output channel channel
  int C_ARRAY_COL_NUM_ARR[2] = { 8, 16 }; //first PE being 21rowsx8cols, dedicated to process first layer with 3 chans, second PE being 96rowsx16cols, processes the rest of the network
  int C_ARRAY_ROW_NUM_ARR[2] = { 21, 96 };
  int slice = 1;
  int C_AXI_CFG_WORDS =
      int_ceil(
          (C_ARRAY_COL_NUM_ARR[slice] * 2 * C_ARRAY_SIMUL_OPS * C_SA_CFG_INDEXES
              * 8), C_AXI_DATA_WIDTH);
  int C_ARRAY_KMEM_WIDTH = 32;
  int imgi_act_repl_sects = srcReplSectNum;
  int imgi_act_full_sects = srcFullSectNum;
  int imgi_act_repl_units = srcReplSectUnitNum;
  int krnl_area = kernWidth * kernHeight;
  int imgo_depth = outChans;

  int max_conv_krnl_data_per_dsp =
      (imgi_act_repl_sects == 0) ?
          (imgi_act_full_sects * krnl_area) :
          (imgi_act_full_sects * krnl_area
              + int_ceil((krnl_area * imgi_act_repl_sects), imgi_act_repl_units));

  int conv_halfrate = int(convHalfRateMode);

  int conv_krnl_data_per_dsp =
      (conv_halfrate == 1) ?
          (max_conv_krnl_data_per_dsp+1)/2 :
          max_conv_krnl_data_per_dsp;

  int max_conv_och_per_column = 2 * C_ARRAY_SIMUL_OPS;
  int max_conv_och_per_iteration = C_ARRAY_COL_NUM_ARR[slice]
      * max_conv_och_per_column;
  int conv_och_per_iteration =
      (conv_halfrate == 0) ?
          max_conv_och_per_iteration : (max_conv_och_per_iteration / 2); //ceil or floor?

  int num_axi_words_per_array_rows = int_ceil(
      (C_ARRAY_ROW_NUM_ARR[slice] * C_ARRAY_KMEM_WIDTH), C_AXI_DATA_WIDTH);

  int conv_axi_maccfg_per_iter = int_ceil(
      (max_conv_och_per_iteration * C_SA_CFG_INDEXES * 8), C_AXI_DATA_WIDTH);

  int conv_axi_filter_per_iter = (((max_conv_och_per_iteration
      * num_axi_words_per_array_rows) / max_conv_och_per_column)
      * conv_krnl_data_per_dsp);

  int och_iterations = int_ceil(imgo_depth, conv_och_per_iteration);

  int conv_axi_words_per_iter = conv_axi_maccfg_per_iter
      + conv_axi_filter_per_iter;
  int fmem_size_per_iteration = ((conv_axi_words_per_iter) * C_AXI_DATA_WIDTH)
      / C_ARRAY_KMEM_WIDTH;
  int axi_fmem_size = (och_iterations * fmem_size_per_iteration);
  if(XDNN_VERBOSE) std::cout << LOCATION << "XDNNV3ComputeWeightsBias total size in ints : " << kernWidth << " " << outChans << " " << axi_fmem_size << std::endl;
  return axi_fmem_size;
}

std::vector<int> XDNNV3ComputeWeightsBiasQuantByteOffsets(int &baseOffset,
    int kernWidth, int kernHeight, int inChans, int outChans, bool doQuant,
    int srcFullSectNum, int srcReplSectNum, int srcReplUnitNum, bool is8bit,
    bool convHalfRateMode) {
  // byte offets for weight, bias, pre-shift, scale, post-shift
  std::vector<int> byteOffsets;

  // start of weights
  byteOffsets.push_back(baseOffset);

  int totalSizeInts = XDNNV3ComputeWeightsBiasQuantSize(kernWidth, kernHeight,
      outChans, srcFullSectNum, srcReplSectNum, srcReplUnitNum, is8bit, 
      convHalfRateMode);
  // update base offset
  baseOffset += totalSizeInts * 2 * sizeof(short);

  for (int i=0; i < 5; i++)
    byteOffsets.push_back(0); // unused but code expects values

  return byteOffsets;
}

short *XDNNMakeWeightsBiasQuantBlob(int totalDdrSize) {
  const int totalSize = totalDdrSize * sizeof(short);
#if DEBUG
  if (XDNN_VERBOSE) std::cout << LOCATION << "XDNNMakeWeightsBiasQuantBlob total Size : " << totalSize << std::endl;
#endif
  return (short*) mallocAligned(totalSize);
}

std::vector<int> XDNNComputeWeightsBiasQuantByteOffsets(int &baseOffset,
    int kernWidth, int kernHeight, int inChans, int outChans, bool doQuant, XDNNOperation op_type) {
  // byte offets for weight, bias, pre-shift, scale, post-shift
  std::vector<int> byteOffsets;

  // start of weights
  byteOffsets.push_back(baseOffset);

  int totalDdrSize = 0;

  if(op_type == XDNN_CONV) {
    unsigned int ddrRows = 0, ddrCols = 0;
    XDNNGetMatToDDRSize(kernWidth, kernHeight, inChans, outChans, ddrRows, ddrCols);
    totalDdrSize += ddrRows * ddrCols;
  }
  else if(op_type == XDNN_SCALE) {
    const int wtSize = ceil((float) outChans / 32.) * 32;
    totalDdrSize += wtSize;
  }

  // start of bias
  byteOffsets.push_back(baseOffset + (totalDdrSize * sizeof(short)));

  const int biasSize = ceil((float) outChans / 32.) * 32;
    totalDdrSize += biasSize;

  if (doQuant)
    for (int i = 0; i < 3; i++) {
      // start of pre-shift / scale / post-shift
      byteOffsets.push_back(baseOffset + (totalDdrSize * sizeof(short)));
      totalDdrSize += biasSize;
    }

  // update base offset
  baseOffset += totalDdrSize * sizeof(short);

  return byteOffsets;
}

void XDNNV3LoadCfgData(uint64_t* conv_och_cfg, unsigned int outChans, int *&axi_fmem ) {

    uint64_t *out = (uint64_t*) axi_fmem;
    for(int i=0; i<outChans; i++)
    {
      out[i] = conv_och_cfg[i];
    }
    
}

template<class SrcT>
void XDNNV3MatToDDR(SrcT *matrix_filter, uint64_t* conv_och_cfg,
    unsigned short kern_w, unsigned short kern_h, unsigned int inChans,
    unsigned int outChans, int *&axi_fmem, bool is8Bit, int srcFullSectNum,
    int srcReplSectNum, int srcReplUnitNum, int srcReplUnitWidth,
    bool convHalfRateMode, int slice, std::vector<int> &cfgWtEachItnDemarkIdx) {
  bool precision8Bit = is8Bit;

  const int C_CLK2X_CLK_RATIO = 2; 

  int C_AXI_DATA_WIDTH = 512; //Currently this is 512 for VCU1525, 128/256 may also be possible depending on the architecture.
  int C_ARRAY_SIMUL_OPS = (precision8Bit) ? 2 : 1;

  int C_SA_CFG_INDEXES = 8; //8-bytes of configuration for each output channel channel
  int C_ARRAY_COL_NUM_ARR[2] = { 8, 16 }; //first PE being 21rowsx8cols, dedicated to process first layer with 3 chans, second PE being 96rowsx16cols, processes the rest of the network
  int C_ARRAY_ROW_NUM_ARR[2] = { 21, 96 };

  int C_AXI_CFG_WORDS =
      int_ceil(
          (C_ARRAY_COL_NUM_ARR[slice] * 2 * C_ARRAY_SIMUL_OPS * C_SA_CFG_INDEXES
              * 8), C_AXI_DATA_WIDTH);

  int C_ARRAY_KMEM_WIDTH = 32;

  int imgi_act_repl_sects = srcReplSectNum;
  int imgi_act_full_sects = srcFullSectNum;
  int imgi_act_full_unitw = 96;
  int imgi_act_repl_units = srcReplUnitNum;
  int imgi_act_repl_unitw = srcReplUnitWidth;
  int krnl_area = int(kern_w) * int(kern_h);
  int imgo_depth = outChans;
  int imgi_depth = inChans;

  int max_conv_krnl_data_per_dsp =
      (imgi_act_repl_sects == 0) ?
          (imgi_act_full_sects * krnl_area) :
          (imgi_act_full_sects * krnl_area
              + int_ceil((krnl_area * imgi_act_repl_sects), imgi_act_repl_units));

  int conv_halfrate = int(convHalfRateMode);

  int conv_krnl_data_per_dsp = 
      (conv_halfrate == 1) ?
          (max_conv_krnl_data_per_dsp+1)/2 : 
          max_conv_krnl_data_per_dsp;

  int max_conv_och_per_column = 2 * C_ARRAY_SIMUL_OPS;

  int max_conv_och_per_iteration = C_ARRAY_COL_NUM_ARR[slice]
      * max_conv_och_per_column;

  int conv_och_per_iteration =
      (conv_halfrate == 0) ?
          max_conv_och_per_iteration : (max_conv_och_per_iteration / 2); //ceil or floor?

  int num_axi_words_per_array_rows = int_ceil(
      (C_ARRAY_ROW_NUM_ARR[slice] * C_ARRAY_KMEM_WIDTH), C_AXI_DATA_WIDTH);

  int conv_axi_maccfg_per_iter = int_ceil(
      (max_conv_och_per_iteration * C_SA_CFG_INDEXES * 8), C_AXI_DATA_WIDTH);

  int conv_axi_filter_per_iter = (((max_conv_och_per_iteration
      * num_axi_words_per_array_rows) / max_conv_och_per_column)
      * conv_krnl_data_per_dsp);
  int och_iterations = int_ceil(imgo_depth, conv_och_per_iteration);

  int conv_axi_words_per_iter = conv_axi_maccfg_per_iter
      + conv_axi_filter_per_iter;

  int fmem_size_per_iteration = ((conv_axi_words_per_iter) * C_AXI_DATA_WIDTH)
      / C_ARRAY_KMEM_WIDTH;

  int axi_fmem_size = (och_iterations * fmem_size_per_iteration);

  int axi_idx = 0;
  int axi_sidx = 0;
  uint64_t ocfg_word = 0;

  int cur_kd = 0;
  int cur_ka = 0;
  int cur_kr = 0;
  int cur_kc = 0;
  int cur_kn = 0;
  int kmem_word = 0;
  int flag = 0;
  int flag1 = 0;
  int flag2 = 0;

  int och_offset = 0;
  int kidx_offset = 0;
  int wgt_idx = 0;
  int fs_wgts = 0;
  int fs_dpth = 0;
  int rs_wgt_idx = 0;
  int pad = 0;

  int cfgInsertIdx = 0;

  for (int kn_offset = 0; kn_offset < imgo_depth;
      kn_offset = kn_offset + conv_och_per_iteration) {
    axi_sidx = 0;

    if(axi_idx==0)
    {
      assert(cfgWtEachItnDemarkIdx.size()>0);
      cfgInsertIdx = cfgWtEachItnDemarkIdx[cfgWtEachItnDemarkIdx.size()-1]/2;
      cfgWtEachItnDemarkIdx.pop_back();
    }
      cfgWtEachItnDemarkIdx.push_back(axi_idx+cfgInsertIdx); //start of cfg for every iteration
    for (int kn = 0; kn < max_conv_och_per_iteration; kn++) {
      if (kn + kn_offset >= imgo_depth) {
        ocfg_word = 0;
      } else if (kn >= conv_och_per_iteration) {
        ocfg_word = 0;
      } else {
        ocfg_word = conv_och_cfg[kn + kn_offset];
      }

      for (int ki = 0; ki < C_SA_CFG_INDEXES; ki++) {
        if (((axi_sidx * 8) % (C_ARRAY_KMEM_WIDTH)) == 0) {
          axi_fmem[axi_idx] = (ocfg_word >> (ki * 8)) & 0xFF; //takes lower 8 bits from 8bytes chnl cfg

        } else {
          axi_fmem[axi_idx] = axi_fmem[axi_idx]
              | (((ocfg_word >> (ki * 8)) & 0xFF)
                  << ((axi_sidx * 8) % (C_ARRAY_KMEM_WIDTH)));
        }
        axi_sidx++;
        if (((axi_sidx * 8) % (C_ARRAY_KMEM_WIDTH)) == 0) {
          axi_idx++;
        }
      }
    }
    while (axi_idx % (C_AXI_DATA_WIDTH / C_ARRAY_KMEM_WIDTH) != 0) {
      axi_fmem[axi_idx] = 0;
      axi_idx++;
    }
    cfgWtEachItnDemarkIdx.push_back(axi_idx+cfgInsertIdx); //start of filter for every iteration
    // Half_rate formatting
    if (conv_halfrate == 1) {
      for (int kn = 0; kn < C_ARRAY_COL_NUM_ARR[slice]; kn++) {
        for (int kidx = 0; kidx < conv_krnl_data_per_dsp; kidx++) {
          for (int ridx = 0; ridx < C_ARRAY_ROW_NUM_ARR[slice]; ridx++) {
            och_offset = 0;
            kidx_offset = 0;
            kmem_word = 0;
            for (int c = 0; c < C_CLK2X_CLK_RATIO; c++) {
              for (int s = 0;s < C_ARRAY_SIMUL_OPS; s++) {
                pad = 0;
                cur_kn = kn_offset + kn + och_offset;
                wgt_idx = 2 * kidx + kidx_offset;
                fs_wgts = imgi_act_full_sects * krnl_area;
                if (wgt_idx < fs_wgts) {
                  cur_kd = ((wgt_idx / krnl_area) * C_ARRAY_ROW_NUM_ARR[slice]) + (ridx % C_ARRAY_ROW_NUM_ARR[slice]);
                  cur_kr = (wgt_idx % krnl_area) / int(kern_w);
                  cur_kc = (wgt_idx % krnl_area) - (cur_kr * int(kern_w));
                  cur_ka = cur_kr * int(kern_w) + cur_kc;
                }
                else if (wgt_idx < max_conv_krnl_data_per_dsp) {
                  fs_dpth = imgi_act_full_sects * C_ARRAY_ROW_NUM_ARR[slice];
                  rs_wgt_idx = ((wgt_idx - fs_wgts) * imgi_act_repl_units) + (ridx / imgi_act_repl_unitw);
                  cur_kd = fs_dpth + ((rs_wgt_idx / krnl_area) * imgi_act_repl_unitw) + (ridx % imgi_act_repl_unitw);
                  cur_kr = (rs_wgt_idx % krnl_area) / int(kern_w);
                  cur_kc = (rs_wgt_idx % krnl_area) - (cur_kr * int(kern_w));
                  cur_ka = cur_kr * int(kern_w) + cur_kc;
                }
                else {
                  pad = 1;
                }
                // check other cases where weight should be padded
                if ((ridx >= C_ARRAY_ROW_NUM_ARR[slice]) ||
                    (cur_kn > imgo_depth)                ||
                    (cur_kd >= imgi_depth)               ||
                    (cur_kr >= int(kern_h))              ||
                    (cur_kc >= int(kern_w))
                    ) {
                  pad = 1;
                }
                if (pad == 0) {
                  if (precision8Bit) {
                    kmem_word = kmem_word
                      | ((matrix_filter[cur_kn * imgi_depth * krnl_area
                                        + cur_kd * krnl_area + cur_ka] & 0xFF)
                         << (8 * ((c * C_ARRAY_SIMUL_OPS) + s)));
                  } else {
                    kmem_word = kmem_word
                      | ((matrix_filter[cur_kn * imgi_depth * krnl_area
                                        + cur_kd * krnl_area + cur_ka] & 0xFFFF)
                         << (16 * ((c * C_ARRAY_SIMUL_OPS) + s)));
                  }
                }
                och_offset = och_offset + C_ARRAY_COL_NUM_ARR[slice];
              }
              if (och_offset >= (((C_CLK2X_CLK_RATIO * C_ARRAY_SIMUL_OPS) / 2) * C_ARRAY_COL_NUM_ARR[slice])) {
                och_offset = 0;
                kidx_offset++;
              }
            }
            axi_fmem[axi_idx] = kmem_word;
            axi_idx++;
          }
        }
      }
    }
    else {
      for (int kn = 0; kn < C_ARRAY_COL_NUM_ARR[slice]; kn++) {
        for (int kd_full = 0; kd_full < imgi_act_full_sects; kd_full++) {
          for (int ka = 0; ka < krnl_area; ka = ka + 1) {
            for (int kd = 0; kd < imgi_act_full_unitw; kd = kd + 1) {
              cur_kd = (kd_full * imgi_act_full_unitw) + kd;

              kmem_word = 0;
              if ((kd_full * imgi_act_full_unitw) + kd < imgi_depth) {

                for (int c = 0; c < 2; c++) {
                  for (int s = 0; s < C_ARRAY_SIMUL_OPS; s++) {
                    cur_kn = kn_offset + kn
                      + ((c * C_ARRAY_SIMUL_OPS + s)
                         * C_ARRAY_COL_NUM_ARR[slice]);
                    cur_ka = ka;
                    if (precision8Bit) {
                      kmem_word = kmem_word
                        | ((matrix_filter[cur_kn * imgi_depth * krnl_area
                                          + cur_kd * krnl_area + cur_ka] & 0xFF)
                           << (8 * ((c * C_ARRAY_SIMUL_OPS) + s)));
                    } else {
                      kmem_word = kmem_word
                        | ((matrix_filter[cur_kn * imgi_depth * krnl_area
                                          + cur_kd * krnl_area + cur_ka] & 0xFFFF)
                           << (16 * ((c * C_ARRAY_SIMUL_OPS) + s)));
                    }
                  }
                }
              }
              axi_fmem[axi_idx] = kmem_word;
              axi_idx++;
            }
            while (axi_idx % (C_AXI_DATA_WIDTH / C_ARRAY_KMEM_WIDTH) != 0) {
              axi_fmem[axi_idx] = 0;
              axi_idx++;
            }
          }
        }

        //			/**********************
        //			 *******Repl. Sects****
        //			 *********************/
        //
        int kd_repl = 0;
        int kd_iter = 0;
        if (imgi_act_repl_sects != 0) {
          int kia_rnd = int_ceil((imgi_act_repl_sects * krnl_area),
                                 imgi_act_repl_units) * imgi_act_repl_units;

          for (int ki = 0; ki < kia_rnd; ki = ki + 1) {
            if (ki >= ((kd_iter + 1) * krnl_area))
              kd_iter++;
            for (int kd = 0; kd < imgi_act_repl_unitw; kd = kd + 1) {
              cur_kd = (imgi_act_full_sects * imgi_act_full_unitw)
                + (kd_iter * imgi_act_repl_unitw) + kd;

              kmem_word = 0;
              for (int c = 0; c < 2; c++) {
                for (int s = 0; s < C_ARRAY_SIMUL_OPS; s++) {
                  cur_kn =
                    kn_offset + kn
                    + ((c * C_ARRAY_SIMUL_OPS + s)
                       * C_ARRAY_COL_NUM_ARR[slice]);
                  cur_ka = ki % krnl_area;
                  if (cur_kd >= inChans) {
                    kmem_word = kmem_word;
                  } else if (ki >= (imgi_act_repl_sects * krnl_area)) {
                    kmem_word = kmem_word;
                  } else {
                    if (precision8Bit) {
                      kmem_word = kmem_word
                        | ((matrix_filter[cur_kn * imgi_depth * krnl_area
                                          + cur_kd * krnl_area + cur_ka] & 0xFF)
                           << (8 * ((c * C_ARRAY_SIMUL_OPS) + s)));
                    } else {
                      kmem_word = kmem_word
                        | ((matrix_filter[cur_kn * imgi_depth * krnl_area
                                          + cur_kd * krnl_area + cur_ka] & 0xFFFF)
                           << (16 * ((c * C_ARRAY_SIMUL_OPS) + s)));
                    }

                  }

                }

              }
              axi_fmem[axi_idx] = kmem_word;
              axi_idx++;
              if (((((ki % imgi_act_repl_units) + 1) * (kd + 1))
                   % C_ARRAY_ROW_NUM_ARR[slice]) == 0) {
                while (axi_idx % (C_AXI_DATA_WIDTH / C_ARRAY_KMEM_WIDTH) != 0) {
                  axi_fmem[axi_idx] = 0;
                  axi_idx++;
                }
              }
            }
            kd_repl++;
            if (kd_repl == imgi_act_repl_units)
              kd_repl = 0;
          }
        }
        /******************
         *****Repl End*****
         *****************/
      }
    }
  }
}

void XDNNMatToDDR(short *src, unsigned short kern_w, unsigned short kern_h,
    unsigned int inChans, unsigned int outChans, short *&out,
    unsigned int &outRows, unsigned int &outCols) {
  const int chanPerBlock = 32;

  unsigned int tmpInChans = inChans;
  unsigned int tmpOutChans = outChans;

  // inchannels has to be even
  if (tmpInChans % 2 != 0)
    ++tmpInChans;

  // outChans has to multiple of chanPerBlock
  if (tmpOutChans % chanPerBlock != 0) {
    tmpOutChans = ((tmpOutChans / chanPerBlock) + 1) * chanPerBlock;
  }

  // The row/col matrix and converts to DDR format matrix...

  int ddrBlockCols = chanPerBlock;
  int ddrBlockRows = kern_w * kern_h * tmpInChans;

  // Temporary ddr sized matrix to help copy...
  std::vector<short> f(tmpOutChans * kern_w * kern_h * tmpInChans, 0);
  int mRows = outChans;
  int mCols = kern_w * kern_h * inChans;

  int fRows = tmpOutChans;
  int fCols = kern_w * kern_h * tmpInChans;

  // Copy smaller src matrix into larger f matrix
  for (int r = 0; r < mRows; ++r) {
    for (int c = 0; c < mCols; ++c) {
      f[r * fCols + c] = src[r * mCols + c];
    }
  }

  // now interleave channels on f...
  assert(tmpInChans % 2 == 0);
  for (int r = 0; r < fRows; ++r) {
    std::vector<short> row(fCols);
    int blocks = tmpInChans / 2;
    for (int b = 0; b < blocks; ++b) {
      for (int c = 0; c < kern_w * kern_h; ++c) {
        row[b * 2 * kern_w * kern_h + 2 * c + 0] = f[r * fCols
            + (b * 2 * kern_w * kern_h) + c];
        row[b * 2 * kern_w * kern_h + 2 * c + 1] = f[r * fCols
            + (b * 2 * kern_w * kern_h) + kern_w * kern_h + c];
      }
    }
    for (int c = 0; c < fCols; ++c) {
      f[r * fCols + c] = row[c];
    }
  }

  {
    if (out == NULL)
      out = (short*) mallocAligned(sizeof(short) * f.size());

    int blocks = tmpOutChans / chanPerBlock;
    short *src = &f[0];
    short *dst = out;
    for (int blockId = 0; blockId < blocks; ++blockId) {
      transpose(src + blockId * (chanPerBlock * kern_w * kern_h * tmpInChans),
          dst + blockId * (chanPerBlock * kern_w * kern_h * tmpInChans),
          chanPerBlock, ddrBlockRows, ddrBlockRows, ddrBlockCols);
    }
    outRows = 64;
    assert(f.size() % outRows == 0);
    outCols = f.size() / outRows;
  }
}

void concatenateScale(std::vector<int> &concatenatedScales, std::vector<int> &posScales, std::vector<int> &negScales)
{
     for(int i=0; i<posScales.size(); i++)
     {
         
         concatenatedScales[i] = posScales[i] & 0xFF;
         int negScaleToConcatenate = (negScales[i] & 0xFF) << 8;
         concatenatedScales[i] |= (negScaleToConcatenate);
     }
}

template<typename DType>
int XDNNV3FillWeightsBiasQuantBlob(short *blob, int offset,
    std::string layerName, std::string fpgaCfgFile, const DType *origA,
    unsigned int origAsize, const DType *origBias,
    unsigned int origBiasSize, unsigned short kernWidth,
    unsigned short kernHeight, unsigned int inChans, unsigned int outChans,
    int srcFullSectNum, int srcReplSectNum, int srcReplUnitNum,
    int srcReplUnitWidth, bool convHalfRateMode, std::string opType, 
    int slice, std::vector<int> &cfgWtEachItnDemarkIdx) {

  XDNNLayerQuantParam *qp = XDNNLayerQuantParamDB::getLayerQuantParam(
      fpgaCfgFile, layerName);
  bool skipLoadingWeights = false;
  if(opType!="XNConv" && opType!="XNMaxPoolPipelined")
  {
     skipLoadingWeights = true;
  }
  int ddrInsertIdx = 0;
  std::vector<short> shortA(origAsize);
  short *filterMatrix = shortA.data();
  if(origAsize > 0) {
    if (qp) {
      // quantize weights per channeli
      for (int ch = 0; ch < qp->thresh.size(); ch++)
      {
        const int channelSize = origAsize / qp->thresh.size();
        XDNNQuantize((float*) (origA + (ch * channelSize)),
            &(shortA[ch * channelSize]), channelSize, qp->thresh[ch],
            qp->bitWidth, /*doRounding=*/true);
      }
    } else
      for (int i = 0; i < origAsize; i++) {
        int dataVal = (int) (origA[i]);
        dataVal = std::min(SHRT_MAX, dataVal);
        dataVal = std::max(SHRT_MIN, dataVal);
        shortA[i] = short(dataVal);
      }

    filterMatrix = &(shortA[0]);
    if (qp && qp->bitWidth == 8) {
      XDNNQuantizeCompress(filterMatrix, origAsize);
      char *matrixFilterXDNNV3 = (char*) filterMatrix;
    }
  }

  /*****quantize Bias*****/
  origBiasSize = origBiasSize == 0 ? outChans : origBiasSize;
  std::vector<int> shortB(origBiasSize);
  int *fpgaBias = &(shortB[0]);
  assert(origBiasSize == outChans);
  if (qp) {

    if(opType == "XNEltwise")
       XDNNQuantizeBiasV3_scale((float*) (origBias), fpgaBias, origBiasSize,
        qp->threshOut, &(qp->postShift), qp->bitWidth, true);
    else    
    XDNNQuantizeBiasV3((float*) (origBias), fpgaBias, origBiasSize,
        qp->threshIn, &(qp->thresh), qp->bitWidth,
        /*doRounding=*/true);
  } else
    for (int i = 0; i < origBiasSize; i++) {
      int dataVal = origBias[i];
      dataVal = std::min(SHRT_MAX, dataVal);
      dataVal = std::max(SHRT_MIN, dataVal);
      fpgaBias[i] = short(dataVal);
    }
  const int* toDumpBias = fpgaBias;

  /*********oCfg Params Ddr Format*******/

  std::vector < uint64_t > ocfgs(origBiasSize, 0);
  std::vector<V3ChannelConfig> ocfgObjs(origBiasSize);
  std::vector<int> preShifts(origBiasSize, 0);
  std::vector<int> scales(origBiasSize, 0);
  std::vector<int> postShifts(origBiasSize, 0);
  if (qp)
  {
    preShifts = qp->preShift;
    
    if(qp->posScale.size()>0)
    {
      concatenateScale(scales, qp->posScale, qp->negScale);
    }
    else
    {
      scales = qp->scale;
    }
    postShifts = qp->postShift;
  }
  
  // Special case for avgpool: if no qp for avgpool, fill default values
  // to support legacy quant json
  if(!qp && (opType == "XNAvgPool")) {
    std::fill(scales.begin(), scales.end(), 1338);
    std::fill(postShifts.begin(), postShifts.end(), 16);
  }

  for (int i = 0; i < origBiasSize; i++) {
    int preshift = preShifts.size() > 1 ? preShifts.at(i) : preShifts.at(0);
    int scale = scales.size() > 1 ? scales.at(i) : scales.at(0);
    int postshift = postShifts.size() > 1 ? postShifts.at(i) : postShifts.at(0);
    ocfgObjs[i].setConfig(preshift, true, scale, int(fpgaBias[i]), postshift);
    ocfgs[i] = ocfgObjs[i].getConfig();
  }

  /*******Entire Ddr Format Conversion*******/

  short *outPut = &(blob[offset]);
  int *output = (int*) outPut;

  if(skipLoadingWeights)
  {
     XDNNV3LoadCfgData( &(ocfgs[0]), outChans, output );

  }
  else
  {
      XDNNV3MatToDDR<char>((char*) (&(filterMatrix[0])), &(ocfgs[0]), kernWidth,
          kernHeight, inChans, outChans, output, true, srcFullSectNum,
          srcReplSectNum, srcReplUnitNum, srcReplUnitWidth, convHalfRateMode,
          slice, cfgWtEachItnDemarkIdx);
  }


  if(opType == "XNAvgPool") kernWidth = kernHeight = 0;

  ddrInsertIdx += XDNNV3ComputeWeightsBiasQuantSize(int(kernWidth),
      int(kernHeight), outChans, srcFullSectNum, srcReplSectNum, srcReplUnitNum,
      true, convHalfRateMode);
  ddrInsertIdx = ddrInsertIdx * 2; //because computeSize returns size in ints, but we want size in shorts
  return ddrInsertIdx;
}

void XDNNQuantizeCompress(short *in, int sz) {
  // (compress 16-bit values into 8-bit)

  //std::cout << "ANDBG b4 compress ";
  //for (int i=0; i < 10; i++)
  //  std::cout << in[i] << " ";
  //std::cout << std::endl;

  char *out = (char*) in;
  for (int i = 0; i < sz; i++)
    out[i] = char(in[i] & 0xFF);

  //std::cout << "ANDBG compressed ";
  //for (int i=0; i < 10; i++)
  //  std::cout << short(out[i]) << " ";
  //std::cout << std::endl;
}

void XDNNHBMDataSplitCfgBlob(int startCfgIdx, int endCfgIdx, std::vector<std::vector<int> > &dataBlobsHBM, std::vector<int> &idxInBankToFill, int* &dataBlobToSplit){
   for(int j=startCfgIdx; j<=endCfgIdx; j++)
   {
     for(int k=0; k<dataBlobsHBM.size(); k++)
     {
       dataBlobsHBM[k].push_back(dataBlobToSplit[j]);
       idxInBankToFill[k]++;
     }
   }

}

void XDNNHBMDataSplitWtsBlob(int startWtIdx, int endWtIdx, std::vector<std::vector<int> > &dataBlobsHBM, std::vector<int> &idxInBankToFill, int* &dataBlobToSplit){
   int bankToFill = 0;
   for(int k=startWtIdx; k<=endWtIdx; k=k+2)
   {
     dataBlobsHBM[bankToFill].push_back(dataBlobToSplit[k]);
     dataBlobsHBM[bankToFill].push_back(dataBlobToSplit[k+1]);
     if(bankToFill==5)
     {
       bankToFill=0;
     }
     else
     {
       bankToFill++;
     }
     idxInBankToFill[bankToFill]=idxInBankToFill[bankToFill]+2;
   }
   if(bankToFill!=5)
   {
      for(int y=bankToFill; y<=5; y++)
      {
        dataBlobsHBM[y].push_back(0);
        dataBlobsHBM[y].push_back(0);
        idxInBankToFill[y]=idxInBankToFill[y]+2;
      }
   }

}

void XDNNHBMDataSplit(std::vector<std::vector<int> > &dataBlobsHBM, std::vector<int> &breakIdx, int totalSize, int* &dataBlobToSplit, std::vector<int> &sizeOfEachDataBlobInShorts){
   int totalNumberOfIterations = breakIdx.size()/2;
   int startCfgIdx = 0;
   int endCfgIdx = 0;
   int startWtIdx = 0;
   int endWtIdx = 0;

   std::vector<int> idxInBankToFill(dataBlobsHBM.size(),0);
   int bankToFill = 0;

   for(int i=0; i < totalNumberOfIterations; i++)
   {
     startCfgIdx = breakIdx[i*2];
     endCfgIdx = breakIdx[(i*2)+1]-1;
     startWtIdx = endCfgIdx + 1;
     if(i!=totalNumberOfIterations-1)
       endWtIdx = breakIdx[(i+1)*2]-1;
     else
       endWtIdx = totalSize;

     if(i%2==0)
     {
        //config block
        XDNNHBMDataSplitCfgBlob(startCfgIdx, endCfgIdx, dataBlobsHBM, idxInBankToFill, dataBlobToSplit);
     }
     else
     {
        //weights block
        XDNNHBMDataSplitWtsBlob(startWtIdx, endWtIdx, dataBlobsHBM, idxInBankToFill, dataBlobToSplit);
     }
   }
   for(int i=0; i<sizeOfEachDataBlobInShorts.size(); i++)
   {
     sizeOfEachDataBlobInShorts[i] = idxInBankToFill[i]*sizeof(short);
   }

}


void copyKernBlock(short *copyFrom, short *copyTo, int copyFromStartIdx,
    int copyFromStopIdx, int copyToStartIdx) {
  for (int i = copyFromStartIdx; i < copyFromStopIdx; i++) {
    copyTo[copyToStartIdx] = copyFrom[i];
    copyToStartIdx++;
  }
}

void XDNNDepthConvFilterPadding(short *src, short *dest, unsigned int outChans,
    unsigned short kernW, unsigned short kernH) {
  int channelSize = kernW * kernH;
  int outChannel = 0;
  int paddedFiltersStartBlockIdx = 0;
  int paddedFiltersStopBlockIdx = paddedFiltersStartBlockIdx - 1
      + (32 * channelSize);
  int tempIdxRollOver = 0;
  for (int i = 0; i < outChans * kernW * kernH; i = i + channelSize) {
    if (outChannel % 32 == 0) {
      tempIdxRollOver = 0;
    }
    copyKernBlock(src, dest, i, i + channelSize,
        paddedFiltersStartBlockIdx + (tempIdxRollOver * channelSize));
    paddedFiltersStartBlockIdx = paddedFiltersStopBlockIdx + 1;
    paddedFiltersStopBlockIdx = paddedFiltersStartBlockIdx - 1
        + (32 * channelSize);
    outChannel++;
    tempIdxRollOver++;
  }

}

template<typename DType>
int XDNNFillWeightsBiasQuantBlob(short *blob, int offset, std::string layerName,
    std::string fpgaCfgFile, const DType *origA, unsigned int origAsize,
    const DType *origBias, unsigned int origBiasSize,
    unsigned short kernWidth, unsigned short kernHeight,
    unsigned int inChans, unsigned int outChans, XDNNOperation op) {

  /*
   * convert Dtype to short or apply quantization
   */
  std::vector<short> shortA;
  if (op != XDNN_ELTWISE && op != XDNN_ELTWISE_SCALE) {
    assert(origAsize > (unsigned int) 0x0); // Must have greater than zero size to avoid seg fault
    shortA.resize(origAsize);
  }

  XDNNLayerQuantParam *qp = XDNNLayerQuantParamDB::getLayerQuantParam(fpgaCfgFile, layerName);

  if (op == XDNN_ELTWISE) {
  } 
  else if (op == XDNN_ELTWISE_SCALE) {
    if(qp) {
      float* origA_sp = (float*) origA;
      std::vector<float> thresh = qp->thresh.size() > 1 ? qp->thresh : std::vector<float>(origAsize, qp->thresh[0]);
      for(int i=0; i<origAsize; ++i) {
        shortA[i] = XDNNQuantize(origA_sp[i], thresh[i], qp->bitWidth, /*doRounding=*/1);
      }
    }
    else {
     for (int i = 0; i < origAsize; i++) {
       int dataVal = (int) (origA[i] );
       dataVal = std::min(SHRT_MAX, dataVal);
       dataVal = std::max(SHRT_MIN, dataVal);
       shortA[i] = short(dataVal);
     }
    }
  }
  else {  //Convolution etc. 
   if (qp) {
     // quantize weights per channel
     int channelSize = origAsize / qp->thresh.size();
     for (int ch = 0; ch < qp->thresh.size(); ch++)
       XDNNQuantize((float*) (origA + (ch * channelSize)),
           &(shortA[ch * channelSize]), channelSize, qp->thresh[ch],
           qp->bitWidth,
           /*doRounding=*/true);
   } else {
     for (int i = 0; i < origAsize; i++) {
       int dataVal = (int) (origA[i]);
       dataVal = std::min(SHRT_MAX, dataVal);
       dataVal = std::max(SHRT_MIN, dataVal);
       shortA[i] = short(dataVal);
     }
   }
  }

  /*
   * format A for RTL kernel
   */
  int ddrInsertIdx = 0;
  short *fpgaA = &(blob[offset + ddrInsertIdx]);
  unsigned int fpgaRows = 0, fpgaCols = 0;
  if (op == XDNN_ELTWISE) {
  }
  else if (op == XDNN_ELTWISE_SCALE) {
    for(int i=0; i<origAsize; ++i) fpgaA[i] = shortA[i];
    const int wtSize = ceil((float) origAsize / 32.) * 32;
    for (int c = origAsize; c < wtSize; ++c)
      fpgaA[c] = 0;

    if(qp && qp->bitWidth == 8)
      XDNNQuantizeCompress(fpgaA, wtSize);

    ddrInsertIdx += wtSize;
  }
  else {
    if (op == XDNN_CONVDEPTH) {
      assert(inChans == outChans);
      std::vector<short> paddedFilters(outChans * kernWidth * kernHeight * 32, 0);
      XDNNDepthConvFilterPadding(&(shortA[0]), &(paddedFilters[0]), outChans,
          kernWidth, kernHeight);
      const unsigned int depthConvMatToDDRInChans = 32;
      XDNNMatToDDR(&(paddedFilters[0]), kernWidth, kernHeight,
          depthConvMatToDDRInChans, outChans, fpgaA, fpgaRows, fpgaCols);
    } else {
      XDNNMatToDDR(&(shortA[0]), kernWidth, kernHeight, inChans, outChans, fpgaA,
          fpgaRows, fpgaCols);
    }
    if (qp && qp->bitWidth == 8)
      XDNNQuantizeCompress(fpgaA, fpgaRows * fpgaCols);

    ddrInsertIdx += fpgaRows * fpgaCols;
  }

  /*
   * now write bias after weight
   * bias vectors must be written in multiples of 32 shorts...
  */
  short *fpgaBias = &(blob[offset + ddrInsertIdx]);
  if (op == XDNN_ELTWISE || op == XDNN_ELTWISE_SCALE) {
    const int biasSize = ceil((float) outChans / 32.) * 32;
    for(int c = 0; c < biasSize; ++c) {
        fpgaBias[c] = 0;
    }
    if (qp && qp->bitWidth == 8)
      XDNNQuantizeCompress(fpgaBias, biasSize);

    ddrInsertIdx += biasSize;
  }
  else {
    // abidk : std::cout << LOCATION << origBiasSize <<  " " << outChans << std::endl;
    assert(origBiasSize == outChans);
    assert(qp);
      XDNNQuantize((float*) (origBias), fpgaBias, origBiasSize, qp->threshOut,
          qp->bitWidth,
          /*doRounding=*/true);

    const int biasSize = ceil((float) outChans / 32.) * 32;
    for (int c = origBiasSize; c < biasSize; ++c)
      fpgaBias[c] = 0;

    if (qp && qp->bitWidth == 8)
      XDNNQuantizeCompress(fpgaBias, biasSize);

    ddrInsertIdx += biasSize;
  }

  const int biasSize = ceil((float) outChans / 32.) * 32;
  if (qp) {
    // write shift/scale values after bias
    for (int t = 0; t < 3; t++) {
      short *extraVal = &(blob[offset + ddrInsertIdx]);

      // use quantization values from layer cfg file
      std::vector<int> vals = qp->preShift;
      if (t == 1)
        vals = qp->scale;
      else if (t == 2)
        vals = qp->postShift;
      for (int i = 0; i < vals.size(); i++)
        extraVal[i] = vals[i];
      for (int i = vals.size(); i < biasSize; i++)
        extraVal[i] = 0;

      if (qp->bitWidth == 8)
        XDNNQuantizeCompress(extraVal, biasSize);

      ddrInsertIdx += biasSize;
    }
  }
  return ddrInsertIdx;
}


static std::vector<XDNNCompilerOp> 
buildWeightLayersParams(const XDNNDescriptorDB &descDB)
{
  const std::vector<string> &layerNames = descDB.getLayerNames();
  std::vector<XDNNCompilerOp> weightLayersParams;

  std::vector<std::string> fieldsWeWant = {
    "name", "kernel_w", "kernel_h",
    "inchan", "outchan", "slice",
    "src_full_sect_num", "src_repl_sect_num", 
    "src_repl_unit_num", "src_repl_unit_width",
    "en_halfrate_mode", "XNOp"};

  std::map<std::string, bool> visitedLayers;
  for (string l : layerNames) {
    std::string uniqueLayerName = getUniqueLayerName(l);

    XDNNCompilerOp op = descDB.getLayerJsonParams(l);
    std::string opName = op.getParams().at("XNOp");

    if (opName == "XNGather" 
      || opName == "XNScatter"
      || opName == "XNMaxPool"
      || opName == "XNThroughputInterbuf"
      || opName == "XNUpsample")
      continue; // no weights

    if (l == uniqueLayerName // not a partial # name 
      && visitedLayers.find(uniqueLayerName) != visitedLayers.end())
    {
      std::cerr << "ERROR: duplicate layer name found in compiler.json: " << l << std::endl;
      exit(1);
    }
    visitedLayers[uniqueLayerName] = true;

    std::map<std::string, std::string> keyVals;
    for (auto field: fieldsWeWant) 
      if (op.getParams().find(field) != op.getParams().end())
        keyVals[field] = op.getParams().at(field);

    // save a separate weight layer name 
    // -- some layers may adopt other names for weights
    // e.g. MaxPoolPipelined takes the conv name
    keyVals["weight_name"] = keyVals["name"]; 

    if (opName == "XNMaxPoolPipelined")
    {
      keyVals["weight_name"] = op.getParams().at("conv_name");
      keyVals["kernel_w"] = op.getParams().at("conv_kernel_w");
      keyVals["kernel_h"] = op.getParams().at("conv_kernel_h");
      keyVals["outchan"] = op.getParams().at("pool_inchan");
    }

    keyVals["name"] = getUniqueLayerName(keyVals["name"]);
    keyVals["weight_name"] = getUniqueLayerName(keyVals["weight_name"]);

    weightLayersParams.push_back(XDNNCompilerOp(keyVals));
  }

  return weightLayersParams;
}

bool has_suffix(const std::string &str, const std::string &suffix)
{
  return str.size() >= suffix.size() && 
         str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

bool path_exists(const std::string &path, bool testIsDir)
{
  struct stat buf;
  int retval = stat(path.c_str(), &buf);
  bool isValid = false;

  if(!retval) {
    // Found something...
    bool isFile = buf.st_mode & S_IFREG;
    bool isDir = buf.st_mode & S_IFDIR;
    if(isFile) {
      std::cout << "Path " << path << " is a file." << std::endl;
    } else if(isDir) {
      std::cout << "Path " << path << " is a directory." << std::endl;
    }
    if(testIsDir != isDir) {
      isValid = false;
    } else {
      isValid = true;
    }
  }    

  return isValid;
}

template<typename Dtype>
short* XDNNScriptExecutor<Dtype>::XDNNLoadWeights(const std::vector<XBLASHandle*> & xblas_handles,
  const std::string &weightsPath,
  const std::string &xdnnNetFile, const std::string &quantizeCfgFile,
  unsigned int cuMask) {
  if(XDNN_VERBOSE) std::cout << LOCATION << "XDNNLoadWeights Starts" << std::endl;
  XDNNDescriptorDB descDB(xdnnNetFile);
  std::vector<XDNNCompilerOp> weightLayersParamsArr = buildWeightLayersParams(descDB);

  std::map<std::string, std::vector<Dtype> > allWeights;
  std::map<std::string, std::vector<Dtype> > allBias;


  bool isDir = !has_suffix(weightsPath,".h5");
  if(!path_exists(weightsPath, isDir)) {
    std::cerr << "ERROR: Weight path not valid: " << weightsPath << std::endl;
    exit(1);
  }
  
	H5::Exception::dontPrint(); // Don't print H5 exceptions, we will handle

	const H5std_string FILE_NAME(weightsPath);  // Open up weights
	H5::H5File file(FILE_NAME, H5F_ACC_RDONLY);  //   as hdf5 file

	// Load up all weights
	for (int fi=0; ; fi++)
	{
	  try
	  {

		// Open the HDF5 Dataset for given layer
		std::string dsname = "fwbqb_" + std::to_string(fi);
		const H5std_string DATASET_NAME(dsname);
		H5::DataSet dataset = file.openDataSet(DATASET_NAME);

		// Get the dataspace // Defines number of dimensions, and the size of each dimension
		H5::DataSpace dataspace = dataset.getSpace();

		// Get the number of dimensions in the dataspace.
		int rank = dataspace.getSimpleExtentNdims();

		// Get the dimension size of each dimension in the dataspace
		std::vector<hsize_t> dims(rank);
		int ndims = dataspace.getSimpleExtentDims(&dims[0], NULL);

		// Get the layerName from the dataset's layer_name attribute
		const H5std_string  ATTR_NAME("layer_name");
		H5::Attribute attribute = dataset.openAttribute(ATTR_NAME);
		H5::StrType stype = attribute.getStrType();
		std::string layerName;
		attribute.read(stype,layerName);

		if (XDNN_VERBOSE)
		{
		  std::cout << "Loading HDF5 dataset: " << DATASET_NAME << ", from file: " << FILE_NAME << ",layerName: " << layerName << ", having dataspace:" << std::endl;
		  std::cout << "rank: " << rank << ", dimensions: ";
		  for (int i=0;i<dims.size();i++)
			std::cout << (unsigned long)(dims[i]) << " x ";
		  std::cout << std::endl;
		}

		// Get the raw data
		std::vector<Dtype> weights;
		int space = 1;
		for (int i=0;i<rank;i++)
		  space *= dims[i];
		weights.resize(space);
		dataset.read(&weights[0],H5::PredType::NATIVE_FLOAT,dataspace,dataspace);

		// Uncomment below if you don't trust hdf5 weights, and you need to see them
		//std::ofstream outfile;
		//outfile.open("outfile.txt." + std::to_string(fi));
		//for (int i=0;i<space;i++)
		//  outfile << std::fixed << weights[i] << " ";

		allWeights[layerName] = weights;
	  }
	  catch( H5::FileIException error )
	  {
		if (XDNN_VERBOSE)
		  std::cout << "No more weights in HDF5" << std::endl;
		break;
	  }
	}

	// Load up all bias
	for (int fi=0; ; fi++)
	{
	  try
	  {
		// Open the HDF5 Dataset for given layer
		std::string dsname = "fwbqb_bias_" + std::to_string(fi);
		const H5std_string DATASET_NAME(dsname);
		H5::DataSet dataset = file.openDataSet(DATASET_NAME);

		// Get the dataspace // Defines number of dimensions, and the size of each dimension
		H5::DataSpace dataspace = dataset.getSpace();

		// Get the number of dimensions in the dataspace.
		int rank = dataspace.getSimpleExtentNdims();

		// Get the dimension size of each dimension in the dataspace
		std::vector<hsize_t> dims(rank);
		int ndims = dataspace.getSimpleExtentDims(&dims[0], NULL);

		// Get the layerName from the dataset's layer_name attribute
		const H5std_string  ATTR_NAME("layer_name");
		H5::Attribute attribute = dataset.openAttribute(ATTR_NAME);
		H5::StrType stype = attribute.getStrType();
		std::string layerName;
		attribute.read(stype,layerName);

		if (XDNN_VERBOSE)
		{
		  std::cout << "Loading HDF5 dataset: " << DATASET_NAME << ", from file: " << FILE_NAME << ",layerName: " << layerName << ", having dataspace:" << std::endl;
		  std::cout << "rank: " << rank << ", dimensions: ";
		  for (int i=0;i<dims.size();i++)
			std::cout << (unsigned long)(dims[i]) << " x ";
		  std::cout << std::endl;
		}

		// Get the raw data
		std::vector<Dtype> bias;
		int space = 1;
		for (int i=0;i<rank;i++)
		  space *= dims[i];
		bias.resize(space);
		dataset.read(&bias[0],H5::PredType::NATIVE_FLOAT,dataspace,dataspace);

		allBias[layerName] = bias;
	  }
	  catch( H5::FileIException error )
	  {
		if (XDNN_VERBOSE)
		  std::cout << "No more bias in HDF5" << std::endl;
		break;
	  }
	}
  
  // if there are fewer weights provided, dummy weights will be loaded below
  //assert(allWeights.size() == weightLayersParamsArr.size());

  std::map<std::string, XDNNWeightParam<Dtype> > m;
  for (int i=0; i < weightLayersParamsArr.size(); i++)
  {
    std::string layerName 
      = weightLayersParamsArr.at(i).getStr("name");
    std::string weightName 
      = weightLayersParamsArr.at(i).getStr("weight_name");

    if (allWeights.find(weightName) == allWeights.end())
    {

      if (weightLayersParamsArr.at(i).getOp() == XDNN_CONV)
        std::cout << "WARNING: loading zero weights for " << weightName << std::endl;

      allWeights[weightName] = std::vector<Dtype>(
        weightLayersParamsArr.at(i).getInt("kernel_w", 0)
        *weightLayersParamsArr.at(i).getInt("kernel_h", 0)
        *weightLayersParamsArr.at(i).getInt("inchan")
        *weightLayersParamsArr.at(i).getInt("outchan"));
    }
    if (allBias.find(weightName) == allBias.end())
    {
      if (weightLayersParamsArr.at(i).getOp() == XDNN_CONV)
        std::cout << "WARNING: loading zero bias for " << weightName << std::endl;
      allBias[weightName] = std::vector<Dtype>(
        weightLayersParamsArr.at(i).getInt("outchan"));
    }
      
    m[layerName] = {
      .wgt_ = allWeights[weightName].data(),
      .bias_ = allBias[weightName].data(),
      .wgt_sz_ = allWeights[weightName].size(),
      .bias_sz_ = allBias[weightName].size()
    };
  }
  if(XDNN_VERBOSE) std::cout << "ARK : XDNNLoadWeights ends @ line : "  << __LINE__ << std::endl;

  return XDNNLoadWeights(xblas_handles, m, xdnnNetFile, quantizeCfgFile, cuMask);
}

template<typename Dtype>
short* XDNNScriptExecutor<Dtype>::XDNNLoadWeights(const std::vector<XBLASHandle*> & xblas_handles,
    const std::map<std::string, XDNNWeightParam<Dtype> > & m,
    const std::string &xdnnNetFile, const std::string & quantizeCfgFile,
    unsigned int cuMask) {
  if(XDNN_VERBOSE) std::cout << "ARK : starting XDNNLoadWeights" <<  " , thread_id : " << std::this_thread::get_id() <<  " @ " << __LINE__ << std::endl;
  std::cout << "Loading weights/bias/quant_params to FPGA..." << std::endl;

  XDNNDescriptorDB descDB(xdnnNetFile);

  const std::vector<string> & layerNames = descDB.getLayerNames();
  std::map<std::string, bool> visitedLayers;
  // set up big A matrix
  int totalDdrSize = 0;

  std::vector<XDNNCompilerOp> weightLayersParamsArr = buildWeightLayersParams(descDB);

  std::map<std::string, XDNNCompilerOp> weightLayersParams;

  for (int li=0; li < weightLayersParamsArr.size(); li++) {
    weightLayersParams.insert({
      weightLayersParamsArr.at(li).getParams().at("name"),
      weightLayersParamsArr.at(li)});
    if(XDNN_VERBOSE) std::cout << LOCATION << 
        "weightLayersParamsArr " << " : " << weightLayersParamsArr.at(li).getParams().at("name") << std::endl;
  }

  for (string l : layerNames) {
    std::string uniqueLayerName = getUniqueLayerName(l);
    if (visitedLayers.find(uniqueLayerName) != visitedLayers.end())
      continue;
    visitedLayers[uniqueLayerName] = true;

    if (weightLayersParams.find(uniqueLayerName) == weightLayersParams.end())
      continue; // this layer doesn't have weights

    if (m.find(uniqueLayerName) == m.end()) {
      /*
       * FIXME TODO
       * http://jira.xilinx.com/browse/ML-754
       * we are bypassing this sanity check for now, to let layers without 
       * weights/params go through.
       * In the future, compiler should tell us which operations expect
       *  weights/params so we can enforce this check for specific layers
       */
      //assert(wIt != m.end());
      continue;
    }

    XDNNCompilerOp &params = weightLayersParams.find(uniqueLayerName)->second;

    bool is8bit = true;
    if (!quantizeCfgFile.empty())
    {
      XDNNLayerQuantParam *qp = XDNNLayerQuantParamDB::getLayerQuantParam(quantizeCfgFile, l);
      if (qp)
        is8bit = (qp->bitWidth == 8) ? true : false;
    }

    int kernel_w = params.getInt("kernel_w", 0);
    int kernel_h = params.getInt("kernel_h", 0);

    // Special case for avgpool
    if(params.getOp() == XDNN_AVGPOOL) {
        kernel_w = 0;
        kernel_h = 0;
    }

    totalDdrSize += XDNNV3ComputeWeightsBiasQuantSize(
        kernel_w, kernel_h,
        params.getInt("outchan"),
        params.getInt("src_full_sect_num", 0), params.getInt("src_repl_sect_num", 0),
        params.getInt("src_repl_unit_num", 0), is8bit, 
        params.getInt("en_halfrate_mode", 0) == 1) * sizeof(short);
    if(XDNN_VERBOSE) std::cout << LOCATION << "DDR Size : " << totalDdrSize << " after layer : " << uniqueLayerName  << std::endl;
    if(XDNN_VERBOSE) std::cout << LOCATION << "Params : " << params.getInt("kernel_w", 0) << " " << params.getInt("kernel_h", 0) << " "
                                << params.getInt("inchan") << " " << params.getInt("outchan") << std::endl;
  }

  visitedLayers.clear();

  if (XDNN_VERBOSE)
    std::cout << "[XDNN] total A size " << totalDdrSize << std::endl;

  short *ddrBlob = XDNNMakeWeightsBiasQuantBlob(totalDdrSize);
  int ddrInsertIdx = 0;
  std::string layer2OffsetMapStr = "";
  std::vector<int> cfgWtEachItnDemarkIdx;
  for (string l : layerNames) {
    std::string uniqueLayerName = getUniqueLayerName(l);
    if  (visitedLayers.find(uniqueLayerName) != visitedLayers.end())
      continue;
    visitedLayers[uniqueLayerName] = true;

    if (weightLayersParams.find(uniqueLayerName) == weightLayersParams.end())
      continue; // this layer doesn't have weights

    auto wIt = m.find(uniqueLayerName);
    if (wIt == m.end ()) {
      /*
       * FIXME TODO
       * http://jira.xilinx.com/browse/ML-754
       * we are bypassing this sanity check for now, to let layers without 
       * weights/params go through.
       * In the future, compiler should tell us which operations expect
       *  weights/params so we can enforce this check for specific layers
       */
      //assert(wIt != m.end());
      continue;
    }

    XDNNCompilerOp &params = weightLayersParams.find(uniqueLayerName)->second;

    if (layer2OffsetMapStr != "")
      layer2OffsetMapStr += ",";
    layer2OffsetMapStr += getUniqueLayerName(params.getParams().at("weight_name"))
      + ":" + std::to_string(ddrInsertIdx);

    if(XDNN_VERBOSE) std::cout << LOCATION << " layer2OffsetMapStr = " 
            << getUniqueLayerName(params.getParams().at("weight_name")) 
            << " " << ddrInsertIdx 
            << " " << HEX(ddrInsertIdx) << std::endl;
            
    cfgWtEachItnDemarkIdx.push_back(ddrInsertIdx);
    ddrInsertIdx += XDNNV3FillWeightsBiasQuantBlob(
      ddrBlob, ddrInsertIdx, params.getParams().at("weight_name"), quantizeCfgFile,
      wIt->second.wgt_, wIt->second.wgt_sz_, wIt->second.bias_, wIt->second.bias_sz_,
      params.getInt("kernel_w", 0), params.getInt("kernel_h", 0),
      params.getInt("inchan"), params.getInt("outchan"),
      params.getInt("src_full_sect_num", 0), params.getInt("src_repl_sect_num", 0),
      params.getInt("src_repl_unit_num", 0), params.getInt("src_repl_unit_width", 0),
      params.getInt("en_halfrate_mode", 0), params.getParams().at("XNOp"),
      params.getInt("slice", 0), cfgWtEachItnDemarkIdx);
  }

#ifdef DEBUG
  xdnn::XDebugJournal().logBlob("DDR BLOB ", ddrBlob, totalDdrSize, 50);
#endif

  //if (ddrInsertIdx != totalDdrSize) {
  //  std::cout << "XDNN ERROR A/bias/quant size mismatch ddrInsertIdx " << ddrInsertIdx
  //      << " vs totalDdrSize " << totalDdrSize << std::endl;
  //  exit(1);
  //}
#ifdef ENABLE_HBM
   int numberOfDdrBanks = 6;
   std::vector<std::vector<int> > dataBlobsHBM(numberOfDdrBanks);
   std::vector<int> sizeOfEachDataBlobInShorts(numberOfDdrBanks,0);
   int *dataBlobToSplit = (int*) ddrBlob;
   int totalDataSizeToSplit = totalDdrSize/sizeof(short);
   XDNNHBMDataSplit(dataBlobsHBM, cfgWtEachItnDemarkIdx, totalDataSizeToSplit, dataBlobToSplit,sizeOfEachDataBlobInShorts);
#endif

  std::vector<int> cuIdxVec;
  XDNNMaskToIndex(cuIdxVec, cuMask);

  // load big A to FPGA
#ifdef ENABLE_HBM
  int *ptrToEachBlob = NULL;
  short *ptrToEachBlobShort = NULL;
  for (int di = 0; di < xblas_handles.size(); di++) {
    for(int j=0; j<numberOfDdrBanks; j++)
      {
        ptrToEachBlob = &(dataBlobsHBM[j][0]);
        ptrToEachBlobShort = (short*)ptrToEachBlob;
        xRegMem(*xblas_handles[di], ptrToEachBlobShort, sizeOfEachDataBlobInShorts[j]*sizeof(short));
      }
    for (auto cuIdx : cuIdxVec)
      {
      xblasHBMLoadA(*xblas_handles[di], dataBlobsHBM, layer2OffsetMapStr.c_str(), cuIdx);
     }
  }

#else
  const int totalSize = totalDdrSize * sizeof(short);
  for (int di = 0; di < xblas_handles.size(); di++) {
    xRegMem(*xblas_handles[di], ddrBlob, totalSize);
    for (auto cuIdx : cuIdxVec)
      {
      xblasLoadA(*xblas_handles[di], ddrBlob, layer2OffsetMapStr.c_str(), cuIdx);
     }
  }

#endif
  
#ifdef ENABLE_HBM
   int *ptrToFirstDdrBlob = &(dataBlobsHBM[0][0]);
   short *ptrToFirstDdrBlobInShorts = (short*)ptrToFirstDdrBlob;
#endif

  if(XDNN_VERBOSE) std::cout << "ARK : Returning from XDNNLoadWeights" << " @ " << __LINE__ << std::endl;
#ifdef ENABLE_HBM
   return ptrToFirstDdrBlobInShorts;
#else
   return ddrBlob;
#endif
}

void XDNNDescPrinter::dispatch(XDNNDataDescriptor * d) {

  std::cout << "XDNNDataDescriptor params fcMode: "
      << d->_shapeType << " shape: [" << d->_n << " " << d->_c
      << " " << d->_h << " " << d->_w << "]" << " dataTypeSize: "
      << d->_dataTypeSize << " hwAddrOffset: " << d->_hwAddrOffset << " hwsizebytes: " << d->_hwSizeInBytes
      << d->_singleStep << std::endl;
}

void XDNNDescPrinter::dispatch(XDNNInputDescriptor * d) {
  this->dispatch((XDNNDataDescriptor*) d);
}

void XDNNDescPrinter::dispatch(XDNNOutputDescriptor * d) {
  std::cout << "XDNNOutputDescriptor ";
  this->dispatch((XDNNDataDescriptor*) d);
}
void XDNNDescPrinter::dispatch ( XDNNDescriptor * d )
{
  std::cout << "XDNNDescriptor " << d->OpType() << std::endl;
  const std::map<std::string, std::string> &params = d->_cOp->getParams();
  for (auto it=params.begin(); it != params.end(); it++)
    std::cout << " " << it->first << ":" << it->second << std::endl;
  std::cout << std::endl;
}

std::string getUniqueLayerName(std::string layerName) {
  std::string uniqueLyrNm(layerName.begin(),
      std::find(layerName.begin(), layerName.end(), '#'));
  return uniqueLyrNm;
}

void XDNNDescriptorDB::buildDescriptors(
    const short * A, const std::string &quantizeFile) {
  const bool doQuantize = !quantizeFile.empty();
  xdnn::XTimer t;
  std::map<std::string, std::vector<int> > uniqueLayerNamesAbyteOffsets;
  std::vector<int> AbyteOffsets;
  std::string uniqueLayerName;

  for ( auto l : _activeLayers ){
	XDNNCompilerOp  op = *_compOp[l];
    std::shared_ptr<XDNNDescriptor> d = getDescriptor(l);
    if(XDNN_VERBOSE) std::cout << "ARK : @ " << __LINE__ << " Name : " << d->_name << " desc->_cOp = " << d->_cOp << std::endl;
    if (d->OpType() == XDNN_CONV) {
      const int kernWidth = op.getInt("kernel_w");
      const int kernHeight = op.getInt("kernel_h");
      const int inChans = op.getInt("inchan");
      const int outChans = op.getInt("outchan");
      std::shared_ptr<XDNNConvolutionDescriptor> conv_desc =
          std::dynamic_pointer_cast < XDNNConvolutionDescriptor > (d);

      int AbaseOffset = 0;
      const int srcFullSectNum = op.getInt("src_full_sect_num");
      const int srcReplSectNum = op.getInt("src_repl_sect_num");
      const int srcReplUnitNum = op.getInt("src_repl_unit_num");

      XDNNLayerQuantParam *qp = XDNNLayerQuantParamDB::getLayerQuantParam(
          quantizeFile, conv_desc->_name);
      const bool is8bit = (!qp || qp->bitWidth == 8) ? true : false;
      AbyteOffsets=XDNNV3ComputeWeightsBiasQuantByteOffsets(
          AbaseOffset, kernWidth, kernHeight, inChans, outChans,
          doQuantize, srcFullSectNum, srcReplSectNum, srcReplUnitNum,
          is8bit,
          op.getInt("en_halfrate_mode", 0) == 1);
      conv_desc->_A = A;
      conv_desc->_Aoffset = AbyteOffsets[0];
      conv_desc->_biasOffset = AbyteOffsets[1];
      if (doQuantize) {
        const int preShiftOffset = AbyteOffsets[2];
        const int scaleOffset = AbyteOffsets[3];
        const int postShiftOffset = AbyteOffsets[4];
        conv_desc->_perChannelScale = true;
        conv_desc->_preShift = preShiftOffset;
        conv_desc->_scaleVal = scaleOffset;
        conv_desc->_postShift = postShiftOffset;
      }
    }
    if (d->OpType() == XDNN_MAXPOOLPIPELINED)
    {
      const int kernWidth = op.getInt("conv_kernel_w");
      const int kernHeight = op.getInt("conv_kernel_h");
      const int inChans = op.getInt("inchan");
      const int outChans = op.getInt("pool_inchan");
      const int srcFullSectNum = op.getInt("src_full_sect_num");
      const int srcReplSectNum = op.getInt("src_repl_sect_num");
      const int srcReplUnitNum = op.getInt("src_repl_unit_num");

      std::shared_ptr<XDNNMaxPoolPipelinedDescriptor> maxpoolPipelined_desc = std::dynamic_pointer_cast<XDNNMaxPoolPipelinedDescriptor>(d);
      XDNNLayerQuantParam *qp = XDNNLayerQuantParamDB::getLayerQuantParam(
          quantizeFile, maxpoolPipelined_desc->_name);
      const bool is8bit = (!qp || qp->bitWidth == 8) ? true : false;

      int AbaseOffset = 0;
      AbyteOffsets=XDNNV3ComputeWeightsBiasQuantByteOffsets(
           AbaseOffset, kernWidth, kernHeight, inChans, outChans,
           doQuantize, srcFullSectNum, srcReplSectNum, srcReplUnitNum,
           is8bit,
           op.getInt("en_halfrate_mode", 0) == 1);
      maxpoolPipelined_desc->_A = A;
      maxpoolPipelined_desc->_Aoffset = AbyteOffsets[0];
      maxpoolPipelined_desc->_biasOffset = AbyteOffsets[1];
      if (doQuantize)
      {
        maxpoolPipelined_desc->_perChannelScale = true;
      }
    }


    if (d->OpType() == XDNN_ELTWISE || d->OpType() == XDNN_ELTWISE_SCALE) {
      if(doQuantize) {
        std::shared_ptr<XDNNEltwiseDescriptor> eltwise_desc =std::dynamic_pointer_cast<XDNNEltwiseDescriptor>(d);
        XDNNLayerQuantParam *qp = XDNNLayerQuantParamDB::getLayerQuantParam(quantizeFile, getUniqueLayerName(eltwise_desc->_name));
        if(qp == NULL)
          eltwise_desc->_useQuantParams = false;
        else
          {
            eltwise_desc->_useQuantParams = true;
            if(qp->posScale.size()>0)
            {
              eltwise_desc->_posScale = qp->posScale;
              eltwise_desc->_negScale = qp->negScale;
              eltwise_desc->_perChannelScale = qp->posScale.size() > 1 ? true : false;
            }
            else
            {
              eltwise_desc->_scale = qp->scale;
              eltwise_desc->_perChannelScale = qp->scale.size() > 1 ? true : false;
            }
            eltwise_desc->_postShift = qp->postShift;
            eltwise_desc->_preShift = qp->preShift;
          }
          eltwise_desc->_A=A;
      }
    }

    if (d->OpType() == XDNN_CONVDEPTH) {
      const int kernWidth = op.getInt("kernel_w");
      const int kernHeight = op.getInt("kernel_h");
      const int inChans = op.getInt("inchan");
      const int outChans = op.getInt("outchan");

      int AbaseOffset = 0;
      uniqueLayerName = getUniqueLayerName(l);
      if (uniqueLayerNamesAbyteOffsets.find(uniqueLayerName)
          == uniqueLayerNamesAbyteOffsets.end()) {
        AbyteOffsets = XDNNComputeWeightsBiasQuantByteOffsets(AbaseOffset,
            kernWidth, kernHeight, inChans, outChans, doQuantize);
        uniqueLayerNamesAbyteOffsets.insert(
            make_pair(uniqueLayerName, AbyteOffsets));
      } else {
        AbyteOffsets = uniqueLayerNamesAbyteOffsets[uniqueLayerName];
      }

      std::shared_ptr<XDNNConvolutionDepthDescriptor> ConvDepth_desc =
          std::dynamic_pointer_cast < XDNNConvolutionDepthDescriptor > (d);
      ConvDepth_desc->_A = A;
      ConvDepth_desc->_Aoffset = AbyteOffsets[0];
      ConvDepth_desc->_biasOffset = AbyteOffsets[1];
      if (doQuantize) {
        const int preShiftOffset = AbyteOffsets[2];
        const int scaleOffset = AbyteOffsets[3];
        const int postShiftOffset = AbyteOffsets[4];
        ConvDepth_desc->_perChannelScale = true;
        ConvDepth_desc->_preShift = preShiftOffset;
        ConvDepth_desc->_scaleVal = scaleOffset;
        ConvDepth_desc->_postShift = postShiftOffset;
      }
    }

    if (d->OpType() == XDNN_AVGPOOL) {
      std::shared_ptr<XDNNAvgpoolDescriptor> avgpool_desc =std::dynamic_pointer_cast<XDNNAvgpoolDescriptor>(d);
      if(doQuantize) {
        XDNNLayerQuantParam *qp = XDNNLayerQuantParamDB::getLayerQuantParam(quantizeFile, getUniqueLayerName(avgpool_desc->_name));
        if(qp == NULL) {
          avgpool_desc->_useQuantParams = true;
          avgpool_desc->_scale.push_back(1338);
          avgpool_desc->_postShift.push_back(16);
        }
        else {
          avgpool_desc->_useQuantParams = true;
          avgpool_desc->_scale = qp->scale;
          avgpool_desc->_postShift = qp->postShift;
        }

        // Only per-channel mode is supported. So even if global scale, replicate it.
        int num_channels = avgpool_desc->_cOp->getInt("outchan");
        assert(avgpool_desc->_scale.size() == 1 || avgpool_desc->_scale.size() == num_channels);
        assert(avgpool_desc->_postShift.size() == 1 || avgpool_desc->_postShift.size() == num_channels);
        if(avgpool_desc->_scale.size() == 1)  avgpool_desc->_scale.resize(num_channels, avgpool_desc->_scale.at(0));
        if(avgpool_desc->_postShift.size() == 1)  avgpool_desc->_postShift.resize(num_channels, avgpool_desc->_postShift.at(0));
      }
     avgpool_desc->_A=A;
   }

    _desc[l] = d;
  }
  if (XDNN_VERBOSE)
    std::cout << "XDNNDescriptorDB::getDescriptors elapsed: "
        << t.elapsed() * 1000 << " ms" << std::endl;
}

std::shared_ptr<XDNNDescriptor> XDNNDescriptorDB::getDescriptor(
	const std::string & layerName) const {
	assert(_compOp.find(layerName) != _compOp.end());
	const XDNNCompilerOp & op = *_compOp.find(layerName)->second;
	XDNNOperation xdnn_op = op.getOp();

	if (xdnn_op == XDNN_CONV) {
		return std::shared_ptr<XDNNDescriptor>(
				new XDNNConvolutionDescriptor(NULL, -1, -1, op));
	} else if (xdnn_op == XDNN_CONVDEPTH) {
		return std::shared_ptr<XDNNDescriptor>(
				new XDNNConvolutionDepthDescriptor(NULL, -1, -1, op));
	}
	else if (xdnn_op == XDNN_MAXPOOLPIPELINED) {
		return std::shared_ptr<XDNNDescriptor>(
				new XDNNMaxPoolPipelinedDescriptor(NULL, -1, -1, op));
	}
    else if (xdnn_op == XDNN_UPSAMPLE) {
		return std::shared_ptr<XDNNDescriptor>(
						new XDNNUpsampleDescriptor(op));
    }
    else if (xdnn_op == XDNN_MAXPOOL) {
      return std::shared_ptr < XDNNDescriptor > 
        (new XDNNMaxpoolDescriptor(op));
    } else if (xdnn_op == XDNN_AVGPOOL) {
      return std::shared_ptr < XDNNDescriptor > 
        (new XDNNAvgpoolDescriptor(op));
    } else if (xdnn_op == XDNN_SCATTER) {
      return std::shared_ptr < XDNNDescriptor > 
        (new XDNNScatterDescriptor(op));
    } else if (xdnn_op == XDNN_GATHER) {
      return std::shared_ptr < XDNNDescriptor > 
        (new XDNNGatherDescriptor(op));
    } else if (xdnn_op == XDNN_ELTWISE || xdnn_op == XDNN_ELTWISE_SCALE) {
      std::shared_ptr < XDNNDescriptor > tt(new XDNNEltwiseDescriptor(NULL, op));
      if(XDNN_VERBOSE) std::cout << "ARK @ " << __LINE__ << "  " << layerName << " Eltwise _paramsObjs : " << tt->_cOp << std::endl;
      return tt;
    } else if (xdnn_op == XDNN_SCALE) {
      std::cerr << "ERROR: XDNN_SCALE deprecated" << std::endl;
      exit(1);
    } else if (xdnn_op == XDNN_CONCAT) {
      std::cerr << "ERROR: XDNN_CONCAT not implemented" << std::endl;
      exit(1);
    } else if (xdnn_op == XDNN_BATCHNORM) {
      std::cerr << "ERROR: XDNN_BATCHNORM not implemented" << std::endl;
      exit(1);
    } else if (xdnn_op == XDNN_ACTIVATION) {
      std::cerr << "ERROR: XDNN_ACTIVATION not implemented" << std::endl;
      exit(1);
    }
  return nullptr;
}

std::vector<std::string> XDNNDescriptorDB::split(const std::string &s,
    char delim) {
  std::vector < std::string > elems;
  std::stringstream ss;
  ss.str(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

XDNNOperation XDNNDescriptorDB::getLayerType(
    const std::string & layerName) const {
  if (_compOp.find(layerName) != _compOp.end()) {
    return _compOp.find(layerName)->second->getOp();
  } else {
    return XDNN_UNSUPPORTED;
  }
}

XDNNCompilerOp XDNNDescriptorDB::getLayerJsonParams(
    const std::string & layerName) const {
  assert(_compOp.find(layerName) != _compOp.end());
  return *_compOp.find(layerName)->second;
}

static auto enum_conv_func = [](const std::string & type) {
  if ( type == "XNConv" )
    return XDNN_CONV;
  else if ( type == "XNMaxPool")
    return XDNN_MAXPOOL;
  else if ( type == "XNMaxPoolPipelined")
    return XDNN_MAXPOOLPIPELINED;
  else if ( type == "XNUpsample")
    return XDNN_UPSAMPLE;
  else if ( type == "XNAvgPool")
    return XDNN_AVGPOOL;
  else if ( type == "XNConcat" )
    return XDNN_CONCAT;
  else if ( type == "XNBatchNorm")
    return XDNN_BATCHNORM;
  else if ( type == "XNActivation" )
    return XDNN_ACTIVATION;
  else if ( type == "XNEltwise" )
    return XDNN_ELTWISE;
  else if ( type == "XNScale" )
    return XDNN_SCALE;
  else if (type == "XNConvDepth")
    return XDNN_CONVDEPTH;
  else if ( type== "XNScatter" )
    return XDNN_SCATTER;
  else if(type=="XNGather")
    return XDNN_GATHER;
  else if(type=="XNThroughputInterbuf")
    return XDNN_THROUGHPUT_INTERBUF;
  return XDNN_UNSUPPORTED;
};
void XDNNDescriptorDB::buildParamsFromJson(const std::string & fileName) {
  if(XDNN_VERBOSE) std::cout << "ARK : Calling XDNNDescriptorDB::buildParamsFromJson : " << fileName << " , " << std::this_thread::get_id() << std::endl;
  using boost::property_tree::ptree;
  ptree pt;
  std::ifstream jsonFile(fileName);
  read_json(jsonFile, pt);
  std::unordered_map<std::string, std::pair<std::string, std::string> >  outputSet;
  std::unordered_map<std::string, std::string>  inputSet;
  std::set<std::string> throughputInterbufSet;
  for ( const auto & layer : pt.get_child("inputs")){
	  std::string name = layer.second.get<std::string>("input_name", "");
	  for (const auto & lName : layer.second.get_child("next_layers")) {
		  inputSet[name] = lName.second.get_value<std::string>();
	  }
  }
  if (pt.get_child_optional("throughput_mode_intermediate_buffers"))
  {
    for ( const auto & layer 
      : pt.get_child("throughput_mode_intermediate_buffers")) {
      std::string name = layer.second.get<std::string>("name", "");
      throughputInterbufSet.insert(name);
    }
  }
  for ( const auto & layer : pt.get_child("outputs")){
	  std::string name = layer.second.get<std::string>("output_name", "");
	  for (const auto & tName : layer.second.get_child("previous_tensors")) {
		  outputSet[name].first = tName.second.get_value<std::string>();
	  }
	  for (const auto & lName : layer.second.get_child("previous_layers")) {
		  outputSet[name].second = lName.second.get_value<std::string>();
	  }
  }

  int opIndex = 0;
  for ( auto &layer : pt.get_child("network")) {
    opIndex++;
    std::string type = layer.second.get <std::string>("type", "");
    std::string ops = layer.second.get<std::string>("ops", "");
    std::string name = layer.second.get<std::string>("name", "");
    std::string active = layer.second.get<std::string>("active", "");

    XDNNOperation op = XDNN_UNSUPPORTED;
    std::map < std::string, std::string > xdnnKeyVals;
    xdnnKeyVals["type"] = type;
    xdnnKeyVals["ops"] = ops;
    xdnnKeyVals["name"] = name;
    xdnnKeyVals["active"] = active;
    std::string memDescr = layer.second.get <std::string>("memory_description", "");
    if (!memDescr.empty())
    {
      xdnnKeyVals["memory_description"] = memDescr;

      if (memDescr.find("ddr_to") != std::string::npos)
        xdnnKeyVals["memory_description_src"] = "ddr";
      else if (memDescr.find("am_to") != std::string::npos)
        xdnnKeyVals["memory_description_src"] = "am";

      if (memDescr.find("_to_ddr") != std::string::npos)
        xdnnKeyVals["memory_description_dest"] = "ddr";
      else if (memDescr.find("_to_am") != std::string::npos)
        xdnnKeyVals["memory_description_dest"] = "am";
    }

    std::string memory = layer.second.get <std::string>("type", "");
    for (auto &xdnnOp : layer.second.get_child("xdnn_kv")) {
      std::string key = xdnnOp.first;
      std::string val = xdnnOp.second.get_value<std::string>();
      xdnnKeyVals[key] = val;
      if (!key.compare("name")) name = val; // Overwrite network[i]["name"] with network[i]["xdnn_kv"]["name"] if it exits
    }

    // assign/override names to unnamed ops using opIndex
    if (name.empty() || name == "null")
    {
      name = std::to_string(opIndex) + xdnnKeyVals["XNOp"];
      xdnnKeyVals["name"] = name;
    }

    XDNNCompilerOp compilerOp(xdnnKeyVals);
    if (compilerOp.getOp() == XDNN_UNSUPPORTED)
      continue; // skip

    if ( inputSet.find(name) != inputSet.end()){
    	_compOp[name] = std::shared_ptr<XDNNInputCompilerOp>( new XDNNInputCompilerOp( xdnnKeyVals, name, inputSet[name]));
    }
    else if ( throughputInterbufSet.find(name) != throughputInterbufSet.end()) {
    	_compOp[name] = std::shared_ptr<XDNNThroughputInterbufCompilerOp>( new XDNNThroughputInterbufCompilerOp( xdnnKeyVals));
    }
    else if ( outputSet.find(name) != outputSet.end()){
    	_compOp[name] = std::shared_ptr<XDNNOutputCompilerOp>( new XDNNOutputCompilerOp( xdnnKeyVals, outputSet[name].first, outputSet[name].second));
    } else{
        _compOp[name] = std::shared_ptr<XDNNCompilerOp>( new XDNNCompilerOp( xdnnKeyVals));
    }
    if ( active.empty() || active == "1") _activeLayers.push_back(name);
    //_layerType.push_back(compilerOp.getOp());
  }
  if(XDNN_VERBOSE) std::cout << "ARK : Returning from XDNNDescriptorDB::buildParamsFromJson : " << fileName << std::endl;
}

void XDNNDescriptorDB::buildParams(const std::string & fileName) {
  if (fileName.find(".json") != std::string::npos)
    return buildParamsFromJson(fileName);

  std::cerr << "ERROR: compiler .cmd format no longer supported. Please use .json instead." << std::endl;
  exit(1);
}

////////////////////////////////////////////////////////////////////

XDNNCompilerOp::XDNNCompilerOp( const std::map<std::string, std::string> &kv) {
  _params = kv;
  _op = XDNN_UNSUPPORTED;
  for (auto it = _params.begin(); it != _params.end(); it++) {
    std::string key = it->first;
    std::string val = it->second;

    if (key.find("XN") != std::string::npos) {
      // this is an instruction key
      _op = enum_conv_func(val);
      continue;
    }
  }
  // specialize XDNN_ELTWISE to XDNN_ELTWISE_SCALE if configured as such
  if (_op == XDNN_ELTWISE 
    && ((getInt("add", 0) <= 0) && (getInt("bn", 0) > 0)))
    _op = XDNN_ELTWISE_SCALE;

  // infer missing values to standardize compiler fields across Ops
  {
    std::vector<std::pair<std::string, std::string> > inferFields; // src -> dest
    inferFields.push_back({"input_chan", "inchan"});
    inferFields.push_back({"output_chan", "outchan"});
    inferFields.push_back({"pool_inchan", "outchan"});
    inferFields.push_back({"input_w", "insize_w"});
    inferFields.push_back({"input_h", "insize_h"});
    inferFields.push_back({"output_w", "outsize_w"});
    inferFields.push_back({"output_h", "outsize_h"});

    inferFields.push_back({"insize_w", "outsize_w"});
    inferFields.push_back({"insize_h", "outsize_h"});
    inferFields.push_back({"outsize_w", "insize_w"});
    inferFields.push_back({"outsize_h", "insize_h"});
    inferFields.push_back({"inchan", "outchan"});
    inferFields.push_back({"outchan", "inchan"});

    inferFields.push_back({"uram_src", "inaddr"});
    inferFields.push_back({"uram_dest", "outaddr"});

    for (auto it = inferFields.begin(); it != inferFields.end(); it++)
      if (hasKey(it->first) && !hasKey(it->second))
        _params[it->second] = _params[it->first];
  }
}

double XDNNCompilerOp::getDouble(std::string key) const {
	assert(hasKey(key));
	std::string val = _params.at(key);
	return std::stod(val);
}

int XDNNCompilerOp::getInt(std::string key, int defaultVal) const {
  if (!hasKey(key))
    return defaultVal;

  return getInt(key);
}

int XDNNCompilerOp::getInt(std::string key) const {

  if (!hasKey(key))
  {
    cerr << "[XDNN] ERROR " << key << " does not exist in compiler.json" << std::endl;
    assert(hasKey(key));
    exit(1);
  }

  std::string val = _params.at(key);
  if (val.length() > 2 && val[0] == '0' && val[1] == 'x') {
    // convert address string to int
    int addr = -1;
    std::stringstream ss;
    ss << std::hex << val;
    ss >> addr;
    return addr;
  }

  return std::stoi(val);
}

std::string XDNNCompilerOp::getStr(std::string key, 
  const std::string &defaultVal) const {
  if (!hasKey(key) || getStr(key).empty())
    return defaultVal;

  return getStr(key);
}

std::string XDNNCompilerOp::getStr(std::string key) const {
  if (!hasKey(key))
  {
    cerr << "[XDNN] ERROR: " << key << " does not exist in compiler.json" << std::endl;
    assert(hasKey(key));
    exit(1);
  }
  return _params.at(key);
}

bool XDNNCompilerOp::hasKey(std::string key) const {
  return _params.find(key) != _params.end();
}

////////////////////////////////////////////////////////////////////

static char* allocateScratchSpace(XBLASHandle &handle, int prefCuIdx) {
  const int bytes = 256000000; 
  //Manasa: reducing scratch space needed to 256MB, because HBM ddr bank cannot allocate more than 256MB per bank currently
  static xdnn::XAlignedBlob<short> blob;

  if (blob.sizeInBytes() < bytes) {
    blob.reset(bytes / sizeof(short));
  }

  XBLASKernelConfig &kcfg = handle.m_kConfig;
  for (int ci = 0; ci < kcfg.m_ddrBankA.size(); ci++) {
    if (prefCuIdx != -1 && prefCuIdx != ci) {
      //std::cout << "xblasLoadA skip CU " << ci << std::endl;
      continue;
    }
    XComputeUnit *cu = handle.getComputeUnit(ci);
    if (cu->getScratchDdrMem())
      continue; // already has scratch space allocated

    cl_mem_ext_ptr_t scratch_ext;
#ifdef ENABLE_HBM
    //Manasa Todo: Get ddr bank from overlabyin.json file. The number 12 is hardcoded only for the particular xclbin used for bringup of HBM
    scratch_ext.flags = kcfg.getDDRMap(12);
#else
    scratch_ext.flags = kcfg.getDDRMap(kcfg.m_ddrBankA[ci]);
#endif      
    scratch_ext.obj = blob.get();
    scratch_ext.param = 0;

    const int sizeToAlloc = bytes;
#ifdef DEBUG
    xdnn::XDebugJournal().log("clCreateBuffer_allocateScratchSpace",
        std::to_string(sizeToAlloc));
#endif

    cl_mem_flags memFlags = CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX
        | CL_MEM_USE_HOST_PTR;
    cl_mem scratch_mem = clCreateBuffer(handle.context, memFlags, sizeToAlloc,
        &scratch_ext, NULL);
    assert(scratch_mem);

    cu->saveScratchDdrBase(scratch_mem);

    cl_event writeEvent;
    int err = clEnqueueMigrateMemObjects(handle.commands, 1, &scratch_mem, 0, // do migration from host
        0, NULL, &writeEvent);

    clWaitForEvents(1, &(writeEvent));
  }

  return (char *) blob.get();
}

static char* allocateScratchSpace(const std::vector<XBLASHandle*> & xblas_handles)
{
  for (int di = 0; di < xblas_handles.size(); di++) 
  {
    if (!xblas_handles[di]->m_subscribeId.empty())
      continue; // subscribers don't need to alloc; already alloced by publisher
    allocateScratchSpace(*(xblas_handles[di]), -1);
  }
}

template<typename Dtype>
XDNNScriptExecutor<Dtype>::XDNNScriptExecutor(
    const std::vector<XBLASHandle*> & xblas_handles,
    const std::map<std::string, XDNNWeightParam<Dtype> > &m,
    const std::string & xdnn_net_file, const std::string & quantize_CfgFile,
    unsigned int cu_mask)
{
  if (XDNN_VERBOSE)
    std::cout << "XDNNScriptExecutor CTor #1 cuMask " << cu_mask << std::endl;
  allocateScratchSpace(xblas_handles);
  short * wgt_ptr = XDNNLoadWeights(xblas_handles, m, xdnn_net_file,
      quantize_CfgFile, cu_mask);
  init(xblas_handles, xdnn_net_file, quantize_CfgFile, wgt_ptr, cu_mask);
  // std::cout<<"Load Weights XDNNScriptExecutor"<<std::endl;
}

template<typename Dtype>
XDNNScriptExecutor<Dtype>::XDNNScriptExecutor(
    const std::vector<XBLASHandle*> & xblas_handles,
    const std::string & weights_dir,
    const std::string & xdnn_net_file, const std::string & quantize_CfgFile,
    unsigned int cu_mask)
{
  if (XDNN_VERBOSE)
    std::cout << "XDNNScriptExecutor CTor #3 cuMask " << cu_mask << " thread_id : " << std::this_thread::get_id() << std::endl;

  m_assert( std::ifstream(weights_dir.c_str()).good() , "weights not found" );
  m_assert( std::ifstream(xdnn_net_file.c_str()).good() , "compiler .json not found");
  m_assert( std::ifstream(quantize_CfgFile.c_str()).good() , "quantizer .json not found" );

  allocateScratchSpace(xblas_handles);
  short * wgt_ptr = XDNNLoadWeights(xblas_handles, weights_dir, 
    xdnn_net_file, quantize_CfgFile, cu_mask);

  init(xblas_handles, xdnn_net_file, quantize_CfgFile, wgt_ptr, cu_mask);
  // std::cout<<"Load Weights XDNNScriptExecutor"<<std::endl;
  if (XDNN_VERBOSE)
    std::cout << "Returning from XDNNScriptExecutor CTor #3 cuMask " << cu_mask << " thread_id : " << std::this_thread::get_id() << std::endl;
}

template<typename Dtype>
void XDNNScriptExecutor<Dtype>::init(
    const std::vector<XBLASHandle*> & xblas_handles,
    const std::string & xdnn_net_file,
    const std::string & quantize_CfgFile,
    short * wgt_ptr,
    unsigned int cu_mask) {
  if(XDNN_VERBOSE) std::cout << "ARK : Starting ScriptExecutor::init" << std::endl;
  _wgt_ptr = wgt_ptr;
  _xblas_handles = xblas_handles;
  _compiler_file = xdnn_net_file;
  _quantize_cfg_file = quantize_CfgFile;
  _descDB = std::shared_ptr < XDNNDescriptorDB> (new XDNNDescriptorDB(xdnn_net_file, _quantize_cfg_file, _wgt_ptr));
  //_descs = _descDB->getDescriptors(_wgt_ptr, _quantize_cfg_file);
  _cuMask = cu_mask;
  if (_cuMask && _xblas_handles[0]) {
    int nCu = _xblas_handles[0]->getNumComputeUnits();
    unsigned int maxMask = (0x1U << nCu) - 1;
    if (_cuMask > maxMask) {
      std::cout << "ERROR: Invalid CU mask: 0x" << std::hex << _cuMask
          << ". Max CUs is " << std::dec << nCu << std::endl;
      _cuMask = 0;
    }
  }

  XDNNMaskToIndex(_cuIdxVec, _cuMask);
  _cuIdxCntr = 0;
  if (XDNN_VERBOSE) {
    std::cout << "XDNNScriptExecutor allowed CUs: ";
    for (auto c : _cuIdxVec)
      std::cout << c << " ";
    std::cout << std::endl;
  }
  _startQp = nullptr;
  _lastQp = nullptr;
  
  _bitWidth = 0;
  if ( !_quantize_cfg_file.empty()){
    std::string startQuantizeLayer, lastQuantizeLayer;
    const auto & layerNames = _descDB->getLayerNames();
    for ( const string & ln : layerNames){
      string true_layerName = _descDB->getDescriptor(ln)->_name;
      if (!_startQp){
        startQuantizeLayer = getUniqueLayerName(true_layerName);
        _startQp = XDNNLayerQuantParamDB::getLayerQuantParam( _quantize_cfg_file, startQuantizeLayer);
      }
            
      std::string qlayer = getUniqueLayerName(true_layerName);
      XDNNLayerQuantParam * qp = XDNNLayerQuantParamDB::getLayerQuantParam( _quantize_cfg_file, qlayer);
      if (qp) {
        if (_bitWidth == 0){
          _bitWidth = qp->bitWidth;
        }
        //check for consistent bitwidth throughout quantization file
        assert ( _bitWidth == qp->bitWidth);
        _quantparam[qlayer] = qp;
        _lastQp = qp;  
      }
    }
  }

    for (XBLASHandle * handle : xblas_handles ){
        XBLASKernelConfig &kcfg = handle->m_kConfig;
		if ( _cuMask == 0){
		  for (int ci  =0; ci < kcfg.m_ddrBankA.size(); ci++) {
			  XComputeUnit * cu = handle->getComputeUnit(ci);
			  loadInstructions(handle, cu);
		  }
		} else{
		  for (int ci : _cuIdxVec ){
			  XComputeUnit * cu = handle->getComputeUnit(ci);
			  loadInstructions(handle, cu);
		  }
		}
    }
  if(XDNN_VERBOSE) std::cout << "ARK : Starting ScriptExecutor::init" << std::endl;
}

template<typename Dtype>
float XDNNScriptExecutor<Dtype>::readHardwareCounter(int devIdx, int cuIdx)
{
  
  KernelInterface * ki = KernelInterfaceDB::get(_xblas_handles[devIdx]);

  const float data_clk = float(75000); 
  int counterValue = ki->readImageExecutionCounter(cuIdx, 1);
  float elapsedTime = float(counterValue) / data_clk;
  return elapsedTime;
}

template<typename Dtype>
void XDNNScriptExecutor<Dtype>::loadInstructions( XBLASHandle * handle, XComputeUnit * cu)
{
    KernelInterface * ki = KernelInterfaceDB::get(handle);
    _startIdx = ki->getInstrCounter(cu);
    _dflStartIdx = ki->getDFLInstrCounter(cu);
    XDNNInitScript(cu);
    XDNNCrossBar(cu, _descDB, _compiler_file);

    _descDB->execute (cu);

    //if (!hasExplicitInputOutputNodes)
    //  _outputDesc->execute(cu);

    _stopIdx = ki->getInstrCounter(cu) - 1;
    _dflStopIdx = ki->getDFLInstrCounter(cu) - 1;

// enable XRT
    bool isThroughput = _descDB->getThroughputInterbufDesc() == nullptr ? false:true;
    ki->enableXRT(cu, isThroughput);
    if (XDNN_VERBOSE)
    {
      printf("[XDNN] num instructions loaded: dfl=%d mb=%d\n", 
        _dflStopIdx-_dflStartIdx+1, _stopIdx-_startIdx+1);
    }
}

template<typename Dtype>
void XDNNScriptExecutor<Dtype>::execute_async( const  std::unordered_map <std::string, std::vector<const Dtype*> > input_ptrs,
		std::unordered_map< std::string, std::vector<Dtype*>> output, int streamId) {
  xdnn::XTimer t;
  const int numPEs = _xblas_handles[0]->getNumComputeUnits();
  const auto bsz = input_ptrs.begin()->second.size();
  assert ( output.begin()->second.size() == bsz);

  std::shared_ptr<XDNNQuantBuf> *qbuf = NULL;
  {
    std::unique_lock<std::mutex> lock(_mtx);
    if ( _quantizedBuffers.find( streamId ) == _quantizedBuffers.end() )
      _quantizedBuffers[streamId] = std::unordered_map<int, std::shared_ptr<XDNNQuantBuf> > ();
    if ( _quantizedBuffers[streamId].find(bsz) == _quantizedBuffers[streamId].end() )
      _quantizedBuffers[streamId][bsz] = std::shared_ptr<XDNNQuantBuf> ( new XDNNQuantBuf( bsz, _descDB->getInputSize(), _descDB->getOutputSize() ) );

    qbuf = &(_quantizedBuffers[streamId][bsz]);
  }

  for (int bi = 0; bi < bsz; bi ++) {
    std::unordered_map <std::string, const Dtype* >in_blob;
    std::unordered_map <std::string, Dtype* >out_blob;
    XTimer tXDNN, tXDNN_setup;
    //int devIdx = (bi / numPEs) % _xblas_handles.size();
    int devIdx = streamId % _xblas_handles.size();
    if (XDNN_VERBOSE)
      std::cout << "XDNNScriptExecutor::execute fpgaXdnn_setup time " << bi
          << ": " << tXDNN_setup.elapsed() * 1000 << " ms" << std::endl;

    int ret = 0;

	for (const auto &kv : input_ptrs){
		in_blob[kv.first] = (const Dtype*)kv.second[bi] ;
	}

	for (const auto &kv : output){
		out_blob[kv.first] = (Dtype*)kv.second[bi] ;
	}

	ret = fpgaXdnn(*(_xblas_handles[devIdx]), in_blob, out_blob, *qbuf,  bi, streamId);

    if (ret != 0) {
      std::cout << "ANDBG fpgaXdnn failed" << std::endl;
      exit(1);
    }
    if (XDNN_VERBOSE)
      std::cout << "XDNNScriptExecutor::execute fpgaXdnn time " << bi << ": "
          << tXDNN.elapsed() * 1000 << " ms" << std::endl;
  }
  if (XDNN_VERBOSE)
    std::cout << "XDNNScriptExecutor::execute time: " << t.elapsed() * 1000
        << " ms" << std::endl;

  if (XDNN_VERBOSE)
    std::cout << "XDNNScriptExecutor::execute total time: "
        << t.elapsed() * 1000 << " ms" << std::endl;
}

template<typename Dtype>
void XDNNScriptExecutor<Dtype>::execute(const  std::unordered_map <std::string, std::vector<const Dtype*> > & input_ptrs,
		std::unordered_map< std::string,std::vector< Dtype*>> & output, int streamId) {

    execute_async(input_ptrs, output, streamId);
    waitForResults(streamId);
}

template<typename Dtype>
void XDNNScriptExecutor<Dtype>::waitForResults(int streamId) {
  for (int di = 0; di < _xblas_handles.size(); di++)
	_xblas_handles[di]->waitForResults(streamId);
}

void XDNNMaskToIndex(std::vector<int>& cuIdxVec, unsigned int cuMask) {
  cuIdxVec.clear();
  if (!cuMask) {
    cuIdxVec.push_back(-1);
  } else {
    // use LUT if this is called often
    unsigned int mask = 0x1;
    for (int i = 0; i < sizeof(unsigned int); ++i) {
      if (cuMask & mask << i) {
        cuIdxVec.push_back(i);
      }
    }
  }
}

/****************************************
 * Quantization code
 ****************************************/

/****************************************
 * Layer param reader (JSON)
 * Reference: http://zenol.fr/blog/boost-property-tree/en.html
 ****************************************/
using boost::property_tree::ptree;
std::vector<XDNNLayerQuantParam> XDNNloadLayerQuantizationParams(
    std::string fname) {
  // TODO: skip layers that don't have quant params
  ptree pt;
  std::ifstream jsonFile(fname);
  read_json(jsonFile, pt);

  std::vector<XDNNLayerQuantParam> layerParams;
  for (auto &layer : pt.get_child("network")) {
    XDNNLayerQuantParam layerParam;
    layerParam.name = layer.second.get < std::string > ("name");
    if (layer.second.count("bw_params") != 0)
      layerParam.bitWidth = layer.second.get<int>("bw_params");
    if (layer.second.count("th_layer_in") != 0)
      layerParam.threshIn = layer.second.get<float>("th_layer_in");
    if (layer.second.count("th_layer_out") != 0)
      layerParam.threshOut = layer.second.get<float>("th_layer_out");
    if (layer.second.count("th_params") != 0)
      for (auto &e : layer.second.get_child("th_params"))
        layerParam.thresh.push_back(e.second.get_value<float>());
    if (layer.second.count("prescale_shift") != 0)
      for (auto &e : layer.second.get_child("prescale_shift"))
        layerParam.preShift.push_back(e.second.get_value<int>());
    if (layer.second.count("scale") != 0)
      for (auto &e : layer.second.get_child("scale"))
        layerParam.scale.push_back(e.second.get_value<int>());
    if (layer.second.count("postscale_shift") != 0)
      for (auto &e : layer.second.get_child("postscale_shift"))
        layerParam.postShift.push_back(e.second.get_value<int>());
    if (layer.second.count("posscale") != 0)
      for (auto &e : layer.second.get_child("posscale"))
        layerParam.posScale.push_back(e.second.get_value<int>());
    if (layer.second.count("negscale") != 0)
      for (auto &e : layer.second.get_child("negscale"))
        layerParam.negScale.push_back(e.second.get_value<int>());

    layerParams.push_back(layerParam);
  }

  return layerParams;
}

short XDNNQuantize(float in, float thresh, int bitWidth, bool doRounding) {
  const int maxRange = std::pow(2, bitWidth);
  const int halfRange = (maxRange / 2) - 1;
  const float quantFactor = halfRange / thresh;

  // clamp
  float val = std::max(std::min(in, thresh), -thresh);

  // apply quantization formula
  if (doRounding)
    val = std::round(val * quantFactor);
  else
    val = val * quantFactor;

  return short(val);
}

void XDNNQuantizeBiasV3_scale(const float *in, int *out, int sz, float threshOut,
    std::vector<int>* postShift, int bitWidth, bool doRounding) {

  // if (XDNN_VERBOSE) std::cout << LOCATION << "Bias th : " << threshIn << std::endl;

  const int maxRange = std::pow(2, bitWidth);

  const int halfRange = (maxRange / 2) - 1;

  const float sfout = threshOut / halfRange;
  const float quantFactor = halfRange / threshOut;

  float shift_effect = 0;
  float sfAcc = 0;
  float thAcc = 0;
  float maccRange = 0;
  float maccHalfRange = 0;
  for (int i = 0; i < sz; i++) {
    float shift_val = 0;
    if(postShift->size() > 0) {
        shift_val = (postShift->size() > 1 ) ? postShift->at(i) : postShift->at(0);
        shift_effect = std::pow(2, -shift_val);
    } else {
        shift_effect = 1;
    }
    sfAcc = sfout * shift_effect;
    if (bitWidth == 8) {
      maccRange = std::pow(2, 32);
      maccHalfRange = (maccRange / 2) - 1;
      thAcc = sfAcc * (maccHalfRange);
    } else if (bitWidth == 16) {
      maccRange = std::pow(2, 32);
      maccHalfRange = (maccRange / 2) - 1;
      thAcc = sfAcc * (maccHalfRange);
    }

    // clamp
    float val = std::max(std::min(in[i], thAcc), -thAcc);

    // apply quantization formula
    if (doRounding)
      val = std::round(val / sfAcc);
    else
      val = val / sfAcc;

    if (bitWidth == 8) {
      out[i] = int(val);

    } else if (bitWidth == 16) {
      if (doRounding)
        val = std::round(val / maxRange);
      else
        val = val / maxRange;

      out[i] = int(val);

    }

    //printf("bias idx = %d\t bias float = %f\t bias 32bit = %d\t threshold_out = %f\n", i, in[i], int(val), threshOut);
    // if (XDNN_VERBOSE) std::cout << LOCATION  << "bias value before/after quantization : " << in[i] << " " << out[i] << std::endl;

  }
}

void XDNNQuantizeBiasV3(const float *in, int *out, int sz, float threshIn,
    std::vector<float>* threshParams, int bitWidth, bool doRounding) {

  // if (XDNN_VERBOSE) std::cout << LOCATION << "Bias th : " << threshIn << std::endl;

  const int maxRange = std::pow(2, bitWidth);

  const int halfRange = (maxRange / 2) - 1;

  const float sfIn = threshIn / halfRange;
  const float quantFactor = halfRange / threshIn;

  float sfW = 0;
  float sfAcc = 0;
  float thAcc = 0;
  float maccRange = 0;
  float maccHalfRange = 0;
  for (int i = 0; i < sz; i++) {
    float thParam = halfRange;
    if(threshParams->size() > 0) {
        thParam = (threshParams->size() > 1 ) ? threshParams->at(i) : threshParams->at(0);
        sfW = thParam / halfRange;
    } else {
        sfW = 1;
    }
    sfAcc = sfIn * sfW;
    if (bitWidth == 8) {
      maccRange = std::pow(2, 24);
      maccHalfRange = (maccRange / 2) - 1;
      thAcc = sfAcc * (maccHalfRange);
    } else if (bitWidth == 16) {
      maccRange = std::pow(2, 48);
      maccHalfRange = (maccRange / 2) - 1;
      thAcc = sfAcc * (maccHalfRange);
    }

    // clamp
    float val = std::max(std::min(in[i], thAcc), -thAcc);

    // apply quantization formula
    if (doRounding)
      val = std::round(val / sfAcc);
    else
      val = val / sfAcc;

    if (bitWidth == 8) {
      out[i] = int(val);

    } else if (bitWidth == 16) {
      if (doRounding)
        val = std::round(val / maxRange);
      else
        val = val / maxRange;

      out[i] = int(val);
    }
    // if (XDNN_VERBOSE) std::cout << LOCATION  << "bias value before/after quantization : " << in[i] << " " << out[i] << std::endl;

  }

}

float XDNNUnQuantize(short in, float thresh, int bitWidth, int halfRange) {
  if (halfRange == 0) {
    // caller did not pass this to us, let's compute ourselves
    // caller may optionally pass halfRange to save compute cycles
    // (e.g. when in a loop)
    const int maxRange = std::pow(2, bitWidth);
    halfRange = (maxRange / 2) - 1;
  }

  // apply inverse quantization formula
  assert(bitWidth == 8 || bitWidth == 16);
  float val = in;
  if (bitWidth == 8)
    val = char(in & 0xFF);
  return val / float(halfRange) * thresh;
}

void XDNNUnQuantize(short *in, float *out, int sz, float thresh, int bitWidth) {
  const int maxRange = std::pow(2, bitWidth);
  const int halfRange = (maxRange / 2) - 1;
  //std::cout << "UNQUANT VALS: " << std::endl;
  for (int i = 0; i < sz; i++) {
    out[i] = XDNNUnQuantize(in[i], thresh, bitWidth, halfRange);
    //std::cout << out[i] << " " << in[i] << std::endl;
  }
}

void* XDNNMakeScriptExecutorAndLoadWeights(XBLASHandle **handlePtrs, int numHandles,
    char *weightsPath, char *xdnnNetFile, char *fpgaCfgFile,
   unsigned int cuMask) {
  std::vector<XBLASHandle*> handles;
  for (int i = 0; i < numHandles; i++)
    handles.push_back(handlePtrs[i]);

  return new XDNNScriptExecutor<float>(handles, weightsPath, xdnnNetFile,
      fpgaCfgFile, cuMask);
}

void* XDNNMakeScriptExecutorAndLoadWeightsFromMem(
  XBLASHandle **handlePtrs, int numHandles, const int numWeightLayers,
  const char **weightLayerNames, const float **weights, const int *weightsSz, 
  const float **bias, const int *biasSz,
  char *xdnnNetFile, char *fpgaCfgFile, 
  unsigned int cuMask)
{
  std::vector<XBLASHandle*> handles;
  for (int i = 0; i < numHandles; i++)
    handles.push_back(handlePtrs[i]);

  std::map<std::string, XDNNWeightParam<float> > m;
  for (int i=0; i < numWeightLayers; i++)
    m[weightLayerNames[i]] = {
      .wgt_ = weights[i],
      .bias_ = bias[i],
      .wgt_sz_ = size_t(weightsSz[i]),
      .bias_sz_ = size_t(biasSz[i])
    };

  return new XDNNScriptExecutor<float>(handles, m, xdnnNetFile,
      fpgaCfgFile, cuMask);
}

void XDNNExecute_2D_float(void *scriptExecutor_, const float ***fpgaInputAddr, char ** inNames, unsigned numInput,
		  	  	  	  	  	float ** output, unsigned * output_size, char ** outNames, unsigned numOutput,
							unsigned bsz, int streamId, bool blocking)
{
  XDNNScriptExecutor<float> *scriptExecutor = (XDNNScriptExecutor<float>*) scriptExecutor_;
  std::unordered_map <std::string, std::vector<const float*> >  inputPtrs;
  std::unordered_map <std::string, std::vector<float*> >  outPtrs;

  for (unsigned j = 0; j < numInput; j++){
	  for (int i = 0; i < bsz; i++){
		inputPtrs[inNames[j]].push_back( fpgaInputAddr[j][i] );
	  }
  }

  for (unsigned j = 0; j < numOutput; j++){
	  float * o_ptr = output[j];
	  for (unsigned i = 0; i < bsz; i++){
		  outPtrs[outNames[j]].push_back( o_ptr );
		  o_ptr += output_size[j];
	  }
  }

  if ( blocking )
      scriptExecutor->execute(inputPtrs, outPtrs, streamId);
  else
      scriptExecutor->execute_async(inputPtrs, outPtrs, streamId);
}

void XDNNWaitForResults(void *scriptExecutor_, int streamId) {
  if (XDNN_VERBOSE)
    std::cout << "XDNNWaitForResults start..." << std::endl;

  XDNNScriptExecutor<float> *scriptExecutor =
      (XDNNScriptExecutor<float>*) scriptExecutor_;
  scriptExecutor->waitForResults(streamId);

  if (XDNN_VERBOSE)
    std::cout << "XDNNWaitForResults done." << std::endl;
}

float XDNNReadHardwareCounter(void *scriptExecutor_,int devIdx, int cuIdx) {
  XDNNScriptExecutor<float> *scriptExecutor =
      (XDNNScriptExecutor<float>*) scriptExecutor_;
  return scriptExecutor->readHardwareCounter(devIdx,cuIdx);
}

static void prepareMemForWriteBToDeviceV3(XPipelinePacket &packet)
{
  XDeviceMemPOD<short> *devMemPOD = packet.bDevMemPOD;

  XDNNKernelConfigInfoMgr kConfMgr(
      packet.xHandle->m_kConfig.m_configRegInfo);
  const bool is8Bit = kConfMgr.get(XDNNKernelConfigInfoMgr::MODE8BIT);
  int bytesPerPixel = (is8Bit)? 1 : 2;
  unsigned insz = 0;
  for (const auto & kv : *packet.xdnnInput ){
	XDNNInputDescriptor * inDesc = kv.second.get();
	insz += inDesc->getHWSize();
  }
  devMemPOD->m_data.reset(insz);
  for (const auto & kv : *packet.xdnnInput ){
      XDNNInputDescriptor * inDesc = kv.second.get();
	  short *src= ((short*) packet.B->get()) + ( inDesc->getHWAddrOffset() / sizeof(short));
	  short *dest = ((short*) devMemPOD->m_data.get()) + (inDesc->getHWAddrOffset() / sizeof(short));
	  XDNNV3_InputOutputFormatter<char> inputSizeReq(
	    inDesc->_c, inDesc->_h, inDesc->_w,
	    8, 64, bytesPerPixel);

    if ((inDesc->_c == 3) && (inDesc->_w % 8 == 0))
      inputSizeReq.hw3c2xdnn(src, (char*)dest);
    else
      inputSizeReq.std2xdnnv3(src, (char*)dest);
  }
}

void XDNNPacketExecutor::prepBForDevice(XPipelinePacket &packet)
{
	prepareMemForWriteBToDeviceV3(packet);
}

void XDNNPacketExecutor::prepCForDevice(XPipelinePacket &packet) {
	XDeviceMemPOD<short> *devMemPOD = packet.cDevMemPOD;
	unsigned outsz = 0;
	for (const auto & kv : *packet.xdnnOutput ){
		XDNNOutputDescriptor * outDesc = kv.second.get();
		outsz += outDesc->getHWSize();
	}
	devMemPOD->m_data.reset(outsz);
}

static void unformatFpgaResultMemV3(XPipelinePacket &packet)
{
	short * Cout = (short*)packet.C->get();
	for (const auto & kv : *packet.xdnnOutput){
		XDNNOutputDescriptor * outDesc = kv.second.get();
		short * C_hw = ((short*)packet.cDevMemPOD->m_data.get()) + ( outDesc->getHWAddrOffset() / sizeof(short));
		const int cpu_size = outDesc->getSize();
		const int host_image_rows=outDesc->_h;
		const int host_image_columns=outDesc->_w;

		if(host_image_rows==host_image_columns && host_image_rows==1)
		{
			char *DDR = (char*)C_hw;
			for(int i=0;i<cpu_size;i++)
			{
				Cout[i] = (short)(DDR[i]);
			}
		}
		else
		{
			XDNNV3_InputOutputFormatter<char> outputFormatter(outDesc->_c,outDesc->_h,outDesc->_w, 8, 64, 1);
			outputFormatter.xdnnv3ImgSizeReq();
			outputFormatter.xdnnv32std((char*)C_hw, Cout);
		}
		Cout += cpu_size;
	}
}

void XDNNPacketExecutor::unpackCFromDevice(XPipelinePacket &packet)
{
  unformatFpgaResultMemV3(packet);

  if (XBLAS_EMIT_PROFILING_INFO && packet.kernelComputeUnitSlot == 0)
    XDNNUpdateHardwareCounters(&packet);

#if defined(DEBUG)
  XDNNUpdateErrorStatus(&packet);
#endif

}

void XDNNPacketExecutor::execute(XPipelinePacket &packet) const
{
  /*
   * 4. push execute command after script is loaded
   */
  XDNNScriptExecute(&packet);
}
