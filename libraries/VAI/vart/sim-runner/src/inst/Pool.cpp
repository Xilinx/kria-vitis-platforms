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
#include "Pool.hpp"
#include "PoolInit.hpp"

template <DPUVersion dv>
const DPU_DATA_TYPE Pool<dv>::MIN_DTYPE_VALUE =
    std::numeric_limits<DPU_DATA_TYPE>::min();

template <DPUVersion dv>
PoolInit<dv>* PoolInit<dv>::CUROBJ;

template <>
Pool<DPUVersion::DPUV2>::Pool(int inst_type, int instid,
                              vector<string>& inst_str,
                              vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  // debug
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_POOL);
  Helper<DPUVersion::DPUV2>::InstInit(inst_type_, root_debug_path_, debug_,
                                      opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPUV2>::inst_table;
  // get pool instruction field value
  bank_id_in_ = field_val_[TABLE::POOL_FIELD_BANK_ID_IN];
  bank_id_out_ = field_val_[TABLE::POOL_FIELD_BANK_ID_OUT];
  bank_addr_in_ = field_val_[TABLE::POOL_FIELD_BANK_ADDR_IN];
  pad_left_ = field_val_[TABLE::POOL_FIELD_PAD_LEFT];
  pad_top_ = field_val_[TABLE::POOL_FIELD_PAD_TOP];
  pad_right_ = field_val_[TABLE::POOL_FIELD_PAD_RIGHT];
  pad_bottom_ = field_val_[TABLE::POOL_FIELD_PAD_BOTTOM];
  bank_addr_out_ = field_val_[TABLE::POOL_FIELD_BANK_ADDR_OUT];
  length_ = field_val_[TABLE::POOL_FIELD_LENGTH];
  channel_group_ = field_val_[TABLE::POOL_FIELD_CHANNEL_GROUP];
  jump_read_endl_ = field_val_[TABLE::POOL_FIELD_JUMP_READ_ENDL];
  jump_write_endl_ = field_val_[TABLE::POOL_FIELD_JUMP_WRITE_ENDL];
  bank_addr_in_1_ = field_val_[TABLE::POOL_FIELD_BANK_ADDR_IN_1];
  bank_addr_in_2_ = field_val_[TABLE::POOL_FIELD_BANK_ADDR_IN_2];
  bank_addr_in_3_ = field_val_[TABLE::POOL_FIELD_BANK_ADDR_IN_3];
  downsample_kernel_h_ = field_val_[TABLE::POOL_FIELD_DOWNSAMPLE_KERNEL_H];
  downsample_kernel_w_ = field_val_[TABLE::POOL_FIELD_DOWNSAMPLE_KERNEL_W];
  // others
  // PoolInit instruction field
  using POOLINIT = PoolInit<DPUVersion::DPUV2>;
  pool_type_ = POOLINIT::CUROBJ->get_pool_type();
  stride_h_ = POOLINIT::CUROBJ->get_stride_h();
  stride_w_ = POOLINIT::CUROBJ->get_stride_w();
  stride_offset_in_ = POOLINIT::CUROBJ->get_stride_offset_in();
  valid_pixel_parallel_ = POOLINIT::CUROBJ->get_valid_pixel_parallel();
  stride_offset_out_ = POOLINIT::CUROBJ->get_stride_offset_out();
  stride_out_ = POOLINIT::CUROBJ->get_stride_out();
  jump_read_ = POOLINIT::CUROBJ->get_jump_read();
  jump_write_ = POOLINIT::CUROBJ->get_jump_write();
  kernel_h_ = POOLINIT::CUROBJ->get_kernel_h();
  kernel_w_ = POOLINIT::CUROBJ->get_kernel_w();
  shift_cut_ = POOLINIT::CUROBJ->get_shift_cut();

  if (pool_type_ == 2) {
    kernel_w_ = downsample_kernel_w_;
    kernel_h_ = downsample_kernel_h_;
    stride_w_ = kernel_w_;
    stride_h_ = kernel_h_;
  }
  // hardware config value
  vpp_ = valid_pixel_parallel_;
  pp_pool_ = ArchCfg::Instance().get_param().pool_engine().pixel_parallel();
  assert(vpp_ <= pp_pool_);
  pp_ = ArchCfg::Instance().get_param().conv_engine().pixel_parallel();
  assert(pp_pool_ <= pp_);
  cp_ = ArchCfg::Instance().get_param().pool_engine().channel_parallel();
  // self defined
  cg_ = channel_group_;
  ic_ = cg_ * cp_;  // input_channel is channel_group multiply channel_parallel
  oc_ = ic_;        // output_channel is channel_parallel
  src_h_ = (pp_pool_ - 1) * stride_h_ + kernel_h_;
  real_src_h_ = (vpp_ - 1) * stride_h_ + kernel_h_;
  src_w_ = (length_ - 1) * stride_w_ + kernel_w_;
  dst_h_ = pp_pool_;
  real_dst_h_ = vpp_;
  dst_w_ = length_;
  src_size_ = src_h_ * src_w_ * ic_;

  group_id_in_ = bank_id_in_ / pp_;
  group_id_out_ = bank_id_out_ / pp_;

  // resize related buffer size
  img_.resize(src_h_ * src_w_ * ic_);
  rlt_dtype_.resize(dst_h_ * dst_w_ * oc_);
  // std::fill(rlt_dtype_.begin(), rlt_dtype_.end(), MIN_DTYPE_VALUE);
  if (pool_type_ == 0 || pool_type_ == 2) {  // max pooling
    std::fill(rlt_dtype_.begin(), rlt_dtype_.end(), MIN_DTYPE_VALUE);
  } else if (pool_type_ == 1) {  // average pooling
    std::fill(rlt_dtype_.begin(), rlt_dtype_.end(), 0);
  }
}

