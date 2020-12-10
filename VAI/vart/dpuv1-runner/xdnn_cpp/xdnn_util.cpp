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
#include <codecvt>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <dirent.h>
#include <iomanip>
#include <functional>
#include <locale>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

#include <boost/crc.hpp>  // for boost::crc_32_type

#include "xdnn_env.h"
#include "xdnn_util.h"

#include <CL/opencl.h>

namespace xdnn {

#define DEVICE_NAME_DELIMETER "_"
#define MAX_ITEM_ARRAY 256
#define MAX_CHAR_ARRAY 2048
#define XILINX_PLATFORM_ID 1000
#define XILINX_VENDOR_NAME "Xilinx"
#define XDBG_JOURNAL_FILE "xdnn_debug_journal.txt"

int XDebugJournal::call_counter = -1;

XDebugJournal::XDebugJournal() {
  if (XDebugJournal::call_counter < 0) {
    std::ofstream ofs;
    ofs.open(XDBG_JOURNAL_FILE, std::ofstream::out | std::ofstream::trunc);
    XDebugJournal::call_counter = 0;
  }
}

void XDebugJournal::log(std::string key, std::string entry) {
  std::ofstream ofs;
  ofs.open(XDBG_JOURNAL_FILE, std::ofstream::out | std::ofstream::app);

  auto now = std::chrono::system_clock::now();
  std::time_t now_c = std::chrono::system_clock::to_time_t(now);

  ofs //<< "[" << XDebugJournal::call_counter++ << "] "
  //<< std::chrono::duration_cast<std::chrono::seconds>(
  //    now.time_since_epoch()).count()
      << key.c_str() << " " << entry.c_str() << std::endl;
}

template<class T>
void XDebugJournal::logBlob(std::string key, T* blob, int count,
    int numToPrint) {
  log("blob " + key + "_" +  std::to_string(numToPrint), "begin");

  {
    std::ofstream ofs;
    ofs.open(XDBG_JOURNAL_FILE, std::ofstream::out | std::ofstream::app);
    T *ptr = blob;
    numToPrint = std::min(numToPrint, count);
    for (int p = 0; p < numToPrint; p++)
      if (ptr[p] != 0)
        ofs << "   " << p << " " << ptr[p] << " " << HEX(ptr[p]) << std::endl;
  }

  boost::crc_32_type crc;
  crc.process_bytes(blob, sizeof(T) * count);
  std::stringstream ss;
  ss << "end (crc:" << crc.checksum() << ")";

  log("blob " + key, ss.str());
}

void XDebugJournal::logRegWrite(std::string key, int cuIdx, size_t offset,
    size_t size, const void *data) {
  std::stringstream ss;
  ss << cuIdx << " 0x";
  ss << std::hex << offset << std::dec << " ";
  if (size == 4)
    ss << "0x" << std::hex << *(int*) data << std::dec;
  else
    ss << "0x" << std::hex << *(long long*) data << std::dec;

  log("reg_write " + key, ss.str());
}
void XDebugJournal::logRegRead(std::string key, int cuIdx, size_t offset,
    size_t size, const void *data) {
  std::stringstream ss;
  ss << cuIdx << " 0x";
  ss << std::hex << offset << std::dec << " ";
  if (size == 4)
    ss << "0x" << std::hex << *(int*) data << std::dec;
  else
    ss << "0x" << std::hex << *(long long*) data << std::dec;

  log("reg_read " + key, ss.str());
}

template void XDebugJournal::logBlob<short>(std::string key, short *blob,
    int count, int numToPrint);

#ifdef ZMQ
XStream::XStream() {
	_zcontext = new zmq::context_t(1);
	_zsocket = new zmq::socket_t(*_zcontext, ZMQ_PUB);
  _zsocket->connect("tcp://127.0.0.1:" + std::to_string(XSTREAM_PORT));
}
XStream::~XStream() {
  delete _zsocket;
  delete _zcontext;
}
void XStream::send(const std::string &channel, const std::string &msg) {
  zmq::message_t zchan(channel.size());
  zmq::message_t zmsg(msg.size());

  memcpy((void*)zchan.data(), channel.c_str(), channel.size());
  memcpy((void*)zmsg.data(), msg.c_str(), msg.size());

  _zsocket->send(zchan, ZMQ_SNDMORE); 
  _zsocket->send(zmsg);
}

void XStream::sendDataToServer(const std::string &topic, const std::map<std::string, float> &data) {
  const std::string channel = "__server__";
  std::stringstream msg;
  msg << "{ \"topic\": \"" << topic << "\", \"message\": {";
  for (auto it = data.begin(); it != data.end(); it++)
  {
    if (it != data.begin())
      msg << ",";
    msg << "\"" << it->first << "\": \"" << std::fixed << it->second << "\"";
  }
  msg << "}}";

  send(channel, msg.str());
}
#endif

} // namespace xdnn
