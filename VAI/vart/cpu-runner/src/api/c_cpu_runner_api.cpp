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

#include "c_cpu_runner_api.hpp"

#include "cpu_runner.hpp"
#include "store_mgr.hpp"

#define CPU_RUNNER_PTR(runner) reinterpret_cast<vart::cpu::CPURunner*>(runner)

struct RunnerOverview {
  string runner_name;
  string device_name;
  int op_num;
  int op_type_num;
  string op_type_list;
};

void GetCPURunnerOverview(void* runner, APIRunnerOverview* info) {
  UNI_LOG_DEBUG_INFO << "Enter GetCPURunnerOverview" << endl;

  cout << "runner ptr: " << runner << ", overview ptr: " << (void*)info << endl;

  // get runner overview into c++ store
  auto* cpu_runner = CPU_RUNNER_PTR(runner);

  auto& r = vart::cpu::StoreMgr<RunnerOverview>::Instance();
  r.runner_name = cpu_runner->get_name();
  r.device_name = cpu_runner->get_device();
  r.op_num = cpu_runner->get_op_num();
  r.op_type_num = cpu_runner->get_op_type_num();
  r.op_type_list = cpu_runner->get_op_type_list();

  // make python related pointers point to c++ store
  info->runner_name = r.runner_name.data();
  info->device_name = r.device_name.data();
  info->op_num = r.op_num;
  info->op_type_num = r.op_type_num;
  info->op_type_list = r.op_type_list.data();

  UNI_LOG_DEBUG_INFO << "Leave GetCPURunnerOverview" << endl;
}

void GetOps(void* runner, void** ops_arr) {
  UNI_LOG_DEBUG_INFO << "Enter GetRunners" << endl;
  auto l = CPU_RUNNER_PTR(runner)->get_cpu_ops();

  auto i = 0;
  for (auto it = l.begin(); it != l.end(); it++, i++) {
    ops_arr[i] = *it;
    // for debug
    // cout << "op addr " << *it << ", " << ops_arr[i] << endl;
  }
}