template <>
Pool<DPUVersion::DPUV3E>::Pool(int inst_type, int instid,
                               vector<string>& inst_str,
                               vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  // debug
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_POOL);
  Helper<DPUVersion::DPUV3E>::InstInit(inst_type_, root_debug_path_, debug_,
                                       opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPUV3E>::inst_table;
  // get pool instruction field value
  bank_id_in_ = field_val_[TABLE::POOL_FIELD_BANK_ID_IN];
  bank_id_out_ = field_val_[TABLE::POOL_FIELD_BANK_ID_OUT];
  bank_addr_in_ = field_val_[TABLE::POOL_FIELD_BANK_ADDR_IN];
  pad_left_ = field_val_[TABLE::POOL_FIELD_PAD_LEFT];
  pad_top_ = field_val_[TABLE::POOL_FIELD_PAD_TOP];
  pad_right_ = field_val_[TABLE::POOL_FIELD_PAD_RIGHT];
  pad_bottom_ = field_val_[TABLE::POOL_FIELD_PAD_BOTTOM];
  bank_addr_out_ = field_val_[TABLE::POOL_FIELD_BANK_ADDR_OUT];
  length_ = field_val_[TABLE::POOL_FIELD_LENGTH];
  channel_group_ = field_val_[TABLE::POOL_FIELD_CHANNEL_GROUP];
  jump_read_endl_ = field_val_[TABLE::POOL_FIELD_JUMP_READ_ENDL];
  jump_write_endl_ = field_val_[TABLE::POOL_FIELD_JUMP_WRITE_ENDL];
  bank_addr_in_1_ = field_val_[TABLE::POOL_FIELD_BANK_ADDR_IN_1];
  bank_addr_in_2_ = field_val_[TABLE::POOL_FIELD_BANK_ADDR_IN_2];
  bank_addr_in_3_ = field_val_[TABLE::POOL_FIELD_BANK_ADDR_IN_3];
  // others
  // PoolInit instruction field
  using POOLINIT = PoolInit<DPUVersion::DPUV3E>;
  pool_type_ = POOLINIT::CUROBJ->get_pool_type();
  stride_h_ = POOLINIT::CUROBJ->get_stride_h();
  stride_w_ = POOLINIT::CUROBJ->get_stride_w();
  stride_offset_in_ = POOLINIT::CUROBJ->get_stride_offset_in();
  valid_pixel_parallel_ = POOLINIT::CUROBJ->get_valid_pixel_parallel();
  stride_offset_out_ = POOLINIT::CUROBJ->get_stride_offset_out();
  stride_out_ = POOLINIT::CUROBJ->get_stride_out();
  jump_read_ = POOLINIT::CUROBJ->get_jump_read();
  jump_write_ = POOLINIT::CUROBJ->get_jump_write();
  kernel_h_ = POOLINIT::CUROBJ->get_kernel_h();
  kernel_w_ = POOLINIT::CUROBJ->get_kernel_w();
  shift_cut_ = POOLINIT::CUROBJ->get_shift_cut();

  // hardware config value
  vpp_ = valid_pixel_parallel_;
  pp_pool_ = ArchCfg::Instance().get_param().pool_engine().pixel_parallel();
  assert(vpp_ <= pp_pool_);
  pp_ = ArchCfg::Instance().get_param().conv_engine().pixel_parallel();
  assert(pp_pool_ <= pp_);
  cp_ = ArchCfg::Instance().get_param().pool_engine().channel_parallel();
  // self defined
  cg_ = channel_group_;
  ic_ = cg_ * cp_;  // input_channel is channel_group multiply channel_parallel
  oc_ = ic_;        // output_channel is channel_parallel
  src_h_ = (pp_pool_ - 1) * stride_h_ + kernel_h_;
  real_src_h_ = (vpp_ - 1) * stride_h_ + kernel_h_;
  src_w_ = (length_ - 1) * stride_w_ + kernel_w_;
  dst_h_ = pp_pool_;
  real_dst_h_ = vpp_;
  dst_w_ = length_;
  src_size_ = src_h_ * src_w_ * ic_;

  group_id_in_ = bank_id_in_ / pp_;
  group_id_out_ = bank_id_out_ / pp_;

  // resize related buffer size
  img_.resize(src_h_ * src_w_ * ic_);
  rlt_dtype_.resize(dst_h_ * dst_w_ * oc_);
  // std::fill(rlt_dtype_.begin(), rlt_dtype_.end(), MIN_DTYPE_VALUE);
  if (pool_type_ == 0) {  // max pooling
    std::fill(rlt_dtype_.begin(), rlt_dtype_.end(), MIN_DTYPE_VALUE);
  } else {  // average pooling
    std::fill(rlt_dtype_.begin(), rlt_dtype_.end(), 0);
  }
}

template <>
Pool<DPUVersion::DPUV3ME>::Pool(int inst_type, int instid,
                                vector<string>& inst_str,
                                vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  // debug
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_POOL);
  Helper<DPUVersion::DPUV3ME>::InstInit(inst_type_, root_debug_path_, debug_,
                                        opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPUV3ME>::inst_table;
  bank_id_in_ = field_val_[TABLE::POOL_FIELD_BANK_ID_IN];
  bank_id_out_ = field_val_[TABLE::POOL_FIELD_BANK_ID_OUT];
  bank_addr_in_ = field_val_[TABLE::POOL_FIELD_BANK_ADDR_IN];
  pad_left_ = field_val_[TABLE::POOL_FIELD_PAD_LEFT];
  pad_top_ = field_val_[TABLE::POOL_FIELD_PAD_TOP];
  pad_right_ = field_val_[TABLE::POOL_FIELD_PAD_RIGHT];
  pad_bottom_ = field_val_[TABLE::POOL_FIELD_PAD_BOTTOM];
  bank_addr_out_ = field_val_[TABLE::POOL_FIELD_BANK_ADDR_OUT];
  length_ = field_val_[TABLE::POOL_FIELD_LENGTH];
  channel_group_ = field_val_[TABLE::POOL_FIELD_CHANNEL_GROUP];
  jump_read_endl_ = field_val_[TABLE::POOL_FIELD_JUMP_READ_ENDL];
  jump_write_endl_ = field_val_[TABLE::POOL_FIELD_JUMP_WRITE_ENDL];

  // others
  // PoolInit instruction field
  using POOLINIT = PoolInit<DPUVersion::DPUV3ME>;
  pool_type_ = POOLINIT::CUROBJ->get_pool_type();
  stride_h_ = POOLINIT::CUROBJ->get_stride_h();
  stride_w_ = POOLINIT::CUROBJ->get_stride_w();
  stride_offset_in_ = POOLINIT::CUROBJ->get_stride_offset_in();
  valid_pixel_parallel_ = POOLINIT::CUROBJ->get_valid_pixel_parallel();
  stride_offset_out_ = POOLINIT::CUROBJ->get_stride_offset_out();
  stride_out_ = POOLINIT::CUROBJ->get_stride_out();
  jump_read_ = POOLINIT::CUROBJ->get_jump_read();
  jump_write_ = POOLINIT::CUROBJ->get_jump_write();
  kernel_h_ = POOLINIT::CUROBJ->get_kernel_h();
  kernel_w_ = POOLINIT::CUROBJ->get_kernel_w();
  shift_cut_ = POOLINIT::CUROBJ->get_shift_cut();

  // hardware config value
  vpp_ = valid_pixel_parallel_;
  pp_pool_ = ArchCfg::Instance().get_param().pool_engine().pixel_parallel();
  assert(vpp_ <= pp_pool_);
  pp_ = ArchCfg::Instance().get_param().conv_engine().pixel_parallel();
  assert(pp_pool_ <= pp_);
  cp_ = ArchCfg::Instance().get_param().pool_engine().channel_parallel();
  // self defined
  cg_ = channel_group_;
  ic_ = cg_ * cp_;  // input_channel is channel_group multiply channel_parallel
  oc_ = ic_;        // output_channel is channel_parallel
  src_h_ = (pp_pool_ - 1) * stride_h_ + kernel_h_;
  real_src_h_ = (vpp_ - 1) * stride_h_ + kernel_h_;
  src_w_ = (length_ - 1) * stride_w_ + kernel_w_;
  dst_h_ = pp_pool_;
  real_dst_h_ = vpp_;
  dst_w_ = length_;
  src_size_ = src_h_ * src_w_ * ic_;

  group_id_in_ = bank_id_in_ / pp_;
  group_id_out_ = bank_id_out_ / pp_;

  // resize related buffer size
  img_.resize(src_h_ * src_w_ * ic_);
  rlt_dtype_.resize(dst_h_ * dst_w_ * oc_);
  // std::fill(rlt_dtype_.begin(), rlt_dtype_.end(), MIN_DTYPE_VALUE);
  if (pool_type_ == 0) {  // max pooling
    std::fill(rlt_dtype_.begin(), rlt_dtype_.end(), MIN_DTYPE_VALUE);
  } else {  // average pooling
    std::fill(rlt_dtype_.begin(), rlt_dtype_.end(), 0);
  }
}

