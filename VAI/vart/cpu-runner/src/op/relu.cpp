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

#include "relu.hpp"

namespace vart {
namespace cpu {

// constructor and deconstructor
template <typename DType>
Relu<DType>::Relu(const xir::Subgraph* subg, const xir::Op* op,
                  IMapTBs_t inputs, CPUTBPtr_t output)
    : ReluBase<DType>(subg, op, inputs, output) {}

template <typename DType>
void Relu<DType>::run() {
  // // do necessary check
  // this->print_param();
  // this->check_param();

  // // read data
  // this->read();

  // do relu
  relu();

  // // do save, debug...
  // this->save();
}

template <typename DType>
void Relu<DType>::relu() {
  copy_n(data_in_ptr_, fmap_o_.num(), data_out_ptr_);

  for (auto i = 0; i < fmap_o_.num(); i++) {
    if (data_out_ptr_[i] < 0) {
      data_out_ptr_[i] = 0;
    }
  }
}

INSTANTIATE_TPCLASS(Relu);
REG_OP_INSTANCE_FUNC("relu", Relu);

}  // namespace cpu
}  // namespace vart
