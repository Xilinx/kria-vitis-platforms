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

#include "c_cpu_op_api.hpp"

#include "cpu_op_base.hpp"
#include "store_mgr.hpp"

#define CPU_OP_PTR(op) reinterpret_cast<vart::cpu::CPUOPBase*>(op)

struct OpOverview {
  string name;
  string type;
  int input_num;
  string input_list;
  string output_tensor_name;
  string data_type;
  uint64_t workload;
};

void GetCPUOpOverview(void* op, APIOpOverview* info) {
  UNI_LOG_DEBUG_INFO << "Enter GetCPUOpOverview" << endl;

  auto* cpu_op = CPU_OP_PTR(op);

  // get op overview into c++ store
  auto& r = vart::cpu::StoreMgr<OpOverview>::Instance();
  r.name = cpu_op->get_name();
  r.type = cpu_op->get_type();
  r.input_num = cpu_op->get_input_num();
  r.input_list = cpu_op->get_input_list();
  r.output_tensor_name = cpu_op->get_output_tensor_name();
  r.data_type = cpu_op->get_data_type();
  r.workload = cpu_op->get_workload();

  // make python related pointers point to c++ store
  info->name = r.name.data();
  info->type = r.type.data();
  info->input_num = r.input_num;
  info->input_list = r.input_list.data();
  info->output_tensor_name = r.output_tensor_name.data();
  info->data_type = r.data_type.data();
  info->workload = r.workload;

  UNI_LOG_DEBUG_INFO << "Leave GetCPUopOverview" << endl;
}

void CPUOpRun(void* op) {
  auto* cpu_op = CPU_OP_PTR(op);
  cpu_op->read();
  cpu_op->run();
}
