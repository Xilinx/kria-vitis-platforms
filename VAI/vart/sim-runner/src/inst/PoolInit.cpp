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
#include "PoolInit.hpp"

template <DPUVersion dv>
PoolInit<dv>* PoolInit<dv>::CUROBJ = nullptr;

template <>
PoolInit<DPUVersion::DPUV2>::PoolInit(int inst_type, int instid,
                                     vector<string>& inst_str,
                                     vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_POOL);
  Helper<DPUVersion::DPUV2>::InstInit(inst_type_, root_debug_path_, debug_,
                                     opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPUV2>::inst_table;
  // get poolinit instruction field value
  pool_type_ = field_val_[TABLE::POOLINIT_FIELD_POOL_TYPE];
  kernel_h_ = field_val_[TABLE::POOLINIT_FIELD_KERNEL_H];
  kernel_w_ = field_val_[TABLE::POOLINIT_FIELD_KERNEL_W];
  stride_offset_in_ = field_val_[TABLE::POOLINIT_FIELD_STRIDE_OFFSET_IN];
  valid_pixel_parallel_ =
      field_val_[TABLE::POOLINIT_FIELD_VALID_PIXEL_PARALLEL];
  stride_offset_out_ = field_val_[TABLE::POOLINIT_FIELD_STRIDE_OFFSET_OUT];
  stride_out_ = field_val_[TABLE::POOLINIT_FIELD_STRIDE_OUT];
  jump_read_ = field_val_[TABLE::POOLINIT_FIELD_JUMP_READ];
  jump_write_ = field_val_[TABLE::POOLINIT_FIELD_JUMP_WRITE];
  stride_h_ = field_val_[TABLE::POOLINIT_FIELD_STRIDE_H];
  stride_w_ = field_val_[TABLE::POOLINIT_FIELD_STRIDE_W];
  shift_cut_ = field_val_[TABLE::POOLINIT_FIELD_SHIFT_CUT];

  CUROBJ = this;
}

template <>
PoolInit<DPUVersion::DPUV3E>::PoolInit(int inst_type, int instid,
                                      vector<string>& inst_str,
                                      vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_POOL);

  Helper<DPUVersion::DPUV3E>::InstInit(inst_type_, root_debug_path_, debug_,
                                      opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPUV3E>::inst_table;
  // get poolinit instruction field value
  pool_type_ = field_val_[TABLE::POOLINIT_FIELD_POOL_TYPE];
  kernel_h_ = field_val_[TABLE::POOLINIT_FIELD_KERNEL_H];
  kernel_w_ = field_val_[TABLE::POOLINIT_FIELD_KERNEL_W];
  stride_offset_in_ = field_val_[TABLE::POOLINIT_FIELD_STRIDE_OFFSET_IN];
  valid_pixel_parallel_ =
      field_val_[TABLE::POOLINIT_FIELD_VALID_PIXEL_PARALLEL];
  stride_offset_out_ = field_val_[TABLE::POOLINIT_FIELD_STRIDE_OFFSET_OUT];
  stride_out_ = field_val_[TABLE::POOLINIT_FIELD_STRIDE_OUT];
  jump_read_ = field_val_[TABLE::POOLINIT_FIELD_JUMP_READ];
  jump_write_ = field_val_[TABLE::POOLINIT_FIELD_JUMP_WRITE];
  stride_h_ = field_val_[TABLE::POOLINIT_FIELD_STRIDE_H];
  stride_w_ = field_val_[TABLE::POOLINIT_FIELD_STRIDE_W];
  shift_cut_ = field_val_[TABLE::POOLINIT_FIELD_SHIFT_CUT];

  CUROBJ = this;
}
template <>
PoolInit<DPUVersion::DPUV4E>::PoolInit(int inst_type, int instid,
                                     vector<string>& inst_str,
                                     vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_POOL);
  Helper<DPUVersion::DPUV2>::InstInit(inst_type_, root_debug_path_, debug_,
                                     opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPUV2>::inst_table;
  // get poolinit instruction field value
  pool_type_ = field_val_[TABLE::POOLINIT_FIELD_POOL_TYPE];
  kernel_h_ = field_val_[TABLE::POOLINIT_FIELD_KERNEL_H];
  kernel_w_ = field_val_[TABLE::POOLINIT_FIELD_KERNEL_W];
  stride_offset_in_ = field_val_[TABLE::POOLINIT_FIELD_STRIDE_OFFSET_IN];
  valid_pixel_parallel_ =
      field_val_[TABLE::POOLINIT_FIELD_VALID_PIXEL_PARALLEL];
  stride_offset_out_ = field_val_[TABLE::POOLINIT_FIELD_STRIDE_OFFSET_OUT];
  stride_out_ = field_val_[TABLE::POOLINIT_FIELD_STRIDE_OUT];
  jump_read_ = field_val_[TABLE::POOLINIT_FIELD_JUMP_READ];
  jump_write_ = field_val_[TABLE::POOLINIT_FIELD_JUMP_WRITE];
  stride_h_ = field_val_[TABLE::POOLINIT_FIELD_STRIDE_H];
  stride_w_ = field_val_[TABLE::POOLINIT_FIELD_STRIDE_W];
  shift_cut_ = field_val_[TABLE::POOLINIT_FIELD_SHIFT_CUT];

  CUROBJ = this;
}


