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
#include "DumpDDR.hpp"
#include "buffer/DDR.hpp"
#include "inst/InstTable.hpp"

// constructor and deconstructor
template <>
DumpDDR<DPUVersion::DPUV2>::DumpDDR(int inst_type, int instid,
                                   vector<string>& inst_str,
                                   vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_DUMP);
  Helper<DPUVersion::DPUV2>::InstInit(inst_type_, root_debug_path_, debug_,
                                     opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPUV2>::inst_table;
  // get dumpddr instruction field value
  save_name_ = field_str_[TABLE::DUMPDDR_FIELD_SAVE_NAME];
  save_fmt_ = field_val_[TABLE::DUMPDDR_FIELD_SAVE_FMT];
  ddr_reg_id_ = field_val_[TABLE::DUMPDDR_FIELD_REG_ID];
  ddr_start_ = field_val_[TABLE::DUMPDDR_FIELD_DDR_START];
  ddr_size_ = field_val_[TABLE::DUMPDDR_FIELD_DDR_SIZE];

  UNI_LOG_CHECK(
      (ddr_start_ + ddr_size_) <= DDR::Instance().GetSize(ddr_reg_id_),
      SIM_OUT_OF_RANGE)
      << "Exceed ddr size: ddr_start(" << ddr_start_ << ") + ddr_size("
      << ddr_size_ << ") > ddr_size[" << ddr_reg_id_ << "]("
      << DDR::Instance().GetSize(ddr_reg_id_) << ")!";
}

template <>
DumpDDR<DPUVersion::DPUV3E>::DumpDDR(int inst_type, int instid,
                                    vector<string>& inst_str,
                                    vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_DUMP);
  Helper<DPUVersion::DPUV3E>::InstInit(inst_type_, root_debug_path_, debug_,
                                      opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPUV3E>::inst_table;
  // get dumpddr instruction field value
  save_name_ = field_str_[TABLE::DUMPDDR_FIELD_SAVE_NAME];
  save_fmt_ = field_val_[TABLE::DUMPDDR_FIELD_SAVE_FMT];
  ddr_reg_id_ = field_val_[TABLE::DUMPDDR_FIELD_REG_ID];
  ddr_start_ = field_val_[TABLE::DUMPDDR_FIELD_DDR_START];
  ddr_size_ = field_val_[TABLE::DUMPDDR_FIELD_DDR_SIZE];

  UNI_LOG_CHECK(
      (ddr_start_ + ddr_size_) <= DDR::Instance().GetSize(ddr_reg_id_),
      SIM_OUT_OF_RANGE)
      << "Exceed ddr size: ddr_start(" << ddr_start_ << ") + ddr_size("
      << ddr_size_ << ") > ddr_size[" << ddr_reg_id_ << "]("
      << DDR::Instance().GetSize(ddr_reg_id_) << ")!";
}

template <>
DumpDDR<DPUVersion::DPUV3ME>::DumpDDR(int inst_type, int instid,
                                   vector<string>& inst_str,
                                   vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_DUMP);
  Helper<DPUVersion::DPUV3ME>::InstInit(inst_type_, root_debug_path_, debug_,
                                     opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPUV3ME>::inst_table;
  // get dumpddr instruction field value
  save_name_ = field_str_[TABLE::DUMPDDR_FIELD_SAVE_NAME];
  save_fmt_ = field_val_[TABLE::DUMPDDR_FIELD_SAVE_FMT];
  ddr_reg_id_ = field_val_[TABLE::DUMPDDR_FIELD_REG_ID];
  ddr_start_ = field_val_[TABLE::DUMPDDR_FIELD_DDR_START];
  ddr_size_ = field_val_[TABLE::DUMPDDR_FIELD_DDR_SIZE];

  UNI_LOG_CHECK(
      (ddr_start_ + ddr_size_) <= DDR::Instance().GetSize(ddr_reg_id_),
      SIM_OUT_OF_RANGE)
      << "Exceed ddr size: ddr_start(" << ddr_start_ << ") + ddr_size("
      << ddr_size_ << ") > ddr_size[" << ddr_reg_id_ << "]("
      << DDR::Instance().GetSize(ddr_reg_id_) << ")!";
}

