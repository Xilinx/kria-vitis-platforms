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
#include "DptWise.hpp"
#include "DWInit.hpp"

// NOTE: it's famous Wundefined-var-template issue,
// if you want to compile using Clang, add instantiation here
template <DPUVersion dv>
DWInit<dv>* DWInit<dv>::CUROBJ;

template <>
DptWise<DPUVersion::DPUV2>::DptWise(int inst_type, int instid,
                                    vector<string>& inst_str,
                                    vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  // dwconv instruction fields
  // debug
  debug_ =
      debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_DWCONV);
  Helper<DPUVersion::DPUV2>::InstInit(inst_type_, root_debug_path_, debug_,
                                      opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPUV2>::inst_table;
  // get dwconv instruction field value
  channel_group_ = field_val_[TABLE::DPTWISE_FIELD_CHANNEL_GROUP];
  bank_addr_in_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ADDR_IN];
  pad_left_ = field_val_[TABLE::DPTWISE_FIELD_PAD_LEFT];
  pad_right_ = field_val_[TABLE::DPTWISE_FIELD_PAD_RIGHT];
  pad_top_ = field_val_[TABLE::DPTWISE_FIELD_PAD_TOP];
  pad_bottom_ = field_val_[TABLE::DPTWISE_FIELD_PAD_BOTTOM];
  channel_offset_ = field_val_[TABLE::DPTWISE_FIELD_CHANNEL_OFFSET];
  bank_id_in_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ID_IN];
  bank_id_out_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ID_OUT];
  bank_addr_out_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ADDR_OUT];
  bank_addr_weights_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ADDR_WEIGHTS];
  length_ = field_val_[TABLE::DPTWISE_FIELD_LENGTH];
  bank_addr_bias_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ADDR_BIAS];

  bank_addr_in_1_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ADDR_IN_1];
  bank_addr_in_2_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ADDR_IN_2];
  bank_addr_in_3_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ADDR_IN_3];
  act_type_ = field_val_[TABLE::DPTWISE_FIELD_ACT_TYPE];

  // dwinit value
  using DWINIT = DWInit<DPUVersion::DPUV2>;
  jump_read_ = DWINIT::CUROBJ->get_jump_read();
  jump_write_ = DWINIT::CUROBJ->get_jump_write();
  jump_read_endl_ = DWINIT::CUROBJ->get_jump_read_endl();
  stride_offset_in_ = DWINIT::CUROBJ->get_stride_offset_in();
  valid_pixel_parallel_ = DWINIT::CUROBJ->get_valid_pixel_parallel();
  stride_w_ = DWINIT::CUROBJ->get_stride_w();
  stride_h_ = DWINIT::CUROBJ->get_stride_h();
  kernel_w_ = DWINIT::CUROBJ->get_kernel_w();
  kernel_h_ = DWINIT::CUROBJ->get_kernel_h();
  jump_write_endl_ = DWINIT::CUROBJ->get_jump_write_endl();
  stride_offset_out_ = DWINIT::CUROBJ->get_stride_offset_out();
  stride_out_ = DWINIT::CUROBJ->get_stride_out();
  shift_bias_ = DWINIT::CUROBJ->get_shift_bias();
  shift_cut_ = DWINIT::CUROBJ->get_shift_cut();

  // constraints from hardware implementation
  UNI_LOG_CHECK(shift_bias_ <= 20, SIM_PARAMETER_FAILED)
      << "invalid shift_bias";

  // self defined
  vpp_ = valid_pixel_parallel_;
  pp_dwconv_ = ArchCfg::Instance().get_param().dwconv_engine().pixel_parallel();
  assert(vpp_ <= pp_dwconv_);
  pp_ = ArchCfg::Instance().get_param().conv_engine().pixel_parallel();
  assert(pp_dwconv_ <= pp_);
  cp_ = ArchCfg::Instance().get_param().dwconv_engine().channel_parallel();
  cg_ = channel_group_;
  co_ = channel_offset_;
  ic_ = cg_ * cp_;  // input_channel is channel_group multiply channel_parallel
  oc_ = ic_;        // output_channel is channel_parallel
  src_h_ = (pp_dwconv_ - 1) * stride_h_ + kernel_h_;
  real_src_h_ = (vpp_ - 1) * stride_h_ + kernel_h_;
  src_w_ = (length_ - 1) * stride_w_ + kernel_w_;
  dst_h_ = pp_dwconv_;
  real_dst_h_ = vpp_;
  dst_w_ = length_;
  src_size_ = src_h_ * src_w_ * ic_;

  group_id_in_ = bank_id_in_ / pp_;
  group_id_out_ = bank_id_out_ / pp_;

  // resize related buffer size
  img_.resize(src_h_ * src_w_ * ic_);
  weights_.resize(oc_ * kernel_h_ * kernel_w_);
  bias_.resize(oc_);
  rlt_s64_.resize(dst_h_ * dst_w_ * oc_);
  rlt_dtype_.resize(dst_h_ * dst_w_ * oc_);
}

template <>
DptWise<DPUVersion::DPUV3E>::DptWise(int inst_type, int instid,
                                     vector<string>& inst_str,
                                     vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  // dwconv instruction fields
  // debug
  debug_ =
      debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_DWCONV);
  Helper<DPUVersion::DPUV3E>::InstInit(inst_type_, root_debug_path_, debug_,
                                       opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPUV3E>::inst_table;
  // get dwconv instruction field value
  channel_group_ = field_val_[TABLE::DPTWISE_FIELD_CHANNEL_GROUP];
  bank_addr_in_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ADDR_IN];
  pad_left_ = field_val_[TABLE::DPTWISE_FIELD_PAD_LEFT];
  pad_right_ = field_val_[TABLE::DPTWISE_FIELD_PAD_RIGHT];
  pad_top_ = field_val_[TABLE::DPTWISE_FIELD_PAD_TOP];
  pad_bottom_ = field_val_[TABLE::DPTWISE_FIELD_PAD_BOTTOM];
  channel_offset_ = field_val_[TABLE::DPTWISE_FIELD_CHANNEL_OFFSET];
  bank_id_in_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ID_IN];
  bank_id_out_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ID_OUT];
  bank_addr_out_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ADDR_OUT];
  bank_addr_weights_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ADDR_WEIGHTS];
  length_ = field_val_[TABLE::DPTWISE_FIELD_LENGTH];
  bank_addr_bias_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ADDR_BIAS];

  bank_addr_in_1_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ADDR_IN_1];
  bank_addr_in_2_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ADDR_IN_2];
  bank_addr_in_3_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ADDR_IN_3];

  // dwinit value
  using DWINIT = DWInit<DPUVersion::DPUV3E>;
  jump_read_ = DWINIT::CUROBJ->get_jump_read();
  jump_write_ = DWINIT::CUROBJ->get_jump_write();
  jump_read_endl_ = DWINIT::CUROBJ->get_jump_read_endl();
  stride_offset_in_ = DWINIT::CUROBJ->get_stride_offset_in();
  valid_pixel_parallel_ = DWINIT::CUROBJ->get_valid_pixel_parallel();
  stride_w_ = DWINIT::CUROBJ->get_stride_w();
  stride_h_ = DWINIT::CUROBJ->get_stride_h();
  kernel_w_ = DWINIT::CUROBJ->get_kernel_w();
  kernel_h_ = DWINIT::CUROBJ->get_kernel_h();
  jump_write_endl_ = DWINIT::CUROBJ->get_jump_write_endl();
  stride_offset_out_ = DWINIT::CUROBJ->get_stride_offset_out();
  stride_out_ = DWINIT::CUROBJ->get_stride_out();
  shift_bias_ = DWINIT::CUROBJ->get_shift_bias();
  shift_cut_ = DWINIT::CUROBJ->get_shift_cut();
  act_type_ = DWINIT::CUROBJ->get_act_type();

  // constraints from hardware implementation
  if (shift_bias_ < 32) {
    UNI_LOG_CHECK(shift_bias_ <= 20, SIM_PARAMETER_FAILED)
        << "invalid shift_bias";
  }

  // self defined
  vpp_ = valid_pixel_parallel_;
  pp_dwconv_ = ArchCfg::Instance().get_param().dwconv_engine().pixel_parallel();
  assert(vpp_ <= pp_dwconv_);
  pp_ = ArchCfg::Instance().get_param().conv_engine().pixel_parallel();
  assert(pp_dwconv_ <= pp_);
  cp_ = ArchCfg::Instance().get_param().dwconv_engine().channel_parallel();
  cg_ = channel_group_;
  co_ = channel_offset_;
  ic_ = cg_ * cp_;  // input_channel is channel_group multiply channel_parallel
  oc_ = ic_;        // output_channel is channel_parallel
  src_h_ = (pp_dwconv_ - 1) * stride_h_ + kernel_h_;
  real_src_h_ = (vpp_ - 1) * stride_h_ + kernel_h_;
  src_w_ = (length_ - 1) * stride_w_ + kernel_w_;
  dst_h_ = pp_dwconv_;
  real_dst_h_ = vpp_;
  dst_w_ = length_;
  src_size_ = src_h_ * src_w_ * ic_;

  group_id_in_ = bank_id_in_ / pp_;
  group_id_out_ = bank_id_out_ / pp_;

  // resize related buffer size
  img_.resize(src_h_ * src_w_ * ic_);
  weights_.resize(oc_ * kernel_h_ * kernel_w_);
  bias_.resize(oc_);
  rlt_s64_.resize(dst_h_ * dst_w_ * oc_);
  rlt_dtype_.resize(dst_h_ * dst_w_ * oc_);
}

