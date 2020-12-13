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
class Threshold : public CPUOPBase {
public:
  enum InputTensorType {
    INPUT,
    THRESHOLD
  };
  // input tensor name
  const static vector<string> ITName;

private:
  template<typename T>
  struct threshold_bit_t {
    T signal : 1, delta : 10, base : 13, unused : 8;
  };

  struct threshold_one_t {
    bool signal;
    int base;
    uint32_t delta;
  };

public:
  Threshold(const xir::Subgraph* subg, const xir::Op* op,
      IMapTBs_t inputs, CPUTBPtr_t output);
  ~Threshold() = default;

  virtual void run() override final;

  virtual void print_param() override final;
  virtual void check_param() override final;

  virtual void read() override final;

  virtual uint64_t get_workload() override final;

private:
  void parse_threshold();
  void threshold();

private:
  FMap_t fmap_i_;
  FMap_t fmap_t_;
  FMap_t fmap_o_;

  vector<threshold_one_t> threshold_;

  // NOTE: according to XIR's definition, threshold's DType is XINT,
  // so related data type in this OP is signed.
  DType* threshold_ptr_{nullptr};
  DType* data_in_{nullptr};
  DType* data_out_{nullptr};
};

} // namespace cpu
} // namespace vart


