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

#include "scale.hpp"

namespace vart {
namespace cpu {

template <typename DType>
const vector<string> Scale<DType>::ITName = {
    "input",
    "scale",
    "bias",
};

// constructor and deconstructor
template <typename DType>
Scale<DType>::Scale(const xir::Subgraph* subg,
    const xir::Op* op, IMapTBs_t inputs, CPUTBPtr_t output)
    : CPUOPBase(subg, op, inputs, output) {

  // get i/o fmap
  GET_INPUT_DIM4_FMAP(raw_fmap_i_, input);
  fmap_i_ = raw_fmap_i_;

  GET_INPUT_DIM1_FMAP(raw_fmap_w_, scale);
  fmap_w_ = raw_fmap_w_;

  has_bias_ = xir_op_->get_input_num(ITName[BIAS]) > 0 ? true : false;
  if (has_bias_) {
    GET_INPUT_DIM1_FMAP(fmap_b_, bias);
  }

  GET_OUTPUT_DIM4_FMAP(fmap_o_);
}

template <typename DType>
void Scale<DType>::print_param() {
  if(!CPUCfg::Instance().debug())
    return;

  raw_fmap_i_.print_param("raw_fmap_i");
  fmap_i_.print_param("fmap_i");
  raw_fmap_w_.print_param("raw_fmap_w");
  fmap_w_.print_param("fmap_w");
  UNI_LOG_DEBUG_INFO << "has_bias: " << Bool2Str(has_bias_) << endl;
  fmap_b_.print_param("fmap_b");
  fmap_o_.print_param("fmap_o");
}

template <typename DType>
void Scale<DType>::check_param() {
  UNI_LOG_CHECK(inputs_.size() == 3, VART_SIZE_ERROR)
      << ", Err: inputs size is " << inputs_.size();
}

template <typename DType>
void Scale<DType>::read() {
  auto* cputb = inputs_.at(ITName[INPUT]).at(0);
  data_in_ptr_ = GET_CPUTB_DType_PTR(DType, cputb);
  auto* weights_tb = inputs_.at(ITName[SCALE]).at(0);
  weights_ptr_ = GET_CPUTB_DType_PTR(DType, weights_tb);
  if(has_bias_) {
      auto* bias_tb = inputs_.at(ITName[BIAS]).at(0);
      bias_ptr_ = GET_CPUTB_DType_PTR(DType, bias_tb);
  }
  data_out_ptr_ = GET_CPUTB_DType_PTR(DType, output_);
}

template <typename DType>
void Scale<DType>::scale() {
  for (auto n = 0; n < fmap_o_.n; n++) {
    for (auto h = 0; h < fmap_o_.h; h++) {
      for (auto w = 0; w < fmap_o_.w; w++) {
        for (auto c = 0; c < fmap_o_.c; c++) {
          int idx = ((n * fmap_o_.h + h) * fmap_o_.w + w) * fmap_o_.c + c;
          data_out_ptr_[idx] = data_in_ptr_[idx] * weights_ptr_[c];
        }
      }
    }
  }
}

template <typename DType>
void Scale<DType>::bias() {
  if(!has_bias_)
    return;

  // NOTE: special handling for conv-fix
  if(std::is_floating_point<DType>::value) {
    for(auto i=0; i<fmap_o_.num(); i++) {
      auto pos = i % fmap_o_.c;
      data_out_ptr_[i] += bias_ptr_[pos];
    }
  } else {
    UNI_LOG_FATAL(VART_EXEC_ERROR)
      << "Pls do not use conv_base's bias while do scale!" << endl;
  }
}

template <typename DType>
void Scale<DType>::run() {
  read();
  scale();
  bias();
}

INSTANTIATE_TPCLASS(Scale);
REG_OP_INSTANCE_FUNC("scale", Scale);

}  // namespace cpu
}  // namespace vart
