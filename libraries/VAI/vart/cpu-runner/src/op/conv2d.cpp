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

#include "conv2d.hpp"

namespace vart {
namespace cpu {

// constructor and deconstructor
template <typename DType>
Conv2d<DType>::Conv2d(const xir::Subgraph* subg, const xir::Op* op,
                      IMapTBs_t inputs, CPUTBPtr_t output)
    : ConvBase<DType>(subg, op, inputs, output) {
  // get kernel and update fmap_w_
  auto xir_kernel = CPUOPBase::xir_op_->get_attr<vector<int>>("kernel");
  auto xir_dilation = CPUOPBase::xir_op_->get_attr<vector<int>>("dilation");
  kernel_ =
      Kernel_t{xir_kernel[1], xir_kernel[0], xir_dilation[1], xir_dilation[0]};
  fmap_w_.h = fmap_w_.h * kernel_.dilation_h - (kernel_.dilation_h - 1);
  fmap_w_.w = fmap_w_.w * kernel_.dilation_w - (kernel_.dilation_w - 1);

  // get stride info
  auto xir_stride = CPUOPBase::xir_op_->get_attr<vector<int>>("stride");
  stride_ = Stride_t{xir_stride[1], xir_stride[0]};

  // update input height and width according to padding info
  pad_ = calc_pad(fmap_i_, fmap_o_, kernel_, stride_, pad_);
  fmap_i_.h += pad_.t + pad_.b;
  fmap_i_.w += pad_.l + pad_.r;
}

template <typename DType>
void Conv2d<DType>::run() {
  // // do necessary check
  // this->print_param();
  // this->check_param();

  // // read data
  // this->read();

  // calc conv
  this->conv();

  // add bias to conv result
  this->bias();

  // // do save, debug...
  // this->save();
}

INSTANTIATE_TPCLASS(Conv2d);

REG_OP_INSTANCE_FUNC("conv2d", Conv2d);

}  // namespace cpu
}  // namespace vart
