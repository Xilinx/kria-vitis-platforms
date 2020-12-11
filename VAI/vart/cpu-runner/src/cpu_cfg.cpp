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
#include "load_cfg.hpp"
#include "cpu_cfg.hpp"

namespace vart {
namespace cpu {

// constructor and deconstructor
CPUCfg::CPUCfg(const string& config_fname)
    : config_fname_(config_fname),
      cpu_run_mode_(CPURunMode::GEMM_THREAD),
      save_bin_(true),
      save_txt_(false),
      save_weights_fmt_(WeightsFmt::OC_KH_KW_IC),
      debug_(false),
      debug_path_("./log/") {}

/* -------------------------------------------------
 * get functions
 * -------------------------------------------------
 */
// global get func

/* -------------------------------------------------
 * set functions
 * -------------------------------------------------
 */

void CPUCfg::enable_debug() { debug_ = true; }
void CPUCfg::disable_debug() { debug_ = false; }
bool CPUCfg::debug() { return debug_; }

string CPUCfg::get_xmodel_run_mode_str() const {
  if(xmodel_run_mode_ == XModelRunMode::CPU) {
    return "CPU";
  } else if(xmodel_run_mode_ == XModelRunMode::SIM) {
    return "SIM";
  } else if(xmodel_run_mode_ == XModelRunMode::DPU) {
    return "DPU";
  } else {
    return "Unknown";
  }
}

void CPUCfg::set_xmodel_run_mode(const string& xmodel_run_mode_str) {
  if(xmodel_run_mode_str=="CPU" || xmodel_run_mode_str=="cpu") {
    xmodel_run_mode_ = XModelRunMode::CPU;
  } else if(xmodel_run_mode_str=="SIM" || xmodel_run_mode_str=="sim") {
    xmodel_run_mode_ = XModelRunMode::SIM;
  } else if(xmodel_run_mode_str=="DPU" || xmodel_run_mode_str=="runtime") {
    xmodel_run_mode_ = XModelRunMode::DPU;
  } else {
    UNI_LOG_ERROR(VAIE_NOT_SUPPORT) << "Not support run mode " << xmodel_run_mode_str << endl;
    abort();
  }
}

void CPUCfg::set_cpu_run_mode(int cpu_run_mode) {
  if (cpu_run_mode == 0) {
    cpu_run_mode_ = CPURunMode::NORMAL;
  } else if (cpu_run_mode == 1) {
    cpu_run_mode_ = CPURunMode::NORMAL_THREAD;
  } else if (cpu_run_mode == 2) {
    cpu_run_mode_ = CPURunMode::GEMM;
  } else if (cpu_run_mode == 3) {
    cpu_run_mode_ = CPURunMode::GEMM_THREAD;
  } else if (cpu_run_mode == 10) {
    cpu_run_mode_ = CPURunMode::PRINT_PARAM;
  } else if (cpu_run_mode == 11) {
    cpu_run_mode_ = CPURunMode::CHECK_PARAM;
  } else if (cpu_run_mode == 12) {
    cpu_run_mode_ = CPURunMode::WORKLOAD;
  } else if (cpu_run_mode == 13) {
    cpu_run_mode_ = CPURunMode::OPLIST;
  } else if (cpu_run_mode == 14) {
    cpu_run_mode_ = CPURunMode::READ;
  } else {
    cpu_run_mode_ = CPURunMode::NORMAL;
  }
}

void CPUCfg::set_save_weights_fmt(WeightsFmt fmt) {
  UNI_LOG_CHECK(fmt==WeightsFmt::OC_KH_KW_IC
      || fmt==WeightsFmt::KH_KW_IC_OC, VART_INVALID_VALUE);

  save_weights_fmt_ = fmt;
}

void CPUCfg::set_save_weights_fmt(const string& fmt) {
  if(fmt == "ohwi") {
    save_weights_fmt_ = WeightsFmt::OC_KH_KW_IC;
  } else if(fmt == "hwio") {
    save_weights_fmt_ = WeightsFmt::KH_KW_IC_OC;
  } else {
    UNI_LOG_ERROR(VART_NOT_SUPPORT) << ", err fmt: " << fmt << endl;
    abort();
  }
}

void CPUCfg::set_debug_path(const string& debug_path) {
  debug_path_ = debug_path;
  if (debug_path.back() != '/') {
    debug_path_ += "/";
  }
}

void CPUCfg::LoadConfig() {
  vart::cpu::LoadCfg lc(config_fname_);
  const string HEX_CHARS = "xXabcdefABCDEF";

  model_name_ = lc.GetValue<string>("model_name", "", false);
  input_name_ = lc.GetValue<string>("input_name", "", false);
  input_random_flag_ = lc.GetValue<bool>("input_random_flag", false, false);
  input_random_seed_ = lc.GetValue<uint64_t>("input_random_seed", 0, false);

  auto xmodel_run_mode_str = lc.GetValue<string>("xmodel_run_mode", "CPU", true);
  set_xmodel_run_mode(xmodel_run_mode_str);

  auto cpu_run_mode_int = lc.GetValue<int>("cpu_run_mode", 1, true);
  set_cpu_run_mode(cpu_run_mode_int);

  debug_ = lc.GetValue<bool>("global", true, true);
  save_txt_ = lc.GetValue<bool>("save_txt", false, true);
  save_bin_ = lc.GetValue<bool>("save_bin", true, true);
  auto save_weights_fmt_str = lc.GetValue<string>("save_weights_fmt", "ohwi", true);
  set_save_weights_fmt(save_weights_fmt_str);
}

void CPUCfg::PrintConfig() {
  string print;

  print = "============= " + config_fname_ + " ============= \n";
  print += "model_name: " + model_name_ + "\n";
  print += "input_name: " + input_name_ + "\n";
  print += "input_random_flag: " + vart::cpu::Bool2Str(input_random_flag_) + "\n";
  print += "input_random_seed: " + std::to_string(input_random_seed_) + "\n";
  if(xmodel_run_mode_ == XModelRunMode::CPU) {
    print += "xmodel_run_mode: CPU\n";
  } else if(xmodel_run_mode_ == XModelRunMode::SIM) {
    print += "xmodel_run_mode: SIM\n";
  } else if(xmodel_run_mode_ == XModelRunMode::DPU) {
    print += "xmodel_run_mode: DPU\n";
  }
  // print += "cpu_run_mode: " + std::to_string(cpu_run_mode_) + "\n";

  print += "global_debug: " + vart::cpu::Bool2Str(debug_) + "\n";
  print += "save_txt: " + vart::cpu::Bool2Str(save_txt_) + "\n";
  print += "save_bin: " + vart::cpu::Bool2Str(save_bin_) + "\n";
  // print += "save_weights_fmt: " + save_weights_fmt_ + "\n";

  // output debug info at last
  UNI_LOG_DEBUG_INFO << endl << print << endl;
}

}  // namespace cpu
}  // namespace vart
