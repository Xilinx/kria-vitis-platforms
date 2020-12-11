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
class Pad : public CPUOPBase {
public:
  enum InputTensorType {
    INPUT,
  };
  // input tensor name
  const static vector<string> ITName;

  enum PadMode {
    CONSTANT,
    REFLECT,
    SYMMETRIC
  };

public:
  Pad(const xir::Subgraph* subg, const xir::Op* op,
      IMapTBs_t inputs, CPUTBPtr_t output);
  ~Pad() = default;

  virtual void run() override final;

  virtual void print_param() override final;
  virtual void check_param() override final;

  virtual void read() override final;

  virtual uint64_t get_workload() override final;

private:
  void pad();

private:
  FMap_t raw_fmap_i_;
  FMap_t fmap_i_;
  FMap_t fmap_o_;

  Pad_t pad_;

  vector<int> constant_values_;

  // caculate buffer
  // DDR and Bank are all with stride, but caculate buffer don't have
  DType* img_{nullptr};
  DType* rlt_{nullptr};
};

} // namespace cpu
} // namespace vart


