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
class PoolBase : public CPUOPBase {
public:
  enum PoolType {
    MAX_POOL,
    AVG_POOL
  };

  enum InputTensorType {
    INPUT,
  };
  // input tensor name
  const static vector<string> ITName;

public:
  PoolBase(const xir::Subgraph* subg, const xir::Op* op,
      IMapTBs_t inputs, CPUTBPtr_t output);
  virtual ~PoolBase() = default;

  virtual void run() override {}

  virtual void print_param() override;
  virtual void check_param() override;

  virtual void read() override final;

  virtual uint64_t get_workload() override final;

protected:
  int pool_type_;

  FMap_t raw_fmap_i_;
  FMap_t fmap_i_;
  FMap_t fmap_o_;

  Pad_t raw_pad_;
  Pad_t pad_;
  Kernel_t kernel_;
  Stride_t stride_;

  bool is_global_;

  DType pad_value_;

  DType* data_in_ptr_{nullptr};
  DType* data_out_ptr_{nullptr};

  uint32_t THREAD_NUM;
  uint32_t THREAD_WORKLOAD;
  uint32_t FMAP_SIZE;
};

} // namespace cpu
} // namespace vart

