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

#include "conv2d_fix.hpp"

namespace vart {
namespace cpu {

// constructor and deconstructor
template <typename DType>
Conv2dFix<DType>::Conv2dFix(const xir::Subgraph* subg, const xir::Op* op,
                            IMapTBs_t inputs, CPUTBPtr_t output)
    : Conv2d<DType>(subg, op, inputs, output) {
  auto* xir_tensor_i = CPUOPBase::xir_op_->get_input_tensor("input");
  fp_input_ = xir_tensor_i->get_attr<int>("fix_point");

  auto* xir_tensor_w = CPUOPBase::xir_op_->get_input_tensor("weights");
  fp_weights_ = xir_tensor_w->get_attr<int>("fix_point");

  if (has_bias_) {
    auto* xir_tensor_b = CPUOPBase::xir_op_->get_input_tensor("bias");
    fp_bias_ = xir_tensor_b->get_attr<int>("fix_point");
    shift_bias_ = fp_input_ + fp_weights_ - fp_bias_;
  }

  auto* xir_tensor_o = CPUOPBase::xir_op_->get_output_tensor();
  fp_output_ = xir_tensor_o->get_attr<int>("fix_point");
  shift_cut_ = fp_input_ + fp_weights_ - fp_output_;

  auto nonlinear_type_str = CPUOPBase::xir_op_->get_attr<string>("nonlinear");
  if (nonlinear_type_str == "NONE" || nonlinear_type_str == "")
    nonlinear_type_ = 0;
  else if (nonlinear_type_str == "RELU")
    nonlinear_type_ = 1;
  else if (nonlinear_type_str == "PRELU")
    nonlinear_type_ = 2;
  else if (nonlinear_type_str == "LEAKYRELU")
    nonlinear_type_ = 3;
  else if (nonlinear_type_str == "RELU6")
    nonlinear_type_ = 4;
  else
    UNI_LOG_FATAL(VART_NOT_SUPPORT)
      << "Unsupported nonlinear type: " << nonlinear_type_str << ".";
}

template <typename DType>
void Conv2dFix<DType>::print_param() {
  ConvBase<DType>::print_param();

  UNI_LOG_DEBUG_INFO << "fp_input: " << fp_input_ << endl;
  UNI_LOG_DEBUG_INFO << "fp_weights: " << fp_weights_ << endl;
  if (has_bias_) {
    UNI_LOG_DEBUG_INFO << "fp_bias: " << fp_bias_ << endl;
  }
  UNI_LOG_DEBUG_INFO << "fp_output: " << fp_output_ << endl;

  if (has_bias_) {
    UNI_LOG_DEBUG_INFO << "shift_bias: " << shift_bias_ << endl;
  }
  UNI_LOG_DEBUG_INFO << "shift_cut: " << shift_cut_ << endl;
  UNI_LOG_DEBUG_INFO << "nonlinear_type: " << nonlinear_type_ << endl;
}

template <typename DType>
void Conv2dFix<DType>::check_param() {
  ConvBase<DType>::check_param();

  UNI_LOG_CHECK(nonlinear_type_ == 0 || nonlinear_type_ == 1 ||
                    nonlinear_type_ == 3 || nonlinear_type_ == 4,
                VART_INVALID_VALUE)
      << " not supported. ";
}

template <typename DType>
void Conv2dFix<DType>::run() {
  // // do necessary check
  // print_param();
  // check_param();

  // // read data
  // this->read();

  // calc conv
  this->conv();

  // do fix
  this->fix();

  // // do save, debug...
  // this->save();
}

template <typename DType>
void Conv2dFix<DType>::fix() {
  // do fix for bias
  if (has_bias_) {
    for (auto i = 0; i < fmap_o_.num(); i++) {
      auto pos = i % fmap_o_.c;
      data_out_ptr_[i] *= 2;
      data_out_ptr_[i] += floor(2.0 * bias_ptr_[pos] * pow(2.0, shift_bias_));
    }
  }

  // do fix for output
  auto factor = pow(2, shift_cut_ + 1);
  for (auto i = 0; i < fmap_o_.num(); i++) {
    double tmp = data_out_ptr_[i];
    tmp /= factor;
    if (nonlinear_type_ == 1) {
      if (tmp < 0) tmp = 0;
    } else if (nonlinear_type_ == 3) {
      if (tmp < 0) tmp = tmp * 26. / 256;
    } else if (nonlinear_type_ == 4) {
      if (tmp < 0) tmp = 0;
      if(fp_output_ <= 4) {
        auto thr6 = 6 << 4;
        if (tmp >= thr6) tmp = thr6;
      }
    }
    data_out_ptr_[i] =
        DPURound<DType>(tmp, CPUOPBase::data_min_, CPUOPBase::data_max_);
  }
}

INSTANTIATE_TPCLASS(Conv2dFix);
REG_OP_INSTANCE_FUNC("conv2d-fix", Conv2dFix);

}  // namespace cpu
}  // namespace vart
