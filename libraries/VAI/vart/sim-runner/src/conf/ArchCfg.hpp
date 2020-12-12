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
#pragma once

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>
#include <fstream>
#include <string>

#include "UniLog/UniLog.hpp"
#include "vitis/ai/target.pb.h"

using google::protobuf::io::FileInputStream;

class ArchCfg {
 public:
  static ArchCfg& Instance() {
    static ArchCfg param;
    return param;
  }

  vitis::ai::Target& get_param() { return arch_param_; }

 private:
  ArchCfg() = default;
  ~ArchCfg() = default;

 private:
  vitis::ai::Target arch_param_;
};
// Target::ArchParam ArchCfg::arch_param_;