template <>
DptWise<DPUVersion::DPUV3ME>::DptWise(int inst_type, int instid,
                                      vector<string>& inst_str,
                                      vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  // dwconv instruction fields
  // debug
  debug_ =
      debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_DWCONV);
  Helper<DPUVersion::DPUV3ME>::InstInit(inst_type_, root_debug_path_, debug_,
                                        opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPUV3ME>::inst_table;
  channel_group_ = field_val_[TABLE::DPTWISE_FIELD_CHANNEL_GROUP];
  bank_addr_in_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ADDR_IN];
  pad_left_ = field_val_[TABLE::DPTWISE_FIELD_PAD_LEFT];
  pad_right_ = field_val_[TABLE::DPTWISE_FIELD_PAD_RIGHT];
  pad_top_ = field_val_[TABLE::DPTWISE_FIELD_PAD_TOP];
  pad_bottom_ = field_val_[TABLE::DPTWISE_FIELD_PAD_BOTTOM];
  channel_offset_ = field_val_[TABLE::DPTWISE_FIELD_CHANNEL_OFFSET];
  bank_id_in_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ID_IN];
  bank_id_out_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ID_OUT];
  bank_addr_out_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ADDR_OUT];
  bank_addr_weights_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ADDR_WEIGHTS];
  length_ = field_val_[TABLE::DPTWISE_FIELD_LENGTH];
  bank_addr_bias_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ADDR_BIAS];

  // dwinit value
  using DWINIT = DWInit<DPUVersion::DPUV3ME>;
  jump_read_ = DWINIT::CUROBJ->get_jump_read();
  jump_write_ = DWINIT::CUROBJ->get_jump_write();
  jump_read_endl_ = DWINIT::CUROBJ->get_jump_read_endl();
  stride_offset_in_ = DWINIT::CUROBJ->get_stride_offset_in();
  valid_pixel_parallel_ = DWINIT::CUROBJ->get_valid_pixel_parallel();
  stride_w_ = DWINIT::CUROBJ->get_stride_w();
  stride_h_ = DWINIT::CUROBJ->get_stride_h();
  kernel_w_ = DWINIT::CUROBJ->get_kernel_w();
  kernel_h_ = DWINIT::CUROBJ->get_kernel_h();
  jump_write_endl_ = DWINIT::CUROBJ->get_jump_write_endl();
  stride_offset_out_ = DWINIT::CUROBJ->get_stride_offset_out();
  stride_out_ = DWINIT::CUROBJ->get_stride_out();
  shift_relusix_ = DWINIT::CUROBJ->get_shift_relusix();
  shift_bias_ = DWINIT::CUROBJ->get_shift_bias();
  shift_cut_ = DWINIT::CUROBJ->get_shift_cut();
  act_type_ = DWINIT::CUROBJ->get_act_type();

  // constraints from hardware implementation
  if (shift_bias_ < 32) {
    UNI_LOG_CHECK(shift_bias_ <= 20, SIM_PARAMETER_FAILED)
        << "invalid shift_bias";
  }

  // self defined
  vpp_ = valid_pixel_parallel_;
  pp_dwconv_ = ArchCfg::Instance().get_param().dwconv_engine().pixel_parallel();
  assert(vpp_ <= pp_dwconv_);
  pp_ = ArchCfg::Instance().get_param().conv_engine().pixel_parallel();
  assert(pp_dwconv_ <= pp_);
  cp_ = ArchCfg::Instance().get_param().dwconv_engine().channel_parallel();
  cg_ = channel_group_;
  co_ = channel_offset_;
  ic_ = cg_ * cp_;  // input_channel is channel_group multiply channel_parallel
  oc_ = ic_;        // output_channel is channel_parallel
  src_h_ = (pp_dwconv_ - 1) * stride_h_ + kernel_h_;
  real_src_h_ = (vpp_ - 1) * stride_h_ + kernel_h_;
  src_w_ = (length_ - 1) * stride_w_ + kernel_w_;
  dst_h_ = pp_dwconv_;
  real_dst_h_ = vpp_;
  dst_w_ = length_;
  src_size_ = src_h_ * src_w_ * ic_;

  int icp =
      ArchCfg::Instance().get_param().conv_engine().input_channel_parallel();
  scale_ = cp_ / icp;

  // input bank id check
  int fmap_bank_group = SimCfg::Instance().get_fmap_bank_group();
  int group_id_in = bank_id_in_ / pp_;
  UNI_LOG_CHECK(group_id_in == (fmap_bank_group - 1), SIM_PARAMETER_FAILED)
      << "invalid input bank id";

  UNI_LOG_CHECK(jump_read_ == channel_group_, SIM_PARAMETER_FAILED)
      << "Jump read should equal to channel_group. Now jump_read = "
      << jump_read_ << ", channel_group = " << channel_group_;

  // output bank id check
  int group_id_out = bank_id_out_ / pp_;
  UNI_LOG_CHECK(group_id_out < (fmap_bank_group - 1), SIM_PARAMETER_FAILED)
      << "invalid bank_id for write HBM";

  // resize related buffer size
  img_.resize(src_h_ * src_w_ * ic_);
  weights_.resize(oc_ * kernel_h_ * kernel_w_);
  bias_.resize(oc_);
  rlt_s64_.resize(dst_h_ * dst_w_ * oc_);
  rlt_dtype_.resize(dst_h_ * dst_w_ * oc_);
}

template <>
DptWise<DPUVersion::DPU4F>::DptWise(int inst_type, int instid,
                                    vector<string>& inst_str,
                                    vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  // dwconv instruction fields
  // debug
  debug_ =
      debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_DWCONV);
  Helper<DPUVersion::DPU4F>::InstInit(inst_type_, root_debug_path_, debug_,
                                      opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPU4F>::inst_table;
  // get dwconv instruction field value
  channel_group_ = field_val_[TABLE::DPTWISE_FIELD_CHANNEL_GROUP];
  bank_addr_in_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ADDR_IN];
  pad_left_ = field_val_[TABLE::DPTWISE_FIELD_PAD_LEFT];
  pad_right_ = field_val_[TABLE::DPTWISE_FIELD_PAD_RIGHT];
  pad_top_ = field_val_[TABLE::DPTWISE_FIELD_PAD_TOP];
  pad_bottom_ = field_val_[TABLE::DPTWISE_FIELD_PAD_BOTTOM];
  channel_offset_ = field_val_[TABLE::DPTWISE_FIELD_CHANNEL_OFFSET];
  bank_id_in_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ID_IN];
  bank_id_out_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ID_OUT];
  bank_addr_out_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ADDR_OUT];
  bank_addr_weights_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ADDR_WEIGHTS];
  length_ = field_val_[TABLE::DPTWISE_FIELD_LENGTH];
  bank_addr_bias_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ADDR_BIAS];

  bank_addr_in_1_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ADDR_IN_1];
  bank_addr_in_2_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ADDR_IN_2];
  bank_addr_in_3_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ADDR_IN_3];
  act_type_ = field_val_[TABLE::DPTWISE_FIELD_ACT_TYPE];

  // dwinit value
  using DWINIT = DWInit<DPUVersion::DPU4F>;
  jump_read_ = DWINIT::CUROBJ->get_jump_read();
  jump_write_ = DWINIT::CUROBJ->get_jump_write();
  jump_read_endl_ = DWINIT::CUROBJ->get_jump_read_endl();
  stride_offset_in_ = DWINIT::CUROBJ->get_stride_offset_in();
  valid_pixel_parallel_ = DWINIT::CUROBJ->get_valid_pixel_parallel();
  stride_w_ = DWINIT::CUROBJ->get_stride_w();
  stride_h_ = DWINIT::CUROBJ->get_stride_h();
  kernel_w_ = DWINIT::CUROBJ->get_kernel_w();
  kernel_h_ = DWINIT::CUROBJ->get_kernel_h();
  jump_write_endl_ = DWINIT::CUROBJ->get_jump_write_endl();
  stride_offset_out_ = DWINIT::CUROBJ->get_stride_offset_out();
  stride_out_ = DWINIT::CUROBJ->get_stride_out();
  // shift_bias_ = DWINIT::CUROBJ->get_shift_bias();
  // shift_cut_ = DWINIT::CUROBJ->get_shift_cut();

  // self defined
  vpp_ = valid_pixel_parallel_;
  pp_dwconv_ = ArchCfg::Instance().get_param().dwconv_engine().pixel_parallel();
  assert(vpp_ <= pp_dwconv_);
  pp_ = ArchCfg::Instance().get_param().conv_engine().pixel_parallel();
  assert(pp_dwconv_ <= pp_);
  cp_ = ArchCfg::Instance().get_param().dwconv_engine().channel_parallel();
  cg_ = channel_group_;
  co_ = channel_offset_;
  ic_ = cg_ * cp_;  // input_channel is channel_group multiply channel_parallel
  oc_ = ic_;        // output_channel is channel_parallel
  src_h_ = (pp_dwconv_ - 1) * stride_h_ + kernel_h_;
  real_src_h_ = (vpp_ - 1) * stride_h_ + kernel_h_;
  src_w_ = (length_ - 1) * stride_w_ + kernel_w_;
  dst_h_ = pp_dwconv_;
  real_dst_h_ = vpp_;
  dst_w_ = length_;
  src_size_ = src_h_ * src_w_ * ic_;

  group_id_in_ = bank_id_in_ / pp_;
  group_id_out_ = bank_id_out_ / pp_;

  // resize related buffer size
  img_.resize(src_h_ * src_w_ * ic_);
  weights_.resize(oc_ * kernel_h_ * kernel_w_);
  bias_.resize(oc_);
  shift_bias_buf_.resize(oc_);
  shift_cut_buf_.resize(oc_);
  rlt_s64_.resize(dst_h_ * dst_w_ * oc_);
  rlt_dtype_.resize(dst_h_ * dst_w_ * oc_);
  for (auto& shift : shift_bias_buf_)
    UNI_LOG_CHECK(shift <= 20, SIM_PARAMETER_FAILED) << "invalid shift_bias";
}

