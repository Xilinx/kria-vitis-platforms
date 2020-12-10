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
#include "Layer.hpp"
#include "InstFactory.hpp"
#include "ReadInst.hpp"

Layer::Layer(int netid, int layerid, string debug_path,
             const vector<string>& inst_vec)
    : netid_(netid),
      layerid_(layerid),
      debug_path_(debug_path),
      inst_vec_(inst_vec) {}

Layer::~Layer() {}

void Layer::Run() {
  init();

  ReadInst ac(inst_vec_);
  inst_num_ = ac.GetInstNum();
  for (int inst_id = 0; inst_id < inst_num_; inst_id++) {
    auto inst_str = ac.GetInstStr(inst_id);
    auto inst_val = ac.GetInstVal(inst_id);
    add_inst(inst_id, inst_str, inst_val);
  }

  print_inst_num();

  exec_by_order();

  // save machine code
  mc_vec_ = ac.GetMC();
  SaveMC(DATA_FMT_BIN);
  SaveMC(DATA_FMT_HEX_CONT_SMALLEND_DDRADDR);
}

void Layer::SaveMC(int fmt) {
  string save_name = debug_path_ + "/" + "instr_mc";

  if (fmt == DATA_FMT_HEX_CONT_BIGEND) { // save standard format MC
    // open file to save machine code
    fstream fmc(save_name + ".txt.old", std::ios::out | std::ios::trunc);
    for (auto &ele : mc_vec_) {
      fmc << hex << setw(8) << setfill('0') << ele << endl;
    }
    fmc.close();
  } else if (fmt == DATA_FMT_HEX_CONT_SMALLEND_DDRADDR) { // save ADDR : VALUE
                                                          // format MC
    while (mc_vec_.size() % 4 != 0) {
      mc_vec_.push_back(0);
    }

    Util::SaveHexContSmallEndDDRAddr(
        save_name + ".txt", reinterpret_cast<char *>(mc_vec_.data()),
        mc_vec_.size() * 4, SimCfg::Instance().get_hp_width(), 0, 8);
  } else if (fmt == DATA_FMT_BIN) {
    Util::SaveBin(save_name + ".bin", reinterpret_cast<char *>(mc_vec_.data()),
                  mc_vec_.size() * sizeof(uint32_t));
  } else {
    UNI_LOG_FATAL(SIM_OUT_OF_RANGE) << "Not support SaveMC format " << fmt << endl;
  }
}

size_t Layer::GetInstNum(int inst_type) {
  // return all inst num
  if (inst_type >= SimCfg::Instance().get_inst_type_max())
    return v_inst_.size();

  // return sigle classification inst num
  return InstBase::GetInstNum(inst_type);
}

// private funcs

void Layer::init() {
  // set current layer's netid, layerid, debug path
  InstBase::StaticInit(debug_path_); // important, must do this
  InstBase::SetNetID(netid_);
  InstBase::SetLayerID(layerid_);

  // todo
  mc_vec_.clear();
}

void Layer::exec_by_order() {
  for (int i = 0; i < inst_num_; i++) {
    if (SimCfg::Instance().get_dump_instr()) {
      v_inst_[i]->PrintInst();
    }
    v_inst_[i]->Exec();
  }
}

void Layer::add_inst(int inst_id, vector<string> &inst_str,
                     vector<uint32_t> &inst_val) {
  shared_ptr<InstBase> pinst =
      InstFactory::CreateInst(inst_id, inst_str, inst_val);
  v_inst_.push_back(pinst);
}

void Layer::print_inst_num() {
  bool debug_glob = SimCfg::Instance().get_debug_enable();
  if (!debug_glob)
    return;

  string inst_statistics =
      "INST NUMBER: " +
      to_string(GetInstNum(SimCfg::Instance().get_inst_type_max())) + "\n";
  cout << inst_statistics;
  UNI_LOG_INFO << inst_statistics;

  for (int inst_type = 0; inst_type < SimCfg::Instance().get_inst_type_max();
       inst_type++) {
    string statistics = SimCfg::Instance().get_inst_type_name(inst_type);
    statistics += ": ";
    statistics += to_string(GetInstNum(inst_type));
    if(inst_type == SimCfg::Instance().get_inst_type_max() -1) {
      statistics += "\n";
    }
    cout<< statistics << endl;
    UNI_LOG_INFO << statistics << endl;
  }
}
