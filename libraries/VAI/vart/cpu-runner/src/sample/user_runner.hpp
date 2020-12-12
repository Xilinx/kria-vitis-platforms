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

#include "cpu_base_inc.hpp"

namespace vart {
namespace cpu {

class CPUTBFactory;
class CPUOPBase;

class UserRunner : public vart::Runner {
public:
  explicit UserRunner(const xir::Subgraph *subgraph);
  UserRunner(const UserRunner &) = delete;
  UserRunner &operator=(const UserRunner &other) = delete;
  virtual ~UserRunner();

public:
  // pair<jodid, status>
  virtual std::pair<std::uint32_t, int> execute_async(
      const std::vector<vart::TensorBuffer *> &vitis_inputs,
      const std::vector<vart::TensorBuffer *> &vitis_outputs) override final;
  virtual int wait(int jobid, int ms) override final;

  virtual std::vector<const xir::Tensor *> get_input_tensors() override final;
  virtual std::vector<const xir::Tensor *> get_output_tensors() override final;

private:
  const xir::Subgraph *subg_;
  const xir::Graph *g_;
};

}  // namespace cpu
}  // namespace vart
