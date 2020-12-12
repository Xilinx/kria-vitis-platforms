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

#include "downsample.hpp"

namespace vart {
namespace cpu {

template<typename DType>
class DownSampleFix : public DownSample<DType> {
public:
  DownSampleFix(const xir::Subgraph* subg, const xir::Op* op,
      IMapTBs_t inputs, CPUTBPtr_t output);
  virtual ~DownSampleFix() = default;

  virtual void run() override final;

  virtual void print_param() override;

protected:
  void downsample_fix();

protected:
  int fix_width_{8};
  int fp_input_;
  int fp_output_;
  int shift_;

  using DownSample<DType>::fmap_i_;
  using DownSample<DType>::fmap_o_;
  using DownSample<DType>::data_in_ptr_;
  using DownSample<DType>::data_out_ptr_;

};

} // namespace cpu
} // namespace vart

