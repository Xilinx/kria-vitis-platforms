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

#include "resize_fix.hpp"

namespace vart {
namespace cpu {

template <typename DType>
ResizeFix<DType>::ResizeFix(const xir::Subgraph* subg, const xir::Op* op,
                            IMapTBs_t inputs, CPUTBPtr_t output)
    : Resize<DType>(subg, op, inputs, output) {
  auto xir_tensor_i = CPUOPBase::xir_op_->get_input_tensor("input");
  auto xir_tensor_o = CPUOPBase::xir_op_->get_output_tensor();

  fp_input_ = xir_tensor_i->get_attr<int>("fix_point");
  fp_output_ = xir_tensor_o->get_attr<int>("fix_point");
  shift_ = fp_output_ - fp_input_;
}

template <typename DType>
void ResizeFix<DType>::run() {
  this->resize();
  fix();
}

template <typename DType>
void ResizeFix<DType>::print_param() {
  Resize<DType>::print_param();

  UNI_LOG_DEBUG_INFO << "fix_width = " << fix_width_ << endl;
  UNI_LOG_DEBUG_INFO << "fp_input = " << fp_input_ << endl;
  UNI_LOG_DEBUG_INFO << "fp_output = " << fp_output_ << endl;
  UNI_LOG_DEBUG_INFO << "shift = " << shift_ << endl;
}

template <typename DType>
void ResizeFix<DType>::fix() {
  double factor = pow(2, shift_);
  for (auto i = 0; i < fmap_o_.num(); i++) {
    data_out_ptr_[i] = DPURound<DType>(
        rlt_[i] * factor, CPUOPBase::data_min_, CPUOPBase::data_max_);
  }
}

INSTANTIATE_TPCLASS(ResizeFix);
REG_OP_INSTANCE_FUNC("upsample-fix", ResizeFix);

}  // namespace cpu
}  // namespace vart