template <>
Pool<DPUVersion::DPUV4E>::Pool(int inst_type, int instid,
                              vector<string>& inst_str,
                              vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  // debug
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_POOL);
  Helper<DPUVersion::DPUV4E>::InstInit(inst_type_, root_debug_path_, debug_,
                                      opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPUV4E>::inst_table;
  // get pool instruction field value
  bank_id_in_ = field_val_[TABLE::POOL_FIELD_BANK_ID_IN];
  bank_id_out_ = field_val_[TABLE::POOL_FIELD_BANK_ID_OUT];
  bank_addr_in_ = field_val_[TABLE::POOL_FIELD_BANK_ADDR_IN];
  pad_left_ = field_val_[TABLE::POOL_FIELD_PAD_LEFT];
  pad_top_ = field_val_[TABLE::POOL_FIELD_PAD_TOP];
  pad_right_ = field_val_[TABLE::POOL_FIELD_PAD_RIGHT];
  pad_bottom_ = field_val_[TABLE::POOL_FIELD_PAD_BOTTOM];
  bank_addr_out_ = field_val_[TABLE::POOL_FIELD_BANK_ADDR_OUT];
  length_ = field_val_[TABLE::POOL_FIELD_LENGTH];
  channel_group_ = field_val_[TABLE::POOL_FIELD_CHANNEL_GROUP];
  jump_read_endl_ = field_val_[TABLE::POOL_FIELD_JUMP_READ_ENDL];
  jump_write_endl_ = field_val_[TABLE::POOL_FIELD_JUMP_WRITE_ENDL];
  bank_addr_in_1_ = field_val_[TABLE::POOL_FIELD_BANK_ADDR_IN_1];
  bank_addr_in_2_ = field_val_[TABLE::POOL_FIELD_BANK_ADDR_IN_2];
  bank_addr_in_3_ = field_val_[TABLE::POOL_FIELD_BANK_ADDR_IN_3];
  // others
  // PoolInit instruction field
  using POOLINIT = PoolInit<DPUVersion::DPUV4E>;
  pool_type_ = POOLINIT::CUROBJ->get_pool_type();
  stride_h_ = POOLINIT::CUROBJ->get_stride_h();
  stride_w_ = POOLINIT::CUROBJ->get_stride_w();
  stride_offset_in_ = POOLINIT::CUROBJ->get_stride_offset_in();
  valid_pixel_parallel_ = POOLINIT::CUROBJ->get_valid_pixel_parallel();
  stride_offset_out_ = POOLINIT::CUROBJ->get_stride_offset_out();
  stride_out_ = POOLINIT::CUROBJ->get_stride_out();
  jump_read_ = POOLINIT::CUROBJ->get_jump_read();
  jump_write_ = POOLINIT::CUROBJ->get_jump_write();
  kernel_h_ = POOLINIT::CUROBJ->get_kernel_h();
  kernel_w_ = POOLINIT::CUROBJ->get_kernel_w();
  shift_cut_ = POOLINIT::CUROBJ->get_shift_cut();

  // hardware config value
  vpp_ = valid_pixel_parallel_;
  pp_pool_ = ArchCfg::Instance().get_param().pool_engine().pixel_parallel();
  assert(vpp_ <= pp_pool_);
  pp_ = ArchCfg::Instance().get_param().conv_engine().pixel_parallel();
  assert(pp_pool_ <= pp_);
  cp_ = ArchCfg::Instance().get_param().pool_engine().channel_parallel();
  // self defined
  cg_ = channel_group_;
  ic_ = cg_ * cp_;  // input_channel is channel_group multiply channel_parallel
  oc_ = ic_;        // output_channel is channel_parallel
  src_h_ = (pp_pool_ - 1) * stride_h_ + kernel_h_;
  real_src_h_ = (vpp_ - 1) * stride_h_ + kernel_h_;
  src_w_ = (length_ - 1) * stride_w_ + kernel_w_;
  dst_h_ = pp_pool_;
  real_dst_h_ = vpp_;
  dst_w_ = length_;
  src_size_ = src_h_ * src_w_ * ic_;

  group_id_in_ = bank_id_in_ / pp_;
  group_id_out_ = bank_id_out_ / pp_;

  // resize related buffer size
  img_.resize(src_h_ * src_w_ * ic_);
  rlt_dtype_.resize(dst_h_ * dst_w_ * oc_);
  // std::fill(rlt_dtype_.begin(), rlt_dtype_.end(), MIN_DTYPE_VALUE);
  if (pool_type_ == 0) {  // max pooling
    std::fill(rlt_dtype_.begin(), rlt_dtype_.end(), MIN_DTYPE_VALUE);
  } else if (pool_type_ == 1) {  // average pooling
    std::fill(rlt_dtype_.begin(), rlt_dtype_.end(), 0);
  }
}

