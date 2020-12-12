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

#include "conv_base.hpp"

namespace vart {
namespace cpu {

template<typename DType>
class Conv2d : public ConvBase<DType> {
public:
  Conv2d(const xir::Subgraph* subg, const xir::Op* op,
      IMapTBs_t inputs, CPUTBPtr_t output);
  virtual ~Conv2d() = default;

  virtual void run() override;

protected:
  using ConvBase<DType>::raw_fmap_i_;
  using ConvBase<DType>::fmap_i_;
  using ConvBase<DType>::raw_fmap_w_;
  using ConvBase<DType>::fmap_w_;
  using ConvBase<DType>::fmap_b_;
  using ConvBase<DType>::fmap_o_;
  using ConvBase<DType>::pad_;
  using ConvBase<DType>::kernel_;
  using ConvBase<DType>::stride_;
  using ConvBase<DType>::dilation_;
  using ConvBase<DType>::has_bias_;
};

} // namespace cpu
} // namespace vart

