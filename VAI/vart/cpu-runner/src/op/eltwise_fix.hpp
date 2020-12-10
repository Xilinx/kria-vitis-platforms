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

#include "eltwise.hpp"

namespace vart {
namespace cpu {

template<typename DType>
class EltwiseFix : public Eltwise<DType> {
public:
  enum EltwiseNonlinearType {
    NONLINEAR_NONE = 0,
    NONLINEAR_RELU = 1,
    NONLINEAR_PRELU = 2,
    NONLINEAR_LEAKY_RELU = 3,
    NONLINEAR_RELU6 = 4,
  };

public:
  EltwiseFix(const xir::Subgraph* subg, const xir::Op* op,
      IMapTBs_t inputs, CPUTBPtr_t output);
  ~EltwiseFix() = default;

  virtual void run() override final;

  virtual void print_param() override;

protected:
  void eltwise_fix_normal();
  void eltwise_fix_thread();

private:
  int nonlinear_type_;

  vector<int> fp_inputs_;
  int fp_output_;

  vector<int> shift_read_;
  int shift_write_;
  vector<float> shift_read_factor_;
  float shift_write_factor_;

  using Eltwise<DType>::fmap_i_;
  using Eltwise<DType>::fmap_o_;
  using Eltwise<DType>::channel_wise_;
  using Eltwise<DType>::input_num_;
  using Eltwise<DType>::data_in_;
  using Eltwise<DType>::data_out_;

  using Eltwise<DType>::THREAD_NUM;
  using Eltwise<DType>::THREAD_WORKLOAD;
};

} // namespace cpu
} // namespace vart


