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

#include "unary_exp.hpp"

namespace vart {
namespace cpu {

template <typename DType>
void UnaryExp<DType>::calculate() {
  for (auto i = 0; i < fmap_o_.num(); i++) {
    data_out_ptr_[i] = std::exp(data_in_ptr_[i]);
  }
}

INSTANTIATE_TPCLASS(UnaryExp);
REG_OP_INSTANCE_FUNC("exp", UnaryExp);

}  // namespace cpu
}  // namespace vart
