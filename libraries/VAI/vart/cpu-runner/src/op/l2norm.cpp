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

#include "l2norm.hpp"

namespace vart {
namespace cpu {

#define SUBSTITUTE_FOR_0 0.000000000001

template <typename DType>
const vector<string> L2Norm<DType>::ITName = {
    "input",
};

// constructor and deconstructor
template <typename DType>
L2Norm<DType>::L2Norm(const xir::Subgraph* subg, const xir::Op* op,
                              IMapTBs_t inputs, CPUTBPtr_t output)
    : CPUOPBase(subg, op, inputs, output) {
  GET_INPUT_DIMX_FMAP(fmap_i_, input, 4);
  GET_OUTPUT_DIMX_FMAP(fmap_o_, 4);

  // get dims info
  axis_ = xir_op_->get_attr<vector<int>>("axis");
  for (auto i = 0U; i < axis_.size(); i++) {
    if (axis_[i] < 0) axis_[i] += fmap_i_.ndims();
  }

  // construct mean_flag and factor
  fmap_o_ = fmap_i_;
}

template <typename DType>
void L2Norm<DType>::run() {
  // caculate
  for (auto k = 0U; k < axis_.size(); k++) {
    auto dim = axis_[k];
    auto cur_dim = fmap_i_.dim(dim);
    auto cur_cod = fmap_i_.cod(dim);
    auto chunk_size = cur_dim * cur_cod;
    auto n_chunk = fmap_i_.num() / chunk_size;

    for (auto i = 0; i < n_chunk; i++) {
      DType* prlt = data_in_buf_ptr_ + i * cur_dim * cur_cod;
      // init: first element's square
      for (auto k = 0; k < cur_cod; k++) {
        prlt[k] *= prlt[k];
      }

      // accumulate
      for (auto j = 1; j < cur_dim; j++) {
        DType* pcur = prlt + j * cur_cod;

        for (auto k = 0; k < cur_cod; k++) {
          prlt[k] += pcur[k] * pcur[k];
        }
      }
    }
  }

  // copy calculation result from data_in_buf_ptr_ into data_out_ptr_
  for (auto pos = 0; pos < fmap_o_.num(); pos++) {
    auto coord = fmap_o_.pos2coord(pos);
    for (auto k = 0U; k < axis_.size(); k++) {
      auto dim = axis_[k];
      coord[dim] = 0;
    }
    auto src_pos = fmap_i_.coord2pos(coord);

    if (data_in_buf_ptr_[src_pos] < SUBSTITUTE_FOR_0) {
      data_out_ptr_[pos] = data_in_ptr_[pos] / sqrt(SUBSTITUTE_FOR_0);
    } else {
      data_out_ptr_[pos] = data_in_ptr_[pos] / sqrt(data_in_buf_ptr_[src_pos]);
    }
  }
}

template <typename DType>
void L2Norm<DType>::print_param() {
  fmap_i_.print_param("fmap_i");
  fmap_o_.print_param("fmap_o");

  UNI_LOG_DEBUG_INFO << "axis: " << Vec2Str(axis_, ", ") << endl;
}

template <typename DType>
void L2Norm<DType>::check_param() {
  UNI_LOG_CHECK(inputs_.size() == 1, VART_SIZE_ERROR);

  UNI_LOG_CHECK((int)axis_.size() <= fmap_i_.ndims(), VART_INVALID_VALUE)
      << ", " << axis_.size() << " > " << fmap_i_.ndims();
  for (auto i = 0U; i < axis_.size(); i++) {
    UNI_LOG_CHECK(axis_[i] < (int)fmap_i_.ndims(), VART_DIM_ERROR)
        << ", " << axis_[i] << " >= " << (int)fmap_i_.ndims();
  }

  // TODO: Check fmap_o_ dimention's validation
}

// read data from DDR
template <typename DType>
void L2Norm<DType>::read() {
  auto* cputb = inputs_.at(ITName[INPUT]).at(0);
  data_in_ptr_ = GET_CPUTB_DType_PTR(DType, cputb);
  data_in_buf_.resize(fmap_i_.num());
  data_in_buf_ptr_ = data_in_buf_.data();
  std::copy_n(data_in_ptr_, fmap_i_.num(), data_in_buf_ptr_);

  // handle output buffer
  data_out_ptr_ = GET_CPUTB_DType_PTR(DType, output_);
}

template <typename DType>
uint64_t L2Norm<DType>::get_workload() {
  return 0;
}

INSTANTIATE_TPCLASS(L2Norm);
REG_OP_INSTANCE_FUNC("l2_normalize", L2Norm);

}  // namespace cpu
}  // namespace vart
