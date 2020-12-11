
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
#include "ElewInit.hpp"

template <DPUVersion dv>
deque<int> ElewInit<dv>::id_;
template <DPUVersion dv>
deque<int> ElewInit<dv>::bank_id_in_;
template <DPUVersion dv>
deque<int> ElewInit<dv>::shift_read_;
template <DPUVersion dv>
deque<int> ElewInit<dv>::jump_read_;
template <DPUVersion dv>
deque<int> ElewInit<dv>::bank_addr_in_;
template <DPUVersion dv>
deque<int> ElewInit<dv>::jump_read_endl_;

template <>
ElewInit<DPUVersion::DPUV2>::ElewInit(int inst_type, int inst_id,
                                      vector<string>& inst_str,
                                      vector<uint32_t>& inst_val)
    : InstBase(inst_type, inst_id, inst_str, inst_val) {
  // debug
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_ELEW);
  Helper<DPUVersion::DPUV2>::InstInit(inst_type_, root_debug_path_, debug_,
                                      opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPUV2>::inst_table;
  // get elewinit instruction field value
  local_id_ = field_val_[TABLE::ELEWINIT_FIELD_ID];
  local_bank_id_in_ = field_val_[TABLE::ELEWINIT_FIELD_BANK_ID_IN];
  local_shift_read_ = field_val_[TABLE::ELEWINIT_FIELD_SHIFT_READ];
  local_jump_read_ = field_val_[TABLE::ELEWINIT_FIELD_JUMP_READ];
  local_bank_addr_in_ = field_val_[TABLE::ELEWINIT_FIELD_BANK_ADDR_IN];
  local_jump_read_endl_ = field_val_[TABLE::ELEWINIT_FIELD_JUMP_READ_ENDL];

  // push upper param data into deque
  id_.push_back(local_id_);
  bank_id_in_.push_back(local_bank_id_in_);
  shift_read_.push_back(local_shift_read_);
  jump_read_.push_back(local_jump_read_);
  bank_addr_in_.push_back(local_bank_addr_in_);
  jump_read_endl_.push_back(local_jump_read_endl_);
}

template <>
ElewInit<DPUVersion::DPUV3E>::ElewInit(int inst_type, int inst_id,
                                       vector<string>& inst_str,
                                       vector<uint32_t>& inst_val)
    : InstBase(inst_type, inst_id, inst_str, inst_val) {
  // debug
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_ELEW);
  Helper<DPUVersion::DPUV3E>::InstInit(inst_type_, root_debug_path_, debug_,
                                       opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPUV3E>::inst_table;
  // get elewinit instruction field value
  local_id_ = field_val_[TABLE::ELEWINIT_FIELD_ID];
  local_bank_id_in_ = field_val_[TABLE::ELEWINIT_FIELD_BANK_ID_IN];
  local_shift_read_ = field_val_[TABLE::ELEWINIT_FIELD_SHIFT_READ];
  local_jump_read_ = field_val_[TABLE::ELEWINIT_FIELD_JUMP_READ];
  local_bank_addr_in_ = field_val_[TABLE::ELEWINIT_FIELD_BANK_ADDR_IN];
  local_jump_read_endl_ = field_val_[TABLE::ELEWINIT_FIELD_JUMP_READ_ENDL];

  // push upper param data into deque
  id_.push_back(local_id_);
  bank_id_in_.push_back(local_bank_id_in_);
  shift_read_.push_back(local_shift_read_);
  jump_read_.push_back(local_jump_read_);
  bank_addr_in_.push_back(local_bank_addr_in_);
  jump_read_endl_.push_back(local_jump_read_endl_);
}