template <>
DptWise<DPUVersion::XVDPU>::DptWise(int inst_type, int instid,
                                    vector<string>& inst_str,
                                    vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  // dwconv instruction fields
  // debug
  debug_ =
      debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_DWCONV);
  Helper<DPUVersion::XVDPU>::InstInit(inst_type_, root_debug_path_, debug_,
                                      opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::XVDPU>::inst_table;
  // get dwconv instruction field value
  bank_addr_out_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ADDR_OUT];
  bank_id_out_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ID_OUT];

  bank_addr_in_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ADDR_IN];
  bank_id_in_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ID_IN];

  pad_left_ = field_val_[TABLE::DPTWISE_FIELD_PAD_LEFT];
  pad_right_ = field_val_[TABLE::DPTWISE_FIELD_PAD_RIGHT];
  pad_top_ = field_val_[TABLE::DPTWISE_FIELD_PAD_TOP];
  pad_bottom_ = field_val_[TABLE::DPTWISE_FIELD_PAD_BOTTOM];

  bank_addr_in_1_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ADDR_IN_1];
  bank_addr_weights_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ADDR_WEIGHTS];
  valid_pixel_parallel_ = field_val_[TABLE::DPTWISE_FIELD_VALID_PIXEL_PARALLEL];

  bank_addr_in_3_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ADDR_IN_3];
  bank_addr_in_2_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ADDR_IN_2];
  bank_addr_bias_ = field_val_[TABLE::DPTWISE_FIELD_BANK_ADDR_BIAS];

  // dwinit value
  using DWINIT = DWInit<DPUVersion::XVDPU>;
  channel_group_ = DWINIT::CUROBJ->get_channel_group();
  jump_read_endl_ = DWINIT::CUROBJ->get_jump_read_endl();

  stride_h_ = DWINIT::CUROBJ->get_stride_h();
  stride_w_ = DWINIT::CUROBJ->get_stride_w();
  kernel_h_ = DWINIT::CUROBJ->get_kernel_h();
  kernel_w_ = DWINIT::CUROBJ->get_kernel_w();
  stride_offset_in_ = DWINIT::CUROBJ->get_stride_offset_in();
  stride_offset_out_ = DWINIT::CUROBJ->get_stride_offset_out();
  jump_read_ = DWINIT::CUROBJ->get_jump_read();

  stride_out_ = DWINIT::CUROBJ->get_stride_out();
  act_type_ = DWINIT::CUROBJ->get_act_type();
  channel_offset_ = DWINIT::CUROBJ->get_channel_offset();
  jump_write_endl_ = DWINIT::CUROBJ->get_jump_write_endl();

  shift_cut_ = DWINIT::CUROBJ->get_shift_cut();
  shift_bias_ = DWINIT::CUROBJ->get_shift_bias();
  length_ = DWINIT::CUROBJ->get_length();
  jump_write_ = DWINIT::CUROBJ->get_jump_write();

  // constraints from hardware implementation
  UNI_LOG_CHECK(shift_bias_ <= 20, SIM_PARAMETER_FAILED)
      << "invalid shift_bias";

  // self defined
  vpp_ = valid_pixel_parallel_;
  pp_dwconv_ = ArchCfg::Instance().get_param().dwconv_engine().pixel_parallel();
  assert(vpp_ <= pp_dwconv_);
  pp_ = ArchCfg::Instance().get_param().conv_engine().pixel_parallel();
  assert(pp_dwconv_ <= pp_);
  cp_ = ArchCfg::Instance().get_param().dwconv_engine().channel_parallel();
  cg_ = channel_group_;
  co_ = channel_offset_;
  ic_ = cg_ * cp_;  // input_channel is channel_group multiply channel_parallel
  oc_ = ic_;        // output_channel is channel_parallel
  src_h_ = (pp_dwconv_ - 1) * stride_h_ + kernel_h_;
  real_src_h_ = (vpp_ - 1) * stride_h_ + kernel_h_;
  src_w_ = (length_ - 1) * stride_w_ + kernel_w_;
  dst_h_ = pp_dwconv_;
  real_dst_h_ = vpp_;
  dst_w_ = length_;
  src_size_ = src_h_ * src_w_ * ic_;

  group_id_in_ = bank_id_in_ / pp_;
  group_id_out_ = bank_id_out_ / pp_;

  // resize related buffer size
  img_.resize(src_h_ * src_w_ * ic_);
  weights_.resize(oc_ * kernel_h_ * kernel_w_);
  bias_.resize(oc_);
  rlt_s64_.resize(dst_h_ * dst_w_ * oc_);
  rlt_dtype_.resize(dst_h_ * dst_w_ * oc_);
}

template <DPUVersion T>
DptWise<T>::~DptWise() {}

// public funcs
template <>
void DptWise<DPUVersion::XVDPU>::Exec() {
  // read img, weights, bias data from bank
  read();
  debug_input_fmap("read");
  debug_read_pos_tick();
  debug_input_tick("read");
  // padding for img data, also handle channel offset
  pad();
  debug_input_fmap("pad");
  debug_input_tick("pad");
  // do conv operation of the whole feature map
  dw_conv();
  debug_output_fmap(0, "dwconv_rlt_fmap");
  // add bias to conv result
  bias();
  debug_output_fmap(0, "bias_rlt_fmap");
  // do shift, trunc operation
  transform();
  debug_output_fmap(1, "transform_rlt_fmap");
  debug_output_tick("dwconv_result_tick");
  // save result to bank
  save();
}

template <DPUVersion T>
void DptWise<T>::Exec() {
  // read img, weights, bias data from bank
  read();
  debug_input_fmap("read");
  debug_read_pos_tick();
  // padding for img data, also handle channel offset
  pad();
  debug_input_fmap("pad");
  debug_input_tick();
  // do conv operation of the whole feature map
  dw_conv();
  debug_output_fmap(0, "dwconv_rlt_fmap");
  // add bias to conv result
  bias();
  debug_output_fmap(0, "bias_rlt_fmap");
  // do shift, trunc operation
  transform();
  debug_output_fmap(1, "transform_rlt_fmap");
  debug_output_tick("dwconv_result_tick");
  // save result to bank
  save();
}

