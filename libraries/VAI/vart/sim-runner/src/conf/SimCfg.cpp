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
#include "SimCfg.hpp"
#include "inst/InstTable.hpp"
#include "util/Util.hpp"

SimCfg::SimCfg(const std::string& config_fname) {
  // arch
  hp_width_ = 16;
  run_mode_ = "debug";
  isa_version_ = "v1.4.0";
  bank_init_ = false;
  bank_init_type_ = 8;
  bank_init_file_ = "./config/bank_init.txt";
  debug_ = true;
  debug_path_ = ".log";
  batch_index_ = 0;
  debug_layer_ = false;
  debug_single_layer_ = false;
  debug_layer_name_ = "";
  layer_dump_format_ = 0;
  ddr_dump_net_ = false;
  ddr_dump_layer_ = true;
  ddr_dump_init_ = true;
  ddr_dump_end_ = true;
  ddr_dump_split_ = true;
  ddr_dump_format_ = 0;
  dump_inst_ = false;
  debug_inst_ = {false, false, false, false, false, false, false, false};
  gen_aie_data_ = false;
  xvdpu_conv_remain_ = 0;
  xvdpu_conv_num_ = 0;

  UNI_LOG_INFO << "SimCfg initializing...";
  ifstream fd(config_fname);
  if (fd.fail()) {
    UNI_LOG_WARNING << "can't open file :" << config_fname;
  } else {
    // parse the configuration file
    std::unordered_map<std::string, std::string> cfg;
    while (!fd.eof()) {
      std::string line;
      std::getline(fd, line);
      line = Util::Trim(line);
      if (line.size() == 0 || line[0] == '#') continue;
      auto vec = Util::Split(line, ":");
      if (vec.size() == 1) continue;
      UNI_LOG_CHECK(vec.size() == 2, SIM_OUT_OF_RANGE)
          << "config line :" << line;
      if (vec[1][0] == '[') {
        std::string temp = vec[1].substr(1, vec[1].size() - 2);
        cfg[vec[0]] = temp;
      } else
        cfg[vec[0]] = vec[1];
    }

    // fill each field
    for (auto& item : cfg) {
      std::string key = item.first;
      if (key == "bank_init_file")
        bank_init_file_ = item.second;
      else if (key == "debug_path")
        debug_path_ = item.second;
      else if (key == "isa_version")
        isa_version_ = item.second;
      else if (key == "run_mode")
        run_mode_ = item.second;

      else if (key == "hp_width")
        hp_width_ = stoi(item.second);
      //      else if (key == "fmap_bank_group")
      // fmap_bank_group_ = stoi(item.second);
      else if (key == "bank_init_type")
        bank_init_type_ = stoi(item.second);
      else if (key == "layer_dump_format")
        layer_dump_format_ = stoi(item.second);
      else if (key == "ddr_dump_format")
        ddr_dump_format_ = stoi(item.second);
      else if (key == "batch_index")
        batch_index_ = stoi(item.second);

      else if (key == "bank_init")
        bank_init_ = Util::Str2Bool(item.second);
      else if (key == "debug")
        debug_ = Util::Str2Bool(item.second);
      else if (key == "debug_layer")
        debug_layer_ = Util::Str2Bool(item.second);
      else if (key == "debug_single_layer")
        debug_single_layer_ = Util::Str2Bool(item.second);
      else if (key == "debug_layer_name")
        debug_layer_name_ = item.second;
      else if (key == "ddr_dump_net")
        ddr_dump_net_ = Util::Str2Bool(item.second);
      else if (key == "ddr_dump_layer")
        ddr_dump_layer_ = Util::Str2Bool(item.second);
      else if (key == "ddr_dump_init")
        ddr_dump_init_ = Util::Str2Bool(item.second);
      else if (key == "ddr_dump_end")
        ddr_dump_end_ = Util::Str2Bool(item.second);
      else if (key == "ddr_dump_split")
        ddr_dump_split_ = Util::Str2Bool(item.second);
      else if (key == "dump_inst")
        dump_inst_ = Util::Str2Bool(item.second);

      else if (key == "debug_inst") {
        auto vec = Util::Split(item.second);
        std::transform(
            vec.begin(), vec.end(), debug_inst_.begin(),
            [](std::string& str) -> bool { return Util::Str2Bool(str); });
      } else if (key == "gen_aie_data") {
        gen_aie_data_ = Util::Str2Bool(item.second);
      }
    }
  }
}

int SimCfg::get_hp_width() const { return hp_width_; }
int SimCfg::get_fmap_bank_group() const { return fmap_bank_group_; }
std::string SimCfg::get_run_mode() const { return run_mode_; }
void SimCfg::set_run_mode(std::string run_mode) { run_mode_ = run_mode; }

int SimCfg::get_isa_version() const {
  int rlt = 0;
  if (isa_version_ == "DPUCZDX8G") {
    rlt = int(DPUVersion::DPUV2);
  } else if (isa_version_ == "DPUCAHX8H") {
    rlt = int(DPUVersion::DPUV3E);
  } else if (isa_version_ == "DPUCZDI4G") {
    rlt = int(DPUVersion::DPU4F);
  } else if (isa_version_ == "DPUCAHX8L") {
    rlt = int(DPUVersion::DPUV3ME);
  } else if (isa_version_ == "DPUCVDX8G") {
    rlt = int(DPUVersion::XVDPU);
  } else if (isa_version_ == "DPUCVDX8H") {
    rlt = int(DPUVersion::DPUV4E);
  } else {
    UNI_LOG_FATAL(SIM_OUT_OF_RANGE)
        << "Not supported isa version, default is DPUCAHX8H";
  }
  return rlt;
}

