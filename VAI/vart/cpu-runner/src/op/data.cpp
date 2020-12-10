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

#include "data.hpp"

namespace vart {
namespace cpu {

// constructor and deconstructor
template <typename DType>
Data<DType>::Data(const xir::Subgraph* subg, const xir::Op* op,
                  IMapTBs_t inputs, CPUTBPtr_t output)
    : CPUOPBase(subg, op, inputs, output) {
  // get output tensor's fmap
  GET_OUTPUT_DIMX_FMAP(fmap_o_, 4);
}

// public funcs
template <typename DType>
void Data<DType>::print_param() {
  fmap_o_.print_param("fmap_o");
}

template <typename DType>
void Data<DType>::run() {
  // // do necessary check
  // print_param();
  // check_param();

  // // read data
  // read();

  // // do save, debug...
  // save();
}

// private funcs
template <typename DType>
void Data<DType>::check_param() {
  UNI_LOG_CHECK(inputs_.size() == 0, VART_SIZE_ERROR);
}

template <typename DType>
void Data<DType>::read() {
  // handle output buffer
  rlt_ = GET_CPUTB_DType_PTR(DType, output_);
}

template <typename DType>
uint64_t Data<DType>::get_workload() {
  // TODO
  return 0;
}

INSTANTIATE_TPCLASS(Data);
REG_OP_INSTANCE_FUNC("data", Data);
REG_OP_INSTANCE_FUNC("data-fix", Data);

}  // namespace cpu
}  // namespace vart