template <>
Pool<DPUVersion::XVDPU>::Pool(int inst_type, int instid,
                              vector<string>& inst_str,
                              vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  // debug
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_POOL);
  Helper<DPUVersion::XVDPU>::InstInit(inst_type_, root_debug_path_, debug_,
                                      opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::XVDPU>::inst_table;
  // get pool instruction field value
  bank_id_in_ = field_val_[TABLE::POOL_FIELD_BANK_ID_IN];
  bank_id_out_ = field_val_[TABLE::POOL_FIELD_BANK_ID_OUT];
  bank_addr_in_ = field_val_[TABLE::POOL_FIELD_BANK_ADDR_IN];
  pad_left_ = field_val_[TABLE::POOL_FIELD_PAD_LEFT];
  pad_top_ = field_val_[TABLE::POOL_FIELD_PAD_TOP];
  pad_right_ = field_val_[TABLE::POOL_FIELD_PAD_RIGHT];
  pad_bottom_ = field_val_[TABLE::POOL_FIELD_PAD_BOTTOM];
  bank_addr_out_ = field_val_[TABLE::POOL_FIELD_BANK_ADDR_OUT];
  length_ = field_val_[TABLE::POOL_FIELD_LENGTH];
  channel_group_ = field_val_[TABLE::POOL_FIELD_CHANNEL_GROUP];
  jump_read_endl_ = field_val_[TABLE::POOL_FIELD_JUMP_READ_ENDL];
  jump_write_endl_ = field_val_[TABLE::POOL_FIELD_JUMP_WRITE_ENDL];
  bank_addr_in_1_ = field_val_[TABLE::POOL_FIELD_BANK_ADDR_IN_1];
  bank_addr_in_2_ = field_val_[TABLE::POOL_FIELD_BANK_ADDR_IN_2];
  bank_addr_in_3_ = field_val_[TABLE::POOL_FIELD_BANK_ADDR_IN_3];
  // others
  // PoolInit instruction field
  using POOLINIT = PoolInit<DPUVersion::XVDPU>;
  pool_type_ = POOLINIT::CUROBJ->get_pool_type();
  stride_h_ = POOLINIT::CUROBJ->get_stride_h();
  stride_w_ = POOLINIT::CUROBJ->get_stride_w();
  stride_offset_in_ = POOLINIT::CUROBJ->get_stride_offset_in();
  valid_pixel_parallel_ = POOLINIT::CUROBJ->get_valid_pixel_parallel();
  stride_offset_out_ = POOLINIT::CUROBJ->get_stride_offset_out();
  stride_out_ = POOLINIT::CUROBJ->get_stride_out();
  jump_read_ = POOLINIT::CUROBJ->get_jump_read();
  jump_write_ = POOLINIT::CUROBJ->get_jump_write();
  kernel_h_ = POOLINIT::CUROBJ->get_kernel_h();
  kernel_w_ = POOLINIT::CUROBJ->get_kernel_w();
  shift_cut_ = POOLINIT::CUROBJ->get_shift_cut();

  // hardware config value
  vpp_ = valid_pixel_parallel_;
  pp_pool_ = ArchCfg::Instance().get_param().pool_engine().pixel_parallel();
  assert(vpp_ <= pp_pool_);
  pp_ = ArchCfg::Instance().get_param().conv_engine().pixel_parallel();
  assert(pp_pool_ <= pp_);
  cp_ = ArchCfg::Instance().get_param().pool_engine().channel_parallel();
  // self defined
  cg_ = channel_group_;
  ic_ = cg_ * cp_;  // input_channel is channel_group multiply channel_parallel
  oc_ = ic_;        // output_channel is channel_parallel
  src_h_ = (pp_pool_ - 1) * stride_h_ + kernel_h_;
  real_src_h_ = (vpp_ - 1) * stride_h_ + kernel_h_;
  src_w_ = (length_ - 1) * stride_w_ + kernel_w_;
  dst_h_ = pp_pool_;
  real_dst_h_ = vpp_;
  dst_w_ = length_;
  src_size_ = src_h_ * src_w_ * ic_;

  group_id_in_ = bank_id_in_ / pp_;
  group_id_out_ = bank_id_out_ / pp_;

  // resize related buffer size
  img_.resize(src_h_ * src_w_ * ic_);
  rlt_dtype_.resize(dst_h_ * dst_w_ * oc_);
  // std::fill(rlt_dtype_.begin(), rlt_dtype_.end(), MIN_DTYPE_VALUE);
  if (pool_type_ == 0) {  // max pooling
    std::fill(rlt_dtype_.begin(), rlt_dtype_.end(), MIN_DTYPE_VALUE);
  } else {  // average pooling
    std::fill(rlt_dtype_.begin(), rlt_dtype_.end(), 0);
  }
}

