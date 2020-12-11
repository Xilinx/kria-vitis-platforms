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

#include "cpu_runner.hpp"
#include "cpu_op_base.hpp"
#include "cpu_reg_func.hpp"
#include "cpu_tb_factory.hpp"
#include "cpu_tensor_buffer.hpp"
#include "op_schedule.hpp"

#include "check_param_visitor.hpp"
#include "print_param_visitor.hpp"
#include "read_visitor.hpp"
#include "run_visitor.hpp"
#include "save_visitor.hpp"
#include "workload_visitor.hpp"
#include "oplist_visitor.hpp"

namespace vart {
namespace cpu {

CPURunner::CPURunner(const xir::Subgraph* subgraph)
    : subg_(subgraph), g_(subg_->get_graph()) {
  if (VART_DEBUG) {
    UNI_LOG_DEBUG_INFO << "Constructing CPURunner ..." << endl;
  }

  print_xir_subg(subgraph);

  xir_ops_ = const_cast<xir::Subgraph*>(subg_)->topological_sort();

  // build all op's tbs and inference ops
  create_ops_and_tbs();
}

CPURunner::~CPURunner() = default;
CPURunner::CPURunner(CPURunner&&) = default;
CPURunner& CPURunner::operator=(CPURunner&&) = default;

std::pair<uint32_t, int> CPURunner::execute_async(
    const std::vector<TensorBuffer*>& subg_input_tbs,
    const std::vector<TensorBuffer*>& subg_output_tbs) {
  set_subg_input_tbs(subg_input_tbs);
  set_subg_output_tbs(subg_output_tbs);

  auto debug_path = CPUCfg::Instance().get_debug_path();
  SaveBin(debug_path + CPUOPBase::SUBG_DIFF_SCRIPT,
          CPUOPBase::SUBG_DIFF_SCRIPT_HEADER.data(),
          CPUOPBase::SUBG_DIFF_SCRIPT_HEADER.size(), SM_TRUNC);

  fut_ = async(std::launch::async, &CPURunner::run, this);

  static int jobid = 0;
  return std::make_pair(jobid++, 0);
}

int CPURunner::wait(int jobid, int ms) {
  // NOTE: not use jobid now
  std::chrono::milliseconds t = (ms == 0)
                                    ? std::chrono::milliseconds(0xFFFFFFFF)
                                    : std::chrono::milliseconds(ms);

  auto status = fut_.wait_for(t);
  if (status == std::future_status::timeout) {
    UNI_LOG_ERROR(VART_EXEC_ERROR)
        << "Executing timeout(time limit is " << ms << " ms)" << endl;
    abort();
  } else if (status == std::future_status::deferred) {
    UNI_LOG_ERROR(VART_EXEC_ERROR) << "Executing deferred!" << endl;
    abort();
  } else if (status == std::future_status::ready) {
    UNI_LOG_DEBUG_INFO << "Executing over!" << endl;
  }

  return 0;
}

std::vector<const xir::Tensor*> CPURunner::get_input_tensors() {
  std::vector<const xir::Tensor*> inputs;
  for (auto tb : get_input_tbs()) {
    inputs.push_back(tb->get_tensor());
  }
  return inputs;
}

std::vector<const xir::Tensor*> CPURunner::get_output_tensors() {
  std::vector<const xir::Tensor*> outputs;
  for (auto tb : get_output_tbs()) {
    outputs.push_back(tb->get_tensor());
  }
  return outputs;
}

void CPURunner::run() {
  if (VART_DEBUG) {
    UNI_LOG_DEBUG_INFO << "CPURunner begin to run ..." << endl;
    PRINT_DIVIDING_LINE();
  }

  // You can install supported visitors in vis folder,
  // each visitor can do special work, of course you
  // can extend your own visitor.
  auto s = make_unique<OPSchedule>(cpu_ops_);
  if (CPU_RUN_MODE == CPURunMode::PRINT_PARAM) {
    s->install(PrintParamVisitor::make());
  } else if (CPU_RUN_MODE == CPURunMode::CHECK_PARAM) {
    s->install(CheckParamVisitor::make());
  } else if (CPU_RUN_MODE == CPURunMode::WORKLOAD) {
    s->install(WorkloadVisitor::make());
  } else if (CPU_RUN_MODE == CPURunMode::OPLIST) {
    s->install(OpListVisitor::make());
  } else if (CPU_RUN_MODE == CPURunMode::READ) {
    set_subg_input_data();
    s->install(ReadVisitor::make());
    set_subg_output_data();
  } else {
    set_subg_input_data();
    s->install(RunVisitor::make());
    set_subg_output_data();
  }
}

std::vector<TensorBuffer*> CPURunner::get_input_tbs() {
  std::vector<TensorBuffer*> input_tbs;
  auto input_ops = get_input_ops(subg_);

  for (auto* op : input_ops) {
    auto op_name = op->get_name();
    TensorBuffer* tb = CPUTBFactory::Instance().get_by_op_name(op_name);
    input_tbs.push_back(tb);
  }

  return input_tbs;
}

std::vector<TensorBuffer*> CPURunner::get_output_tbs() {
  std::vector<TensorBuffer*> output_tbs;
  auto output_ops = get_output_ops(subg_);

  for (auto* op : output_ops) {
    auto op_name = op->get_name();
    TensorBuffer* tb = CPUTBFactory::Instance().get_by_op_name(op_name);
    output_tbs.push_back(tb);
  }

  return output_tbs;
}

TensorBuffer* CPURunner::get_tb(const string& tensor_name) {
  return CPUTBFactory::Instance().get_by_tensor_name(tensor_name);
}

void CPURunner::set_subg_input_tbs(
    const std::vector<TensorBuffer*>& subg_input_tbs) {
  outer_subg_input_tbs_ = subg_input_tbs;
}

void CPURunner::set_subg_output_tbs(
    const std::vector<TensorBuffer*>& subg_output_tbs) {
  outer_subg_output_tbs_ = subg_output_tbs;
}

void CPURunner::set_subg_input_data() {
  auto input_tbs = get_input_tbs();
  UNI_LOG_CHECK(input_tbs.size() == outer_subg_input_tbs_.size(),
                VART_SIZE_ERROR)
      << ", " << input_tbs.size() << " != " << outer_subg_input_tbs_.size();
  for (auto i = 0U; i < outer_subg_input_tbs_.size(); i++) {
    auto* outer_tb = outer_subg_input_tbs_[i];
    auto* tensor = outer_tb->get_tensor();
    auto* inner_tb =
        CPUTBFactory::Instance().get_by_tensor_name(tensor->get_name());

    inner_tb->copy_data_in(TBPTR(outer_tb));
  }
}

void CPURunner::set_subg_output_data() {
  auto output_tbs = get_output_tbs();
  UNI_LOG_CHECK(output_tbs.size() == outer_subg_output_tbs_.size(),
                VART_SIZE_ERROR)
      << ", " << output_tbs.size() << " != " << outer_subg_output_tbs_.size();
  for (auto i = 0U; i < outer_subg_output_tbs_.size(); i++) {
    auto* outer_tb = outer_subg_output_tbs_[i];
    auto* tensor = outer_tb->get_tensor();
    auto* inner_tb =
        CPUTBFactory::Instance().get_by_tensor_name(tensor->get_name());

    inner_tb->copy_data_out(TBPTR(outer_tb));
  }
}

void CPURunner::create_ops_and_tbs() {
  auto idx = 0;
  for (auto* xir_op : xir_ops_) {
    const auto op_name = xir_op->get_name();
    const auto op_type = xir_op->get_type();
    xir_op->set_attr<int>(CPU_RUN_ID, idx);

    IMapTBs_t inputs;
    const auto m = get_input_map(xir_op);
    for (const auto& e : m) {
      CPUTBPtrVec_t value;
      for (auto k = 0U; k < e.second.size(); k++) {
        value.emplace_back(CPUTBFactory::Instance().create_or_get(e.second[k]));
      }
      inputs[e.first] = value;
    }

    CPUTBPtr_t output = CPUTBFactory::Instance().create_or_get(xir_op);
    auto* cpu_op = (CPURegFunc::instance()->get_register_func(op_type))(
        subg_, xir_op, inputs, output);
    cpu_ops_.push_back(cpu_op);
    idx++;
  }
}

string CPURunner::get_name() const { return subg_->get_name(); }

string CPURunner::get_device() const {
  if (subg_->has_attr("device")) return subg_->get_attr<string>("device");

  return "CPU";
}

int CPURunner::get_op_num() const { return subg_->get_op_num(); }

int CPURunner::get_op_num(const string& type) const {
  map<string, int> subg_op_map;
  for (const auto* xir_op : xir_ops_) {
    auto op_type = xir_op->get_type();
    if (subg_op_map.find(op_type) == subg_op_map.end()) {
      subg_op_map[op_type] = 1;
    } else {
      subg_op_map.at(op_type)++;
    }
  }

  if (subg_op_map.find(type) != subg_op_map.end()) return subg_op_map.at(type);
  return 0;
}

int CPURunner::get_op_type_num() const {
  map<string, int> subg_op_map;
  for (const auto* xir_op : xir_ops_) {
    auto op_type = xir_op->get_type();
    if (subg_op_map.find(op_type) == subg_op_map.end()) {
      subg_op_map[op_type] = 1;
    }
  }
  return subg_op_map.size();
}

string CPURunner::get_op_type_list() const {
  map<string, int> subg_op_map;
  for (const auto* xir_op : xir_ops_) {
    auto op_type = xir_op->get_type();
    if (subg_op_map.find(op_type) == subg_op_map.end()) {
      subg_op_map[op_type] = 1;
    } else {
      subg_op_map.at(op_type)++;
    }
  }

  string op_list;
  auto op_list_idx = 0U;
  for (auto it = subg_op_map.begin(); it != subg_op_map.end();
       it++, op_list_idx++) {
    op_list += it->first;
    op_list += "(";
    op_list += to_string(it->second);
    op_list += ")";
    if (op_list_idx != subg_op_map.size() - 1) op_list += ", ";
  }

  return op_list;
}

list<CPUOPBase*> CPURunner::get_cpu_ops() const { return cpu_ops_; }

CPUOPBase* CPURunner::get_cpu_op(const string& op_name) const {
  // if map is empty, build it first
  if (cpu_op_map_.empty()) {
    for (auto* cpu_op : cpu_ops_) {
      cpu_op_map_[cpu_op->get_xir_op()->get_name()] = cpu_op;
    }
  }

  if (cpu_op_map_.find(op_name) != cpu_op_map_.end())
    return cpu_op_map_.at(op_name);

  // TODO: use log mechanism to abort
  abort();
  return nullptr;
}

xir::Op* CPURunner::get_xir_op(const string& op_name) const {
  // if map is empty, build it first
  if (xir_op_map_.empty()) {
    for (auto* xir_op : xir_ops_) {
      xir_op_map_[xir_op->get_name()] = xir_op;
    }
  }

  if (xir_op_map_.find(op_name) != xir_op_map_.end())
    return xir_op_map_.at(op_name);

  // TODO: use log mechanism to abort
  abort();
  return nullptr;
}

}  // namespace cpu
}  // namespace vart

extern "C" vart::Runner* create_runner(const xir::Subgraph* subgraph) {
  auto ret = std::make_unique<vart::cpu::CPURunner>(subgraph);
  return ret.release();
}
