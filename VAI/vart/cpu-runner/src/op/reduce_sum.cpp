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

#include "reduce_sum.hpp"

namespace vart {
namespace cpu {

template <typename DType>
void ReduceSum<DType>::calculate() {
  // caculate
  for (auto idx : reduce_dims_) {
    auto cur_dim = fmap_i_.dim(idx);
    auto cur_cod = fmap_i_.cod(idx);
    auto n_cur_chunk = fmap_i_.num() / (cur_dim * cur_cod);

    for (auto i = 0; i < n_cur_chunk; i++) {
      DType* prlt = data_in_ptr_ + i * cur_dim * cur_cod;

      for (auto j = 1; j < cur_dim; j++) {
        DType* pcur = prlt + j * cur_cod;

        // accumulate
        for (auto k = 0; k < cur_cod; k++) {
          prlt[k] += pcur[k];
        }
      }
    }
  }
}

INSTANTIATE_TPCLASS(ReduceSum);
REG_OP_INSTANCE_FUNC("reduction_sum", ReduceSum);

}  // namespace cpu
}  // namespace vart