template <>
Pool<DPUVersion::DPU4F>::Pool(int inst_type, int instid,
                              vector<string>& inst_str,
                              vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  // debug
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_POOL);
  Helper<DPUVersion::DPU4F>::InstInit(inst_type_, root_debug_path_, debug_,
                                      opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPU4F>::inst_table;
  // get pool instruction field value
  bank_id_in_ = field_val_[TABLE::POOL_FIELD_BANK_ID_IN];
  bank_id_out_ = field_val_[TABLE::POOL_FIELD_BANK_ID_OUT];
  bank_addr_in_ = field_val_[TABLE::POOL_FIELD_BANK_ADDR_IN];
  pad_left_ = field_val_[TABLE::POOL_FIELD_PAD_LEFT];
  pad_top_ = field_val_[TABLE::POOL_FIELD_PAD_TOP];
  pad_right_ = field_val_[TABLE::POOL_FIELD_PAD_RIGHT];
  pad_bottom_ = field_val_[TABLE::POOL_FIELD_PAD_BOTTOM];
  bank_addr_out_ = field_val_[TABLE::POOL_FIELD_BANK_ADDR_OUT];
  length_ = field_val_[TABLE::POOL_FIELD_LENGTH];
  channel_group_ = field_val_[TABLE::POOL_FIELD_CHANNEL_GROUP];
  jump_read_endl_ = field_val_[TABLE::POOL_FIELD_JUMP_READ_ENDL];
  jump_write_endl_ = field_val_[TABLE::POOL_FIELD_JUMP_WRITE_ENDL];
  bank_addr_in_1_ = field_val_[TABLE::POOL_FIELD_BANK_ADDR_IN_1];
  bank_addr_in_2_ = field_val_[TABLE::POOL_FIELD_BANK_ADDR_IN_2];
  bank_addr_in_3_ = field_val_[TABLE::POOL_FIELD_BANK_ADDR_IN_3];
  // others
  // PoolInit instruction field
  using POOLINIT = PoolInit<DPUVersion::DPU4F>;
  pool_type_ = POOLINIT::CUROBJ->get_pool_type();
  stride_h_ = POOLINIT::CUROBJ->get_stride_h();
  stride_w_ = POOLINIT::CUROBJ->get_stride_w();
  stride_offset_in_ = POOLINIT::CUROBJ->get_stride_offset_in();
  valid_pixel_parallel_ = POOLINIT::CUROBJ->get_valid_pixel_parallel();
  stride_offset_out_ = POOLINIT::CUROBJ->get_stride_offset_out();
  stride_out_ = POOLINIT::CUROBJ->get_stride_out();
  jump_read_ = POOLINIT::CUROBJ->get_jump_read();
  jump_write_ = POOLINIT::CUROBJ->get_jump_write();
  kernel_h_ = POOLINIT::CUROBJ->get_kernel_h();
  kernel_w_ = POOLINIT::CUROBJ->get_kernel_w();
  shift_cut_ = POOLINIT::CUROBJ->get_shift_cut();

  // hardware config value
  vpp_ = valid_pixel_parallel_;
  pp_pool_ = ArchCfg::Instance().get_param().pool_engine().pixel_parallel();
  assert(vpp_ <= pp_pool_);
  pp_ = ArchCfg::Instance().get_param().conv_engine().pixel_parallel();
  assert(pp_pool_ <= pp_);
  cp_ = ArchCfg::Instance().get_param().pool_engine().channel_parallel();
  // self defined
  cg_ = channel_group_;
  ic_ = cg_ * cp_;  // input_channel is channel_group multiply channel_parallel
  oc_ = ic_;        // output_channel is channel_parallel
  src_h_ = (pp_pool_ - 1) * stride_h_ + kernel_h_;
  real_src_h_ = (vpp_ - 1) * stride_h_ + kernel_h_;
  src_w_ = (length_ - 1) * stride_w_ + kernel_w_;
  dst_h_ = pp_pool_;
  real_dst_h_ = vpp_;
  dst_w_ = length_;
  src_size_ = src_h_ * src_w_ * ic_;

  group_id_in_ = bank_id_in_ / pp_;
  group_id_out_ = bank_id_out_ / pp_;

  // resize related buffer size
  img_.resize(src_h_ * src_w_ * ic_);
  rlt_dtype_.resize(dst_h_ * dst_w_ * oc_);
  // std::fill(rlt_dtype_.begin(), rlt_dtype_.end(), MIN_DTYPE_VALUE);
  if (pool_type_ == 0) {  // max pooling
    std::fill(rlt_dtype_.begin(), rlt_dtype_.end(), MIN_DTYPE_VALUE);
  } else {  // average pooling
    std::fill(rlt_dtype_.begin(), rlt_dtype_.end(), 0);
  }
}

template <DPUVersion dv>
Pool<dv>::~Pool() {}

// public funcs
template <DPUVersion dv>
void Pool<dv>::Exec() {
  // read img, weights, bias data from bank
  read();
  debug_input_fmap("read");
  // padding for img data
  pad();
  debug_input_fmap("pad");
  debug_input_tick();
  // do conv operation of the whole feature map
  pool();
  debug_output_fmap();
  debug_output_tick();
  // save result to bank
  save();
}

template <DPUVersion dv>
void Pool<dv>::read() {
  // UNI_LOG_INFO << endl;
  // UNI_LOG_INFO << "read start..." << endl;

  // read img data from bank
  // UNI_LOG_INFO << "reading pooling data..." << endl;
  UNI_LOG_CHECK(stride_offset_in_ < stride_h_, SIM_PARAMETER_FAILED)
      << "invalid stride_offset_in";

  auto stride_offset = stride_offset_in_;
  auto bank_id = bank_id_in_;
  auto base_bank_id = group_id_in_ * pp_;
  auto addr_group = 0;
  UNI_LOG_CHECK(
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_in_) != nullptr,
      SIM_OUT_OF_RANGE)
      << "bank_id " << bank_id_in_ << " out of range!" << endl;
  auto bank_depth =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_in_)->GetH();
  for (int i = 0; i < real_src_h_; i++) {
    for (int j = 0; j < src_w_; j++) {
      int ddr_addr = i * src_w_ * ic_ + j * ic_;
      auto bank_addr_group_base = get_group_base_addr(addr_group);
      int bank_addr = (bank_addr_group_base + stride_offset * jump_read_endl_ +
                       j * jump_read_) %
                      bank_depth;
      auto bank = Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id);
      UNI_LOG_CHECK(bank != nullptr, SIM_OUT_OF_RANGE)
          << "bank_id " << bank_id << " out of range!" << endl;
      bank->Read(bank_addr, ic_, img_.data() + ddr_addr);
    }
    if (++stride_offset == stride_h_) {
      stride_offset = 0;
      if (++bank_id == base_bank_id + pp_) {
        bank_id = base_bank_id;
        ++addr_group;
      }
    }
  }

  // UNI_LOG_INFO << "read finish!" << endl;
}

template <>
void Pool<DPUVersion::DPU4F>::read() {
  UNI_LOG_CHECK(stride_offset_in_ < stride_h_, SIM_PARAMETER_FAILED)
      << "invalid stride_offset_in";

  auto stride_offset = stride_offset_in_;
  auto bank_id = bank_id_in_;
  auto base_bank_id = group_id_in_ * pp_;
  auto addr_group = 0;
  UNI_LOG_CHECK(
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_in_) != nullptr,
      SIM_OUT_OF_RANGE)
      << "bank_id " << bank_id_in_ << " out of range!" << endl;
  auto bank_depth =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_in_)->GetH();
  for (int i = 0; i < real_src_h_; i++) {
    for (int j = 0; j < src_w_; j++) {
      int ddr_addr = i * src_w_ * ic_ + j * ic_;
      auto bank_addr_group_base = get_group_base_addr(addr_group);
      int bank_addr = (bank_addr_group_base + stride_offset * jump_read_endl_ +
                       j * jump_read_) %
                      bank_depth;
      auto bank = Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id);
      UNI_LOG_CHECK(bank != nullptr, SIM_OUT_OF_RANGE)
          << "bank_id " << bank_id << " out of range!" << endl;
      BankShell::read(true, bank_id, ic_, bank_addr, img_.data() + ddr_addr);
    }
    if (++stride_offset == stride_h_) {
      stride_offset = 0;
      if (++bank_id == base_bank_id + pp_) {
        bank_id = base_bank_id;
        ++addr_group;
      }
    }
  }
}

