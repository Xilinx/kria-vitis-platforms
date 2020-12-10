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
class Resize : public CPUOPBase {
public:
  enum InputTensorType {
    INPUT,
    SIZE,
  };
  // input tensor name
  const static vector<string> ITName;

public:
  Resize(const xir::Subgraph* subg, const xir::Op* op,
      IMapTBs_t inputs, CPUTBPtr_t output);
  ~Resize() = default;

  virtual void run() override;

  virtual void print_param() override;
  virtual void check_param() override final;

  virtual void read() override final;

  virtual uint64_t get_workload() override final;

protected:
  void resize();

  FMap_t raw_fmap_i_;
  FMap_t fmap_i_;
  FMap_t fmap_o_;

  std::string mode_;
  bool align_corners_;
  bool half_pixel_centers_;

  vector<DType> img_;
  std::vector<float> rlt_;
  DType* data_in_ptr_{nullptr};
  DType* data_out_ptr_{nullptr};
};

} // namespace cpu
} // namespace vart

