/*
 * Copyright 2019 Xilinx Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <fstream>
#include <iostream>
#include <random>
#include "SimCfg.hpp"
#include "UniLog/UniLog.hpp"
#include "buffer/DDR.hpp"
#include "sim_runner.hpp"
#include "sim_tensor_buffer.hpp"
#include "vart/runner.hpp"
#include "xir/graph/graph.hpp"
#include "xir/graph/subgraph.hpp"

#include <fcntl.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>

#include "conf/ArchCfg.hpp"
#include "util/Util.hpp"

using google::protobuf::io::FileInputStream;

using namespace std;

int main(int argc, char* argv[]) {
  UniLog::Initial(argv[0], UNI_LOG_FILE, UNI_LOG_LEVEL_INFO,
                  UNI_LOG_STD_LEVEL_INFO);

  string log_dir = "./unilog";
  Util::ChkFolder(log_dir);
  UniLog::setUniLogPath(log_dir);

  // init arch config
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  auto& arch = ArchCfg::Instance().get_param();
  auto fd = open("./config/ArchCfg.txt", ios_base::in);
  google::protobuf::io::FileInputStream fstream(fd);
  if (!google::protobuf::TextFormat::Parse(&fstream, &arch)) {
    UNI_LOG_FATAL(SIM_FILE_OPEN_FAILED) << "Failed to parse arch param.";
    abort();
  }

  vart::sim::SimRunner runner;
  runner.run_ac();

  return 0;
}
