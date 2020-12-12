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
class BN : public CPUOPBase {
public:
  enum InputTensorType {
    INPUT,
    GAMMA,
    BETA,
    MOVING_MEAN,
    MOVING_VAR,
  };
  // input tensor name
  const static vector<string> ITName;

public:
  BN(const xir::Subgraph* subg, const xir::Op* op,
      IMapTBs_t inputs, CPUTBPtr_t output);
  ~BN() = default;

  virtual void run() override final;

  virtual void print_param() override final;
  virtual void check_param() override final;

  virtual void read() override final;

  virtual uint64_t get_workload() override final;

private:
  void read_input();
  void read_gamma();
  void read_beta();
  void read_moving_mean();
  void read_moving_var();

  void bn();

private:
  FMap_t raw_fmap_i_;
  FMap_t fmap_i_;
  FMap_t fmap_gamma_;
  FMap_t fmap_beta_;
  FMap_t fmap_mm_;
  FMap_t fmap_mv_;

  FMap_t fmap_o_;

  DType* img_{nullptr};
  DType* gamma_{nullptr};
  DType* beta_{nullptr};
  DType* moving_mean_{nullptr};
  DType* moving_var_{nullptr};

  DType epsilon_;

  DType* rlt_{nullptr};
};

} // namespace cpu
} // namespace vart


