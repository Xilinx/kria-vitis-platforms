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

#include "cpu_op_base.hpp"

namespace vart {
namespace cpu {


template<typename DType>
class Scale : public CPUOPBase {
public:
  enum InputTensorType {
    INPUT,
    SCALE,
    BIAS,
  };
  // input tensor name
  const static vector<string> ITName;

public:
  Scale(const xir::Subgraph *subg, const xir::Op *op,
        IMapTBs_t inputs, CPUTBPtr_t output);
  ~Scale() = default;

  virtual void run() override final;

  virtual void print_param() override final;
  virtual void check_param() override final;

  virtual void read() override final;

private:
  void scale();
  void bias();

private:
  FMap_t raw_fmap_i_;
  FMap_t fmap_i_;
  FMap_t raw_fmap_w_;
  FMap_t fmap_w_;
  FMap_t fmap_b_;

  bool has_bias_;
  FMap_t fmap_o_;

  DType* data_in_ptr_{nullptr};
  DType* weights_ptr_{nullptr};
  DType* bias_ptr_{nullptr};
  DType* data_out_ptr_{nullptr};
};

} // namespace cpu
} // namespace vart


