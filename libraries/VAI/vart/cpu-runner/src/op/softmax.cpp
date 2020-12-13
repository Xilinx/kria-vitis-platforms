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

#include "softmax.hpp"

namespace vart {
namespace cpu {

template <typename DType>
const vector<string> Softmax<DType>::ITName = {
    "input",
};

// constructor and deconstructor
template <typename DType>
Softmax<DType>::Softmax(const xir::Subgraph* subg, const xir::Op* op,
                        IMapTBs_t inputs, CPUTBPtr_t output)
    : CPUOPBase(subg, op, inputs, output) {
  GET_INPUT_DIMX_FMAP(fmap_i_, input, 6);
  GET_OUTPUT_DIMX_FMAP(fmap_o_, 6);

  // get pad info
  axis_ = xir_op_->get_attr<int>("axis");
  axis_ = axis_ == -1 ? (xir_op_->get_input_tensor("input")->get_shape().size() - 1)
                      : axis_;
}

template <typename DType>
void Softmax<DType>::run() {
  // // do necessary check
  // print_param();
  // check_param();

  // // read data
  // read();

  // do softmax
  softmax();

  // // do save, debug...
  // save();
}

template <typename DType>
void Softmax<DType>::print_param() {
  UNI_LOG_DEBUG_INFO << "fmap_i_ = " << Vec2Str(fmap_i_, ", ") << endl;
  UNI_LOG_DEBUG_INFO << "fmap_o_ = " << Vec2Str(fmap_o_, ", ") << endl;

  UNI_LOG_DEBUG_INFO << "axis = " << axis_ << endl;
}

template <typename DType>
void Softmax<DType>::check_param() {
  UNI_LOG_CHECK(inputs_.size() == 1, VART_SIZE_ERROR);

  // check fmap_out height
  UNI_LOG_CHECK(fmap_i_.size() == fmap_o_.size(), VART_SIZE_ERROR)
      << fmap_i_.size() << " != " << fmap_o_.size();
  for (auto i = 0U; i < fmap_i_.size(); i++) {
    UNI_LOG_CHECK(fmap_i_[i] == fmap_o_[i], VART_INVALID_VALUE)
        << ", " << fmap_i_[i] << " != " << fmap_o_[i];
  }
}

// read data from DDR
template <typename DType>
void Softmax<DType>::read() {
  // read img data
  auto* cputb = inputs_.at(ITName[INPUT]).at(0);
  img_ = GET_CPUTB_DType_PTR(DType, cputb);

  // handle output buffer
  rlt_ = GET_CPUTB_DType_PTR(DType, output_);
}

template <typename DType>
uint64_t Softmax<DType>::get_workload() {
  // TODO
  return 0;
}

template <typename DType>
void Softmax<DType>::softmax() {
  if (fmap_i_.size() == 1) {
    UNI_LOG_CHECK(axis_ == 0, VART_INVALID_VALUE) << ", " << axis_ << " != 0";
    vart::cpu::softmax(img_, (int)(fmap_i_[0]), rlt_);
  } else if (fmap_i_.size() == 2) {
    UNI_LOG_CHECK(axis_ == 1, VART_INVALID_VALUE) << ", " << axis_ << " != 1";
    for (auto i = 0; i < fmap_i_[0]; i++) {
      vart::cpu::softmax(img_ + i * fmap_i_[1], (int)fmap_i_[1],
                         rlt_ + i * fmap_i_[1]);
    }
  } else if (fmap_i_.size() == 3) {
    UNI_LOG_CHECK(axis_ == 2, VART_INVALID_VALUE) << ", " << axis_ << " != 2";
    for (auto i = 0; i < fmap_i_[0]; i++) {
      for (auto j = 0; j < fmap_i_[1]; j++) {
        vart::cpu::softmax(img_ + i * fmap_i_[1] * fmap_i_[2] + j * fmap_i_[2],
                           (int)fmap_i_[2],
                           rlt_ + i * fmap_i_[1] * fmap_i_[2] + j * fmap_i_[2]);
      }
    }
  } else if (fmap_i_.size() == 4) {
    UNI_LOG_CHECK(axis_ == 3, VART_INVALID_VALUE) << ", " << axis_ << " != 3";

    for (auto i = 0; i < fmap_i_[0]; i++) {
      for (auto j = 0; j < fmap_i_[1]; j++) {
        for (auto k = 0; k < fmap_i_[2]; k++) {
          vart::cpu::softmax(img_ + i * fmap_i_[1] * fmap_i_[2] * fmap_i_[3] +
                                 j * fmap_i_[2] * fmap_i_[3] + k * fmap_i_[3],
                             (int)fmap_i_[3],
                             rlt_ + i * fmap_i_[1] * fmap_i_[2] * fmap_i_[3] +
                                 j * fmap_i_[2] * fmap_i_[3] + k * fmap_i_[3]);
        }
      }
    }
  }
}

INSTANTIATE_TPCLASS(Softmax);
REG_OP_INSTANCE_FUNC("softmax", Softmax);

}  // namespace cpu
}  // namespace vart
