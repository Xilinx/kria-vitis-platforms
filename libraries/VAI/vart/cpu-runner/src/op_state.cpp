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

#include "op_state.hpp"
#include "vart/xir_helper.hpp"

namespace vart {
namespace cpu {

static mutex mtx;

void CPUOPState::set_state(const string& op_name, OPState state) {
  std::lock_guard<mutex> lock(mtx);
  op_state_[op_name] = state;
}

OPState CPUOPState::get_state(const string& op_name) {
  std::lock_guard<mutex> lock(mtx);
  if (op_state_.find(op_name) == op_state_.end()) return OPState::Ready;

  return op_state_.at(op_name);
}

void CPUOPState::set_op_ready(const xir::Op* xir_op) {
  set_state(xir_op->get_name(), OPState::Ready);
}

bool CPUOPState::get_op_ready(const xir::Op* xir_op) {
  auto input_ops = vec_input_ops(xir_op->get_input_ops());

  auto rlt = std::all_of(
      input_ops.begin(), input_ops.end(), [this](const xir::Op* xir_op) {
        return get_state(xir_op->get_name()) == OPState::Ready;
      });

  return rlt;
}

}  // namespace cpu
}  // namespace vart
