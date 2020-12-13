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
#include "cpu_op_visitor.hpp"

class CPUOPBase;

namespace vart {
namespace cpu {

class WorkloadVisitor : public CPUOPVisitor {
public:
  explicit WorkloadVisitor() = default;
  VART_BIG_THREE_LAW(WorkloadVisitor);

public:
  virtual void run(CPUOPBase *op) {
    auto wl = op->get_workload();

    auto *xir_op = op->get_xir_op();
    UNI_LOG_DEBUG_INFO << "op" << xir_op->get_attr<int>(CPU_RUN_ID) << " "
      << left << setw(32) << setfill(' ') << xir_op->get_type()
      << ":\t" << wl / 1000000000. << "Gops" << endl;

    CPUOPBase::subg_ops += op->get_workload();
  }

  static unique_ptr<WorkloadVisitor> make() {
    return make_unique<WorkloadVisitor>();
  }
  static const VisitorType type = VisitorType::WorkloadVisitor;

};

} // namespace cpu
} // namespace vart