template <>
DumpDDR<DPUVersion::XVDPU>::DumpDDR(int inst_type, int instid,
                                    vector<string>& inst_str,
                                    vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_DUMP);
  Helper<DPUVersion::XVDPU>::InstInit(inst_type_, root_debug_path_, debug_,
                                      opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::XVDPU>::inst_table;
  // get dumpddr instruction field value
  save_name_ = field_str_[TABLE::DUMPDDR_FIELD_SAVE_NAME];
  save_fmt_ = field_val_[TABLE::DUMPDDR_FIELD_SAVE_FMT];
  ddr_reg_id_ = field_val_[TABLE::DUMPDDR_FIELD_REG_ID];
  ddr_start_ = field_val_[TABLE::DUMPDDR_FIELD_DDR_START];
  ddr_size_ = field_val_[TABLE::DUMPDDR_FIELD_DDR_SIZE];

  UNI_LOG_CHECK(
      (ddr_start_ + ddr_size_) <= DDR::Instance().GetSize(ddr_reg_id_),
      SIM_OUT_OF_RANGE)
      << "Exceed ddr size: ddr_start(" << ddr_start_ << ") + ddr_size("
      << ddr_size_ << ") > ddr_size[" << ddr_reg_id_ << "]("
      << DDR::Instance().GetSize(ddr_reg_id_) << ")!";
}

template <>
DumpDDR<DPUVersion::DPU4F>::DumpDDR(int inst_type, int instid,
                                      vector<string>& inst_str,
                                      vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_DUMP);
  Helper<DPUVersion::DPU4F>::InstInit(inst_type_, root_debug_path_, debug_,
                                        opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPU4F>::inst_table;
  // get dumpddr instruction field value
  save_name_ = field_str_[TABLE::DUMPDDR_FIELD_SAVE_NAME];
  save_fmt_ = field_val_[TABLE::DUMPDDR_FIELD_SAVE_FMT];
  ddr_reg_id_ = field_val_[TABLE::DUMPDDR_FIELD_REG_ID];
  ddr_start_ = field_val_[TABLE::DUMPDDR_FIELD_DDR_START];
  ddr_size_ = field_val_[TABLE::DUMPDDR_FIELD_DDR_SIZE];

  UNI_LOG_CHECK(
      (ddr_start_ + ddr_size_) <= DDR::Instance().GetSize(ddr_reg_id_),
      SIM_OUT_OF_RANGE)
      << "Exceed ddr size: ddr_start(" << ddr_start_ << ") + ddr_size("
      << ddr_size_ << ") > ddr_size[" << ddr_reg_id_ << "]("
      << DDR::Instance().GetSize(ddr_reg_id_) << ")!";
}

template <DPUVersion dv>
DumpDDR<dv>::~DumpDDR() {}

template <DPUVersion dv>
void DumpDDR<dv>::Exec() {
  dump_ddr_range();
}

template <DPUVersion dv>
void DumpDDR<dv>::dump_ddr_range() {
  // check validation
  if ((!debug_) || (ddr_size_ == 0)) return;

  // get save file name
  stringstream ss;
  string fname;
  if (save_name_ == "nullptr") {
    ss.clear();
    ss.str("");
    ss << debug_path_ << "ddr_"
       << "net_" << netid_ << "_layer_" << layerid_ << "_inst_" << instid_
       << "_reg_" << ddr_reg_id_ << "_start_" << ddr_start_ << "_size_"
       << ddr_size_;
    ss >> fname;
    fname += Util::GetFileNameSuffix(save_fmt_);
  } else {
    fname = InstBase::root_debug_path_ + "/" + "batch_" +
            std::to_string(SimCfg::Instance().get_batch_index()) + "_" +
            save_name_.substr(save_name_.find_last_of("/") + 1);
  }

  // save ddr data into file
  auto* data_char_ptr = DDR::Instance().GetAddr(ddr_reg_id_, ddr_start_);
  auto* data_dtype_ptr = reinterpret_cast<DPU_DATA_TYPE*>(data_char_ptr);
  auto data_dtype_size = ddr_size_ / sizeof(DPU_DATA_TYPE);

  if (save_fmt_ <= DATA_FMT_HEX_CONT_BIGEND) {
    auto line_size =
        (save_fmt_ == DATA_FMT_DEC) ? 1 : (hp_width_ / sizeof(DPU_DATA_TYPE));
    Util::SaveData(save_fmt_, fname, data_dtype_ptr, data_dtype_size,
                   line_size);
  } else if (save_fmt_ == DATA_FMT_HEX_CONT_SMALLEND_DDRADDR) {
    Util::SaveHexContSmallEndDDRAddr(fname, data_char_ptr, ddr_size_, hp_width_,
                                     ddr_start_, ddr_reg_id_, SM_TRUNC);
  } else if (save_fmt_ == DATA_FMT_HEX_CONT_BIGEND_DDRADDR) {
    Util::SaveHexContBigEndDDRAddr(fname, data_char_ptr, ddr_size_, hp_width_,
                                   ddr_start_, ddr_reg_id_, SM_TRUNC);
  } else {
    UNI_LOG_FATAL(SIM_OUT_OF_RANGE)
        << "Unsupported DDR dump format: " << save_fmt_ << "!";
  }
}

template <DPUVersion T>
void DumpDDR<T>::GenInstStr(int inst_fmt) {
  ac_ = Helper<T>::GetInstStr(inst_type_, inst_fmt, dpdon_, dpdby_, field_str_);
}
