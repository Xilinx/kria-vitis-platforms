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

#include "user_runner.hpp"

namespace vart {
namespace cpu {

UserRunner::UserRunner(const xir::Subgraph *subgraph)
    : subg_(subgraph), g_(subg_->get_graph()) {
  if (VART_DEBUG) {
    UNI_LOG_INFO_LOW << "Constructing UserRunner ..." << endl;
    UNI_LOG_INFO_LOW
        << "----------------------------------------------------------" << endl;
  }
}

UserRunner::~UserRunner() {}

std::pair<uint32_t, int> UserRunner::execute_async(
    const std::vector<vart::TensorBuffer *> &vitis_inputs,
    const std::vector<vart::TensorBuffer *> &vitis_outputs) {

  static int jobid = 0;
  return std::make_pair(jobid++, 0);
}

int UserRunner::wait(int jobid, int ms) { return 0; }

std::vector<const xir::Tensor *> UserRunner::get_input_tensors() { return {}; }

std::vector<const xir::Tensor *> UserRunner::get_output_tensors() { return {}; }

}  // namespace cpu
}  // namespace vart