template <>
void DptWise<DPUVersion::DPU4F>::Exec() {
  // read img, weights, bias data from bank
  read();
  debug_input_fmap("read");
  debug_input_tick("read");
  debug_read_pos_tick();
  // padding for img data, also handle channel offset
  pad();
  debug_input_fmap("pad");
  debug_input_tick("pad");
  // do conv operation of the whole feature map
  dw_conv();
  debug_output_fmap(0, "dwconv_rlt_fmap");
  // add bias to conv result
  bias();
  debug_output_fmap(0, "bias_rlt_fmap");
  // do shift, trunc operation
  transform();
  debug_output_fmap(1, "transform_rlt_fmap");
  debug_output_tick("dwconv_result_tick");
  // save result to bank
  save();
}

// private funcs
template <DPUVersion T>
void DptWise<T>::read() {
  // UNI_LOG_INFO << endl;
  // UNI_LOG_INFO << "read start..." << endl;

  // read img data from bank
  // UNI_LOG_INFO << "reading img..." << endl;

  UNI_LOG_CHECK(stride_offset_in_ < stride_h_, SIM_PARAMETER_FAILED)
      << "invalid stride_offset_in";
  auto stride_offset = stride_offset_in_;
  auto img_bank_id = bank_id_in_;
  auto base_bank_id = (bank_id_in_ / pp_) * pp_;
  auto addr_group = 0;
  auto bank_depth =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_in_)->GetH();
  for (auto idx_src_h = 0; idx_src_h < real_src_h_; idx_src_h++) {
    for (auto idx_src_w = 0; idx_src_w < src_w_; idx_src_w++) {
      auto ddr_addr = (idx_src_h * src_w_ + idx_src_w) * ic_;
      auto bank_addr_group_base = get_group_base_addr(addr_group);
      auto bank_addr = (bank_addr_group_base + stride_offset * jump_read_endl_ +
                        idx_src_w * jump_read_) %
                       bank_depth;
      auto bank = Buffer<DPU_DATA_TYPE>::Instance().GetBank(img_bank_id);
      bank->Read(bank_addr, ic_, img_.data() + ddr_addr);
    }
    if (++stride_offset == stride_h_) {
      stride_offset = 0;
      if (++img_bank_id == base_bank_id + pp_) {
        img_bank_id = base_bank_id;
        ++addr_group;
      }
    }
  }

  // read weights from bank
  // UNI_LOG_INFO << "reading weights..." << endl;
  auto wgt_bank_group =
      ArchCfg::Instance().get_param().dwconv_engine().weight_bank();
  auto bank_depth_wgt =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(wgt_bank_group, 0)->GetH();
  for (int i = 0; i < 1; i++) {
    int bank_id = i % cp_;
    for (int j = 0; j < kernel_h_; j++) {
      for (int k = 0; k < kernel_w_; k++) {
        int ddr_addr =
            i * kernel_h_ * kernel_w_ * ic_ + j * kernel_w_ * ic_ + k * ic_;
        int bank_addr = (bank_addr_weights_ + j * kernel_w_ * cg_ + k * cg_) %
                        bank_depth_wgt;
        auto bank =
            Buffer<DPU_DATA_TYPE>::Instance().GetBank(wgt_bank_group, bank_id);
        bank->Read(bank_addr, ic_, weights_.data() + ddr_addr);
      }
    }
  }

  // read bias from bank
  // UNI_LOG_INFO << "reading bias..." << endl;
  {
    auto bias_bank_group =
        ArchCfg::Instance().get_param().dwconv_engine().bias_bank();
    auto bank = Buffer<DPU_DATA_TYPE>::Instance().GetBank(bias_bank_group, 0);
    bank->Read(bank_addr_bias_, oc_, bias_.data());
  }
  // UNI_LOG_INFO << "read finish!" << endl;
}

template <>
void DptWise<DPUVersion::DPU4F>::read() {
  UNI_LOG_CHECK(stride_offset_in_ < stride_h_, SIM_PARAMETER_FAILED)
      << "invalid stride_offset_in";
  auto stride_offset = stride_offset_in_;
  auto img_bank_id = bank_id_in_;
  auto base_bank_id = (bank_id_in_ / pp_) * pp_;
  auto addr_group = 0;
  auto bank_depth =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_in_)->GetH();
  for (auto idx_src_h = 0; idx_src_h < real_src_h_; idx_src_h++) {
    for (auto idx_src_w = 0; idx_src_w < src_w_; idx_src_w++) {
      auto ddr_addr = (idx_src_h * src_w_ + idx_src_w) * ic_;
      auto bank_addr_group_base = get_group_base_addr(addr_group);
      auto bank_addr = (bank_addr_group_base + stride_offset * jump_read_endl_ +
                        idx_src_w * jump_read_) %
                       bank_depth;
      auto bank = Buffer<DPU_DATA_TYPE>::Instance().GetBank(img_bank_id);
      BankShell::read(true, img_bank_id, ic_, bank_addr,
                      img_.data() + ddr_addr);
    }
    if (++stride_offset == stride_h_) {
      stride_offset = 0;
      if (++img_bank_id == base_bank_id + pp_) {
        img_bank_id = base_bank_id;
        ++addr_group;
      }
    }
  }

  // read weights from bank
  auto wgt_bank_group =
      ArchCfg::Instance().get_param().dwconv_engine().weight_bank();
  auto bank_depth_wgt =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(wgt_bank_group, 0)->GetH();
  for (int i = 0; i < 1; i++) {
    int bank_id = i % cp_;
    for (int j = 0; j < kernel_h_; j++) {
      for (int k = 0; k < kernel_w_; k++) {
        int ddr_addr =
            i * kernel_h_ * kernel_w_ * ic_ + j * kernel_w_ * ic_ + k * ic_;
        int bank_addr = (bank_addr_weights_ + j * kernel_w_ * cg_ + k * cg_) %
                        bank_depth_wgt;
        auto bank =
            Buffer<DPU_DATA_TYPE>::Instance().GetBank(wgt_bank_group, bank_id);
        bank->Read(bank_addr, ic_, weights_.data() + ddr_addr);
      }
    }
  }

  // read bias from bank
  {
    auto bias_bank_group =
        ArchCfg::Instance().get_param().dwconv_engine().bias_bank();
    auto bank = Buffer<DPU_DATA_TYPE>::Instance().GetBank(bias_bank_group, 0);
    auto addr = bank_addr_bias_;
    for (auto idx_cg = 0; idx_cg < cg_; idx_cg++) {
      bank->Read(addr++, cp_, bias_.data() + idx_cg * cp_);
      bank->Read(addr++, cp_, shift_bias_buf_.data() + idx_cg * cp_);
      bank->Read(addr++, cp_, shift_cut_buf_.data() + idx_cg * cp_);
    }
  }
}

