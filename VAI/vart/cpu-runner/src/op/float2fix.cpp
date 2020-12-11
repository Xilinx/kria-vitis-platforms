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

#include "float2fix.hpp"

namespace vart {
namespace cpu {

Float2Fix::Float2Fix(const xir::Subgraph* subg, const xir::Op* op,
                     IMapTBs_t inputs, CPUTBPtr_t output)
    : Fix<float, int32_t>(subg, op, inputs, output) {}

void Float2Fix::run() {
  // // do necessary check
  // this->print_param();
  // this->check_param();

  // // read data
  // read();

  // do fix
  this->float2fix();

  // // do save, debug...
  // save();
}

void Float2Fix::read() {
  // read img data
  auto* cputb = inputs_.at("input").at(0);
  data_in_ptr_ = GET_CPUTB_FLOAT_PTR(cputb);

  // handle output buffer
  fix_rlt_ptr_ = GET_CPUTB_INT32_PTR(output_);
}

REG_NONTP_OP_INSTANCE_FUNC("float2fix", Float2Fix);

}  // namespace cpu
}  // namespace vart
