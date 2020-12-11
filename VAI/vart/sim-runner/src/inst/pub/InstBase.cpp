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
#include "InstBase.hpp"

// static variable initialization
int InstBase::netid_ = 0;
int InstBase::layerid_ = 0;
int InstBase::hp_width_;
string InstBase::root_debug_path_;
uint32_t InstBase::reserved_ = 0;
vector<uint32_t> InstBase::inst_num_;

void InstBase::StaticInit(const string debug_path) {
  // init static class member value
  netid_ = 0;
  layerid_ = 0;
  hp_width_ = SimCfg::Instance().get_hp_width();
  root_debug_path_ = debug_path;
  reserved_ = 0;

  auto inst_type_max = SimCfg::Instance().get_inst_type_max();
  inst_num_.resize(inst_type_max);
  std::fill(inst_num_.begin(), inst_num_.end(), 0);
}

// constructor and deconstructor
InstBase::InstBase(int inst_type, int instid, vector<string>& inst_str,
                   vector<uint32_t>& inst_val)
    : inst_type_(inst_type),
      instid_(instid),
      field_str_(inst_str),
      field_val_(inst_val),
      dpdon_(field_str_[1]),
      dpdby_(field_str_[2]) {
  // update inst number and instid_classify
  instid_classify_ = inst_num_[inst_type_]++;

  // debug_ = SimCfg::Instance().get_debug_enable();
  debug_ = SimCfg::Instance().get_debug_enable() &&
           (SimCfg::Instance().get_debug_layer() ||
            SimCfg::Instance().get_debug_single_layer());
}

InstBase::~InstBase() {}

void InstBase::PrintInst() {
  bool debug_glob = SimCfg::Instance().get_debug_enable();
  if (!debug_glob) return;

  
  cout<< "[" << netid_ << ", " << layerid_ << ", " << instid_ << ", "
            << instid_classify_ << "] " << GetInstStr(INST_FMT_AC_DESP_YES)
            << endl;
  UNI_LOG_INFO << "[" << netid_ << ", " << layerid_ << ", " << instid_ << ", "
            << instid_classify_ << "] " << GetInstStr(INST_FMT_AC_DESP_YES)
            << endl;
}
