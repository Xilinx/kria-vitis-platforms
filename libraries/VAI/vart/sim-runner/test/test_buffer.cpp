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
#include <fcntl.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>
#include <fstream>
#include <iostream>

#include "buffer/Buffer.hpp"
#include "conf/ArchCfg.hpp"

using google::protobuf::io::FileInputStream;

int main() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  auto& arch = ArchCfg::Instance().get_param();

  auto fd = open("./config/ArchCfg.txt", ios_base::in);
  google::protobuf::io::FileInputStream fstream(fd);
  if (!google::protobuf::TextFormat::Parse(&fstream, &arch)) {
    UNI_LOG_FATAL(SIM_FILE_OPEN_FAILED) << "Failed to parse arch param.";
    abort();
  }

  //  GOOGLE_PROTOBUF_VERIFY_VERSION;
  std::cout << "Target name: " << ArchCfg::Instance().get_param().name()
            << std::endl;
  std::cout << "Target type: " << ArchCfg::Instance().get_param().type()
            << std::endl;
  std::cout << "Bank group size: "
            << ArchCfg::Instance().get_param().bank_group_size() << std::endl;
  std::cout << "Load engine: "
            << ArchCfg::Instance().get_param().load_engine().channel_parallel()
            << std::endl;
  std::cout << "Bank number: " << Buffer<DPU_DATA_TYPE>::Instance().GetBankNum()
            << std::endl;
  std::cout << "Bank depth: "
            << Buffer<DPU_DATA_TYPE>::Instance().GetBank("CB", 0)->GetH()
            << std::endl;
  std::cout << "Bank size: " << Buffer<DPU_DATA_TYPE>::Instance().GetBankSize()
            << std::endl;
}
