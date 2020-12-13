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

#include "reduce_base.hpp"

namespace vart {
namespace cpu {

template <typename DType>
const vector<string> ReduceBase<DType>::ReduceTypeName = {
    "max",      //
    "min",      //
    "sum",      //
    "prod",     //
    "mean",     //
    "unknown",  //
};

template <typename DType>
const vector<string> ReduceBase<DType>::ITName = {
    "input",
};

// constructor and deconstructor
template <typename DType>
ReduceBase<DType>::ReduceBase(const xir::Subgraph* subg, const xir::Op* op,
                              IMapTBs_t inputs, CPUTBPtr_t output)
    : CPUOPBase(subg, op, inputs, output) {
  GET_INPUT_DIMX_FMAP(fmap_i_, input, 4);
  GET_OUTPUT_DIMX_FMAP(fmap_o_, 4);

  // get dims info
  reduce_dims_ = xir_op_->get_attr<vector<int>>("axis");
  keep_dim_flag_ = xir_op_->get_attr<bool>("keep_dims");
  for (auto i = 0U; i < reduce_dims_.size(); i++) {
    if (reduce_dims_[i] < 0) reduce_dims_[i] += fmap_i_.ndims();
  }

  // construct mean_flag and factor
  fmap_o_ = fmap_i_;
  reduce_flag_.resize(fmap_i_.ndims(), false);
  for (auto idx : reduce_dims_) {
    reduce_flag_[idx] = true;
    fmap_o_.set_dim(idx, 1);
  }
}

template <typename DType>
void ReduceBase<DType>::run() {
  // Use this virtual func to dispatch sub-class's
  // calculation implementation routine.
  calculate();

  // copy calculation result from data_in_ptr_ into data_out_ptr_
  for (auto pos = 0; pos < fmap_o_.num(); pos++) {
    auto coord = fmap_o_.pos2coord(pos);
    auto src_pos = fmap_i_.coord2pos(coord);

    data_out_ptr_[pos] = data_in_ptr_[src_pos];
  }
}

template <typename DType>
void ReduceBase<DType>::print_param() {
  UNI_LOG_DEBUG_INFO << "reduce_type: " << ReduceTypeName[reduce_type_] << endl;
  fmap_i_.print_param("fmap_i");
  fmap_o_.print_param("fmap_o");

  UNI_LOG_DEBUG_INFO << "reduce_flag: " << Vec2Str(reduce_flag_, ", ") << endl;
  UNI_LOG_DEBUG_INFO << "reduce_dims: " << Vec2Str(reduce_dims_, ", ") << endl;
  UNI_LOG_DEBUG_INFO << "keep_dim_flag: " << Bool2Str(keep_dim_flag_) << endl;
}

template <typename DType>
void ReduceBase<DType>::check_param() {
  UNI_LOG_CHECK(inputs_.size() == 1, VART_SIZE_ERROR);

  UNI_LOG_CHECK((int)reduce_dims_.size() <= fmap_i_.ndims(), VART_INVALID_VALUE)
      << ", " << reduce_dims_.size() << " > " << fmap_i_.ndims();
  for (auto i = 0U; i < reduce_dims_.size(); i++) {
    UNI_LOG_CHECK(reduce_dims_[i] < (int)fmap_i_.ndims(), VART_DIM_ERROR)
        << ", " << reduce_dims_[i] << " >= " << (int)fmap_i_.ndims();
  }

  // TODO: Check fmap_o_ dimention's validation
}

// read data from DDR
template <typename DType>
void ReduceBase<DType>::read() {
  auto* cputb = inputs_.at(ITName[INPUT]).at(0);
  auto* origin_in_ptr_ = GET_CPUTB_DType_PTR(DType, cputb);
  data_in_buf_.resize(fmap_i_.num());
  data_in_ptr_ = data_in_buf_.data();
  std::copy_n(origin_in_ptr_, fmap_i_.num(), data_in_ptr_);

  // handle output buffer
  data_out_ptr_ = GET_CPUTB_DType_PTR(DType, output_);
}

template <typename DType>
uint64_t ReduceBase<DType>::get_workload() {
  return 0;
}

INSTANTIATE_TPCLASS(ReduceBase);

}  // namespace cpu
}  // namespace vart
