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

template <typename DType>
class Const : public CPUOPBase {
public:
  Const(const xir::Subgraph* subg, const xir::Op* op,
      IMapTBs_t inputs, CPUTBPtr_t output);
  ~Const() = default;

  virtual void run() override final;

  virtual void print_param() override final;
  virtual void check_param() override final;

  virtual void read() override final;

protected:
  void process();
  void process_normal();
  void process_for_threshold();

private:
  xir::Tensor* tensor{nullptr};

  // NOTE: Do not use FMapAttr struct, because it doesn't support dims > 4
  vector<int> fmap_o_;
  int data_num_;

  vector<char> data_in_vec_;

  // caculate buffer
  char* data_in_ptr_{nullptr};
  DType* data_out_ptr_{nullptr};
};

}  // namespace cpu
}  // namespace vart
