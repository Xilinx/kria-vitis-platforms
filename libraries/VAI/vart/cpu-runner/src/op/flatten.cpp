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

#include "flatten.hpp"

namespace vart {
namespace cpu {

template <typename DType>
const vector<string> Flatten<DType>::ITName = {
    "input",
};

// constructor and deconstructor
template <typename DType>
Flatten<DType>::Flatten(const xir::Subgraph* subg, const xir::Op* op,
                        IMapTBs_t inputs, CPUTBPtr_t output)
    : CPUOPBase(subg, op, inputs, output) {
  GET_INPUT_DIMX_FMAP(fmap_i_, input, 4);
  GET_OUTPUT_DIMX_FMAP(fmap_o_, 4);

  // get start and end dim
  start_dim_ = xir_op_->get_attr<int>("start_axis");
  if (start_dim_ < 0) start_dim_ += fmap_i_.size();
  end_dim_ = xir_op_->get_attr<int>("end_axis");
  if (end_dim_ < 0) end_dim_ += fmap_i_.size();
}

template <typename DType>
void Flatten<DType>::run() {
  // // do necessary check
  // print_param();
  // check_param();

  // // read data
  // read();

  // do flatten
  flatten();

  // // do save, debug...
  // save();
}

template <typename DType>
void Flatten<DType>::print_param() {
  UNI_LOG_DEBUG_INFO << "fmap_i: " << Vec2Str(fmap_i_, ", ") << endl;
  UNI_LOG_DEBUG_INFO << "fmap_o: " << Vec2Str(fmap_o_, ", ") << endl;
  UNI_LOG_DEBUG_INFO << "start_dim: " << start_dim_ << endl;
  UNI_LOG_DEBUG_INFO << "end_dim: " << end_dim_ << endl;
}

template <typename DType>
void Flatten<DType>::check_param() {
  UNI_LOG_CHECK(inputs_.size() == 1, VART_SIZE_ERROR);

  // check start and end dim range
  UNI_LOG_CHECK(start_dim_ >= 0, VART_DIM_ERROR)
      << ", " << start_dim_ << " < 0";
  UNI_LOG_CHECK(end_dim_ < (int)fmap_i_.size(), VART_DIM_ERROR)
      << ", " << end_dim_ << " >= " << fmap_i_.size();

  // check fmap_o_'s validation
  vector<int> fmap_tmp;
  for (auto i = 0; i < (int)fmap_i_.size();) {
    if (i < start_dim_ || i > end_dim_) {
      fmap_tmp.push_back(fmap_i_[i++]);
      continue;
    } else {
      auto factor = 1;
      for (auto j = i; j <= end_dim_; j++) {
        factor *= fmap_i_[j];
      }
      fmap_tmp.push_back(factor);
      i = end_dim_ + 1;
      continue;
    }
  }

  UNI_LOG_CHECK(fmap_tmp.size() == fmap_o_.size(), VART_SIZE_ERROR)
      << ", " << fmap_tmp.size() << " != " << fmap_o_.size();
  for (auto i = 0U; i < fmap_tmp.size(); i++) {
    UNI_LOG_CHECK(fmap_tmp[i] == fmap_o_[i], VART_INVALID_VALUE)
        << ", " << fmap_tmp[i] << " != " << fmap_o_[i];
  }

  // check fmap_i_ and fmap_o_'s size
  UNI_LOG_CHECK(get_vec_mul(fmap_i_) == get_vec_mul(fmap_o_),
                VART_INVALID_VALUE)
      << ", " << get_vec_mul(fmap_i_) << " != " << get_vec_mul(fmap_o_);
}

// read data from DDR
template <typename DType>
void Flatten<DType>::read() {
  // read img data
  auto* cputb = inputs_.at(ITName[INPUT]).at(0);
  data_in_ = GET_CPUTB_DType_PTR(DType, cputb);

  // handle output buffer
  data_out_ = GET_CPUTB_DType_PTR(DType, output_);
}

template <typename DType>
uint64_t Flatten<DType>::get_workload() {
  // TODO
  return 0;
}

template <typename DType>
void Flatten<DType>::flatten() {
  // just assign value, nothing to do else
  std::copy_n(data_in_, get_vec_mul(fmap_i_), data_out_);
}

INSTANTIATE_TPCLASS(Flatten);
REG_OP_INSTANCE_FUNC("flatten", Flatten);

}  // namespace cpu
}  // namespace vart
