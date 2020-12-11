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
#include "sim_runner.hpp"
#include "ArchCfg.hpp"
#include "SimCfg.hpp"
#include "buffer/DDR.hpp"
#include "inst/pub/Layer.hpp"
#include "inst/pub/ReadInst.hpp"
#include "vart/mm/host_flat_tensor_buffer.hpp"

#include <cstring>
#include <vitis/ai/target_factory.hpp>
#include <xir/graph/graph.hpp>
#include <xir/util/tool_function.hpp>

namespace vart {
namespace sim {

SimRunner::SimRunner(const xir::Subgraph* subgraph)
    : Runner(), subgraph_(subgraph), layer_id_(0), batch_idx_(0) {
  UNI_LOG_CHECK(subgraph_->get_attr<std::string>("device") == "DPU",
                SIM_OUT_OF_RANGE);

  auto& arch = ArchCfg::Instance().get_param();
  UNI_LOG_CHECK(subgraph_->has_attr("dpu_name"), SIM_ATTR_ERROR);
  auto dpu_name = subgraph_->get_attr<std::string>("dpu_name");
  arch = vitis::ai::target_factory()->create(dpu_name);
  auto isa_version = arch.type();

  SimCfg::Instance().set_fmap_bank_group();
  SimCfg::Instance().set_isa_version(isa_version);
  if (SimCfg::Instance().get_debug_enable()) {
    Util::ChkFolder(SimCfg::Instance().get_debug_path(), false);
  }

  // initial weight/bias in ddr
  Buffer<DPU_DATA_TYPE>::Instance();
  DDR::Instance().Initial(subgraph_);

  // get input/output nodes, and create vitis tensor from xir tensor
  for (auto input_tensor : subgraph_->get_input_tensors()) {
    inputs_.push_back(input_tensor);
    tensor_map.emplace(input_tensor->get_name(), input_tensor);
  }
  for (auto output_tensor : subgraph_->get_output_tensors()) {
    outputs_.push_back(output_tensor);
    tensor_map.emplace(output_tensor->get_name(), output_tensor);
  }
}

SimRunner::SimRunner() : layer_id_(0), batch_idx_(0) {
  if (SimCfg::Instance().get_debug_enable()) {
    Util::ChkFolder(SimCfg::Instance().get_debug_path(), true);
  }
  // set ISA version
  auto isa_version = ArchCfg::Instance().get_param().type();
  SimCfg::Instance().set_fmap_bank_group();
  SimCfg::Instance().set_isa_version(isa_version);
  DDR::Instance().Initial();
  Buffer<DPU_DATA_TYPE>::Instance();

  if (SimCfg::Instance().get_debug_enable() &&
      SimCfg::Instance().get_ddr_dump_init()) {
    std::string ddr_file = SimCfg::Instance().get_debug_path() + "/ddr_init";
    DDR::Instance().SaveDDR(ddr_file, SimCfg::Instance().get_ddr_dump_format());
  }
}

void SimRunner::run_ac(const string& inst_file) {
  auto ac_code = get_ac(inst_file);
  std::string layer_dbg_path = SimCfg::Instance().get_debug_path();
  Layer layer(0, layer_id_++, layer_dbg_path, ac_code);
  layer.Run();
  if (SimCfg::Instance().get_debug_enable() &&
      SimCfg::Instance().get_ddr_dump_end()) {
    std::string ddr_file = SimCfg::Instance().get_debug_path() + "/ddr_end";
    DDR::Instance().SaveDDR(ddr_file, SimCfg::Instance().get_ddr_dump_format());
  }
}

vector<string> SimRunner::get_ac(const string& inst_file) {
  ifstream ifs(inst_file);
  if (ifs.fail()) {
    UNI_LOG_FATAL(SIM_FILE_OPEN_FAILED)
        << "Open AC file error: " << inst_file << endl;
  }

  vector<string> ac_code;

  // read all kinds of insts, put them in a vector
  while (!ifs.eof()) {
    string line;

    // get one line from file
    getline(ifs, line);
    line = Util::Trim(line);
    if (line.size() == 0) {  // empty line
      continue;
    } else if (line[0] == '#') {  // annotation line
      continue;
    }

    ac_code.emplace_back(line);
  }
  return ac_code;
}

void SimRunner::run_subgraph(const xir::Subgraph* subg) {
  if (!subg->get_children_num()) {
    if (subg->has_attr("ac_code")) {
      run_superlayer(subg);
    }
  } else {
    UNI_LOG_CHECK(subg->has_attr("children_topological_sort"), SIM_ATTR_ERROR)
        << "This subgraph has no children_topological_sort attr!";
    auto children_topological_sort =
        subg->get_attr<std::vector<std::string>>("children_topological_sort");
    auto children = subg->get_children();
    for (auto& subgraph_name : children_topological_sort) {
      for (auto it = children.begin(); it != children.end(); it++) {
        if ((*it)->get_name() == subgraph_name) {
          run_subgraph(*it);
          break;
        }
      }
    }
  }
}

static const xir::Subgraph* get_device_subgraph(const xir::Subgraph* subgraph) {
  if (subgraph->has_attr("device")) {
    return subgraph;
  }
  if (subgraph->is_root()) {
    return nullptr;
  } else {
    // search the parent
    return get_device_subgraph(subgraph->get_parent());
  }
}

static std::unique_ptr<vart::mm::HostFlatTensorBuffer> create_internal_tb(
    const xir::Subgraph* subgraph, const xir::Tensor* tensor) {
  auto device_subgraph = get_device_subgraph(subgraph);
  UNI_LOG_CHECK(device_subgraph != nullptr, SIM_FIND_DEVICE_SUBGRAPH_FAILED)
      << "Can't find " << subgraph->to_string() << "'s device info.";
  auto tensor_producer = tensor->get_producer();
  const xir::Tensor* internal_tensor{nullptr};
  if (device_subgraph->has_op(tensor_producer)) {
    // if this tensor is in inside the current device
    if (tensor->has_attr("ddr_addr")) {
      // if it has DDR space
      internal_tensor = tensor;
    } else {
      // if it does't have DDR space
      if (tensor_producer->get_type() == "download") {
        // if it's an download, get it's inputs tensor
        internal_tensor = tensor_producer->get_input_tensor("input", 0);
      }
    }
  } else {
    // if the tensor is outside the current device, try to find it's fanout
    // upload op in the current device
    for (auto& fanout_op : tensor_producer->get_fanout_ops()) {
      if ((device_subgraph->has_op(fanout_op)) &&
          (fanout_op->get_type() == "upload")) {
        internal_tensor = fanout_op->get_output_tensor();
      }
    }
  }
  UNI_LOG_CHECK(nullptr != internal_tensor, SIM_TENSOR_NOT_IN_DDR)
      << "Can't find " << tensor->to_string() << " in "
      << device_subgraph->to_string() << "'s DDR space.";
  return std::make_unique<vart::mm::HostFlatTensorBuffer>(internal_tensor);
}

static void load_tensor_to_DDR(const xir::Subgraph* subgraph,
                               mm::HostFlatTensorBuffer* buffer,
                               int32_t idx_batch) {
  auto tensor = buffer->get_tensor();
  UNI_LOG_CHECK(tensor->get_attr<std::int32_t>("location") == 1,
                SIM_TENSOR_LOCATION_ERROR);
  auto reg_id = tensor->get_attr<std::int32_t>("reg_id");
  auto ddr_addr = tensor->get_attr<std::int32_t>("ddr_addr");
  auto batch_size = static_cast<uint32_t>(std::ceil(buffer->strides[0] / 8.f));
  auto ptr_src =
      reinterpret_cast<char*>(buffer->data().first) + idx_batch * batch_size;
  auto ptr_dst = DDR::Instance().GetAddr(reg_id, ddr_addr);
  UNI_LOG_INFO << "Load tensor " << tensor->get_name() << "(size=" << batch_size
               << "B)"
               << " to DDR(id=" << reg_id << ", addr=" << ddr_addr << ")";
  std::memcpy(ptr_dst, ptr_src, batch_size);
}

static void save_tensor_from_DDR(const xir::Subgraph* subgraph,
                                 mm::HostFlatTensorBuffer* buffer,
                                 int32_t idx_batch) {
  auto tensor = buffer->get_tensor();
  auto tensor_producer = tensor->get_producer();
  UNI_LOG_CHECK(get_device_subgraph(subgraph)->has_op(tensor_producer),
                SIM_TENSOR_NOT_IN_DDR)
      << "Can't find " << tensor->to_string() << " in " << subgraph->to_string()
      << ".";
  UNI_LOG_CHECK(tensor->get_attr<std::int32_t>("location") == 1,
                SIM_TENSOR_LOCATION_ERROR);
  auto reg_id = tensor->get_attr<std::int32_t>("reg_id");
  auto ddr_addr = tensor->get_attr<std::int32_t>("ddr_addr");
  auto batch_size = static_cast<uint32_t>(std::ceil(buffer->strides[0] / 8.f));
  auto ptr_dst =
      reinterpret_cast<char*>(buffer->data().first) + idx_batch * batch_size;
  auto ptr_src = DDR::Instance().GetAddr(reg_id, ddr_addr);
  UNI_LOG_INFO << "Save tensor " << tensor->get_name() << "(size=" << batch_size
               << "B)"
               << " from DDR(id=" << reg_id << ", addr=" << ddr_addr << ")";
  std::memcpy(ptr_dst, ptr_src, batch_size);
}

std::pair<uint32_t, int> SimRunner::execute_async(
    const std::vector<TensorBuffer*>& input,
    const std::vector<TensorBuffer*>& output) {
  std::string reg_file = SimCfg::Instance().get_debug_path() + "/case.cfg";
  DDR::Instance().SaveReg(reg_file);

  std::vector<int> batches;
  for (auto& tb : input) {
    auto vtensor = tb->get_tensor();
    batches.push_back(vtensor->get_shape().at(0));
  }
  UNI_LOG_CHECK(std::adjacent_find(batches.begin(), batches.end(),
                                   std::not_equal_to<>()) == batches.end(),
                SIM_TENSOR_DIM_ERROR)
      << "the batch size for all input tensors is not identical";

  for (int i = 0; i < batches[0]; i++) {
    // copy input data from TensorBuffer to DDR
    for (auto tb : input) {
      auto ibuf = create_internal_tb(this->subgraph_, tb->get_tensor());
      vart::mm::tensorbuffer_copy(
          static_cast<vart::mm::HostFlatTensorBuffer*>(tb), ibuf.get());
      load_tensor_to_DDR(this->subgraph_, ibuf.get(), i);
    }
    run();
    // copy output data from DDR to TensorBuffer
    for (auto tb : output) {
      auto ibuf = create_internal_tb(this->subgraph_, tb->get_tensor());
      save_tensor_from_DDR(this->subgraph_, ibuf.get(), i);
      vart::mm::tensorbuffer_copy(
          ibuf.get(), static_cast<vart::mm::HostFlatTensorBuffer*>(tb));
    }
    batch_idx_++;
  }
  return std::pair<uint32_t, int>{0, 0};
}

void SimRunner::run() {
  UNI_LOG_INFO << "run batch " << batch_idx_ << "...";
  SimCfg::Instance().set_batch_index(batch_idx_);

  if (SimCfg::Instance().get_debug_enable() &&
      SimCfg::Instance().get_ddr_dump_net() &&
      SimCfg::Instance().get_ddr_dump_init()) {
    std::string ddr_file = SimCfg::Instance().get_debug_path() + "/" +
                           "batch_" + std::to_string(batch_idx_) + "_ddr_init";
    DDR::Instance().SaveDDR(ddr_file, SimCfg::Instance().get_ddr_dump_format());
  }

  if (SimCfg::Instance().get_run_mode() == "release") {
    run_superlayer(subgraph_);
  } else {
    run_subgraph(subgraph_);
  }

  if (SimCfg::Instance().get_debug_enable() &&
      SimCfg::Instance().get_ddr_dump_net() &&
      SimCfg::Instance().get_ddr_dump_end()) {
    std::string ddr_file = SimCfg::Instance().get_debug_path() + "/" +
                           "batch_" + std::to_string(batch_idx_) + "_ddr_end";
    DDR::Instance().SaveDDR(ddr_file, SimCfg::Instance().get_ddr_dump_format());
  }

  layer_id_ = 0;
}

void SimRunner::run_superlayer(const xir::Subgraph* super) {
  auto super_name = xir::remove_xfix(super->get_name());
  for_each(super_name.begin(), super_name.end(), [](char& c) {
    if (c == '/') c = '_';
  });
  UNI_LOG_INFO << super_name;
  std::string layer_dbg_path = SimCfg::Instance().get_debug_path() + super_name;
  Util::ChkFolder(layer_dbg_path, false);

  auto ac_raw = super->get_attr<std::vector<std::string>>("ac_code");
  auto has_mc_code = super->has_attr("mc_code");
  if (has_mc_code) {
    auto& mc_in_xmodel = super->get_attr<std::vector<char>>("mc_code");
    ReadInst readhelper(ac_raw);
    auto mc_from_sim = readhelper.GetMC();
    UNI_LOG_CHECK(mc_from_sim.size() * 4 == mc_in_xmodel.size(),
                  SIM_PARAMETER_FAILED)
        << super_name
        << " 's mc_code size in xmodel not equal to sim-runner generated"
        << " in xmodel: " << mc_in_xmodel.size()
        << ", sim-runner generated: " << mc_from_sim.size();
    char one_word[4];
    for (auto idx_w = 0u; idx_w < mc_from_sim.size(); idx_w++) {
      memcpy(one_word, mc_in_xmodel.data() + idx_w * 4, 4);
      uint32_t xmodel_mc_val = *(reinterpret_cast<uint32_t*>(one_word));
      UNI_LOG_CHECK(xmodel_mc_val == mc_from_sim[idx_w], SIM_PARAMETER_FAILED)
          << super_name << ", "
          << " line " << idx_w
          << ": mc_code in xmodel diff with sim-runner generate one."
          << " xmodel is 0x" << hex << xmodel_mc_val
          << " while sim-runner is 0x" << hex << mc_from_sim[idx_w];
    }
  }

  std::vector<std::string> acp_raw;
  if (super->has_attr("ac_code_preload")) {
    acp_raw = super->get_attr<std::vector<std::string>>("ac_code_preload");
  }
  // erase last END instr, concat ac_code_preload and ac_code
  std::vector<std::string> ac_code;
  for (auto idx_acp = 0U; idx_acp < acp_raw.size(); idx_acp++) {
    if (idx_acp == acp_raw.size() - 1) continue;
    std::string acp_line = acp_raw[idx_acp];
    if (idx_acp == acp_raw.size() - 2) {
      const int pos_dpdby = 10;
      acp_line.replace(pos_dpdby, 4, "0001");
    }
    ac_code.push_back(acp_line);
  }

  for (auto& ele : ac_raw) ac_code.push_back(ele);

  //  auto layer_param =
  //  super->get_attr<std::vector<std::string>>("superlayer");

  // dump assemble code and layer parameter
  if (SimCfg::Instance().get_debug_enable() && (batch_idx_ == 0)) {
    std::string ac_file = layer_dbg_path + "/" + "instr_ac.txt";
    fstream fac(ac_file, std::ios::out | std::ios::trunc);
    for (auto& ele : ac_code) fac << ele << std::endl;
    fac.close();

    // std::string lp_file = layer_dbg_path + "/" + "layer_param.txt";
    // fstream flp(lp_file, std::ios::out | std::ios::trunc);
    // for (auto& ele : layer_param) flp << ele << std::endl;
    // flp.close();
  }
  if (SimCfg::Instance().get_debug_enable() &&
      (SimCfg::Instance().get_debug_layer_name() == super_name))
    SimCfg::Instance().set_debug_single_layer(true);

  // dump input nodes' tensor
  if (SimCfg::Instance().get_debug_enable() &&
      (SimCfg::Instance().get_debug_layer() ||
       SimCfg::Instance().get_debug_single_layer())) {
    for (auto tensor : super->get_input_tensors()) {
      std::string name = xir::remove_xfix(tensor->get_name());
      for_each(name.begin(), name.end(), [](char& c) {
        if (c == '/') c = '_';
      });
      std::string dump_file =
          layer_dbg_path + "/" + "batch_" + std::to_string(batch_idx_) + "_" +
          name +
          Util::GetFileNameSuffix(SimCfg::Instance().get_layer_dump_format());

      auto buffer = create_internal_tb(this->subgraph_, tensor);
      save_tensor_from_DDR(this->subgraph_, buffer.get(), batch_idx_);
      mm::dump_to_file(buffer.get(), dump_file);
    }
  }

  // dump ddr for each layer
  if (SimCfg::Instance().get_debug_enable() &&
      SimCfg::Instance().get_ddr_dump_layer() &&
      SimCfg::Instance().get_ddr_dump_init()) {
    std::string ddr_file = layer_dbg_path + "/" + "batch_" +
                           std::to_string(batch_idx_) + "_ddr_init";

    using RegEntry = std::tuple<int32_t, int32_t, int32_t>;
    std::vector<RegEntry> tin;
    for (auto& in_tensor : super->get_input_tensors()) {
      auto tensor = in_tensor;
      if (!tensor->has_attr("ddr_addr")) {
        for (auto& fanout_op : tensor->get_producer()->get_fanout_ops()) {
          if (fanout_op->get_type() == "upload") {
            tensor = fanout_op->get_output_tensor();
            break;
          } else {
            // do nothing
          }
        }  // for fanout
      }    // handle some no ddr addr situation
      int32_t reg_id = tensor->get_attr<std::int32_t>("reg_id");
      int32_t ddr_addr = tensor->get_attr<std::int32_t>("ddr_addr");
      int32_t elem_num = tensor->get_element_num() / tensor->get_shape().at(0);
      tin.push_back(
          std::make_tuple(reg_id, ddr_addr, elem_num * sizeof(DPU_DATA_TYPE)));
    }
    // alignment for result comparison
    for (auto& out_tensor : super->get_output_tensors()) {
      auto tensor = out_tensor;
      if (!tensor->has_attr("ddr_addr")) {
        auto tensor_producer = tensor->get_producer();
        if (tensor_producer->get_type() == "download") {
          // handle the download op
          tensor =
              tensor_producer->get_input_op("input", 0)->get_output_tensor();
        } else {
          // handle the upload op
          for (auto& fanout_op : tensor_producer->get_fanout_ops()) {
            if (fanout_op->get_type() == "upload") {
              tensor = fanout_op->get_output_tensor();
              break;
            } else {
              // do nothing
            }
          }  // for fanout
        }
      }  // handle some no ddr addr situation
      int32_t reg_id = tensor->get_attr<std::int32_t>("reg_id");
      int32_t ddr_addr = tensor->get_attr<std::int32_t>("ddr_addr");
      int32_t elem_num = tensor->get_element_num() / tensor->get_shape().at(0);
      auto hp_width = SimCfg::Instance().get_hp_width();
      auto head_offset = ddr_addr / hp_width * hp_width;
      auto head =
          (head_offset == ddr_addr) ? 0 : (head_offset + hp_width - ddr_addr);
      auto remain = elem_num * sizeof(DPU_DATA_TYPE) - head;
      auto tail_offset = remain / hp_width * hp_width;
      auto tail = (tail_offset == remain) ? 0 : (remain - tail_offset);
      if (head) tin.push_back(std::make_tuple(reg_id, head_offset, hp_width));
      if (tail) {
        if (head)
          tail_offset += head_offset + hp_width;
        else
          tail_offset += head_offset;
        tin.push_back(std::make_tuple(reg_id, tail_offset, hp_width));
      }
    }
    DDR::Instance().SaveDDR(ddr_file, tin,
                            SimCfg::Instance().get_ddr_dump_format(),
                            (batch_idx_ != 0));
  }

  Layer layer(0, layer_id_++, layer_dbg_path, ac_code);
  layer.Run();

  // dump output nodes' tensor
  if (SimCfg::Instance().get_debug_enable() &&
      (SimCfg::Instance().get_debug_layer() ||
       SimCfg::Instance().get_debug_single_layer())) {
    for (auto tensor : super->get_output_tensors()) {
      std::string name = xir::remove_xfix(tensor->get_name());
      for_each(name.begin(), name.end(), [](char& c) {
        if (c == '/') c = '_';
      });
      std::string dump_file =
          layer_dbg_path + "/" + "batch_" + std::to_string(batch_idx_) + "_" +
          name +
          Util::GetFileNameSuffix(SimCfg::Instance().get_layer_dump_format());

      auto buffer = create_internal_tb(this->subgraph_, tensor);
      save_tensor_from_DDR(this->subgraph_, buffer.get(), batch_idx_);
      mm::dump_to_file(buffer.get(), dump_file);
    }
  }

  // dump ddr after execution for each layer
  if (SimCfg::Instance().get_debug_enable() &&
      SimCfg::Instance().get_ddr_dump_layer() &&
      SimCfg::Instance().get_ddr_dump_end()) {
    std::string ddr_file = layer_dbg_path + "/" + "batch_" +
                           std::to_string(batch_idx_) + "_ddr_end";

    using RegEntry = std::tuple<int32_t, int32_t, int32_t>;
    std::vector<RegEntry> tout;
    for (auto& out_tensor : super->get_output_tensors()) {
      auto tensor = out_tensor;
      if (!tensor->has_attr("ddr_addr")) {
        auto tensor_producer = tensor->get_producer();
        if (tensor_producer->get_type() == "download") {
          // handle the download op
          tensor =
              tensor_producer->get_input_op("input", 0)->get_output_tensor();
        } else {
          // handle the upload op
          for (auto& fanout_op : tensor_producer->get_fanout_ops()) {
            if (fanout_op->get_type() == "upload") {
              tensor = fanout_op->get_output_tensor();
              break;
            } else {
              // do nothing
            }
          }  // for fanout
        }
      }  // handle some no ddr addr situation
      int32_t reg_id = tensor->get_attr<std::int32_t>("reg_id");
      int32_t ddr_addr = tensor->get_attr<std::int32_t>("ddr_addr");
      int32_t elem_num = tensor->get_element_num() / tensor->get_shape().at(0);
      tout.push_back(
          std::make_tuple(reg_id, ddr_addr, elem_num * sizeof(DPU_DATA_TYPE)));
    }
    DDR::Instance().SaveDDR(ddr_file, tout,
                            SimCfg::Instance().get_ddr_dump_format(),
                            (batch_idx_ != 0));
  }

  if (SimCfg::Instance().get_debug_enable() &&
      (SimCfg::Instance().get_debug_layer_name() == super_name))
    SimCfg::Instance().set_debug_single_layer(false);
}

std::vector<const xir::Tensor*> SimRunner::get_input_tensors() {
  return inputs_;
}

std::vector<const xir::Tensor*> SimRunner::get_output_tensors() {
  return outputs_;
}

int SimRunner::wait(int jobid, int timeout) { return 0; }

}  // namespace sim
}  // namespace vart

extern "C" vart::Runner* create_runner(const xir::Subgraph* subgraph) {
  auto ret = std::make_unique<vart::sim::SimRunner>(subgraph);
  return ret.release();
}
