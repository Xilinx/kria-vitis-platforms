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
#include "op_state.hpp"

namespace vart {
namespace cpu {

class OPSchedule;
class CPUOPVisitor;

typedef void (OPSchedule::*AcceptVis_t)(CPUOPVisitor *v);

class OPSchedule {
public:
  explicit OPSchedule(const list<CPUOPBase*>& ops) : ops_(ops) {  }
  ~OPSchedule() = default;
  OPSchedule(const OPSchedule& other) = default;
  OPSchedule& operator=(const OPSchedule& other) = default;

public:
  void attach(CPUOPBase* op) {
    ops_.push_back(op);
  }

  // void detach(CPUOPBase* op) {
  //   ops_.remove(op);
  // }

  void install(unique_ptr<CPUOPVisitor> v) {
    auto cpu_run_mode = CPUCfg::Instance().get_cpu_run_mode();
    if(cpu_run_mode == CPURunMode::NORMAL) {
      run_normal(std::move(v));
    } else if(cpu_run_mode == CPURunMode::NORMAL_THREAD
        || cpu_run_mode == CPURunMode::GEMM
        || cpu_run_mode == CPURunMode::GEMM_THREAD){
      run_thread(std::move(v));
    } else {
      run_debug(std::move(v));
    }
  }

private:
  void run_normal(unique_ptr<CPUOPVisitor> v) {
    // do global initialization in current subg
    CPUOPBase::StaticInit();

    for(auto e=ops_.begin(); e!=ops_.end(); e++) {
      struct timeval start;
      time_start(start);

      (*e)->accept(v.get());

      auto ms = time_finish(start, TIME_UNIT_MS);
      if (VART_DEBUG) {
        UNI_LOG_DEBUG_INFO << (*e)->get_xir_op()->get_type()
          << " " << ms << "ms" << endl;
      }
    }

    PRINT_DIVIDING_LINE();
    print_summary();
  }

  void run_thread(shared_ptr<CPUOPVisitor> v) {
    // do global initialization in current subg
    CPUOPBase::StaticInit();

    // init op state
    for(auto *op : ops_) {
      op_state_.set_state(op->get_xir_op()->get_name(), OPState::Blocked);
    }

    // every op create a thread to run
    auto THREAD_NUM = ops_.size();
    vector<std::future<int>> fut(THREAD_NUM);

    int id = 0;
    for(auto *op : ops_) {
      fut[id] = std::async(std::launch::async,
          [this, v, op](decltype(id) id) {
        const auto* xir_op = op->get_xir_op();
        while(!op_state_.get_op_ready(xir_op)) {
          std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        struct timeval start;
        time_start(start);

        op->accept(v.get());
        op_state_.set_op_ready(xir_op);

        auto ms = time_finish(start, TIME_UNIT_MS);
        if (VART_DEBUG) {
          UNI_LOG_DEBUG_INFO << xir_op->get_type()
            << " " << ms << "ms" << endl;
        }

        return 0;
      }, id);

      id++;
    }

    for(auto i=0U; i<THREAD_NUM; i++) {
      fut[i].wait();
    }
    PRINT_DIVIDING_LINE();
    print_summary();
  }

  void run_debug(unique_ptr<CPUOPVisitor> v) {
    // do global initialization in current subg
    CPUOPBase::StaticInit();

    for(auto e=ops_.begin(); e!=ops_.end(); e++) {
      (*e)->accept(v.get());
    }

    PRINT_DIVIDING_LINE();
    print_summary();
  }

  void print_summary() {
    VART_DEBUG_GO_ON();

    map<string, int> subg_op_map;
    for(const auto *op: ops_) {
      const auto *xir_op = op->get_xir_op();
      auto op_type = xir_op->get_type();
      if(subg_op_map.find(op_type) == subg_op_map.end()) {
        subg_op_map[op_type] = 1;
      } else {
        subg_op_map.at(op_type)++;
      }
    }

    // list all used op and workload
    string op_list;
    auto op_list_idx = 0U;
    for(auto it=subg_op_map.begin();
        it!=subg_op_map.end();
        it++, op_list_idx++) {

      op_list += it->first;
      op_list += "(";
      op_list += to_string(it->second);
      op_list += ")";
      if(op_list_idx != subg_op_map.size() - 1)
        op_list += ", ";
    }
    UNI_LOG_DEBUG_INFO << "subg_op_list("
      << subg_op_map.size() << "): " << op_list << endl;
    UNI_LOG_DEBUG_INFO << "subg_ops: "
      << CPUOPBase::subg_ops / 1000000000. << "Gops" << endl;
  }

private:
  list<CPUOPBase*> ops_;

  CPUOPState op_state_;
};

} // namespace cpu
} // namespace vart
