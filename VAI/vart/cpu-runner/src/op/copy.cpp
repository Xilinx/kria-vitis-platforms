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

#include "copy.hpp"

namespace vart {
namespace cpu {

template <typename DType>
const vector<string> Copy<DType>::ITName = {
    "input",
};

// constructor and deconstructor
template <typename DType>
Copy<DType>::Copy(const xir::Subgraph* subg, const xir::Op* op,
                          IMapTBs_t inputs, CPUTBPtr_t output)
    : CPUOPBase(subg, op, inputs, output) {
  GET_INPUT_DIMX_FMAP(fmap_i_, input, 6);
  GET_OUTPUT_DIMX_FMAP(fmap_o_, 6);
}

template <typename DType>
void Copy<DType>::run() {
  // just assign value, nothing to do else
  std::copy_n(data_in_, fmap_i_.num(), data_out_);
}

template <typename DType>
void Copy<DType>::print_param() {
  fmap_i_.print_param("fmap_i");
  fmap_o_.print_param("fmap_o");
}

template <typename DType>
void Copy<DType>::check_param() {
  UNI_LOG_CHECK(inputs_.size() == 1, VART_SIZE_ERROR);
  UNI_LOG_CHECK(fmap_i_.num() == fmap_o_.num(), VART_SIZE_ERROR);
}

// read data from DDR
template <typename DType>
void Copy<DType>::read() {
  // read img data
  auto* cputb = inputs_.at(ITName[INPUT]).at(0);
  data_in_ = GET_CPUTB_DType_PTR(DType, cputb);

  // handle output buffer
  data_out_ = GET_CPUTB_DType_PTR(DType, output_);
}

INSTANTIATE_TPCLASS(Copy);
REG_OP_INSTANCE_FUNC("identity", Copy);
REG_OP_INSTANCE_FUNC("upload", Copy);
REG_OP_INSTANCE_FUNC("download", Copy);

}  // namespace cpu
}  // namespace vart