template <DPUVersion dv>
int Pool<dv>::get_group_base_addr(int addr_group) {
  int ret = bank_addr_in_;
  if (addr_group == 0) {
    ret = bank_addr_in_;
  } else if (addr_group == 1) {
    ret = bank_addr_in_1_;
  } else if (addr_group == 2) {
    ret = bank_addr_in_2_;
  } else if (addr_group == 3) {
    ret = bank_addr_in_3_;
  } else {
    UNI_LOG_FATAL(SIM_PARAMETER_FAILED)
        << "bank group addr in is not enough, needed at least " << addr_group
        << ", infact it privide 4" << endl;
  }
  return ret;
}

template <>
int Pool<DPUVersion::DPUV3ME>::get_group_base_addr(int addr_group) {
  return bank_addr_in_ + addr_group * stride_h_ * jump_read_endl_;
}

template <DPUVersion dv>
void Pool<dv>::pad() {
  // UNI_LOG_INFO << endl;
  // UNI_LOG_INFO << "pad start..." << endl;
  if (pool_type_ == 2) return;

  auto pad_value = pool_type_ == 0 ? MIN_DTYPE_VALUE : 0;
  // padding left
  // UNI_LOG_INFO << "padding left..." << endl;
  for (int i = 0; i < real_src_h_; i++) {
    for (int j = 0; j < pad_left_ && j < src_w_; j++) {
      for (int k = 0; k < ic_; k++) {
        unsigned int addr = i * src_w_ * ic_ + j * ic_ + k;
        UNI_LOG_CHECK(addr < img_.size(), SIM_OUT_OF_RANGE)
            << addr << " >= " << img_.size();
        img_[addr] = pad_value;
      }
    }
  }

  // padding top
  // UNI_LOG_INFO << "padding top..." << endl;
  for (int i = 0; i < pad_top_ && i < real_src_h_; i++) {
    for (int j = 0; j < src_w_; j++) {
      for (int k = 0; k < ic_; k++) {
        unsigned int addr = i * src_w_ * ic_ + j * ic_ + k;
        UNI_LOG_CHECK(addr < img_.size(), SIM_OUT_OF_RANGE)
            << addr << " >= " << img_.size();
        img_[addr] = pad_value;
      }
    }
  }

  // padding right
  // UNI_LOG_INFO << "padding right..." << endl;
  for (int i = 0; i < real_src_h_; i++) {
    for (int j = src_w_ - kernel_w_ + pad_right_ + 1; j < src_w_; j++) {
      for (int k = 0; k < ic_; k++) {
        unsigned int addr = i * src_w_ * ic_ + j * ic_ + k;
        UNI_LOG_CHECK(addr < img_.size(), SIM_OUT_OF_RANGE)
            << addr << " >= " << img_.size();
        img_[addr] = pad_value;
      }
    }
  }

  // padding bottom, it's different from padding right
  // UNI_LOG_INFO << "padding bottom..." << endl;
  for (int i = real_src_h_ - kernel_h_ + pad_bottom_ + 1; i < real_src_h_;
       i++) {
    for (int j = 0; j < src_w_; j++) {
      for (int k = 0; k < ic_; k++) {
        unsigned int addr = i * src_w_ * ic_ + j * ic_ + k;
        UNI_LOG_CHECK(addr < img_.size(), SIM_OUT_OF_RANGE)
            << addr << " >= " << img_.size();
        img_[addr] = pad_value;
      }
    }
  }
  // UNI_LOG_INFO << "pad finish!" << endl;
}

template <DPUVersion dv>
void Pool<dv>::pool() {
  // UNI_LOG_INFO << endl;
  // UNI_LOG_INFO << "pool start..." << endl;
  for (int i = 0; i + kernel_h_ <= real_src_h_; i += stride_h_) {
    for (int j = 0; j + kernel_w_ <= src_w_; j += stride_w_) {
      pool_one(i, j);
    }
  }

  // UNI_LOG_INFO << "pool finish!" << endl;
}

template <DPUVersion dv>
void Pool<dv>::pool_one(int idx_src_h, int idx_src_w) {
  // init result value
  int idx_dst_h = idx_src_h / stride_h_;
  int idx_dst_w = idx_src_w / stride_w_;

  // calculate conv value
  if (pool_type_ == 0 || pool_type_ == 2) {
    for (int k = 0; k < ic_; k++) {
      int rlt_addr = idx_dst_h * dst_w_ * oc_ + idx_dst_w * oc_ + k;
      for (int i = 0; i < kernel_h_; i++) {
        for (int j = 0; j < kernel_w_; j++) {
          // calculate address
          int img_addr =
              (idx_src_h + i) * src_w_ * ic_ + (idx_src_w + j) * ic_ + k;
          if (rlt_dtype_[rlt_addr] < img_[img_addr])
            rlt_dtype_[rlt_addr] = img_[img_addr];
        }
      }
      rlt_dtype_[rlt_addr] = Calc::DPURound<DPU_DATA_TYPE>(
          shift_cut_ >= 8
              ? static_cast<int>(rlt_dtype_[rlt_addr]) / pow(2, shift_cut_ - 8)
              : static_cast<int>(rlt_dtype_[rlt_addr]) * pow(2, shift_cut_));
    }
  } else {
    for (int k = 0; k < ic_; k++) {
      auto tmp = 0.f;
      for (int i = 0; i < kernel_h_; i++) {
        for (int j = 0; j < kernel_w_; j++) {
          // calculate address
          int img_addr =
              (idx_src_h + i) * src_w_ * ic_ + (idx_src_w + j) * ic_ + k;
          tmp += img_[img_addr];
        }
      }
      int rlt_addr = idx_dst_h * dst_w_ * oc_ + idx_dst_w * oc_ + k;
      // tmp /= kernel_w_ * kernel_h_;
      // divide pattern from v1.3.1-multsize-pool
      switch (kernel_w_) {
        case 1:
          tmp /= (kernel_w_ * kernel_h_ * 1.0);
          break;
        case 2:
          tmp /= (kernel_w_ * kernel_h_ * 1.0);
          break;
        case 3:
          tmp *= (28.0 / 256);
          break;
        case 4:
          tmp /= (kernel_w_ * kernel_h_ * 1.0);
          break;
        case 5:
          tmp *= (10.0 / 256);
          break;
        case 6:
          tmp *= (7.0 / 256);
          break;
        case 7:
          tmp *= (21.0 / 1024);
          break;
        case 8:
          tmp /= (kernel_w_ * kernel_h_ * 1.0);
          break;
        default:
          UNI_LOG_FATAL(SIM_PARAMETER_FAILED) << "Kernel size error!" << endl;
      }
      tmp = shift_cut_ >= 8 ? tmp * 1.0 / pow(2.0, shift_cut_ - 8)
                            : tmp * 1.0 * pow(2.0, shift_cut_);
      rlt_dtype_[rlt_addr] = Calc::DPURound<DPU_DATA_TYPE>(tmp);
    }
  }
}

