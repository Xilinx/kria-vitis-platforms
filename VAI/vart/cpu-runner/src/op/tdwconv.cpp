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

#include "tdwconv.hpp"

namespace vart {
namespace cpu {

// constructor and deconstructor
template <typename DType>
TDWConv<DType>::TDWConv(const xir::Subgraph* subg, const xir::Op* op,
                        IMapTBs_t inputs, CPUTBPtr_t output)
    : DWConvBase<DType>(subg, op, inputs, output) {
  // get dilation info
  auto xir_dilation = CPUOPBase::xir_op_->get_attr<vector<int>>("stride");
  dilation_ = Stride_t{xir_dilation[1], xir_dilation[0]};

  // get input tensor's fmap
  GET_INPUT_DIM4_FMAP(raw_fmap_i_, input);
  auto tmp_n = raw_fmap_i_.n;
  auto tmp_h = raw_fmap_i_.h * dilation_.h - (dilation_.h - 1);
  auto tmp_w = raw_fmap_i_.w * dilation_.w - (dilation_.w - 1);
  auto tmp_c = raw_fmap_i_.c;
  fmap_i_ = FMapAttr{tmp_n, tmp_h, tmp_w, tmp_c};

  // get kernel info
  auto xir_kernel = CPUOPBase::xir_op_->get_attr<vector<int>>("kernel");
  kernel_ = Kernel_t{xir_kernel[1], xir_kernel[0]};

  // get pad info
  pad_.l = kernel_.w - 1 - pad_.l;
  pad_.t = kernel_.h - 1 - pad_.t;
  pad_.r = kernel_.w - 1 - pad_.r;
  pad_.b = kernel_.h - 1 - pad_.b;

  fmap_i_.h += pad_.t + pad_.b;
  fmap_i_.w += pad_.l + pad_.r;
}

template <typename DType>
void TDWConv<DType>::run() {
  this->dwconv();
  this->bias();
}

template <typename DType>
void TDWConv<DType>::check_param() {
  DWConvBase<DType>::check_param();

  // check stride
  UNI_LOG_CHECK(stride_.h == 1, VART_INVALID_VALUE)
      << ", " << stride_.h << " != 1";
  UNI_LOG_CHECK(stride_.w == 1, VART_INVALID_VALUE)
      << ", " << stride_.w << " != 1";
  UNI_LOG_CHECK(kernel_.dilation_h == 1, VART_INVALID_VALUE)
      << ", " << kernel_.dilation_h << " != 1";
  UNI_LOG_CHECK(kernel_.dilation_w == 1, VART_INVALID_VALUE)
      << ", " << kernel_.dilation_w << " != 1";
}

INSTANTIATE_TPCLASS(TDWConv);
REG_OP_INSTANCE_FUNC("transposed-depthwise-conv2d", TDWConv);

}  // namespace cpu
}  // namespace vart
