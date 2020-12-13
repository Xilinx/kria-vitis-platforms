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

#include "unary_neg.hpp"

namespace vart {
namespace cpu {

template <typename DType>
void UnaryNeg<DType>::calculate() {
  std::transform(data_in_ptr_, data_in_ptr_ + fmap_i_.num(), data_out_ptr_,
                 [](const auto e) { return -e; });
}

INSTANTIATE_TPCLASS(UnaryNeg);
REG_OP_INSTANCE_FUNC("neg", UnaryNeg);

}  // namespace cpu
}  // namespace vart
