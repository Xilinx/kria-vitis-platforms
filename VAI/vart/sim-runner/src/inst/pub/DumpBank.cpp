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
#include "DumpBank.hpp"

// constructor and deconstructor
template <>
DumpBank<DPUVersion::DPUV2>::DumpBank(int inst_type, int instid,
                                     vector<string>& inst_str,
                                     vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_DUMP);
  Helper<DPUVersion::DPUV2>::InstInit(inst_type_, root_debug_path_, debug_,
                                     opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPUV2>::inst_table;
  // get dumpbank instruction field value
  save_name_ = field_str_[TABLE::DUMPBANK_FIELD_SAVE_NAME];
  save_fmt_ = field_val_[TABLE::DUMPBANK_FIELD_SAVE_FMT];
  bank_start_ = field_val_[TABLE::DUMPBANK_FIELD_BANK_START];
  bank_dump_num_ = field_val_[TABLE::DUMPBANK_FIELD_BANK_NUM];
}

template <>
DumpBank<DPUVersion::DPUV3E>::DumpBank(int inst_type, int instid,
                                      vector<string>& inst_str,
                                      vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_DUMP);
  Helper<DPUVersion::DPUV3E>::InstInit(inst_type_, root_debug_path_, debug_,
                                      opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPUV3E>::inst_table;
  // get dumpbank instruction field value
  save_name_ = field_str_[TABLE::DUMPBANK_FIELD_SAVE_NAME];
  save_fmt_ = field_val_[TABLE::DUMPBANK_FIELD_SAVE_FMT];
  bank_start_ = field_val_[TABLE::DUMPBANK_FIELD_BANK_START];
  bank_dump_num_ = field_val_[TABLE::DUMPBANK_FIELD_BANK_NUM];
}

template <>
DumpBank<DPUVersion::DPUV3ME>::DumpBank(int inst_type, int instid,
                                     vector<string>& inst_str,
                                     vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_DUMP);
  Helper<DPUVersion::DPUV3ME>::InstInit(inst_type_, root_debug_path_, debug_,
                                     opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPUV3ME>::inst_table;
  // get dumpbank instruction field value
  save_name_ = field_str_[TABLE::DUMPBANK_FIELD_SAVE_NAME];
  save_fmt_ = field_val_[TABLE::DUMPBANK_FIELD_SAVE_FMT];
  bank_start_ = field_val_[TABLE::DUMPBANK_FIELD_BANK_START];
  bank_dump_num_ = field_val_[TABLE::DUMPBANK_FIELD_BANK_NUM];
}

template <>
DumpBank<DPUVersion::XVDPU>::DumpBank(int inst_type, int instid,
                                      vector<string>& inst_str,
                                      vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_DUMP);
  Helper<DPUVersion::XVDPU>::InstInit(inst_type_, root_debug_path_, debug_,
                                      opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::XVDPU>::inst_table;
  // get dumpbank instruction field value
  save_name_ = field_str_[TABLE::DUMPBANK_FIELD_SAVE_NAME];
  save_fmt_ = field_val_[TABLE::DUMPBANK_FIELD_SAVE_FMT];
  bank_start_ = field_val_[TABLE::DUMPBANK_FIELD_BANK_START];
  bank_dump_num_ = field_val_[TABLE::DUMPBANK_FIELD_BANK_NUM];
}

template <>
DumpBank<DPUVersion::DPU4F>::DumpBank(int inst_type, int instid,
                                        vector<string>& inst_str,
                                        vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_DUMP);
  Helper<DPUVersion::DPU4F>::InstInit(inst_type_, root_debug_path_, debug_,
                                        opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPU4F>::inst_table;
  // get dumpbank instruction field value
  save_name_ = field_str_[TABLE::DUMPBANK_FIELD_SAVE_NAME];
  save_fmt_ = field_val_[TABLE::DUMPBANK_FIELD_SAVE_FMT];
  bank_start_ = field_val_[TABLE::DUMPBANK_FIELD_BANK_START];
  bank_dump_num_ = field_val_[TABLE::DUMPBANK_FIELD_BANK_NUM];
}

template <DPUVersion dv>
DumpBank<dv>::~DumpBank() {}

template <DPUVersion dv>
void DumpBank<dv>::Exec() {
  dump_bank_range();
}

template <DPUVersion T>
void DumpBank<T>::dump_bank_range() {
  if ((!debug_) || (bank_dump_num_ == 0)) return;

  // get save file name
  stringstream ss;
  string fname;
  if (save_name_ == "nullptr") {
    ss.clear();
    ss.str("");
    ss << debug_path_ << "bank_"
       << "net_" << netid_ << "_layer_" << layerid_ << "_inst_" << instid_
       << "_start_" << bank_start_ << "_banknum_" << bank_dump_num_;
    ss >> fname;
    fname += Util::GetFileNameSuffix(save_fmt_);
  } else {
    fname = InstBase::root_debug_path_ + "/" + "batch_" +
            std::to_string(SimCfg::Instance().get_batch_index()) + "_" +
            save_name_.substr(save_name_.find_last_of("/") + 1);
  }

  // save bank contents into file according different format bank by bank
  auto start = static_cast<uint32_t>(bank_start_);
  auto bank_id_max = Buffer<DPU_DATA_TYPE>::Instance().GetBankIDMax();
  for (auto i = 0; i < bank_dump_num_; i++) {
    // get current bank's contents
    auto bank_id = start;
    while (Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id) == nullptr) {
      bank_id++;
      UNI_LOG_CHECK(bank_id <= bank_id_max, SIM_OUT_OF_RANGE)
          << "Bank id:" << bank_id << " is out of range " << bank_id_max;
    }
    auto bank = Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id);
    auto bank_h = bank->GetH();
    auto bank_w = bank->GetW();
    auto bank_size = bank_h * bank_w;

    vector<DPU_DATA_TYPE> buf(bank_size);
    bank->Read(0, bank_size, buf.data());

    // save bank's contents
    int line_size = (save_fmt_ <= DATA_FMT_DEC) ? 1 : bank_w;
    if (save_fmt_ <= DATA_FMT_HEX_CONT_BIGEND) {
      Util::SaveData(save_fmt_, fname, buf.data(), buf.size(), line_size,
                     SM_APPEND);
    } else if (save_fmt_ == DATA_FMT_HEX_CONT_SMALLEND_BANKADDR) {
      Util::SaveHexContSmallEndBankAddr(
          fname, reinterpret_cast<const char*>(buf.data()),
          buf.size() * sizeof(DPU_DATA_TYPE), line_size * sizeof(DPU_DATA_TYPE),
          bank_h, bank_id, 0, SM_APPEND);
    } else if (save_fmt_ == DATA_FMT_HEX_CONT_BIGEND_BANKADDR) {
      Util::SaveHexContBigEndBankAddr(
          fname, reinterpret_cast<const char*>(buf.data()),
          buf.size() * sizeof(DPU_DATA_TYPE), line_size * sizeof(DPU_DATA_TYPE),
          bank_h, bank_id, 0, SM_APPEND);
    } else {
      UNI_LOG_FATAL(SIM_OUT_OF_RANGE)
          << "Unsupported bank dump format: " << save_fmt_ << "!";
    }
    start = bank_id + 1;
  }
}

template <DPUVersion T>
void DumpBank<T>::GenInstStr(int inst_fmt) {
  ac_ = Helper<T>::GetInstStr(inst_type_, inst_fmt, dpdon_, dpdby_, field_str_);
}
