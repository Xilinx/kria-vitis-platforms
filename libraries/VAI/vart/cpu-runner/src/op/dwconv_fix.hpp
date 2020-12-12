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

#include "dwconv.hpp"

namespace vart {
namespace cpu {

template<typename DType>
class DWConvFix : public DWConv<DType> {
public:
  DWConvFix(const xir::Subgraph* subg, const xir::Op* op,
      IMapTBs_t inputs, CPUTBPtr_t output);
  virtual ~DWConvFix() = default;

  virtual void run() override final;
  virtual void print_param() override final;

private:
  void fix();

protected:
  using DWConvBase<DType>::fmap_b_;
  using DWConvBase<DType>::fmap_o_;
  using DWConvBase<DType>::has_bias_;
  using DWConvBase<DType>::bias_ptr_;
  using DWConvBase<DType>::data_out_ptr_;

  int fp_input_;
  int fp_weights_;
  int fp_bias_;
  int fp_output_;

  int shift_bias_{0};
  int shift_cut_{0};

  int nonlinear_type_;
};

} // namespace cpu
} // namespace vart