template <>
ElewInit<DPUVersion::DPUV3ME>::ElewInit(int inst_type, int inst_id,
                                        vector<string>& inst_str,
                                        vector<uint32_t>& inst_val)
    : InstBase(inst_type, inst_id, inst_str, inst_val) {
  // debug
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_ELEW);
  Helper<DPUVersion::DPUV3ME>::InstInit(inst_type_, root_debug_path_, debug_,
                                        opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPUV3ME>::inst_table;
  // get elewinit instruction field value
  local_id_ = field_val_[TABLE::ELEWINIT_FIELD_ID];
  local_bank_id_in_ = field_val_[TABLE::ELEWINIT_FIELD_BANK_ID_IN];
  local_shift_read_ = field_val_[TABLE::ELEWINIT_FIELD_SHIFT_READ];
  local_jump_read_ = field_val_[TABLE::ELEWINIT_FIELD_JUMP_READ];
  local_bank_addr_in_ = field_val_[TABLE::ELEWINIT_FIELD_BANK_ADDR_IN];
  local_jump_read_endl_ = field_val_[TABLE::ELEWINIT_FIELD_JUMP_READ_ENDL];

  // push upper param data into deque
  id_.push_back(local_id_);
  bank_id_in_.push_back(local_bank_id_in_);
  shift_read_.push_back(local_shift_read_);
  jump_read_.push_back(local_jump_read_);
  bank_addr_in_.push_back(local_bank_addr_in_);
  jump_read_endl_.push_back(local_jump_read_endl_);
}

template <>
ElewInit<DPUVersion::DPUV4E>::ElewInit(int inst_type, int inst_id,
                                       vector<string>& inst_str,
                                       vector<uint32_t>& inst_val)
    : InstBase(inst_type, inst_id, inst_str, inst_val) {
  // debug
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_ELEW);
  Helper<DPUVersion::DPUV4E>::InstInit(inst_type_, root_debug_path_, debug_,
                                       opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPUV4E>::inst_table;
  // get elewinit instruction field value
  local_id_ = field_val_[TABLE::ELEWINIT_FIELD_ID];
  local_bank_id_in_ = field_val_[TABLE::ELEWINIT_FIELD_BANK_ID_IN];
  local_shift_read_ = field_val_[TABLE::ELEWINIT_FIELD_SHIFT_READ];
  local_jump_read_ = field_val_[TABLE::ELEWINIT_FIELD_JUMP_READ];
  local_bank_addr_in_ = field_val_[TABLE::ELEWINIT_FIELD_BANK_ADDR_IN];
  local_jump_read_endl_ = field_val_[TABLE::ELEWINIT_FIELD_JUMP_READ_ENDL];

  if (local_bank_id_in_ >= 32) {
    auto bank_group_name =
        ArchCfg::Instance().get_param().eltwise_engine().input_bank()[0];
    Buffer<DPU_DATA_TYPE>::Instance().virtual_bank_acquire(bank_group_name);
    local_bank_id_in_ = Buffer<DPU_DATA_TYPE>::Instance().virtual_bank_mapping(
        local_bank_id_in_, bank_group_name);
  }

  // push upper param data into deque
  id_.push_back(local_id_);
  bank_id_in_.push_back(local_bank_id_in_);
  shift_read_.push_back(local_shift_read_);
  jump_read_.push_back(local_jump_read_);
  bank_addr_in_.push_back(local_bank_addr_in_);
  jump_read_endl_.push_back(local_jump_read_endl_);
}

template <>
ElewInit<DPUVersion::XVDPU>::ElewInit(int inst_type, int inst_id,
                                      vector<string>& inst_str,
                                      vector<uint32_t>& inst_val)
    : InstBase(inst_type, inst_id, inst_str, inst_val) {
  // debug
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_ELEW);
  Helper<DPUVersion::XVDPU>::InstInit(inst_type_, root_debug_path_, debug_,
                                      opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::XVDPU>::inst_table;
  // get elewinit instruction field value
  local_id_ = field_val_[TABLE::ELEWINIT_FIELD_ID];
  local_bank_id_in_ = field_val_[TABLE::ELEWINIT_FIELD_BANK_ID_IN];
  local_shift_read_ = field_val_[TABLE::ELEWINIT_FIELD_SHIFT_READ];
  local_jump_read_ = field_val_[TABLE::ELEWINIT_FIELD_JUMP_READ];
  local_bank_addr_in_ = field_val_[TABLE::ELEWINIT_FIELD_BANK_ADDR_IN];
  local_jump_read_endl_ = field_val_[TABLE::ELEWINIT_FIELD_JUMP_READ_ENDL];

  // push upper param data into deque
  id_.push_back(local_id_);
  bank_id_in_.push_back(local_bank_id_in_);
  shift_read_.push_back(local_shift_read_);
  jump_read_.push_back(local_jump_read_);
  bank_addr_in_.push_back(local_bank_addr_in_);
  jump_read_endl_.push_back(local_jump_read_endl_);
}

