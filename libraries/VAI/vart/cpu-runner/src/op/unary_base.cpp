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

#include "unary_base.hpp"

namespace vart {
namespace cpu {

template <typename DType>
const vector<string> UnaryBase<DType>::UnaryTypeName = {
    "exp",      //
    "unknown",  //
};

template <typename DType>
const vector<string> UnaryBase<DType>::ITName = {
    "input",
};

// constructor and deconstructor
template <typename DType>
UnaryBase<DType>::UnaryBase(const xir::Subgraph* subg, const xir::Op* op,
                            IMapTBs_t inputs, CPUTBPtr_t output)
    : CPUOPBase(subg, op, inputs, output) {
  GET_INPUT_DIMX_FMAP(fmap_i_, input, 4);
  GET_OUTPUT_DIMX_FMAP(fmap_o_, 4);
}

template <typename DType>
void UnaryBase<DType>::run() {
  calculate();
}

template <typename DType>
void UnaryBase<DType>::print_param() {
  fmap_i_.print_param("fmap_i");
  fmap_o_.print_param("fmap_o");
}

template <typename DType>
void UnaryBase<DType>::check_param() {
  UNI_LOG_CHECK(inputs_.size() == 1, VART_SIZE_ERROR);
  UNI_LOG_CHECK(fmap_i_ == fmap_o_, VART_SIZE_ERROR);
}

// read data from DDR
template <typename DType>
void UnaryBase<DType>::read() {
  auto* cputb = inputs_.at(ITName[INPUT]).at(0);
  data_in_ptr_ = GET_CPUTB_DType_PTR(DType, cputb);

  // handle output buffer
  data_out_ptr_ = GET_CPUTB_DType_PTR(DType, output_);
}

template <typename DType>
uint64_t UnaryBase<DType>::get_workload() {
  return fmap_o_.num();
}

INSTANTIATE_TPCLASS(UnaryBase);

}  // namespace cpu
}  // namespace vart
