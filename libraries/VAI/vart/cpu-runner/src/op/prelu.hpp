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

#include "relu_base.hpp"

namespace vart {
namespace cpu {

template<typename DType>
class PRelu : public ReluBase<DType> {
public:
  enum InputTensorType {
    INPUT,
    ALPHA,
  };
  // input tensor name
  const static vector<string> ITName;

public:
  PRelu(const xir::Subgraph* subg, const xir::Op* op,
      IMapTBs_t inputs, CPUTBPtr_t output);
  ~PRelu() = default;

  virtual void run() override final;

  virtual void print_param() override;
  virtual void check_param() override;

  virtual void read() override;

private:
  void prelu();

private:
  Dimension fmap_alpha_;

  DType* alpha_{nullptr};

  using ReluBase<DType>::fmap_i_;
  using ReluBase<DType>::fmap_o_;
  using ReluBase<DType>::data_in_ptr_;
  using ReluBase<DType>::data_out_ptr_;
};

} // namespace cpu
} // namespace vart


