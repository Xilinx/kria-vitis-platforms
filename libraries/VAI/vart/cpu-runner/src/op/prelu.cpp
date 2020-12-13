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

#include "prelu.hpp"

namespace vart {
namespace cpu {

template <typename DType>
const vector<string> PRelu<DType>::ITName = {
    "input",
    "alpha",
};

// constructor and deconstructor
template <typename DType>
PRelu<DType>::PRelu(const xir::Subgraph* subg, const xir::Op* op,
                    IMapTBs_t inputs, CPUTBPtr_t output)
    : ReluBase<DType>(subg, op, inputs, output) {
  GET_INPUT_DIMX_FMAP(fmap_alpha_, alpha, 1);
}

template <typename DType>
void PRelu<DType>::run() {
  // // do necessary check
  // print_param();
  // check_param();

  // // read data
  // read();

  // do prelu
  prelu();

  // // do save, debug...
  // this->save();
}

template <typename DType>
void PRelu<DType>::print_param() {
  ReluBase<DType>::print_param();

  fmap_alpha_.print_param("fmap_alpha");
}

template <typename DType>
void PRelu<DType>::check_param() {
  ReluBase<DType>::check_param();

  UNI_LOG_CHECK(this->inputs_.size() == 2, VART_SIZE_ERROR);

  // check weights dim
  UNI_LOG_CHECK(fmap_i_.back() == fmap_alpha_.num(), VART_INVALID_VALUE)
      << fmap_i_.back() << " != " << fmap_alpha_.num();
}

template <typename DType>
void PRelu<DType>::read() {
  ReluBase<DType>::read();

  // read alpha data
  auto* cputb = this->inputs_.at(ITName[ALPHA]).at(0);
  alpha_ = GET_CPUTB_DType_PTR(DType, cputb);
}

template <typename DType>
void PRelu<DType>::prelu() {
  auto N = fmap_alpha_.num();

  copy_n(data_in_ptr_, fmap_o_.num(), data_out_ptr_);

  for (auto i = 0; i < fmap_o_.num(); i++) {
    if (data_out_ptr_[i] < 0) {
      data_out_ptr_[i] *= alpha_[i % N];
    }
  }
}

INSTANTIATE_TPCLASS(PRelu);
REG_OP_INSTANCE_FUNC("prelu", PRelu);

}  // namespace cpu
}  // namespace vart
