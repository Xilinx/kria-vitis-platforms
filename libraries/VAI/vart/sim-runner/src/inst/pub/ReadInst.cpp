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
#include "ReadInst.hpp"
#include "InstBase.hpp"

// constructor and deconstructor
ReadInst::ReadInst(const vector<string>& inst_vec) {
  inst_fmt_ = INST_FMT_AC_DESP_YES;
  auto isa_version = SimCfg::Instance().get_isa_version();
  vector<vector<bool>> minus_mask;
  vector<vector<uint32_t>> field_mask;
  if (isa_version == int(DPUVersion::DPUV2)) {
    // initial mask
    Helper<DPUVersion::DPUV2>::ReadInstInit(minus_mask, field_mask);
    for (auto& inst : inst_vec) {
      vector<string> inst_str;
      vector<uint32_t> inst_val;
      Helper<DPUVersion::DPUV2>::GenInst(inst, inst_fmt_, minus_mask, field_mask,
                                        inst_str, inst_val, v_inst_mc_);
      v_inst_str_.emplace_back(inst_str);
      v_inst_val_.emplace_back(inst_val);
    }
  } else if (isa_version == int(DPUVersion::DPUV3E)) {
    // initial mask
    Helper<DPUVersion::DPUV3E>::ReadInstInit(minus_mask, field_mask);
    for (auto& inst : inst_vec) {
      vector<string> inst_str;
      vector<uint32_t> inst_val;
      Helper<DPUVersion::DPUV3E>::GenInst(inst, inst_fmt_, minus_mask,
                                         field_mask, inst_str, inst_val,
                                         v_inst_mc_);
      v_inst_str_.emplace_back(inst_str);
      v_inst_val_.emplace_back(inst_val);
    }
  } else if (isa_version == int(DPUVersion::DPUV3ME)) {
    // initial mask
    Helper<DPUVersion::DPUV3ME>::ReadInstInit(minus_mask, field_mask);
    for (auto& inst : inst_vec) {
      vector<string> inst_str;
      vector<uint32_t> inst_val;
      Helper<DPUVersion::DPUV3ME>::GenInst(inst, inst_fmt_, minus_mask, field_mask,
                                        inst_str, inst_val, v_inst_mc_);
      v_inst_str_.emplace_back(inst_str);
      v_inst_val_.emplace_back(inst_val);
    }
  } else if (isa_version == int(DPUVersion::DPU4F)) {
    // initial mask
    Helper<DPUVersion::DPU4F>::ReadInstInit(minus_mask, field_mask);
    for (auto& inst : inst_vec) {
      vector<string> inst_str;
      vector<uint32_t> inst_val;
      Helper<DPUVersion::DPU4F>::GenInst(inst, inst_fmt_, minus_mask,
                                           field_mask, inst_str, inst_val,
                                           v_inst_mc_);
      v_inst_str_.emplace_back(inst_str);
      v_inst_val_.emplace_back(inst_val);
    }
  } else if (isa_version == int(DPUVersion::XVDPU)){
    Helper<DPUVersion::XVDPU>::ReadInstInit(minus_mask, field_mask);
    for (auto& inst : inst_vec) {
      vector<string> inst_str;
      vector<uint32_t> inst_val;
      Helper<DPUVersion::XVDPU>::GenInst(inst, inst_fmt_, minus_mask,
                                           field_mask, inst_str, inst_val,
                                           v_inst_mc_);
      v_inst_str_.emplace_back(inst_str);
      v_inst_val_.emplace_back(inst_val);
    }
  } else if (isa_version == int(DPUVersion::DPUV4E)) {
    Helper<DPUVersion::DPUV4E>::ReadInstInit(minus_mask, field_mask);
    for (auto& inst : inst_vec) {
      vector<string> inst_str;
      vector<uint32_t> inst_val;
      Helper<DPUVersion::DPUV4E>::GenInst(inst, inst_fmt_, minus_mask,
                                         field_mask, inst_str, inst_val,
                                         v_inst_mc_);
      v_inst_str_.emplace_back(inst_str);
      v_inst_val_.emplace_back(inst_val);
    }
  } else{
    cout << "ISA version not support now." << endl;
    UNI_LOG_FATAL(SIM_OUT_OF_RANGE) << "unsupported ISA version";
  }
}
