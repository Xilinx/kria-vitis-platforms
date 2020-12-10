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

#include "fix2float.hpp"

namespace vart {
namespace cpu {

Fix2Float::Fix2Float(const xir::Subgraph* subg, const xir::Op* op,
                     IMapTBs_t inputs, CPUTBPtr_t output)
    : Fix<float, int32_t>(subg, op, inputs, output) {}

void Fix2Float::run() {
  // // do necessary check
  // this->print_param();
  // this->check_param();

  // // read data
  // read();

  // do fix
  this->fix2float();

  // // do save, debug...
  // save();
}

void Fix2Float::read() {
  // read img data
  auto* cputb = inputs_.at("input").at(0);
  fix_rlt_ptr_ = GET_CPUTB_INT32_PTR(cputb);

  // handle output buffer
  data_out_ptr_ = GET_CPUTB_FLOAT_PTR(output_);
}

REG_NONTP_OP_INSTANCE_FUNC("fix2float", Fix2Float);

}  // namespace cpu
}  // namespace vart
