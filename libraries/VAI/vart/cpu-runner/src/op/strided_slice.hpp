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
class StridedSlice : public CPUOPBase {
public:
  enum InputTensorType {
    INPUT,
  };
  // input tensor name
  const static vector<string> ITName;

public:
  StridedSlice(const xir::Subgraph* subg, const xir::Op* op,
      IMapTBs_t inputs, CPUTBPtr_t output);
  ~StridedSlice() = default;

  virtual void run() override final;

  virtual void print_param() override final;
  virtual void check_param() override final;

  virtual void read() override final;

  virtual uint64_t get_workload() override final;

private:
  void strided_slice();
  void draw_dim1(int pos, int begin, int end, int stride, vector<int>& fmap, vector<DType> &data);
  void draw_dim2(int pos, int begin, int end, int stride, vector<int>& fmap, vector<DType> &data);
  void draw_dim3(int pos, int begin, int end, int stride, vector<int>& fmap, vector<DType> &data);
  void draw_dim4(int pos, int begin, int end, int stride, vector<int>& fmap, vector<DType> &data);

private:
  vector<int> fmap_i_;
  vector<int> fmap_o_;

  vector<int> begin_;
  vector<int> end_;
  vector<int> strides_;
  int begin_mask_;
  int end_mask_;
  int ellipsis_mask_;
  int new_axis_mask_;
  int shrink_axis_mask_;

  // caculate buffer
  DType* data_in_{nullptr};
  DType* data_out_{nullptr};
};

} // namespace cpu
} // namespace vart


