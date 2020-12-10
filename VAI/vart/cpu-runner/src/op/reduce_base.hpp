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
class ReduceBase : public CPUOPBase {
public:
  // binary operation type and name
  enum ReduceType {
    MAX,
    MIN,
    SUM,
    PROD,
    MEAN,
    Unknown
  };
  const static vector<string> ReduceTypeName;

  // input tensor enum and name
  enum InputTensorType {
    INPUT,
  };
  const static vector<string> ITName;

public:
  ReduceBase(const xir::Subgraph* subg, const xir::Op* op,
      IMapTBs_t inputs, CPUTBPtr_t output);
  virtual ~ReduceBase() = default;

  virtual void run() override;

  virtual void print_param() override final;
  virtual void check_param() override final;

  virtual void read() override final;

  virtual uint64_t get_workload() override final;

protected:
  virtual void calculate() {}

protected:
  Dimension fmap_i_;
  Dimension fmap_o_;

  vector<bool> reduce_flag_;
  vector<int> reduce_dims_;
  bool keep_dim_flag_;

  int reduce_type_{Unknown};

  // i/o buffer
  vector<DType> data_in_buf_;
  DType* data_in_ptr_{nullptr};
  DType* data_out_ptr_{nullptr};
};

} // namespace cpu
} // namespace vart


