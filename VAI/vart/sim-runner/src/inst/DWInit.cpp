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
#include "DWInit.hpp"

template <DPUVersion dv>
DWInit<dv>* DWInit<dv>::CUROBJ = nullptr;

template <>
DWInit<DPUVersion::DPUV2>::DWInit(int inst_type, int instid,
                                 vector<string>& inst_str,
                                 vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  debug_ =
      debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_DWCONV);
  Helper<DPUVersion::DPUV2>::InstInit(inst_type_, root_debug_path_, debug_,
                                     opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPUV2>::inst_table;
  // get convinit instruction field value
  jump_read_ = field_val_[TABLE::DWINIT_FIELD_JUMP_READ];
  jump_write_ = field_val_[TABLE::DWINIT_FIELD_JUMP_WRITE];
  jump_read_endl_ = field_val_[TABLE::DWINIT_FIELD_JUMP_READ_ENDL];
  stride_offset_in_ = field_val_[TABLE::DWINIT_FIELD_STRIDE_OFFSET_IN];
  valid_pixel_parallel_ = field_val_[TABLE::DWINIT_FIELD_VALID_PIXEL_PARALLEL];
  stride_w_ = field_val_[TABLE::DWINIT_FIELD_STRIDE_W];
  stride_h_ = field_val_[TABLE::DWINIT_FIELD_STRIDE_H];
  kernel_w_ = field_val_[TABLE::DWINIT_FIELD_KERNEL_W];
  kernel_h_ = field_val_[TABLE::DWINIT_FIELD_KERNEL_H];
  jump_write_endl_ = field_val_[TABLE::DWINIT_FIELD_JUMP_WRITE_ENDL];
  stride_offset_out_ = field_val_[TABLE::DWINIT_FIELD_STRIDE_OFFSET_OUT];
  stride_out_ = field_val_[TABLE::DWINIT_FIELD_STRIDE_OUT];
  shift_bias_ = field_val_[TABLE::DWINIT_FIELD_SHIFT_BIAS];
  shift_cut_ = field_val_[TABLE::DWINIT_FIELD_SHIFT_CUT];

  CUROBJ = this;
}

template <>
DWInit<DPUVersion::DPUV3E>::DWInit(int inst_type, int instid,
                                  vector<string>& inst_str,
                                  vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  debug_ =
      debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_DWCONV);
  Helper<DPUVersion::DPUV3E>::InstInit(inst_type_, root_debug_path_, debug_,
                                      opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPUV3E>::inst_table;
  // get convinit instruction field value
  jump_read_endl_ = field_val_[TABLE::DWINIT_FIELD_JUMP_READ_ENDL];
  act_type_ = field_val_[TABLE::DWINIT_FIELD_ACT_TYPE];
  stride_out_ = field_val_[TABLE::DWINIT_FIELD_STRIDE_OUT];

  jump_write_endl_ = field_val_[TABLE::DWINIT_FIELD_JUMP_WRITE_ENDL];
  valid_pixel_parallel_ = field_val_[TABLE::DWINIT_FIELD_VALID_PIXEL_PARALLEL];
  stride_w_ = field_val_[TABLE::DWINIT_FIELD_STRIDE_W];
  stride_h_ = field_val_[TABLE::DWINIT_FIELD_STRIDE_H];
  kernel_w_ = field_val_[TABLE::DWINIT_FIELD_KERNEL_W];
  kernel_h_ = field_val_[TABLE::DWINIT_FIELD_KERNEL_H];

  jump_read_ = field_val_[TABLE::DWINIT_FIELD_JUMP_READ];
  shift_bias_ = field_val_[TABLE::DWINIT_FIELD_SHIFT_BIAS];
  shift_cut_ = field_val_[TABLE::DWINIT_FIELD_SHIFT_CUT];
  stride_offset_in_ = field_val_[TABLE::DWINIT_FIELD_STRIDE_OFFSET_IN];
  stride_offset_out_ = field_val_[TABLE::DWINIT_FIELD_STRIDE_OFFSET_OUT];
  jump_write_ = field_val_[TABLE::DWINIT_FIELD_JUMP_WRITE];

  CUROBJ = this;
}

template <>
DWInit<DPUVersion::DPUV3ME>::DWInit(int inst_type, int instid,
                                 vector<string>& inst_str,
                                 vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  debug_ =
      debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_DWCONV);
  Helper<DPUVersion::DPUV3ME>::InstInit(inst_type_, root_debug_path_, debug_,
                                     opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPUV3ME>::inst_table;

  // get convinit instruction field value
  jump_read_ = field_val_[TABLE::DWINIT_FIELD_JUMP_READ];
  jump_write_ = field_val_[TABLE::DWINIT_FIELD_JUMP_WRITE];
  jump_read_endl_ = field_val_[TABLE::DWINIT_FIELD_JUMP_READ_ENDL];
  stride_offset_in_ = field_val_[TABLE::DWINIT_FIELD_STRIDE_OFFSET_IN];
  valid_pixel_parallel_ = field_val_[TABLE::DWINIT_FIELD_VALID_PIXEL_PARALLEL];
  stride_w_ = field_val_[TABLE::DWINIT_FIELD_STRIDE_W];
  stride_h_ = field_val_[TABLE::DWINIT_FIELD_STRIDE_H];
  kernel_w_ = field_val_[TABLE::DWINIT_FIELD_KERNEL_W];
  kernel_h_ = field_val_[TABLE::DWINIT_FIELD_KERNEL_H];
  jump_write_endl_ = field_val_[TABLE::DWINIT_FIELD_JUMP_WRITE_ENDL];
  stride_offset_out_ = field_val_[TABLE::DWINIT_FIELD_STRIDE_OFFSET_OUT];
  stride_out_ = field_val_[TABLE::DWINIT_FIELD_STRIDE_OUT];
  shift_relusix_ = field_val_[TABLE::DWINIT_FIELD_SHIFT_RELUSIX];
  shift_bias_ = field_val_[TABLE::DWINIT_FIELD_SHIFT_BIAS];
  shift_cut_ = field_val_[TABLE::DWINIT_FIELD_SHIFT_CUT];
  act_type_ = field_val_[TABLE::DWINIT_FIELD_ACT_TYPE];

  CUROBJ = this;
}

