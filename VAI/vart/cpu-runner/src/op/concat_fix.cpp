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

#include "concat_fix.hpp"

namespace vart {
namespace cpu {

// constructor and deconstructor
template <typename DType>
ConcatFix<DType>::ConcatFix(const xir::Subgraph* subg, const xir::Op* op,
                            IMapTBs_t inputs, CPUTBPtr_t output)
    : Concat<DType>(subg, op, inputs, output) {
  for (auto i = 0; i < input_num_; i++) {
    auto* xir_tensor_i = CPUOPBase::xir_op_->get_input_tensor("input", i);
    UNI_LOG_CHECK(xir_tensor_i != nullptr, VART_NULL_PTR);
    auto fp = xir_tensor_i->get_attr<int>("fix_point");
    fp_inputs_.push_back(fp);
  }

  auto* xir_tensor_o = CPUOPBase::xir_op_->get_output_tensor();
  UNI_LOG_CHECK(xir_tensor_o != nullptr, VART_NULL_PTR);
  fp_output_ = xir_tensor_o->get_attr<int>("fix_point");
}

template <typename DType>
void ConcatFix<DType>::run() {
  Concat<DType>::run();
}

template <typename DType>
void ConcatFix<DType>::print_param() {
  Concat<DType>::print_param();

  UNI_LOG_DEBUG_INFO << "fix_width = " << fix_width_ << endl;
  for (auto i = 0; i < input_num_; i++) {
    UNI_LOG_DEBUG_INFO << "fp_input" << i << " = " << fp_inputs_[i] << endl;
  }
  UNI_LOG_DEBUG_INFO << "fp_output = " << fp_output_ << endl;
}

// read data from DDR
template <typename DType>
void ConcatFix<DType>::read() {
  auto key = "input";

  // handle input buffer
  data_in_.resize(input_num_);
  for (auto i = 0; i < input_num_; i++) {
    auto* cputb = this->inputs_.at(key).at(i);
    data_in_[i] = GET_CPUTB_DType_PTR(DType, cputb);

    auto shift = fp_output_ - fp_inputs_[i];
    auto factor = pow(2.0, shift);
    for (auto j = 0; j < fmap_i_[i].num(); j++) {
      data_in_[i][j] =
          DPURound<DType>((double)(data_in_[i][j]) * factor,
                          CPUOPBase::data_min_, CPUOPBase::data_max_);
    }
  }

  // handle output buffer
  data_out_ = GET_CPUTB_DType_PTR(DType, CPUOPBase::output_);
}

INSTANTIATE_TPCLASS(ConcatFix);
REG_OP_INSTANCE_FUNC("concat-fix", ConcatFix);

}  // namespace cpu
}  // namespace vart
