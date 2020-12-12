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
class Matmul : public CPUOPBase {
public:
  enum InputTensorType {
    INPUT,
    BIAS,
  };
  // input tensor name
  const static vector<string> ITName;

public:
  Matmul(const xir::Subgraph* subg, const xir::Op* op,
      IMapTBs_t inputs, CPUTBPtr_t output);
  ~Matmul() = default;

  virtual void run() override final;

  virtual void print_param() override final;
  virtual void check_param() override final;

  virtual void read() override final;

  virtual uint64_t get_workload() override final;

private:
  void calc_param();

private:
  // ia means input matrix a
  // ib means input matrix b
  // fmap_i_ is the original matrix dimension
  // fmap_ia_ and fmap_ib_ are transposed and extended dimension because of broadcast
  vector<Dimension> fmap_i_;
  Dimension fmap_ia_;
  Dimension fmap_ib_;
  Dimension fmap_b_;
  Dimension fmap_o_;

  // calc out
  bool has_bias_;
  // 1st matrix is MxK
  // 2nd matrix is KxN
  // result is MxN
  // M, K, N are all transposed value.
  int M_, K_, N_;
  // because  input a and input b support broadcast,
  // so batch_a_ and batch_b_ are different.
  int batch_a_;
  int batch_b_;

  // read from attrs
  bool transpose_a_;
  bool transpose_b_;

  // caculate buffer
  DType* data_ina_{nullptr};
  DType* data_inb_{nullptr};
  DType* data_bias_{nullptr};
  DType* data_out_{nullptr};
};

} // namespace cpu
} // namespace vart