template <DPUVersion dv>
void Pool<dv>::save() {
  // UNI_LOG_INFO << endl;
  // UNI_LOG_INFO << "pool save start..." << endl;
  /* for(int i=0; i<real_dst_h_; i++) {
       int id = (bank_id_out_%pp_+i) % pp_;
       int cycles = (bank_id_out_%pp_+i) / pp_;
       int bank_id = group_id_out_*pp_ + id;
       for(int j=0; j<dst_w_; j++) {
           int bank_addr = (cycles*jump_write_endl_ + j*jump_write_ +
   bank_addr_out_) % bank_h_[SimCfg::BANK_TYPE_IMG]; int rlt_addr =
   i*dst_w_*oc_
   + j*oc_; auto bank = Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id);
           bank->Write(bank_addr, oc_, rlt_dtype_.data()+rlt_addr);
       }
   }*/
  UNI_LOG_CHECK(
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_out_) != nullptr,
      SIM_OUT_OF_RANGE)
      << "bank_id " << bank_id_out_ << " out of range!" << endl;
  auto bank_depth =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_out_)->GetH();
  auto bank_id_out_offset = bank_id_out_ % pp_;
  auto bank_id_out_base = bank_id_out_ - bank_id_out_ % pp_;
  for (auto idx_h = 0; idx_h < real_dst_h_; idx_h++) {
    // bank change, when (idx_h + stride_offset_out) > stride_out
    auto bank_offset = static_cast<uint32_t>(
        floor((float)(idx_h + stride_offset_out_) / stride_out_));
    // bank group inner idx increase
    auto bank_group_offset = static_cast<uint32_t>(
        floor((float)(bank_id_out_offset + bank_offset) / pp_));
    auto bank_id_this_row =
        bank_id_out_base + (bank_id_out_offset + bank_offset) % pp_;
    auto bank_addr_this_row =
        bank_addr_out_                                        // base_addr
        + bank_group_offset * stride_out_ * jump_write_endl_  // change bank
        + ((idx_h + stride_offset_out_) % stride_out_) *
              jump_write_endl_  // change stride offset
        ;
    auto bank = Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_this_row);
    UNI_LOG_CHECK(bank != nullptr, SIM_OUT_OF_RANGE)
        << "bank_id " << bank_id_this_row << " out of range!" << endl;
    for (int idx_w = 0; idx_w < dst_w_; idx_w++) {
      auto bank_addr_this_pixel =
          (bank_addr_this_row + idx_w * jump_write_) % bank_depth;
      auto rlt_addr_this_pixel = idx_h * dst_w_ * oc_ + idx_w * oc_;
      bank->Write(bank_addr_this_pixel, oc_,
                  rlt_dtype_.data() + rlt_addr_this_pixel);
    }
  }
  // UNI_LOG_INFO << "pool save finish!" << endl;
}

template <>
void Pool<DPUVersion::DPU4F>::save() {
  UNI_LOG_CHECK(
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_out_) != nullptr,
      SIM_OUT_OF_RANGE)
      << "bank_id " << bank_id_out_ << " out of range!" << endl;
  auto bank_depth =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_out_)->GetH();
  auto bank_id_out_offset = bank_id_out_ % pp_;
  auto bank_id_out_base = bank_id_out_ - bank_id_out_ % pp_;
  for (auto idx_h = 0; idx_h < real_dst_h_; idx_h++) {
    // bank change, when (idx_h + stride_offset_out) > stride_out
    auto bank_offset = static_cast<uint32_t>(
        floor((float)(idx_h + stride_offset_out_) / stride_out_));
    // bank group inner idx increase
    auto bank_group_offset = static_cast<uint32_t>(
        floor((float)(bank_id_out_offset + bank_offset) / pp_));
    auto bank_id_this_row =
        bank_id_out_base + (bank_id_out_offset + bank_offset) % pp_;
    auto bank_addr_this_row =
        bank_addr_out_                                        // base_addr
        + bank_group_offset * stride_out_ * jump_write_endl_  // change bank
        + ((idx_h + stride_offset_out_) % stride_out_) *
              jump_write_endl_  // change stride offset
        ;
    auto bank = Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_this_row);
    UNI_LOG_CHECK(bank != nullptr, SIM_OUT_OF_RANGE)
        << "bank_id " << bank_id_this_row << " out of range!" << endl;
    for (int idx_w = 0; idx_w < dst_w_; idx_w++) {
      auto bank_addr_this_pixel =
          (bank_addr_this_row + idx_w * jump_write_) % bank_depth;
      auto rlt_addr_this_pixel = idx_h * dst_w_ * oc_ + idx_w * oc_;
      BankShell::write(true, bank_id_this_row, oc_, bank_addr_this_pixel,
                       rlt_dtype_.data() + rlt_addr_this_pixel);
    }
  }
}