template <DPUVersion dv>
int DptWise<dv>::get_group_base_addr(int addr_group) {
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
int DptWise<DPUVersion::DPUV3ME>::get_group_base_addr(int addr_group) {
  return bank_addr_in_ + addr_group * stride_h_ * jump_read_endl_;
}

template <DPUVersion dv>
void DptWise<dv>::pad() {
  // UNI_LOG_INFO << endl;
  // UNI_LOG_INFO << "pad start..." << endl;
  // padding left
  // UNI_LOG_INFO << "padding left..." << endl;
  for (int i = 0; i < real_src_h_; i++) {
    for (int j = 0; j < pad_left_ && j < src_w_; j++) {
      for (int k = 0; k < ic_; k++) {
        int addr = i * src_w_ * ic_ + j * ic_ + k;
        img_[addr] = 0;
      }
    }
  }

  // padding top
  // UNI_LOG_INFO << "padding top..." << endl;
  for (int i = 0; i < pad_top_ && i < real_src_h_; i++) {
    for (int j = 0; j < src_w_; j++) {
      for (int k = 0; k < ic_; k++) {
        int addr = i * src_w_ * ic_ + j * ic_ + k;
        img_[addr] = 0;
      }
    }
  }

  // padding right
  // UNI_LOG_INFO << "padding right..." << endl;
  for (int i = 0; i < real_src_h_; i++) {
    for (int j = src_w_ - kernel_w_ + pad_right_ + 1; j < src_w_; j++) {
      for (int k = 0; k < ic_; k++) {
        int addr = i * src_w_ * ic_ + j * ic_ + k;
        img_[addr] = 0;
      }
    }
  }

  // padding bottom, it's different from padding right
  // UNI_LOG_INFO << "padding bottom..." << endl;
  for (int i = real_src_h_ - kernel_h_ + pad_bottom_ + 1; i < real_src_h_;
       i++) {
    for (int j = 0; j < src_w_; j++) {
      for (int k = 0; k < ic_; k++) {
        int addr = i * src_w_ * ic_ + j * ic_ + k;
        img_[addr] = 0;
      }
    }
  }
  // UNI_LOG_INFO << "pad finish!" << endl;

  // handle channel offset
  for (int i = 0; i < real_src_h_; i++) {
    for (int j = 0; j < src_w_; j++) {
      int ddr_addr = i * src_w_ * ic_ + j * ic_ + ic_ - co_;
      for (int k = 0; k < co_; k++) {
        img_[ddr_addr + k] = 0;
      }
    }
  }
}

template <DPUVersion dv>
void DptWise<dv>::dw_conv() {
  // UNI_LOG_INFO << endl;
  // UNI_LOG_INFO << "dwconv start..." << endl;
  for (int i = 0; i + kernel_h_ <= real_src_h_; i += stride_h_) {
    for (int j = 0; j + kernel_w_ <= src_w_; j += stride_w_) {
      for (int k = 0; k < oc_; k++) {
        dw_conv_one(i, j, k);
      }
    }
  }

  // UNI_LOG_INFO << "dwconv finish!" << endl;
}

template <DPUVersion dv>
void DptWise<dv>::dw_conv_one(int idx_src_h, int idx_src_w, int idx_oc) {
  int64_t result = 0;

  // calculate dwconv value
  for (int i = 0; i < kernel_h_; i++) {
    for (int j = 0; j < kernel_w_; j++) {
      int img_addr =
          (idx_src_h + i) * src_w_ * ic_ + (idx_src_w + j) * ic_ + idx_oc;
      int weights_addr = i * kernel_w_ * ic_ + j * ic_ + idx_oc;
      result += img_[img_addr] * weights_[weights_addr];
    }
  }

  // assign dwconv value to rlt_s64_ var
  int idx_dst_h = idx_src_h / stride_h_;
  int idx_dst_w = idx_src_w / stride_w_;
  int rlt_addr = idx_dst_h * dst_w_ * oc_ + idx_dst_w * oc_ + idx_oc;
  rlt_s64_[rlt_addr] = result;
}

template <DPUVersion dv>
void DptWise<dv>::bias() {
  shift_bias_ = (shift_bias_ >= 32) ? (32 - shift_bias_) : shift_bias_;
  for (int i = 0; i < dst_h_; i++) {
    for (int j = 0; j < dst_w_; j++) {
      for (int k = 0; k < oc_; k++) {
        int rlt_addr = i * dst_w_ * oc_ + j * oc_ + k;
        rlt_s64_[rlt_addr] *= 2.0;
        rlt_s64_[rlt_addr] +=
            floor((double)bias_[k] * 2.0 * pow(2, shift_bias_));
      }
    }
  }
}

template <>
void DptWise<DPUVersion::XVDPU>::bias() {
  for (int i = 0; i < dst_h_; i++) {
    for (int j = 0; j < dst_w_; j++) {
      for (int k = 0; k < oc_; k++) {
        int rlt_addr = i * dst_w_ * oc_ + j * oc_ + k;
        rlt_s64_[rlt_addr] *= 2.0;
        rlt_s64_[rlt_addr] += (double)bias_[k] * pow(2, shift_bias_);
      }
    }
  }
}

template <>
void DptWise<DPUVersion::DPUV2>::bias() {
  for (int i = 0; i < dst_h_; i++) {
    for (int j = 0; j < dst_w_; j++) {
      for (int k = 0; k < oc_; k++) {
        int rlt_addr = i * dst_w_ * oc_ + j * oc_ + k;
        rlt_s64_[rlt_addr] *= 2.0;
        rlt_s64_[rlt_addr] += (double)bias_[k] * pow(2, shift_bias_);
      }
    }
  }
}

template <>
void DptWise<DPUVersion::DPU4F>::bias() {
  shift_bias_ = (shift_bias_ >= 32) ? (32 - shift_bias_) : shift_bias_;
  for (int i = 0; i < dst_h_; i++) {
    for (int j = 0; j < dst_w_; j++) {
      for (int k = 0; k < oc_; k++) {
        int rlt_addr = i * dst_w_ * oc_ + j * oc_ + k;
        rlt_s64_[rlt_addr] *= 2.0;
        rlt_s64_[rlt_addr] += (bias_[k] << shift_bias_buf_[k]);
      }
    }
  }
}

template <DPUVersion dv>
void DptWise<dv>::transform() {
  // UNI_LOG_INFO << endl;
  // UNI_LOG_INFO << "transform start..." << endl;
  for (int i = 0; i < dst_h_; i++) {
    for (int j = 0; j < dst_w_; j++) {
      for (int k = 0; k < oc_; k++) {
        int addr = i * dst_w_ * oc_ + j * oc_ + k;

        double tmp = rlt_s64_[addr];
        tmp /= pow(2, (shift_cut_ + 1));

        if (act_type_ == Calc::RELU_TYPE_NONE) {
          // do nothing
        } else if (act_type_ == Calc::RELU_TYPE_RELU) {
          tmp = (tmp < 0) ? 0 : tmp;
        } else if (act_type_ == Calc::RELU_TYPE_LEAKY_RELU) {
          tmp = (tmp < 0) ? (tmp * 26. / 256.) : tmp;
        } else if (act_type_ == Calc::RELU_TYPE_RELUSIX) {
          tmp = (tmp < 0) ? 0 : tmp;
          double thr6 = 6 << 4;
          tmp = (tmp < thr6) ? tmp : thr6;
        } else {
          UNI_LOG_FATAL(SIM_PARAMETER_FAILED)
              << "Not support nonlinear type " << act_type_ << endl;
        }
        rlt_dtype_[addr] = Calc::DPURound<DPU_DATA_TYPE>(tmp);
      }
    }
  }
  // UNI_LOG_INFO << "transform finish!" << endl;
}

template <>
void DptWise<DPUVersion::DPU4F>::transform() {
  for (int i = 0; i < dst_h_; i++) {
    for (int j = 0; j < dst_w_; j++) {
      for (int k = 0; k < oc_; k++) {
        int addr = i * dst_w_ * oc_ + j * oc_ + k;

        double tmp = rlt_s64_[addr];
        tmp /= pow(2, (shift_cut_buf_[k] + 1));

        if (act_type_ == Calc::RELU_TYPE_NONE) {
          // do nothing
        } else if (act_type_ == Calc::RELU_TYPE_RELU) {
          tmp = (tmp < 0) ? 0 : tmp;
        } else if (act_type_ == Calc::RELU_TYPE_LEAKY_RELU) {
          tmp = (tmp < 0) ? (tmp * 26. / 256.) : tmp;
        } else if (act_type_ == Calc::RELU_TYPE_RELUSIX) {
          tmp = (tmp < 0) ? 0 : tmp;
          double thr6 = 6 << 4;
          tmp = (tmp < thr6) ? tmp : thr6;
        } else {
          UNI_LOG_FATAL(SIM_PARAMETER_FAILED)
              << "Not support nonlinear type " << act_type_ << endl;
        }
        rlt_dtype_[addr] = Calc::DPURound<DPU_DATA_TYPE>(tmp);
      }
    }
  }
}

template <DPUVersion T>
void DptWise<T>::save() {
  // UNI_LOG_INFO << endl;
  // UNI_LOG_INFO << "dwconv save start..." << endl;
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
    for (int idx_w = 0; idx_w < dst_w_; idx_w++) {
      auto bank_addr_this_pixel =
          (bank_addr_this_row + idx_w * jump_write_) % bank_depth;
      auto rlt_addr_this_pixel = idx_h * dst_w_ * oc_ + idx_w * oc_;
      bank->Write(bank_addr_this_pixel, oc_,
                  rlt_dtype_.data() + rlt_addr_this_pixel);
    }
  }
  // UNI_LOG_INFO << "dwconv save finish!" << endl;
}

template <>
void DptWise<DPUVersion::DPUV3ME>::save() {
  auto bank_depth =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_out_)->GetH();

  auto bank_id_out_offset = bank_id_out_ % pp_;
  auto bank_id_out_base = bank_id_out_ - bank_id_out_ % pp_;
  auto line_size = cp_;
  for (auto idx_c = 0; idx_c < cg_; idx_c++) {
    if ((idx_c == cg_ - 1) && (co_ >= cp_ / 2)) line_size = cp_ / 2;

    for (auto idx_h = 0; idx_h < real_dst_h_; idx_h++) {
      // bank id
      auto bank_shift = (idx_h + stride_offset_out_) / stride_out_;
      auto stride_offset = (idx_h + stride_offset_out_) % stride_out_;
      auto addr_group = (bank_id_out_offset + bank_shift) / pp_;
      auto bank_id = bank_id_out_base + (bank_id_out_offset + bank_shift) % pp_;
      auto bank = Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id);

      // bank address
      auto bank_addr_base = bank_addr_out_ +
                            addr_group * stride_out_ * jump_write_endl_ +
                            stride_offset * jump_write_endl_;

      for (int idx_w = 0; idx_w < dst_w_; idx_w++) {
        auto bank_addr =
            (bank_addr_base + idx_w * jump_write_ + idx_c * scale_) %
            bank_depth;
        auto rlt_addr = idx_h * dst_w_ * oc_ + idx_w * oc_ + idx_c * cp_;
        bank->Write(bank_addr, line_size, rlt_dtype_.data() + rlt_addr);
      }
    }
  }
}