template <>
PoolInit<DPUVersion::DPUV3ME>::PoolInit(int inst_type, int instid,
                                     vector<string>& inst_str,
                                     vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_POOL);
  Helper<DPUVersion::DPUV3ME>::InstInit(inst_type_, root_debug_path_, debug_,
                                     opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPUV3ME>::inst_table;
  // get poolinit instruction field value
  pool_type_ = field_val_[TABLE::POOLINIT_FIELD_POOL_TYPE];
  kernel_h_ = field_val_[TABLE::POOLINIT_FIELD_KERNEL_H];
  kernel_w_ = field_val_[TABLE::POOLINIT_FIELD_KERNEL_W];
  stride_offset_in_ = field_val_[TABLE::POOLINIT_FIELD_STRIDE_OFFSET_IN];
  valid_pixel_parallel_ =
      field_val_[TABLE::POOLINIT_FIELD_VALID_PIXEL_PARALLEL];
  stride_offset_out_ = field_val_[TABLE::POOLINIT_FIELD_STRIDE_OFFSET_OUT];
  stride_out_ = field_val_[TABLE::POOLINIT_FIELD_STRIDE_OUT];
  jump_read_ = field_val_[TABLE::POOLINIT_FIELD_JUMP_READ];
  jump_write_ = field_val_[TABLE::POOLINIT_FIELD_JUMP_WRITE];
  stride_h_ = field_val_[TABLE::POOLINIT_FIELD_STRIDE_H];
  stride_w_ = field_val_[TABLE::POOLINIT_FIELD_STRIDE_W];
  shift_cut_ = field_val_[TABLE::POOLINIT_FIELD_SHIFT_CUT];

  CUROBJ = this;
}

template <>
PoolInit<DPUVersion::XVDPU>::PoolInit(int inst_type, int instid,
                                      vector<string>& inst_str,
                                      vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_POOL);

  Helper<DPUVersion::XVDPU>::InstInit(inst_type_, root_debug_path_, debug_,
                                      opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::XVDPU>::inst_table;
  // get poolinit instruction field value
  pool_type_ = field_val_[TABLE::POOLINIT_FIELD_POOL_TYPE];
  kernel_h_ = field_val_[TABLE::POOLINIT_FIELD_KERNEL_H];
  kernel_w_ = field_val_[TABLE::POOLINIT_FIELD_KERNEL_W];
  stride_offset_in_ = field_val_[TABLE::POOLINIT_FIELD_STRIDE_OFFSET_IN];
  valid_pixel_parallel_ =
      field_val_[TABLE::POOLINIT_FIELD_VALID_PIXEL_PARALLEL];
  stride_offset_out_ = field_val_[TABLE::POOLINIT_FIELD_STRIDE_OFFSET_OUT];
  stride_out_ = field_val_[TABLE::POOLINIT_FIELD_STRIDE_OUT];
  jump_read_ = field_val_[TABLE::POOLINIT_FIELD_JUMP_READ];
  jump_write_ = field_val_[TABLE::POOLINIT_FIELD_JUMP_WRITE];
  stride_h_ = field_val_[TABLE::POOLINIT_FIELD_STRIDE_H];
  stride_w_ = field_val_[TABLE::POOLINIT_FIELD_STRIDE_W];
  shift_cut_ = field_val_[TABLE::POOLINIT_FIELD_SHIFT_CUT];

  CUROBJ = this;
}

template <>
PoolInit<DPUVersion::DPU4F>::PoolInit(int inst_type, int instid,
                                        vector<string>& inst_str,
                                        vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_POOL);

  Helper<DPUVersion::DPU4F>::InstInit(inst_type_, root_debug_path_, debug_,
                                        opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPU4F>::inst_table;
  // get poolinit instruction field value
  pool_type_ = field_val_[TABLE::POOLINIT_FIELD_POOL_TYPE];
  kernel_h_ = field_val_[TABLE::POOLINIT_FIELD_KERNEL_H];
  kernel_w_ = field_val_[TABLE::POOLINIT_FIELD_KERNEL_W];
  stride_offset_in_ = field_val_[TABLE::POOLINIT_FIELD_STRIDE_OFFSET_IN];
  valid_pixel_parallel_ =
      field_val_[TABLE::POOLINIT_FIELD_VALID_PIXEL_PARALLEL];
  stride_offset_out_ = field_val_[TABLE::POOLINIT_FIELD_STRIDE_OFFSET_OUT];
  stride_out_ = field_val_[TABLE::POOLINIT_FIELD_STRIDE_OUT];
  jump_read_ = field_val_[TABLE::POOLINIT_FIELD_JUMP_READ];
  jump_write_ = field_val_[TABLE::POOLINIT_FIELD_JUMP_WRITE];
  stride_h_ = field_val_[TABLE::POOLINIT_FIELD_STRIDE_H];
  stride_w_ = field_val_[TABLE::POOLINIT_FIELD_STRIDE_W];
  shift_cut_ = field_val_[TABLE::POOLINIT_FIELD_SHIFT_CUT];

  CUROBJ = this;
}

template <DPUVersion dv>
PoolInit<dv>::~PoolInit() {}

template <DPUVersion dv>
void PoolInit<dv>::Exec() {}

template <DPUVersion T>
void PoolInit<T>::GenInstStr(int inst_fmt) {
  ac_ = Helper<T>::GetInstStr(inst_type_, inst_fmt, dpdon_, dpdby_, field_str_);
}
