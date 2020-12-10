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
class UnaryBase : public CPUOPBase {
public:
  // binary operation type and name
  enum UnaryType {
    EXP,
    NEG,
    SIGMOID,
    TANH,
    Unknown
  };
  const static vector<string> UnaryTypeName;

  // input tensor enum and name
  enum InputTensorType {
    INPUT,
  };
  const static vector<string> ITName;

public:
  UnaryBase(const xir::Subgraph* subg, const xir::Op* op,
      IMapTBs_t inputs, CPUTBPtr_t output);
  ~UnaryBase() = default;

  virtual void run() override final;

  virtual void print_param() override final;
  virtual void check_param() override final;

  virtual void read() override final;

  virtual uint64_t get_workload() override final;

protected:
  virtual void calculate() {}

protected:
  Dimension fmap_i_;
  Dimension fmap_o_;

  int unary_type_{Unknown};

  // i/o buffer pointer
  DType* data_in_ptr_{nullptr};
  DType* data_out_ptr_{nullptr};
};

} // namespace cpu
} // namespace vart