template <>
void DptWise<DPUVersion::DPU4F>::save() {
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
void DptWise<dv>::debug_input_fmap(string prefix) {
  if (!InstBase::debug_) return;

  string head;
  string save_name_img;
  string save_name_weights;
  string save_name_bias;

  // make save file name
  stringstream ss;
  ss.clear();
  ss.str("");
  ss << InstBase::debug_path_ << "img_" << prefix << "_fmap."
     << InstBase::instid_;
  ss >> save_name_img;
  ss.clear();
  ss.str("");
  ss << InstBase::debug_path_ << "weights_" << prefix << "_fmap."
     << InstBase::instid_;
  ss >> save_name_weights;
  ss.clear();
  ss.str("");
  ss << InstBase::debug_path_ << "bias_" << prefix << "_fmap."
     << InstBase::instid_;
  ss >> save_name_bias;

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
    Util::SaveBin(save_name_img, head.data(), head.size(), SM_APPEND);
    for (int j = 0; j < src_h_; j++) {
      int addr = i * src_h_ * src_w_ + j * src_w_;
      Util::SaveDec(save_name_img, img.data() + addr, src_w_, src_w_,
                    SM_APPEND);
    }
  }

  // save weights data
  vector<DPU_DATA_TYPE> weights(ic_ * kernel_h_ * kernel_w_);
  Calc::HWC2CHW(kernel_h_, kernel_w_, ic_, weights_.data(), weights.data());
  for (int j = 0; j < ic_; j++) {
    // save input_channel title to file
    ss.clear();
    ss.str("");
    ss << "input_channel_" << j << ":";
    ss >> head;
    head += "\n";
    Util::SaveBin(save_name_weights, head.data(), head.size(), SM_APPEND);
    for (int k = 0; k < kernel_h_; k++) {
      int addr = j * kernel_h_ * kernel_w_ + k * kernel_w_;
      Util::SaveDec(save_name_weights, weights.data() + addr, kernel_w_,
                    kernel_w_, SM_APPEND);
    }
  }

  // save bias data
  for (int i = 0; i < oc_; i++) {
    // save output_channel title to file
    ss.clear();
    ss.str("");
    ss << "output_channel_" << i << "-------------------------------------";
    ss >> head;
    head += "\n";
    Util::SaveBin(save_name_bias, head.data(), head.size(), SM_APPEND);
    Util::SaveDec(save_name_bias, bias_.data() + i, 1, 1, SM_APPEND);
  }
}

template <>
void DptWise<DPUVersion::DPU4F>::debug_input_fmap(string prefix) {
  if (!debug_) return;

  string head;
  string save_name_img;
  string save_name_weights;
  string save_name_bias;
  string save_name_shift_bias;
  string save_name_shift_cut;

  // make save file name
  stringstream ss;
  ss.clear();
  ss.str("");
  ss << debug_path_ << "img_" << prefix << "_fmap." << instid_;
  ss >> save_name_img;
  ss.clear();
  ss.str("");
  ss << debug_path_ << "weights_" << prefix << "_fmap." << instid_;
  ss >> save_name_weights;
  ss.clear();
  ss.str("");
  ss << debug_path_ << "bias_" << prefix << "_fmap." << instid_;
  ss >> save_name_bias;
  ss.clear();
  ss.str("");
  ss << debug_path_ << "shift_bias_" << prefix << "_fmap." << instid_;
  ss >> save_name_shift_bias;
  ss.clear();
  ss.str("");
  ss << debug_path_ << "shift_cut_" << prefix << "_fmap." << instid_;
  ss >> save_name_shift_cut;

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
    Util::SaveBin(save_name_img, head.data(), head.size(), SM_APPEND);
    for (int j = 0; j < src_h_; j++) {
      int addr = i * src_h_ * src_w_ + j * src_w_;
      Util::SaveDec(save_name_img, img.data() + addr, src_w_, src_w_,
                    SM_APPEND);
    }
  }

  // save weights data
  vector<DPU_DATA_TYPE> weights(ic_ * kernel_h_ * kernel_w_);
  Calc::HWC2CHW(kernel_h_, kernel_w_, ic_, weights_.data(), weights.data());
  for (int j = 0; j < ic_; j++) {
    // save input_channel title to file
    ss.clear();
    ss.str("");
    ss << "input_channel_" << j << ":";
    ss >> head;
    head += "\n";
    Util::SaveBin(save_name_weights, head.data(), head.size(), SM_APPEND);
    for (int k = 0; k < kernel_h_; k++) {
      int addr = j * kernel_h_ * kernel_w_ + k * kernel_w_;
      Util::SaveDec(save_name_weights, weights.data() + addr, kernel_w_,
                    kernel_w_, SM_APPEND);
    }
  }

  // save bias data
  for (int i = 0; i < oc_; i++) {
    // save output_channel title to file
    ss.clear();
    ss.str("");
    ss << "output_channel_" << i << "-------------------------------------";
    ss >> head;
    head += "\n";
    Util::SaveBin(save_name_bias, head.data(), head.size(), SM_APPEND);
    Util::SaveDec(save_name_bias, bias_.data() + i, 1, 1, SM_APPEND);
  }

  // save shift_bias data
  for (int i = 0; i < oc_; i++) {
    // save output_channel title to file
    ss.clear();
    ss.str("");
    ss << "output_channel_" << i << "-------------------------------------";
    ss >> head;
    head += "\n";
    Util::SaveBin(save_name_shift_bias, head.data(), head.size(), SM_APPEND);
    Util::SaveDec(save_name_shift_bias, shift_bias_buf_.data() + i, 1, 1,
                  SM_APPEND);
  }

  // save shift_cut data
  for (int i = 0; i < oc_; i++) {
    // save output_channel title to file
    ss.clear();
    ss.str("");
    ss << "output_channel_" << i << "-------------------------------------";
    ss >> head;
    head += "\n";
    Util::SaveBin(save_name_shift_cut, head.data(), head.size(), SM_APPEND);
    Util::SaveDec(save_name_shift_cut, shift_cut_buf_.data() + i, 1, 1,
                  SM_APPEND);
  }
}

template <DPUVersion T>
void DptWise<T>::debug_read_pos_tick() {
  if (!debug_) {
    return;
  }
  string save_name;
  stringstream ss;
  ss.clear();
  ss.str("");
  ss << debug_path_ << "dwconv_read_pos_tick." << instid_;
  ss >> save_name;
  ofstream fout(save_name);
  UNI_LOG_CHECK(stride_offset_in_ < stride_h_, SIM_PARAMETER_FAILED)
      << "invalid stride_offset_in";
  auto bank_depth =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_in_)->GetH();
  auto bank_id_group_base = (bank_id_in_ / pp_) * pp_;
  auto bank_id_kernel_base = bank_id_in_ - bank_id_group_base;
  for (auto idx_length = 0; idx_length < dst_w_; idx_length++) {
    for (auto idx_channel_group = 0; idx_channel_group < cg_;
         idx_channel_group++) {
      for (auto idx_kernel_h = 0; idx_kernel_h < kernel_h_; idx_kernel_h++) {
        for (auto idx_kernel_w = 0; idx_kernel_w < kernel_w_; idx_kernel_w++) {
          auto stride_offset = (stride_offset_in_ + idx_kernel_h) % stride_h_;
          auto bank_id_pixel_base =
              (bank_id_kernel_base +
               (stride_offset_in_ + idx_kernel_h) / stride_h_) %
              pp_;
          auto addr_group_pixel_base =
              (bank_id_kernel_base +
               (stride_offset_in_ + idx_kernel_h) / stride_h_) /
              pp_;
          for (auto idx_pixel_parallel = 0; idx_pixel_parallel < pp_dwconv_;
               idx_pixel_parallel++) {
            auto bank_id = bank_id_group_base +
                           (bank_id_pixel_base + idx_pixel_parallel) % pp_;
            auto addr_group = addr_group_pixel_base +
                              (bank_id_pixel_base + idx_pixel_parallel) / pp_;
            auto bank_addr_group_base = get_group_base_addr(addr_group);
            auto bank_addr =
                (bank_addr_group_base + idx_length * stride_w_ * jump_read_ +
                 stride_offset * jump_read_endl_ + idx_kernel_w * jump_read_ +
                 idx_channel_group) %
                bank_depth;
            fout << setw(2) << setfill('0') << bank_id << "-" << setw(4)
                 << setfill('0') << bank_addr << " ";
          }
          fout << endl;
        }
      }
    }
  }
}

