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

#include <array>
#include <string>
#include "ArchCfg.hpp"
#include "UniLog/UniLog.hpp"

enum InstFmt {
  INST_FMT_MIN = 0,
  INST_FMT_BIN = INST_FMT_MIN,
  INST_FMT_AC_DESP_NO,
  INST_FMT_AC_DESP_YES,
  INST_FMT_MAX,
};

/**
 * @brief configuration interface for simulator
 */
class SimCfg {
 public:
  enum DbgInstType {
    DBG_INSTR_LOAD,
    DBG_INSTR_SAVE,
    DBG_INSTR_CONV,
    DBG_INSTR_POOL,
    DBG_INSTR_DWCONV,
    DBG_INSTR_ELEW,
    DBG_INSTR_THD,
    DBG_INSTR_DUMP,
    DBG_INSTR_MAX
  };

 public:
  /**
   * @brief Static function to create a singleton SimCfg object
   * @param config_name Name of the configuration file
   * @return a global SimCfg instance object
   */
  static SimCfg& Instance(
      const std::string& config_fname = "./config/SIMCfg.txt") {
    static SimCfg sc(config_fname);
    return sc;
  }

 public:
  // access interface
  int get_hp_width() const;
  void set_fmap_bank_group();
  int get_fmap_bank_group() const;
  std::string get_run_mode() const;
  void set_run_mode(std::string);
  int get_isa_version() const;
  int get_inst_type_max() const;
  std::string get_inst_type_name(int idx) const;
  void set_isa_version(const std::string isa);

  bool get_bank_init() const;
  int get_bank_init_type() const;
  std::string get_bank_init_file() const;

  void disable_debug();
  void enable_debug();
  bool get_debug_enable() const;
  void set_debug_single_layer(bool flag);
  int get_batch_index() const;
  bool get_debug_layer() const;
  bool get_debug_single_layer() const;
  string get_debug_layer_name() const;
  std::string get_debug_path() const;
  bool get_ddr_dump_net() const;
  bool get_ddr_dump_layer() const;
  bool get_ddr_dump_init() const;
  bool get_ddr_dump_end() const;
  bool get_ddr_dump_split() const;
  int get_ddr_dump_format() const;
  int get_layer_dump_format() const;
  bool get_dump_instr() const;
  bool get_debug_instr(int type) const;
  void set_batch_index(int idx);
  void set_debug_path(const std::string path);
  bool get_gen_aie_data() const;
  int get_xvdpu_conv_remain() const;
  void set_xvdpu_conv_remain(int conv_remain);
  int get_xvdpu_conv_num() const;
  void set_xvdpu_conv_num(int conv_num );

 private:
  SimCfg(const std::string& config_fname);
  SimCfg(const SimCfg&) = delete;
  SimCfg& operator=(const SimCfg&) = delete;

 private:
  // arch
  int hp_width_;
  int fmap_bank_group_;
  std::string run_mode_;
  std::string isa_version_;

  // bank
  bool bank_init_;
  int bank_init_type_;
  std::string bank_init_file_;

  // debug
  bool debug_;
  std::string debug_path_;
  int batch_index_;
  bool debug_layer_;
  bool debug_single_layer_;
  string debug_layer_name_;
  int layer_dump_format_;
  bool ddr_dump_net_;
  bool ddr_dump_layer_;
  bool ddr_dump_init_;
  bool ddr_dump_end_;
  bool ddr_dump_split_;
  int ddr_dump_format_;
  bool dump_inst_;
  std::array<bool, SimCfg::DBG_INSTR_MAX> debug_inst_;
  bool gen_aie_data_{false};
  int xvdpu_conv_remain_{0};
  int xvdpu_conv_num_{0};
};