template <DPUVersion dv>
void Pool<dv>::debug_input_fmap(string prefix) {
  if (!InstBase::debug_) return;

  string head;
  string save_name;

  // make save file name
  stringstream ss;
  ss.clear();
  ss.str("");
  ss << InstBase::debug_path_ << "pool_" << prefix << "_fmap."
     << InstBase::instid_;
  ss >> save_name;

  // save img data
  vector<DPU_DATA_TYPE> img(ic_ * src_h_ * src_w_);
  Calc::HWC2CHW(src_h_, src_w_, ic_, img_.data(), img.data());
  for (int i = 0; i < ic_; i++) {
    // save input_channel title to file
    ss.clear();
    ss.str("");
    ss << "channel_" << i << ":";
    ss >> head;
    head += "\n";
    Util::SaveBin(save_name, head.data(), head.size(), SM_APPEND);
    for (int j = 0; j < src_h_; j++) {
      int addr = i * src_h_ * src_w_ + j * src_w_;
      Util::SaveDec(save_name, img.data() + addr, src_w_, src_w_, SM_APPEND);
    }
  }
}

// take <kernel_h_ * kernel_w_ * cp_> fmap block as a unit, w->h->cg
template <DPUVersion dv>
void Pool<dv>::debug_input_tick() {
  if (!debug_) return;

  string head;
  string save_name;

  // make save file name
  stringstream ss;
  ss.clear();
  ss.str("");
  ss << debug_path_ << "pool_start_tick." << instid_;
  ss >> save_name;

  for (int i = 0; i < cg_; i++) {
    for (int j = 0; j + kernel_h_ <= src_h_; j += stride_h_) {
      for (int k = 0; k + kernel_w_ <= src_w_; k += stride_w_) {
        vector<DPU_DATA_TYPE> tmp(kernel_h_ * kernel_w_ * cp_);
        for (int m = 0; m < kernel_h_; m++) {
          for (int n = 0; n < kernel_w_; n++) {
            for (int l = 0; l < cp_; l++) {
              int cur_addr = m * kernel_w_ * cp_ + n * cp_ + l;
              int ori_addr = (j + m) * src_w_ * cg_ * cp_ +
                             (k + n) * cg_ * cp_ + i * cp_ + l;

              tmp[cur_addr] = img_[ori_addr];
            }
          }
        }
        Util::SaveHexContBigEnd(save_name,
                                reinterpret_cast<const char*>(tmp.data()),
                                tmp.size() * sizeof(DPU_DATA_TYPE),
                                cp_ * sizeof(DPU_DATA_TYPE), SM_APPEND);
      }
    }
  }
}

template <DPUVersion dv>
void Pool<dv>::debug_output_fmap() {
  if (!InstBase::debug_) return;

  // make save file name
  string save_name;
  stringstream ss;
  ss.clear();
  ss.str("");
  ss << InstBase::debug_path_ << "pool_result_fmap." << InstBase::instid_;
  ss >> save_name;

  // save img data
  vector<DPU_DATA_TYPE> rlt_s8(oc_ * dst_h_ * dst_w_);
  Calc::HWC2CHW(dst_h_, dst_w_, oc_, rlt_dtype_.data(), rlt_s8.data());

  for (int i = 0; i < oc_; i++) {
    // save input_channel title to file
    string head;
    ss.clear();
    ss.str("");
    ss << "channel_" << i << ":";
    ss >> head;
    head += "\n";
    Util::SaveBin(save_name, head.data(), head.size(), SM_APPEND);
    for (int j = 0; j < dst_h_; j++) {
      int addr = i * dst_h_ * dst_w_ + j * dst_w_;
      Util::SaveDec(save_name, rlt_s8.data() + addr, dst_w_, dst_w_, SM_APPEND);
    }
  }
}

template <DPUVersion dv>
void Pool<dv>::debug_output_tick() {
  if (!debug_) return;

  // make save file name
  string save_name;
  stringstream ss;
  ss.clear();
  ss.str("");
  ss << debug_path_ << "pool_result_tick." << instid_;
  ss >> save_name;

  vector<DPU_DATA_TYPE> rlt_tmp(rlt_dtype_.size());
  // cout <<  "rlt_tmp.size = " << rlt_dtype_.size() << endl;
  // change data order from hwc to wch
  for (int i = 0; i < length_; i++) {
    for (int j = 0; j < cp_; j++) {
      for (int k = 0; k < dst_h_; k++) {
        uint32_t addr_tmp = i * cp_ * dst_h_ + j * dst_h_ + k;
        uint32_t addr_ori = k * length_ * cp_ + i * cp_ + j;
        if (addr_tmp >= rlt_dtype_.size() || addr_ori >= rlt_dtype_.size()) {
          cout << "error!" << endl;
          abort();
        }
        rlt_tmp[addr_tmp] = rlt_dtype_[addr_ori];
      }
    }
  }

  // save img
  auto bank_depth =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_out_)->GetH();
  auto* p = reinterpret_cast<const char*>(rlt_dtype_.data());
  auto line_size = cp_ * sizeof(DPU_DATA_TYPE);
  // bank id of the first bank in this bank_group
  auto bank_id_out_offset = bank_id_out_ % pp_;
  auto bank_id_out_base = bank_id_out_ - bank_id_out_ % pp_;
  for (int idx_w = 0; idx_w < length_; idx_w++) {
    for (auto idx_h = 0; idx_h < real_dst_h_; idx_h++) {
      auto bank_offset = static_cast<uint32_t>(
          floor((float)(idx_h + stride_offset_out_) / stride_out_));
      auto bank_group_offset = static_cast<uint32_t>(
          floor((float)(bank_id_out_offset + bank_offset) / pp_));
      auto bank_id_this_row =
          bank_id_out_base + (bank_id_out_offset + bank_offset) % pp_;
      auto bank_addr_this_row =
          bank_addr_out_                                        // base_addr
          + bank_group_offset * stride_out_ * jump_write_endl_  // change bank
          + ((idx_h + stride_offset_out_) % stride_out_) *
                jump_write_endl_  // change stride offset
          ;
      auto bank_addr_this_pixel =
          (bank_addr_this_row + idx_w * jump_write_) % bank_depth;
      auto offset = (idx_w + idx_h * length_) * line_size;
      /*cout << "save_name = " << save_name
           << ", offset = " << offset
           << ", line_size = " << line_size
           << ", bank_id_this_row = " << bank_id_this_row
           << ", bank_addr_this_pixel = " << bank_addr_this_pixel
           << endl;
           */
      Util::SaveHexContSmallEndBankAddr(save_name, p + offset, line_size,
                                        line_size, bank_depth, bank_id_this_row,
                                        bank_addr_this_pixel, SM_APPEND);
    }
  }
}

template <DPUVersion T>
void Pool<T>::GenInstStr(int inst_fmt) {
  ac_ = Helper<T>::GetInstStr(inst_type_, inst_fmt, dpdon_, dpdby_, field_str_);
}