template <DPUVersion T>
void DptWise<T>::debug_input_tick() {
  if (!debug_) return;

  string head;
  string save_name;

  // make save file name
  stringstream ss;
  ss.clear();
  ss.str("");
  ss << debug_path_ << "dwconv_start_tick." << instid_;
  ss >> save_name;

  // save bias data
  Util::SaveHexContBigEnd(save_name,
                          reinterpret_cast<const char*>(bias_.data()),
                          bias_.size() * sizeof(DPU_DATA_TYPE),
                          bias_.size() * sizeof(DPU_DATA_TYPE), SM_TRUNC);

  // save img and weights data
  char head0 = '0';
  char head1 = '1';
  for (int i = 0; i < src_w_ - kernel_w_ + 1; i += stride_w_) {
    for (int j = 0; j < cg_; j++) {
      for (int n = 0; n < kernel_h_; n++) {
        for (int m = 0; m < kernel_w_; m++) {
          if (j == cg_ - 1 && m == kernel_w_ - 1 && n == kernel_h_ - 1) {
            Util::SaveBin(save_name, &head1, 1, SM_APPEND);
          } else {
            Util::SaveBin(save_name, &head0, 1, SM_APPEND);
          }
          // one tick, do following operation
          // get cp_*pp_ img data
          // get cp_*1 weights data
          vector<DPU_DATA_TYPE> img_weights(cp_ * pp_dwconv_ + cp_ * 1);
          for (int k = 0; k < cp_; k++) {
            for (int l = 0; l < src_h_ - kernel_h_ + 1; l += stride_h_) {
              // important: must divide stride_h_ here

              int cur_img_addr = k * pp_dwconv_ + l / stride_h_;
              // img data order hwc: pp_dwconv_/src_w_/cg_*cp_
              int ori_img_addr = (l + n) * src_w_ * cg_ * cp_ +
                                 (i + m) * cg_ * cp_ + j * cp_ + k;

              img_weights[cur_img_addr] = img_[ori_img_addr];
            }
            int cur_weights_addr = cp_ * pp_dwconv_ + k;
            int ori_weights_addr =
                n * kernel_w_ * cg_ * cp_ + m * cg_ * cp_ + k;
            img_weights[cur_weights_addr] = weights_[ori_weights_addr];
          }
          Util::SaveHexContBigEnd(
              save_name, reinterpret_cast<const char*>(img_weights.data()),
              img_weights.size() * sizeof(DPU_DATA_TYPE),
              img_weights.size() * sizeof(DPU_DATA_TYPE), SM_APPEND);
        }
      }
    }
  }
}

template <>
void DptWise<DPUVersion::XVDPU>::debug_input_tick(const string& prefix) {
  if (!debug_) return;

  string head;
  string save_name_img;
  string save_name_wgt;
  string save_name_bias;

  // make save file name
  stringstream ss;
  ss.clear();
  ss.str("");
  ss << debug_path_ << prefix << "_img_tick." << instid_;
  ss >> save_name_img;
  ss.clear();
  ss.str("");
  ss << debug_path_ << prefix << "_wgt_tick." << instid_;
  ss >> save_name_wgt;
  ss.clear();
  ss.str("");
  ss << debug_path_ << prefix << "_bias_tick." << instid_;
  ss >> save_name_bias;

  auto addr_bias = bank_addr_bias_;
  auto bias_bank_group =
      ArchCfg::Instance().get_param().dwconv_engine().bias_bank();
  auto bias_bank =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bias_bank_group, 0);
  auto id_bias = bias_bank->GetID();
  auto bank_depth_bias = bias_bank->GetH();
  for (auto idx_cg = 0; idx_cg < cg_; idx_cg++) {
    // save bias data in smallend mode
    Util::SaveHexContSmallEndBankAddr(
        save_name_bias,
        reinterpret_cast<const char*>(bias_.data() + idx_cg * cp_),
        cp_ * sizeof(DPU_DATA_TYPE), cp_ * sizeof(DPU_DATA_TYPE),
        bank_depth_bias, id_bias, addr_bias++, SM_APPEND);
  }

  auto bank_id_group_base = (bank_id_in_ / pp_) * pp_;
  auto bank_id_offset_base = bank_id_in_ - bank_id_group_base;
  auto bank_depth_img =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_in_)->GetH();
  for (auto idx_stride_w = 0; idx_stride_w + kernel_w_ <= src_w_;
       idx_stride_w += stride_w_) {
    for (auto idx_channel_group = 0; idx_channel_group < cg_;
         idx_channel_group++) {
      for (auto idx_kernel_h = 0; idx_kernel_h < kernel_h_; idx_kernel_h++) {
        auto stride_offset = (stride_offset_in_ + idx_kernel_h) % stride_h_;
        for (auto idx_kernel_w = 0; idx_kernel_w < kernel_w_; idx_kernel_w++) {
          for (auto idx_vpp = 0; idx_vpp < vpp_; idx_vpp++) {
            // generate bank_id & bank_addr
            auto bank_id_offset =
                (stride_offset_in_ + idx_vpp * stride_h_ + idx_kernel_h) /
                stride_h_;
            auto bank_id_img = bank_id_group_base +
                               (bank_id_offset_base + bank_id_offset) % pp_;
            auto wrap_back = (bank_id_offset_base + bank_id_offset) / pp_;
            auto addr_base = get_group_base_addr(wrap_back);
            auto bank_addr_img = (addr_base + stride_offset * jump_read_endl_ +
                                  (idx_stride_w + idx_kernel_w) * jump_read_ +
                                  idx_channel_group) %
                                 bank_depth_img;
            // prepare data
            vector<DPU_DATA_TYPE> tmp_tick(cp_);
            for (auto idx_channel_parallel = 0; idx_channel_parallel < cp_;
                 idx_channel_parallel++) {
              auto ori_addr = (idx_vpp * stride_h_ + idx_kernel_h) * src_w_ *
                                  cg_ * cp_ +  // source_h offset
                              (idx_stride_w * stride_w_ + idx_kernel_w) * cg_ *
                                  cp_ +  // source_w offset
                              idx_channel_group * cp_;
              tmp_tick[idx_channel_parallel] =
                  img_[ori_addr + idx_channel_parallel];
            }
            Util::SaveHexContSmallEndBankAddr(
                save_name_img, reinterpret_cast<const char*>(tmp_tick.data()),
                tmp_tick.size() * sizeof(DPU_DATA_TYPE),
                cp_ * sizeof(DPU_DATA_TYPE), bank_depth_img, bank_id_img,
                bank_addr_img, SM_APPEND);
          }
          // weights data order ohwi: cp_/kernel_h_/kernel_w_/cg_*cp_
          auto wgt_bank_group =
              ArchCfg::Instance().get_param().dwconv_engine().weight_bank();
          auto bank_wgt =
              Buffer<DPU_DATA_TYPE>::Instance().GetBank(wgt_bank_group, 0);
          auto bank_id_weights = bank_wgt->GetID();
          auto bank_depth_wgt = bank_wgt->GetID();
          auto bank_addr_weights_base =
              bank_addr_weights_ + idx_kernel_h * cg_ +
              idx_kernel_w * kernel_h_ * cg_ + idx_channel_group;
          auto bank_addr_weights = bank_addr_weights_base % bank_depth_wgt;

          auto ori_weights_addr = idx_kernel_h * kernel_w_ * cg_ * cp_ +
                                  idx_kernel_w * cg_ * cp_ +
                                  idx_channel_group * cp_;
          Util::SaveHexContSmallEndBankAddr(
              save_name_wgt,
              reinterpret_cast<const char*>(weights_.data() + ori_weights_addr),
              cp_, cp_, bank_depth_wgt, bank_id_weights, bank_addr_weights,
              SM_APPEND);
        }
      }
    }
  }
}

template <DPUVersion dv>
void DptWise<dv>::debug_output_fmap(int type, string save_name) {
  if (!InstBase::debug_) return;

  // make save file name
  stringstream ss;
  ss.clear();
  ss.str("");
  ss << InstBase::debug_path_ << save_name << "." << InstBase::instid_;
  ss >> save_name;

  // save img data
  vector<int64_t> rlt_s64(oc_ * dst_h_ * dst_w_);
  vector<DPU_DATA_TYPE> rlt_dtype(oc_ * dst_h_ * dst_w_);
  if (type == 0) {
    Calc::HWC2CHW(dst_h_, dst_w_, oc_, rlt_s64_.data(), rlt_s64.data());
  } else {
    Calc::HWC2CHW(dst_h_, dst_w_, oc_, rlt_dtype_.data(), rlt_dtype.data());
  }

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
      if (type == 0) {
        Util::SaveDec(save_name, rlt_s64.data() + addr, dst_w_, dst_w_,
                      SM_APPEND);
      } else {
        Util::SaveDec(save_name, rlt_dtype.data() + addr, dst_w_, dst_w_,
                      SM_APPEND);
      }
    }
  }
}