int SimCfg::get_inst_type_max() const {
  int rlt = 0;
  if (isa_version_ == "DPUCZDX8G") {
    rlt = TableInterface<DPUVersion::DPUV2>::inst_table::INST_TYPE_MAX;
  } else if (isa_version_ == "DPUCAHX8H") {
    rlt = TableInterface<DPUVersion::DPUV3E>::inst_table::INST_TYPE_MAX;
  } else if (isa_version_ == "DPUCZDI4G") {
    rlt = TableInterface<DPUVersion::DPU4F>::inst_table::INST_TYPE_MAX;
  } else if (isa_version_ == "DPUCAHX8L") {
    rlt = TableInterface<DPUVersion::DPUV3ME>::inst_table::INST_TYPE_MAX;
  } else if (isa_version_ == "DPUCVDX8G") {
    rlt = TableInterface<DPUVersion::XVDPU>::inst_table::INST_TYPE_MAX;
  } else if (isa_version_ == "DPUCVDX8H") {
    rlt = TableInterface<DPUVersion::DPUV4E>::inst_table::INST_TYPE_MAX;
  } else {
    UNI_LOG_FATAL(SIM_OUT_OF_RANGE) << "not supported isa version";
  }
  return rlt;
}

std::string SimCfg::get_inst_type_name(int idx) const {
  std::string rlt;
  if (isa_version_ == "DPUCZDX8G") {
    rlt = TableInterface<DPUVersion::DPUV2>::inst_table::InstName[idx];
  } else if (isa_version_ == "DPUCAHX8H") {
    rlt = TableInterface<DPUVersion::DPUV3E>::inst_table::InstName[idx];
  } else if (isa_version_ == "DPUCZDI4G") {
    rlt = TableInterface<DPUVersion::DPU4F>::inst_table::InstName[idx];
  } else if (isa_version_ == "DPUCAHX8L") {
    rlt = TableInterface<DPUVersion::DPUV3ME>::inst_table::InstName[idx];
  } else if (isa_version_ == "DPUCVDX8G") {
    rlt = TableInterface<DPUVersion::XVDPU>::inst_table::InstName[idx];
  } else if (isa_version_ == "DPUCVDX8H") {
    rlt = TableInterface<DPUVersion::DPUV4E>::inst_table::InstName[idx];
  } else {
    UNI_LOG_FATAL(SIM_OUT_OF_RANGE) << "not supported isa version";
  }
  return rlt;
}

void SimCfg::set_isa_version(const std::string isa) { isa_version_ = isa; }

void SimCfg::set_fmap_bank_group() {
  // init fmap_bank_group_
  fmap_bank_group_ = 0;
  auto bank_group_size = ArchCfg::Instance().get_param().bank_group_size();
  for (auto idx_bank_group = 0; idx_bank_group < bank_group_size;
       idx_bank_group++) {
    auto type =
        ArchCfg::Instance().get_param().bank_group(idx_bank_group).type();
    if (type != "Param") fmap_bank_group_++;
  }
}

bool SimCfg::get_bank_init() const { return bank_init_; }
int SimCfg::get_bank_init_type() const { return bank_init_type_; }
std::string SimCfg::get_bank_init_file() const { return bank_init_file_; }

bool SimCfg::get_debug_enable() const { return debug_; }
void SimCfg::disable_debug() { debug_ = false; }
void SimCfg::enable_debug() { debug_ = true; }
void SimCfg::set_debug_single_layer(bool flag) { debug_single_layer_ = flag; }
int SimCfg::get_batch_index() const { return batch_index_; }
bool SimCfg::get_debug_layer() const { return debug_layer_; }
bool SimCfg::get_debug_single_layer() const { return debug_single_layer_; }
string SimCfg::get_debug_layer_name() const { return debug_layer_name_; }
std::string SimCfg::get_debug_path() const { return debug_path_; }
bool SimCfg::get_ddr_dump_net() const { return ddr_dump_net_; }
bool SimCfg::get_ddr_dump_layer() const { return ddr_dump_layer_; }
bool SimCfg::get_ddr_dump_init() const { return ddr_dump_init_; }
bool SimCfg::get_ddr_dump_end() const { return ddr_dump_end_; }
bool SimCfg::get_ddr_dump_split() const { return ddr_dump_split_; }
int SimCfg::get_layer_dump_format() const { return layer_dump_format_; }
int SimCfg::get_ddr_dump_format() const { return ddr_dump_format_; }
void SimCfg::set_batch_index(int idx) { batch_index_ = idx; }
void SimCfg::set_debug_path(const std::string path) { debug_path_ = path; }

bool SimCfg::get_dump_instr() const { return dump_inst_; }
bool SimCfg::get_debug_instr(int type) const {
  UNI_LOG_CHECK(type < SimCfg::DBG_INSTR_MAX, SIM_OUT_OF_RANGE)
      << "no inst type supported by debug: " << type;
  return debug_inst_[type];
}
bool SimCfg::get_gen_aie_data() const { return gen_aie_data_; }
int SimCfg::get_xvdpu_conv_remain() const { return xvdpu_conv_remain_; }
void SimCfg::set_xvdpu_conv_remain(int conv_remain){ xvdpu_conv_remain_ = conv_remain;}
int SimCfg::get_xvdpu_conv_num() const { return xvdpu_conv_num_; }
void SimCfg::set_xvdpu_conv_num(int conv_num){ xvdpu_conv_num_ = conv_num;}
