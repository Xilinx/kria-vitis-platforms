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

#include "cpu_std_inc.hpp"
#include "cpu_util.hpp"

#define VART_DEBUG            \
  CPUCfg::Instance().debug()

#define VART_DEBUG_GO_ON()    \
  if(!VART_DEBUG)             \
    return;

#define CPU_RUN_MODE          \
  CPUCfg::Instance().get_cpu_run_mode()

namespace vart {
namespace cpu {

using std::cout;
using std::endl;
using std::string;
using std::vector;

enum class XModelRunMode {
  CPU,
  SIM,
  DPU
};

enum class CPURunMode : int {
  NORMAL = 0,
  NORMAL_THREAD = 1,
  GEMM = 2,
  GEMM_THREAD = 3,
  PRINT_PARAM = 10,
  CHECK_PARAM = 11,
  WORKLOAD = 12,
  OPLIST = 13,
  READ = 14,
};

enum class WeightsFmt {
  OC_KH_KW_IC,
  KH_KW_IC_OC,
};

class CPUCfg {
 private:
  CPUCfg(const string& config_fname = "./config/config.txt");
  ~CPUCfg() = default;
  CPUCfg(const CPUCfg&) = default;
  CPUCfg& operator=(const CPUCfg&) = default;

 public:
  // return a global CPUCfg instance object
  static CPUCfg& Instance(const string& config_fname = "./config/config.txt") {
    static CPUCfg cfg(config_fname);
    return cfg;
  }

 public:  // public get funcs
  // register info
  string get_model_name() const { return model_name_; }
  string get_input_name() const { return input_name_; }
  bool get_input_random_flag() const { return input_random_flag_; }
  uint64_t get_input_random_seed() const { return input_random_seed_; }

  string get_xmodel_run_mode_str() const;
  XModelRunMode get_xmodel_run_mode() const { return xmodel_run_mode_; }
  CPURunMode get_cpu_run_mode() const { return cpu_run_mode_; }

  bool get_save_bin() const { return save_bin_; }
  bool get_save_txt() const { return save_txt_; }
  WeightsFmt get_save_weights_fmt() const { return save_weights_fmt_; }
  string get_debug_path() const { return debug_path_; }

 public:  // public set funcs
  void set_model_name(const string& model_name) { model_name_ = model_name; }
  void set_input_name(const string& input_name) { input_name_ = input_name; }
  void set_input_random_flag(bool input_random_flag) { input_random_flag_ = input_random_flag; }
  void set_input_random_seed(uint64_t input_random_seed) { input_random_seed_ = input_random_seed; }

  void set_xmodel_run_mode(const string& xmodel_run_mode_str);
  void set_cpu_run_mode(CPURunMode cpu_run_mode) { cpu_run_mode_ = cpu_run_mode; }
  void set_cpu_run_mode(int cpu_run_mode);

  void set_save_bin(bool save_bin) { save_bin_ = save_bin; }
  void set_save_txt(bool save_txt) { save_txt_ = save_txt; }
  void set_save_weights_fmt(WeightsFmt fmt);
  void set_save_weights_fmt(const string& fmt);

  void enable_debug();
  void disable_debug();
  bool debug();
  void set_debug_path(const string& debug_path);

 public:  // other public funcs
  void Touch() { }
  void LoadConfig();
  void PrintConfig();

 private:
  // not by read config file
  string config_fname_;

  string model_name_; // full path
  string input_name_; // full path
  bool input_random_flag_;
  uint64_t input_random_seed_;

  XModelRunMode xmodel_run_mode_;
  CPURunMode cpu_run_mode_;

  bool save_bin_;
  bool save_txt_;
  WeightsFmt save_weights_fmt_;

  bool debug_;
  string debug_path_;
};

}  // namespace cpu
}  // namespace vart