template <DPUVersion T>
void DptWise<T>::debug_output_tick(string save_name) {
  if (!debug_) return;

  // make save file name
  stringstream ss;
  ss.clear();
  ss.str("");
  ss << debug_path_ << save_name << "." << instid_;
  ss >> save_name;

  vector<DPU_DATA_TYPE> rlt_tmp(rlt_dtype_.size());

  // change data order from hwc to wch
  for (int i = 0; i < length_; i++) {
    for (int j = 0; j < cp_; j++) {
      for (int k = 0; k < pp_dwconv_; k++) {
        int addr_tmp = i * cp_ * pp_dwconv_ + j * pp_dwconv_ + k;
        int addr_ori = k * length_ * cp_ + i * cp_ + j;

        rlt_tmp[addr_tmp] = rlt_dtype_[addr_ori];
      }
    }
  }

  // save data using upper data order
  /* auto *p = reinterpret_cast<const char *>(rlt_tmp.data());
   int line_size = cp_ * pp_dwconv_ * sizeof(DPU_DATA_TYPE);
   for (int i = 0; i < length_; i++) {
       int offset = i * line_size;
       Util::SaveHexContBigEnd(save_name, p + offset,
               line_size, line_size, SM_APPEND);
   }*/
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
      Util::SaveHexContSmallEndBankAddr(save_name, p + offset, line_size,
                                        line_size, bank_depth, bank_id_this_row,
                                        bank_addr_this_pixel, SM_APPEND);
    }
  }
}

template <DPUVersion T>
void DptWise<T>::GenInstStr(int inst_fmt) {
  ac_ = Helper<T>::GetInstStr(inst_type_, inst_fmt, dpdon_, dpdby_, field_str_);
}
template <DPUVersion T>
void DptWise<T>::debug_input_tick(const string& prefix) {
  if (!debug_) return;

  string head;
  string save_name_img;
  string save_name_wgt;
  string save_name_bias;
  string save_name_shift_bias;
  string save_name_shift_cut;

  // make save file name
  stringstream ss;
  ss.clear();
  ss.str("");
  ss << debug_path_ << prefix << "_img_tick." << instid_;
  ss >> save_name_img;
  ss.clear();
  ss.str("");
  ss << debug_path_ << prefix << "_wgt_tick." << instid_;
  ss >> save_name_wgt;
  ss.clear();
  ss.str("");
  ss << debug_path_ << prefix << "_bias_tick." << instid_;
  ss >> save_name_bias;
  ss.clear();
  ss.str("");
  ss << debug_path_ << "shift_bias_" << prefix << "_tick." << instid_classify_;
  ss >> save_name_shift_bias;
  ss.clear();
  ss.str("");
  ss << debug_path_ << "shift_cut_" << prefix << "_tick." << instid_classify_;
  ss >> save_name_shift_cut;

  auto addr_bias = bank_addr_bias_;
  auto bias_bank_group =
      ArchCfg::Instance().get_param().dwconv_engine().bias_bank();
  auto bias_bank =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bias_bank_group, 0);
  auto id_bias = bias_bank->GetID();
  auto bank_depth_bias = bias_bank->GetH();
  for (auto idx_cg = 0; idx_cg < cg_; idx_cg++) {
    // save bias data in smallend mode
    Util::SaveHexContSmallEndBankAddr(
        save_name_bias,
        reinterpret_cast<const char*>(bias_.data() + idx_cg * cp_),
        cp_ * sizeof(DPU_DATA_TYPE), cp_ * sizeof(DPU_DATA_TYPE),
        bank_depth_bias, id_bias, addr_bias++, SM_APPEND);
    // save shift_bias data in smallend mode
    Util::SaveHexContSmallEndBankAddr(
        save_name_shift_bias,
        reinterpret_cast<const char*>(shift_bias_buf_.data() + idx_cg * cp_),
        cp_ * sizeof(DPU_DATA_TYPE), cp_ * sizeof(DPU_DATA_TYPE),
        bank_depth_bias, id_bias, addr_bias++, SM_APPEND);
    // save shift_cut data in smallend mode
    Util::SaveHexContSmallEndBankAddr(
        save_name_shift_cut,
        reinterpret_cast<const char*>(shift_cut_buf_.data() + idx_cg * cp_),
        cp_ * sizeof(DPU_DATA_TYPE), cp_ * sizeof(DPU_DATA_TYPE),
        bank_depth_bias, id_bias, addr_bias++, SM_APPEND);
  }

  auto bank_id_group_base = (bank_id_in_ / pp_) * pp_;
  auto bank_id_offset_base = bank_id_in_ - bank_id_group_base;
  auto bank_depth_img =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_in_)->GetH();
  for (auto idx_stride_w = 0; idx_stride_w + kernel_w_ <= src_w_;
       idx_stride_w += stride_w_) {
    for (auto idx_channel_group = 0; idx_channel_group < cg_;
         idx_channel_group++) {
      for (auto idx_kernel_h = 0; idx_kernel_h < kernel_h_; idx_kernel_h++) {
        auto stride_offset = (stride_offset_in_ + idx_kernel_h) % stride_h_;
        for (auto idx_kernel_w = 0; idx_kernel_w < kernel_w_; idx_kernel_w++) {
          for (auto idx_vpp = 0; idx_vpp < vpp_; idx_vpp++) {
            // generate bank_id & bank_addr
            auto bank_id_offset =
                (stride_offset_in_ + idx_vpp * stride_h_ + idx_kernel_h) /
                stride_h_;
            auto bank_id_img = bank_id_group_base +
                               (bank_id_offset_base + bank_id_offset) % pp_;
            auto wrap_back = (bank_id_offset_base + bank_id_offset) / pp_;
            auto addr_base = get_group_base_addr(wrap_back);
            auto bank_addr_img = (addr_base + stride_offset * jump_read_endl_ +
                                  (idx_stride_w + idx_kernel_w) * jump_read_ +
                                  idx_channel_group) %
                                 bank_depth_img;
            // prepare data
            vector<DPU_DATA_TYPE> tmp_tick(cp_);
            for (auto idx_channel_parallel = 0; idx_channel_parallel < cp_;
                 idx_channel_parallel++) {
              auto ori_addr = (idx_vpp * stride_h_ + idx_kernel_h) * src_w_ *
                                  cg_ * cp_ +  // source_h offset
                              (idx_stride_w * stride_w_ + idx_kernel_w) * cg_ *
                                  cp_ +  // source_w offset
                              idx_channel_group * cp_;
              tmp_tick[idx_channel_parallel] =
                  img_[ori_addr + idx_channel_parallel];
            }
            Util::SaveHexContSmallEndBankAddr(
                save_name_img, reinterpret_cast<const char*>(tmp_tick.data()),
                tmp_tick.size() * sizeof(DPU_DATA_TYPE),
                cp_ * sizeof(DPU_DATA_TYPE), bank_depth_img, bank_id_img,
                bank_addr_img, SM_APPEND);
          }
          // weights data order ohwi: cp_/kernel_h_/kernel_w_/cg_*cp_
          auto wgt_bank_group =
              ArchCfg::Instance().get_param().dwconv_engine().weight_bank();
          auto bank_wgt =
              Buffer<DPU_DATA_TYPE>::Instance().GetBank(wgt_bank_group, 0);
          auto bank_id_weights = bank_wgt->GetID();
          auto bank_depth_wgt = bank_wgt->GetID();
          auto bank_addr_weights_base =
              bank_addr_weights_ + idx_kernel_h * cg_ +
              idx_kernel_w * kernel_h_ * cg_ + idx_channel_group;
          auto bank_addr_weights = bank_addr_weights_base % bank_depth_wgt;

          auto ori_weights_addr = idx_kernel_h * kernel_w_ * cg_ * cp_ +
                                  idx_kernel_w * cg_ * cp_ +
                                  idx_channel_group * cp_;
          Util::SaveHexContSmallEndBankAddr(
              save_name_wgt,
              reinterpret_cast<const char*>(weights_.data() + ori_weights_addr),
              cp_, cp_, bank_depth_wgt, bank_id_weights, bank_addr_weights,
              SM_APPEND);
        }
      }
    }
  }
}
