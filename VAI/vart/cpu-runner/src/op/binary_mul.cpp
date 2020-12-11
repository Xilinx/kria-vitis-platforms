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

#include "binary_mul.hpp"

namespace vart {
namespace cpu {

template <typename DType>
void BinaryMul<DType>::calculate() {
  for(auto i=0; i<fmap_o_.num(); i++) {
    auto dst_coord = fmap_o_.pos2coord(i);
    auto src_coord0 = dst_coord;
    auto src_coord1 = dst_coord;

    for (auto j = 0U; j < src_coord0.size(); j++) {
      if(src_coord0[j] >= fmap_i_[0][j]) {
        UNI_LOG_CHECK(fmap_i_[0][j] == 1, VART_INVALID_VALUE);
        src_coord0[j] = 0;
      }
    }

    for (auto j = 0U; j < src_coord1.size(); j++) {
      if(src_coord1[j] >= fmap_i_[1][j]) {
        UNI_LOG_CHECK(fmap_i_[1][j] == 1, VART_INVALID_VALUE);
        src_coord1[j] = 0;
      }
    }

    auto pos0 = fmap_i_[0].coord2pos(src_coord0);
    auto pos1 = fmap_i_[1].coord2pos(src_coord1);

    data_out_ptr_[i] = data_in_ptr_[0][pos0] * data_in_ptr_[1][pos1];
  }
}

INSTANTIATE_TPCLASS(BinaryMul);
REG_OP_INSTANCE_FUNC("binary_mul", BinaryMul);

}  // namespace cpu
}  // namespace vart
