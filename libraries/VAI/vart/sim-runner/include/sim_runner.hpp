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

#include <glog/logging.h>
#include <string>
#include <unordered_map>
#include "vart/runner.hpp"
#include "vart/tensor_buffer.hpp"
#include "xir/graph/subgraph.hpp"

namespace vart {
namespace sim {

class SimRunner : public Runner {
 public:
  explicit SimRunner(const xir::Subgraph* subgraph);
  explicit SimRunner();
  SimRunner(const SimRunner&) = delete;
  SimRunner& operator=(const SimRunner& other) = delete;

  virtual ~SimRunner() = default;

 public:
  virtual std::pair<uint32_t, int> execute_async(
      const std::vector<TensorBuffer*>& input,
      const std::vector<TensorBuffer*>& output) override final;

  virtual int wait(int jobid, int timeout) override final;

  virtual std::vector<const xir::Tensor*> get_input_tensors() override final;
  virtual std::vector<const xir::Tensor*> get_output_tensors() override final;

 public:
  void run_ac(const std::string& inst_file = "./config/instr_ac.txt");

 private:
  std::vector<std::string> get_ac(const std::string& inst_file);

 private:
  void run();
  void run_subgraph(const xir::Subgraph* subg);
  void run_superlayer(const xir::Subgraph* super);

 private:
  const xir::Subgraph* subgraph_;
  std::vector<const xir::Tensor*> inputs_;
  std::vector<const xir::Tensor*> outputs_;

  std::unordered_map<std::string, const xir::Tensor*> tensor_map;
  int layer_id_;
  int batch_idx_;
};

}  // namespace sim
}  // namespace vart
