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

#include "unary_tanh.hpp"

namespace vart {
namespace cpu {

template <typename DType>
void UnaryTanh<DType>::calculate() {
  for (auto i = 0; i < fmap_o_.num(); i++) {
    auto exp2x = std::exp(2 * data_in_ptr_[i]);
    data_out_ptr_[i] = (exp2x - 1.0) / (exp2x + 1.0);
  }
}

INSTANTIATE_TPCLASS(UnaryTanh);
REG_OP_INSTANCE_FUNC("tanh", UnaryTanh);

}  // namespace cpu
}  // namespace vart
