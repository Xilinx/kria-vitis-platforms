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

class CPUTBFactory;
class CPUOPBase;

class CPURunner : public Runner {
 public:
  using OpRunInputMap = map<string, vector<shared_ptr<TensorBuffer>>>;
  using OpRunFunc = function<void(OpRunInputMap, shared_ptr<TensorBuffer>)>;

 public:
  explicit CPURunner(const xir::Subgraph* subgraph);
  virtual ~CPURunner();

  CPURunner(const CPURunner&) = delete;
  CPURunner& operator=(const CPURunner&) = delete;
  CPURunner(CPURunner&&);
  CPURunner& operator=(CPURunner&&);

 public:
  virtual std::pair<uint32_t, int>  // pair<jodid, status>
  execute_async(
      const std::vector<TensorBuffer*>& sbug_input_tbs,
      const std::vector<TensorBuffer*>& subg_output_tbs) override final;
  virtual int wait(int jobid, int ms) override final;

  virtual std::vector<const xir::Tensor*> get_input_tensors() override final;
  virtual std::vector<const xir::Tensor*> get_output_tensors() override final;

 public:
  vector<TensorBuffer*> get_input_tbs();
  vector<TensorBuffer*> get_output_tbs();
  TensorBuffer* get_tb(const string& tensor_name);

  void run();
  void set_subg_input_tbs(const std::vector<TensorBuffer*>& sbug_input_tbs);
  void set_subg_output_tbs(const std::vector<TensorBuffer*>& subg_output_tbs);
  void set_subg_input_data();
  void set_subg_output_data();

 public: // mainly used in python api
  string get_name() const;
  string get_device() const;
  int get_op_num() const;
  int get_op_num(const string& type) const;
  int get_op_type_num() const;
  string get_op_type_list() const;

  list<CPUOPBase*> get_cpu_ops() const;
  CPUOPBase* get_cpu_op(const string& op_name) const;
  xir::Op* get_xir_op(const string& op_name) const;

 private:
  void create_ops_and_tbs();

 private:
  const xir::Subgraph* subg_;
  const xir::Graph* g_;

  list<CPUOPBase*> cpu_ops_;
  vector<xir::Op*> xir_ops_;
  mutable map<string, CPUOPBase*> cpu_op_map_;
  mutable map<string, xir::Op*> xir_op_map_;

  std::vector<TensorBuffer*> outer_subg_input_tbs_;
  std::vector<TensorBuffer*> outer_subg_output_tbs_;

  std::future<void> fut_;
};

}  // namespace cpu
}  // namespace vart
