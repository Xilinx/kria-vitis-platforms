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
class Reorg : public CPUOPBase {
public:
  enum InputTensorType {
    INPUT,
  };
  // input tensor name
  const static vector<string> ITName;

public:
  Reorg(const xir::Subgraph* subg, const xir::Op* op,
      IMapTBs_t inputs, CPUTBPtr_t output);
  virtual ~Reorg() = default;

  virtual void run() override;

  virtual void print_param() override;
  virtual void check_param() override;

  virtual void read() override final;

  virtual uint64_t get_workload() override final;

protected:
  void reorg();
  void reorg_reverse_yes();
  void reorg_reverse_no();
  void reorg_one(int idx_h, int idx_w);
  void reorg_one_reverse(int idx_h, int idx_w);

protected:
  FMap_t fmap_i_;
  FMap_t fmap_o_;

  bool reverse_;
  ScaleAttr scale_;

  // buffer
  DType* data_in_;
  vector<DType> data_in_tmp_;
  vector<DType> data_out_tmp_;
  DType* data_out_;
};

} // namespace cpu
} // namespace vart

