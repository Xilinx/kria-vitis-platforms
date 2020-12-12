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
#include "cpu_runner.hpp"

#include "xmodel_run_sample.hpp"

namespace vart {
namespace cpu {

XModelRunSample::XModelRunSample(std::unique_ptr<xir::Graph> g,
                                 XModelRunMode xmodel_run_mode)
    : g_(std::move(g)) {
  if (VART_DEBUG) {
    UNI_LOG_DEBUG_INFO << "Constructing XModelRunSample, graph name: "
                       << g_->get_name() << ", op num:" << g_->get_op_num()
                       << endl;
  }

  // do shape_infer and print some debug info
  // NOTE: shape_infer is very necessary here
  auto ops = g_->topological_sort();

  // create all runners
  auto *subg_root = g_->get_root_subgraph();
  subg_all_ = subg_root->children_topological_sort();

  auto idx = 0;
  auto iter = subg_all_.begin();
  while (iter != subg_all_.end()) {
    const auto* subg = *iter;
    auto device =
        !subg->has_attr("device") ? "CPU" : subg->get_attr<string>("device");

    if (device != "USER") {
      auto runner = create_runner(idx++, subg, device);

      auto input_tbs = get_subg_input_tbs(subg);
      auto output_tbs = get_subg_output_tbs(subg);
      auto* cpu_runner_ptr = reinterpret_cast<CPURunner*>(runner.get());
      cpu_runner_ptr->set_subg_input_tbs(input_tbs);
      cpu_runner_ptr->set_subg_output_tbs(output_tbs);

      // NOTE: not run USER graph, it's meaningless here
      runners_.push_back(std::move(runner));

      iter++;
    } else {
      iter = subg_all_.erase(iter);
    }
  }
}

void XModelRunSample::run() {
  for (auto i = 0U; i < runners_.size(); i++) {
    auto device = !subg_all_[i]->has_attr("device")
                      ? "CPU"
                      : subg_all_[i]->get_attr<string>("device");
    if(VART_DEBUG) {
      // for debug
      UNI_LOG_DEBUG_INFO << endl << endl << endl;
      UNI_LOG_DEBUG_INFO << "============================================" << endl;
      UNI_LOG_DEBUG_INFO << "Begin to run subgraph " << i << ": " << endl;
      UNI_LOG_DEBUG_INFO << "\tsubg_name: " << subg_all_[i]->get_name() << endl;
      UNI_LOG_DEBUG_INFO << "\tsubg_device: " << device << endl;
      UNI_LOG_DEBUG_INFO << "\tsubg_xmodel_run_mode: "
        << CPUCfg::Instance().get_xmodel_run_mode_str() << endl;
      UNI_LOG_DEBUG_INFO << "\tsubg_op_num: " << subg_all_[i]->get_op_num()
                         << endl;
      UNI_LOG_DEBUG_INFO << "============================================" << endl;
    }

    auto input_tbs = get_subg_input_tbs(subg_all_[i]);
    auto output_tbs = get_subg_output_tbs(subg_all_[i]);

    // for debug
    save_subg_i(i, input_tbs);

    if(runners_[i]) {
      auto rlt_pair = runners_[i]->execute_async(input_tbs, output_tbs);
      runners_[i]->wait(rlt_pair.first, 0);
    }

    // for debug
    save_subg_o(i, output_tbs);
  }
}

void XModelRunSample::create_and_run() {
  auto *subg_root = g_->get_root_subgraph();
  UNI_LOG_CHECK(subg_root != nullptr, VAIE_NULL_PTR);
  subg_all_ = subg_root->children_topological_sort();
  UNI_LOG_CHECK(subg_all_.size() >= 1, VAIE_SIZE_ERROR);

  auto idx = 0;
  for(const auto *subg : subg_all_) {
    UNI_LOG_CHECK(subg != nullptr, VAIE_NULL_PTR);
    auto device = !subg->has_attr("device")
      ? "CPU" : subg->get_attr<string>("device");

    if(VART_DEBUG) {
      // for debug
      UNI_LOG_DEBUG_INFO << endl << endl << endl;
      UNI_LOG_DEBUG_INFO << "============================================" << endl;
      UNI_LOG_DEBUG_INFO << "Begin to run subgraph " << idx << ": " << endl;
      UNI_LOG_DEBUG_INFO << "\tsubg_name: " << subg->get_name() << endl;
      UNI_LOG_DEBUG_INFO << "\tsubg_device: " << device << endl;
      UNI_LOG_DEBUG_INFO << "\tsubg_xmodel_run_mode: "
        << CPUCfg::Instance().get_xmodel_run_mode_str() << endl;
      UNI_LOG_DEBUG_INFO << "\tsubg_op_num: " << subg->get_op_num() << endl;
      UNI_LOG_DEBUG_INFO << "============================================" << endl;
    }

    auto input_tbs = get_subg_input_tbs(subg);
    auto output_tbs = get_subg_output_tbs(subg);

    // for debug
    save_subg_i(idx, input_tbs);

    auto runner = create_runner(idx, subg, device);
    if(runner) {
      auto rlt_pair = runner->execute_async(input_tbs, output_tbs);
      runner->wait(rlt_pair.first, 0);
    }

    // for debug
    save_subg_o(idx, output_tbs);

    idx++;
  }
}

vector<TensorBuffer *> XModelRunSample::get_input_tbs() {
  vector<TensorBuffer *> head_tbs;

  auto head_ops = g_->get_head_ops();
  for(auto *op : head_ops) {
    if(op->get_type()=="const" || op->get_type()=="const-fix")
      continue;

    auto *tb = get_tb_from_op(op);
    head_tbs.push_back(tb);
  }

  return head_tbs;
}

vector<TensorBuffer *> XModelRunSample::get_output_tbs() {
  vector<TensorBuffer *> tail_tbs;

  auto tail_ops = g_->get_tail_ops();
  for(auto *op : tail_ops) {
    if(op->get_type()=="const" || op->get_type()=="const-fix")
      continue;

    auto *tb = get_tb_from_op(op);
    tail_tbs.push_back(tb);
  }

  return tail_tbs;
}

unique_ptr<vart::Runner> XModelRunSample::create_runner(
    int idx,
    const xir::Subgraph* subg,
    const string& device) {

  unique_ptr<vart::Runner> runner;

  auto path_name = "subg_" + to_string(idx) + "_" +
                   CPUCfg::Instance().get_xmodel_run_mode_str() + "_" +
                   tensor_name_2_save_name(subg->get_name());

  for(auto i=0U; i<path_name.size(); i++) {
    if(path_name[i] == '/')
      path_name[i] = '_';
  }
  auto debug_path = "./log/" + path_name + "/";
  ChkFolder(debug_path, false);

  // NOTE: only support user and cpu runner here
  if (device == "USER") {
    CPUCfg::Instance().set_debug_path(debug_path);
    runner = std::make_unique<UserRunner>(subg);
  } else {
    CPUCfg::Instance().set_debug_path(debug_path);
    runner = std::make_unique<CPURunner>(subg);
  }

  if (VART_DEBUG) {
    if (device == "USER") {
      UNI_LOG_DEBUG_INFO << "Creating a User Runner!" << endl;
    } else {
      UNI_LOG_DEBUG_INFO << "Creating a CPU Runner!" << endl;
    }
  }

  return runner;
}

TensorBuffer* XModelRunSample::get_tb_from_op(const xir::Op *op) {
  const auto op_name = op->get_name();

  if(tb_map_.find(op_name) == tb_map_.end()) {
    const auto *tensor = op->get_output_tensor();
    auto tb = CPUTensorBuffer::make(op, tensor);
    tb_map_[op_name] = std::move(tb);
  }

  return tb_map_.at(op_name).get();
}

vector<TensorBuffer *> XModelRunSample::get_subg_input_tbs(
    const xir::Subgraph* subg) {

  std::vector<TensorBuffer *> input_tbs;

  auto input_ops = get_input_ops(subg);

  if(VART_DEBUG) {
    // for debug
    UNI_LOG_DEBUG_INFO << "subg input op num: " << input_ops.size() << endl;
    UNI_LOG_DEBUG_INFO << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
  }

  auto idx = 0;

  for(auto *op : input_ops) {
    auto *tb = get_tb_from_op(op);
    input_tbs.push_back(tb);

    if(VART_DEBUG) {
      // for debug
      UNI_LOG_DEBUG_INFO << "input op " << idx++ << ":" << endl;
      UNI_LOG_DEBUG_INFO << "\top_type: " << op->get_type() << endl;
      UNI_LOG_DEBUG_INFO << "\top_name: " << op->get_name() << endl;
      auto *tensor = tb->get_tensor();
      UNI_LOG_DEBUG_INFO << "\ttensor_name: " << tensor->get_name() << endl;
      UNI_LOG_DEBUG_INFO << "\tdata_type: " << get_data_type_str(tensor) << endl;
      UNI_LOG_DEBUG_INFO << "\ttensor_dims: " << Vec2Str(tensor->get_shape(), ",") << endl;
      auto buf_pair = tb->data(vector<int>(tensor->get_shape().size(), 0));
      UNI_LOG_DEBUG_INFO << "\ttensor_buffer_size: " << buf_pair.second << endl;
    }
  }

  return input_tbs;
}

vector<TensorBuffer *> XModelRunSample::get_subg_output_tbs(
    const xir::Subgraph* subg) {

  std::vector<TensorBuffer *> output_tbs;

  auto output_ops = get_output_ops(subg);

  if (VART_DEBUG) {
    // for debug
    UNI_LOG_DEBUG_INFO << "subg output op num: " << output_ops.size() << endl;
    UNI_LOG_DEBUG_INFO << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
  }

  auto idx = 0;

  for(auto *op : output_ops) {
    auto *tb = get_tb_from_op(op);
    output_tbs.push_back(tb);

    if(VART_DEBUG) {
      // for debug
      UNI_LOG_DEBUG_INFO << "output op " << idx++ << ":" << endl;
      UNI_LOG_DEBUG_INFO << "\top_type: " << op->get_type() << endl;
      UNI_LOG_DEBUG_INFO << "\top_name: " << op->get_name() << endl;
      auto *tensor = tb->get_tensor();
      UNI_LOG_DEBUG_INFO << "\ttensor_name: " << tensor->get_name() << endl;
      UNI_LOG_DEBUG_INFO << "\tdata_type: " << get_data_type_str(tensor) << endl;
      UNI_LOG_DEBUG_INFO << "\ttensor_dims: " << Vec2Str(tensor->get_shape(), ",") << endl;
      auto buf_pair = tb->data(vector<int>(tensor->get_shape().size(), 0));
      UNI_LOG_DEBUG_INFO << "\ttensor_buffer_size: " << buf_pair.second << endl;
    }
  }

  return output_tbs;
}

void XModelRunSample::save_subg_i(int subg_id,
    vector<TensorBuffer *> &input_tbs) {

  auto ifname_prefix = "./log/subg_" + to_string(subg_id) + "_i_"
    + CPUCfg::Instance().get_xmodel_run_mode_str() + "_";
  auto bin_suffix = ".bin";

  // save input data
  for(auto *tb : input_tbs) {
    auto *tensor = tb->get_tensor();
    auto tensor_name = tensor->get_name();
    auto buf_pair = tb->data(vector<int>(tensor->get_shape().size(), 0));

    auto bin_fname = ifname_prefix + tensor_name_2_save_name(tensor_name) + bin_suffix;
    SaveBin(bin_fname, (char *)buf_pair.first,
        buf_pair.second, SM_TRUNC);
  }
}

void XModelRunSample::save_subg_o(int subg_id,
    vector<TensorBuffer *> &output_tbs) {

  auto ofname_prefix = "./log/subg_" + to_string(subg_id) + "_o_"
    + CPUCfg::Instance().get_xmodel_run_mode_str() + "_";
  auto bin_suffix = ".bin";

  // save output data
  for(auto *tb : output_tbs) {
    auto *tensor = tb->get_tensor();
    auto tensor_name = tensor->get_name();
    auto buf_pair = tb->data(vector<int>(tensor->get_shape().size(), 0));

    auto bin_fname = ofname_prefix + tensor_name_2_save_name(tensor_name) + bin_suffix;
    SaveBin(bin_fname, (char *)buf_pair.first,
        buf_pair.second, SM_TRUNC);
  }
}

}  // namespace cpu
}  // namespace vart
