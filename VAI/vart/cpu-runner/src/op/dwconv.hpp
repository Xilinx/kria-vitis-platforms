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

#pragma once

#include "dwconv_base.hpp"

namespace vart {
namespace cpu {

template<typename DType>
class DWConv : public DWConvBase<DType> {
public:
  DWConv(const xir::Subgraph* subg, const xir::Op* op,
      IMapTBs_t inputs, CPUTBPtr_t output);
  virtual ~DWConv() = default;

  virtual void run() override;

protected:
  using DWConvBase<DType>::raw_fmap_i_;
  using DWConvBase<DType>::fmap_i_;
  using DWConvBase<DType>::raw_fmap_w_;
  using DWConvBase<DType>::fmap_w_;
  using DWConvBase<DType>::fmap_b_;
  using DWConvBase<DType>::fmap_o_;
  using DWConvBase<DType>::pad_;
  using DWConvBase<DType>::kernel_;
  using DWConvBase<DType>::stride_;
  using DWConvBase<DType>::dilation_;
  using DWConvBase<DType>::has_bias_;
};

} // namespace cpu
} // namespace vart

