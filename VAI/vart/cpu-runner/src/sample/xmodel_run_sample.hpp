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
#include "cpu_helper.hpp"
#include "cpu_tensor_buffer.hpp"
#include "cpu_cfg.hpp"
#include "cpu_op_base.hpp"

namespace vart {
namespace cpu {

class XModelRunSample {
public:
  explicit XModelRunSample(std::unique_ptr<xir::Graph> g,
      XModelRunMode xmodel_run_mode = XModelRunMode::CPU);
  ~XModelRunSample() = default;
  VART_DISABLE_COPY(XModelRunSample);

public:
  void run();
  void create_and_run();
  std::vector<TensorBuffer *> get_input_tbs();
  std::vector<TensorBuffer *> get_output_tbs();

  int get_runner_num() { return runners_.size(); }
  vector<void*> get_runners() {
    vector<void*> r;
    for(auto i=0U; i<runners_.size(); i++) {
      r.push_back(runners_[i].get());
    }

    return r;
  }

private:
  std::unique_ptr<vart::Runner> create_runner(
      int idx,
      const xir::Subgraph* subg,
      const std::string& device);
  TensorBuffer* get_tb_from_op(const xir::Op *op);
  std::vector<TensorBuffer *> get_subg_input_tbs(const xir::Subgraph* subg);
  std::vector<TensorBuffer *> get_subg_output_tbs(const xir::Subgraph* subg);

  void save_subg_i(int subg_id, vector<TensorBuffer *> &input_tbs);
  void save_subg_o(int subg_id, vector<TensorBuffer *> &output_tbs);

private:
  std::unique_ptr<xir::Graph> g_;
  std::vector<xir::Subgraph*> subg_all_;
  std::vector<std::unique_ptr<vart::Runner>> runners_;

  std::map<std::string, unique_ptr<TensorBuffer>> tb_map_;
};

}  // namespace cpu
}  // namespace vart
