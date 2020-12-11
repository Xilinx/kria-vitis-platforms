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

#include "binary_base.hpp"

namespace vart {
namespace cpu {

template <typename DType>
const vector<string> BinaryBase<DType>::BinaryTypeName = {
    "add",      //
    "sub",      //
    "mul",      //
    "div",      //
    "unknown",  //
};

template <typename DType>
const vector<string> BinaryBase<DType>::ITName = {
    "input",  //
};

// constructor and deconstructor
template <typename DType>
BinaryBase<DType>::BinaryBase(const xir::Subgraph* subg, const xir::Op* op,
                            IMapTBs_t inputs, CPUTBPtr_t output)
    : CPUOPBase(subg, op, inputs, output) {
  GET_INPUT_DIMX_FMAPS(fmap_i_, input, 4);
  GET_OUTPUT_DIMX_FMAP(fmap_o_, 4);
}

template <typename DType>
void BinaryBase<DType>::run() {
  calculate();
}

template <typename DType>
void BinaryBase<DType>::print_param() {
  UNI_LOG_DEBUG_INFO << "binary_type: " << BinaryTypeName[binary_type_] << endl;
  fmap_i_[0].print_param("fmap_i0");
  fmap_i_[1].print_param("fmap_i1");
  fmap_o_.print_param("fmap_o");
}

template <typename DType>
void BinaryBase<DType>::check_param() {
  UNI_LOG_CHECK(inputs_.at(ITName[INPUT]).size() == 2, VART_SIZE_ERROR);
}

// read data from DDR
template <typename DType>
void BinaryBase<DType>::read() {
  // align input dimension
  auto vi0 = fmap_i_[0].vdims();
  while ((int)vi0.size() != fmap_o_.ndims()) {
    vi0.insert(vi0.begin(), 1);
  }
  fmap_i_[0] = Dimension(vi0);

  auto vi1 = fmap_i_[1].vdims();
  while ((int)vi1.size() != fmap_o_.ndims()) {
    vi1.insert(vi1.begin(), 1);
  }
  fmap_i_[1] = Dimension(vi1);

  // assign input buffer
  data_in_ptr_.resize(fmap_i_.size());
  for (auto idx = 0U; idx < fmap_i_.size(); idx++) {
    auto* cputb = inputs_.at(ITName[INPUT]).at(idx);
    data_in_ptr_[idx] = GET_CPUTB_DType_PTR(DType, cputb);
  }

  // handle output buffer
  data_out_ptr_ = GET_CPUTB_DType_PTR(DType, output_);
}

template <typename DType>
uint64_t BinaryBase<DType>::get_workload() {
  return fmap_o_.num();
}

INSTANTIATE_TPCLASS(BinaryBase);

}  // namespace cpu
}  // namespace vart
