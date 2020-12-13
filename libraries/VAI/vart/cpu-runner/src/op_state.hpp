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

enum class OPState {
  Blocked,
  Running,
  Ready
};

class CPUOPState {
public:
  explicit CPUOPState() = default;
  ~CPUOPState() = default;

public:
  void set_state(const string& op_name, OPState state);
  OPState get_state(const string& op_name);
  void set_op_ready(const xir::Op* xir_op);
  bool get_op_ready(const xir::Op* xir_op);

private:
  unordered_map<string, OPState> op_state_;
};

} // namespace cpu
} // namespace vart
