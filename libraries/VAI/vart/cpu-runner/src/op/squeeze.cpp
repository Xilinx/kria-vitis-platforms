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

#include "squeeze.hpp"

namespace vart {
namespace cpu {

template <typename DType>
const vector<string> Squeeze<DType>::ITName = {
    "input",
};

// constructor and deconstructor
template <typename DType>
Squeeze<DType>::Squeeze(const xir::Subgraph* subg, const xir::Op* op,
                        IMapTBs_t inputs, CPUTBPtr_t output)
    : CPUOPBase(subg, op, inputs, output) {
  GET_INPUT_DIMX_FMAP(fmap_i_, input, 4);
  GET_OUTPUT_DIMX_FMAP(fmap_o_, 4);

  // get dims info
  dims_ = xir_op_->get_attr<vector<int>>("axis");


  // construct squeeze and factor
  squeeze_flag_.resize(fmap_i_.size());
  for (auto i = 0U; i < fmap_i_.size(); i++) {
    squeeze_flag_[i] = false;
    for (auto j = 0U; j < dims_.size(); j++) {
      if ((int)i == dims_[j]) {
        squeeze_flag_[i] = true;
        break;
      }
    }
  }

  // print param value, used to debug
  print_param();
}

template <typename DType>
void Squeeze<DType>::run() {
  // // do necessary check
  // check_param();

  // // read data
  // read();

  // do squeeze
  squeeze();

  // // do save, debug...
  // save();
}

template <typename DType>
void Squeeze<DType>::print_param() {
  UNI_LOG_DEBUG_INFO << "fmap_i: " << Vec2Str(fmap_i_, ", ") << endl;
  UNI_LOG_DEBUG_INFO << "fmap_o: " << Vec2Str(fmap_o_, ", ") << endl;
  UNI_LOG_DEBUG_INFO << "squeeze_flag: " << Vec2Str(squeeze_flag_, ", ")
                     << endl;

  UNI_LOG_DEBUG_INFO << "dims: " << Vec2Str(dims_, ", ") << endl;
}

template <typename DType>
void Squeeze<DType>::check_param() {
  UNI_LOG_CHECK(inputs_.size() == 1, VART_SIZE_ERROR);

  // check dims validation
  UNI_LOG_CHECK(dims_.size() <= fmap_i_.size(), VART_SIZE_ERROR)
      << ", " << dims_.size() << " > " << fmap_i_.size();
  for (auto i = 0U; i < dims_.size(); i++) {
    UNI_LOG_CHECK(dims_[i] < (int)fmap_i_.size(), VART_DIM_ERROR)
        << ", " << dims_[i] << " >= " << (int)fmap_i_.size();
  }

  // check fmap_o_'s validation
  vector<int> fmap_i_tmp;
  for (auto i = 0U; i < fmap_i_.size(); i++) {
    if (!squeeze_flag_[i]) {
      fmap_i_tmp.push_back(fmap_i_[i]);
    }
  }

  UNI_LOG_CHECK(fmap_i_tmp.size() == fmap_o_.size(), VART_SIZE_ERROR)
      << ", " << fmap_i_tmp.size() << " != " << fmap_o_.size();
  for (auto i = 0U; i < fmap_i_tmp.size(); i++) {
    UNI_LOG_CHECK(fmap_i_tmp[i] == fmap_o_[i], VART_INVALID_VALUE)
        << ", " << fmap_i_tmp[i] << " != " << fmap_o_[i];
  }

  // check fmap_i_ and fmap_o_'s size
  UNI_LOG_CHECK(get_vec_mul(fmap_i_) == get_vec_mul(fmap_o_),
                VART_INVALID_VALUE)
      << ", " << get_vec_mul(fmap_i_) << " != " << get_vec_mul(fmap_o_);
}

// read data from DDR
template <typename DType>
void Squeeze<DType>::read() {
  // read img data
  auto* cputb = inputs_.at(ITName[INPUT]).at(0);
  img_ = GET_CPUTB_DType_PTR(DType, cputb);

  // handle output buffer
  rlt_ = GET_CPUTB_DType_PTR(DType, output_);
}

template <typename DType>
uint64_t Squeeze<DType>::get_workload() {
  // TODO
  return 0;
}

template <typename DType>
void Squeeze<DType>::squeeze() {
  // just assign value, nothing to do else
  for (auto i = 0; i < get_vec_mul(fmap_i_); i++) {
    rlt_[i] = img_[i];
  }
}

INSTANTIATE_TPCLASS(Squeeze);
REG_OP_INSTANCE_FUNC("squeeze", Squeeze);

}  // namespace cpu
}  // namespace vart
