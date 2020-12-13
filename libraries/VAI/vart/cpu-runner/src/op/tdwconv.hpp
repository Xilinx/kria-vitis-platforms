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
class TDWConv : public DWConvBase<DType> {
public:
  TDWConv(const xir::Subgraph* subg, const xir::Op* op,
      IMapTBs_t inputs, CPUTBPtr_t output);
  virtual ~TDWConv() = default;

  virtual void run() override;

  virtual void check_param() override;

protected:
  using DWConvBase<DType>::raw_fmap_i_;
  using DWConvBase<DType>::fmap_i_;
  using DWConvBase<DType>::pad_;
  using DWConvBase<DType>::kernel_;
  using DWConvBase<DType>::stride_;
  using DWConvBase<DType>::dilation_;
};

} // namespace cpu
} // namespace vart

