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

#include "cpu_reg_func.hpp"
#include "cpu_op_base.hpp"

namespace vart {
namespace cpu {

CPURegFunc::CPURegFunc() {}

CPURegFunc& CPURegFunc::register_func(const string& op_type, RegFuncType regfunc) {
  UNI_LOG_CHECK(regfunc_.find(op_type) == regfunc_.end(), VART_FOUND)
      << ", Found " << op_type;

  auto func_name = "instance_" + op_type;
  if (VART_DEBUG) {
    UNI_LOG_DEBUG_INFO << "Registering OP " << op_type << "'s instance func"
                       << endl;
  }

  regfunc_[op_type] = regfunc;

  return *this;
}

CPURegFunc::RegFuncType CPURegFunc::get_register_func(const string& op_type) {
  UNI_LOG_CHECK(regfunc_.find(op_type) != regfunc_.end(), VART_NOT_FOUND)
      << ", Err: Not found " << op_type << " register func!";

  return regfunc_.at(op_type);
}

void CPURegFunc::show_regmap() {
  UNI_LOG_DEBUG_INFO << "Instance OP func size is " << regfunc_.size()
                     << ", contents:" << endl;
  UNI_LOG_DEBUG_INFO << "-------------------------------------"
                     << "-------------------------------------" << endl;

  for (const auto& e : regfunc_) {
    UNI_LOG_DEBUG_INFO << e.first << endl;
  }
}

}  // namespace cpu
}  // namespace vart