template <>
DWInit<DPUVersion::DPU4F>::DWInit(int inst_type, int instid,
                                    vector<string>& inst_str,
                                    vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  debug_ =
      debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_DWCONV);
  Helper<DPUVersion::DPU4F>::InstInit(inst_type_, root_debug_path_, debug_,
                                        opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPU4F>::inst_table;
  // get convinit instruction field value
  jump_read_ = field_val_[TABLE::DWINIT_FIELD_JUMP_READ];
  jump_write_ = field_val_[TABLE::DWINIT_FIELD_JUMP_WRITE];
  jump_read_endl_ = field_val_[TABLE::DWINIT_FIELD_JUMP_READ_ENDL];
  stride_offset_in_ = field_val_[TABLE::DWINIT_FIELD_STRIDE_OFFSET_IN];
  valid_pixel_parallel_ = field_val_[TABLE::DWINIT_FIELD_VALID_PIXEL_PARALLEL];
  stride_w_ = field_val_[TABLE::DWINIT_FIELD_STRIDE_W];
  stride_h_ = field_val_[TABLE::DWINIT_FIELD_STRIDE_H];
  kernel_w_ = field_val_[TABLE::DWINIT_FIELD_KERNEL_W];
  kernel_h_ = field_val_[TABLE::DWINIT_FIELD_KERNEL_H];
  jump_write_endl_ = field_val_[TABLE::DWINIT_FIELD_JUMP_WRITE_ENDL];
  stride_offset_out_ = field_val_[TABLE::DWINIT_FIELD_STRIDE_OFFSET_OUT];
  stride_out_ = field_val_[TABLE::DWINIT_FIELD_STRIDE_OUT];

  CUROBJ = this;
}

template <>
DWInit<DPUVersion::XVDPU>::DWInit(int inst_type, int instid,
                                  vector<string>& inst_str,
                                  vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  debug_ =
      debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_DWCONV);
  Helper<DPUVersion::XVDPU>::InstInit(inst_type_, root_debug_path_, debug_,
                                      opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::XVDPU>::inst_table;
  // get convinit instruction field value
  jump_read_endl_ = field_val_[TABLE::DWINIT_FIELD_JUMP_READ_ENDL];
  channel_group_ = field_val_[TABLE::DWINIT_FIELD_CHANNEL_GROUP];

  jump_read_ = field_val_[TABLE::DWINIT_FIELD_JUMP_READ];
  stride_offset_in_ = field_val_[TABLE::DWINIT_FIELD_STRIDE_OFFSET_IN];
  stride_offset_out_ = field_val_[TABLE::DWINIT_FIELD_STRIDE_OFFSET_OUT];
  stride_w_ = field_val_[TABLE::DWINIT_FIELD_STRIDE_W];
  stride_h_ = field_val_[TABLE::DWINIT_FIELD_STRIDE_H];
  kernel_w_ = field_val_[TABLE::DWINIT_FIELD_KERNEL_W];
  kernel_h_ = field_val_[TABLE::DWINIT_FIELD_KERNEL_H];

  jump_write_endl_ = field_val_[TABLE::DWINIT_FIELD_JUMP_WRITE_ENDL];
  channel_offset_ = field_val_[TABLE::DWINIT_FIELD_CHANNEL_OFFSET];
  act_type_ = field_val_[TABLE::DWINIT_FIELD_ACT_TYPE];
  stride_out_ = field_val_[TABLE::DWINIT_FIELD_STRIDE_OUT];

  jump_write_ = field_val_[TABLE::DWINIT_FIELD_JUMP_WRITE];
  length_ = field_val_[TABLE::DWINIT_FIELD_LENGTH];
  shift_bias_ = field_val_[TABLE::DWINIT_FIELD_SHIFT_BIAS];
  shift_cut_ = field_val_[TABLE::DWINIT_FIELD_SHIFT_CUT];

  CUROBJ = this;
}

template <DPUVersion T>
DWInit<T>::~DWInit() {}

template <DPUVersion T>
void DWInit<T>::Exec() {}

template <DPUVersion T>
void DWInit<T>::GenInstStr(int inst_fmt) {
  ac_ = Helper<T>::GetInstStr(inst_type_, inst_fmt, dpdon_, dpdby_, field_str_);
}