template <>
ElewInit<DPUVersion::DPU4F>::ElewInit(int inst_type, int inst_id,
                                      vector<string>& inst_str,
                                      vector<uint32_t>& inst_val)
    : InstBase(inst_type, inst_id, inst_str, inst_val) {
  // debug
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_ELEW);
  Helper<DPUVersion::DPU4F>::InstInit(inst_type_, root_debug_path_, debug_,
                                      opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPU4F>::inst_table;
  // get elewinit instruction field value
  local_id_ = field_val_[TABLE::ELEWINIT_FIELD_ID];
  local_bank_id_in_ = field_val_[TABLE::ELEWINIT_FIELD_BANK_ID_IN];
  local_shift_read_ = field_val_[TABLE::ELEWINIT_FIELD_SHIFT_READ];
  local_jump_read_ = field_val_[TABLE::ELEWINIT_FIELD_JUMP_READ];
  local_bank_addr_in_ = field_val_[TABLE::ELEWINIT_FIELD_BANK_ADDR_IN];
  local_jump_read_endl_ = field_val_[TABLE::ELEWINIT_FIELD_JUMP_READ_ENDL];

  // push upper param data into deque
  id_.push_back(local_id_);
  bank_id_in_.push_back(local_bank_id_in_);
  shift_read_.push_back(local_shift_read_);
  jump_read_.push_back(local_jump_read_);
  bank_addr_in_.push_back(local_bank_addr_in_);
  jump_read_endl_.push_back(local_jump_read_endl_);
}

template <DPUVersion dv>
ElewInit<dv>::~ElewInit() {}

// public funcs
template <DPUVersion dv>
void ElewInit<dv>::Exec() {}

template <DPUVersion dv>
void ElewInit<dv>::GenInstStr(int inst_fmt) {
  ac_ =
      Helper<dv>::GetInstStr(inst_type_, inst_fmt, dpdon_, dpdby_, field_str_);
}

// other public funcs
template <DPUVersion dv>
int ElewInit<dv>::get_id(int i) {
  int id = id_.front();
  CHECK(i == id);
  id_.pop_front();
  return id;
}

template <DPUVersion dv>
int ElewInit<dv>::get_bank_id_in(int i) {
  int bank_id_in = bank_id_in_.front();
  bank_id_in_.pop_front();
  return bank_id_in;
}

template <DPUVersion dv>
int ElewInit<dv>::get_shift_read(int i) {
  int shift_read = shift_read_.front();
  shift_read_.pop_front();
  return shift_read;
}

template <DPUVersion dv>
int ElewInit<dv>::get_jump_read(int i) {
  int jump_read = jump_read_.front();
  jump_read_.pop_front();
  return jump_read;
}

template <DPUVersion dv>
int ElewInit<dv>::get_bank_addr_in(int i) {
  int bank_addr_in = bank_addr_in_.front();
  bank_addr_in_.pop_front();
  return bank_addr_in;
}

template <DPUVersion dv>
int ElewInit<dv>::get_jump_read_endl(int i) {
  int jump_read_endl = jump_read_endl_.front();
  jump_read_endl_.pop_front();
  return jump_read_endl;
}

template class ElewInit<DPUVersion::DPUV2>;
template class ElewInit<DPUVersion::DPUV3E>;
template class ElewInit<DPUVersion::DPUV3ME>;
template class ElewInit<DPUVersion::DPUV4E>;
template class ElewInit<DPUVersion::XVDPU>;
template class ElewInit<DPUVersion::DPU4F>;
