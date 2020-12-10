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
#include "Conv.hpp"
#include "./xvdpu/MECfg.hpp"
#include "ConvInit.hpp"

template <>
Conv<DPUVersion::DPUV2>::Conv(int inst_type, int instid,
                              vector<string>& inst_str,
                              vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  // debug
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_CONV);
  Helper<DPUVersion::DPUV2>::InstInit(inst_type_, root_debug_path_, debug_,
                                      opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPUV2>::inst_table;
  // get conv instruction field value
  channel_group_ = field_val_[TABLE::CONV_FIELD_CHANNEL_GROUP];
  bank_addr_in_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_IN];
  pad_left_ = field_val_[TABLE::CONV_FIELD_PAD_LEFT];
  pad_top_ = field_val_[TABLE::CONV_FIELD_PAD_TOP];
  pad_right_ = field_val_[TABLE::CONV_FIELD_PAD_RIGHT];
  pad_bottom_ = field_val_[TABLE::CONV_FIELD_PAD_BOTTOM];
  bank_addr_out_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_OUT];
  channel_offset_ = field_val_[TABLE::CONV_FIELD_CHANNEL_OFFSET];
  bank_addr_weights_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_WEIGHTS];
  length_ = field_val_[TABLE::CONV_FIELD_LENGTH];
  bank_addr_bias_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_BIAS];
  bank_id_in_ = field_val_[TABLE::CONV_FIELD_BANK_ID_IN];
  bank_id_out_ = field_val_[TABLE::CONV_FIELD_BANK_ID_OUT];

  bank_addr_in_1_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_IN_1];
  bank_addr_in_2_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_IN_2];
  bank_addr_in_3_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_IN_3];

  // convinit value
  using CONVINIT = ConvInit<DPUVersion::DPUV2>;
  kernel_w_ = CONVINIT::CUROBJ->get_kernel_w();
  kernel_h_ = CONVINIT::CUROBJ->get_kernel_h();
  shift_bias_ = CONVINIT::CUROBJ->get_shift_bias();
  shift_cut_ = CONVINIT::CUROBJ->get_shift_cut();
  act_type_ = CONVINIT::CUROBJ->get_act_type();
  stride_w_ = CONVINIT::CUROBJ->get_stride_w();
  stride_h_ = CONVINIT::CUROBJ->get_stride_h();
  stride_out_ = CONVINIT::CUROBJ->get_stride_out();
  jump_read_ = CONVINIT::CUROBJ->get_jump_read();
  jump_read_endl_ = CONVINIT::CUROBJ->get_jump_read_endl();
  jump_write_ = CONVINIT::CUROBJ->get_jump_write();
  jump_write_endl_ = CONVINIT::CUROBJ->get_jump_write_endl();
  stride_offset_in_ = CONVINIT::CUROBJ->get_stride_offset_in();
  stride_offset_out_ = CONVINIT::CUROBJ->get_stride_offset_out();
  valid_pixel_parallel_ = CONVINIT::CUROBJ->get_valid_pixel_parallel();

  // constraints from hardware implementation
  UNI_LOG_CHECK(shift_bias_ <= 20, SIM_PARAMETER_FAILED)
      << "invalid shift_bias";

  // self defined
  vpp_ = valid_pixel_parallel_;
  pp_ = ArchCfg::Instance().get_param().conv_engine().pixel_parallel();
  // cp_ = SimCfg::Instance().get_channel_parallel(SimCfg::CP_INPUT);
  icp_ = ArchCfg::Instance().get_param().conv_engine().input_channel_parallel();
  ocp_ =
      ArchCfg::Instance().get_param().conv_engine().output_channel_parallel();
  cg_ = channel_group_;
  co_ = channel_offset_;
  ic_ = cg_ * icp_;  // input_channel is channel_group multiply channel_parallel
  oc_ = ocp_;        // output_channel is channel_parallel
  src_h_ = (pp_ - 1) * stride_h_ + kernel_h_;
  real_src_h_ = (vpp_ - 1) * stride_h_ + kernel_h_;
  src_w_ = (length_ - 1) * stride_w_ + kernel_w_;
  dst_h_ = pp_;
  real_dst_h_ = vpp_;
  dst_w_ = length_;
  src_size_ = src_h_ * src_w_ * ic_;
  group_id_out_ = bank_id_out_ / pp_;

  // resize related buffer size
  img_.resize(src_h_ * src_w_ * ic_);
  weights_.resize(oc_ * kernel_h_ * kernel_w_ * ic_);
  bias_.resize(oc_);
  rlt_s64_.resize(dst_h_ * dst_w_ * oc_);
  rlt_dtype_.resize(dst_h_ * dst_w_ * oc_);
}

template <>
Conv<DPUVersion::DPUV3E>::Conv(int inst_type, int instid,
                               vector<string>& inst_str,
                               vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  // debug
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_CONV);
  Helper<DPUVersion::DPUV3E>::InstInit(inst_type_, root_debug_path_, debug_,
                                       opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPUV3E>::inst_table;
  // get conv instruction field value
  channel_group_ = field_val_[TABLE::CONV_FIELD_CHANNEL_GROUP];
  bank_addr_in_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_IN];
  pad_left_ = field_val_[TABLE::CONV_FIELD_PAD_LEFT];
  pad_top_ = field_val_[TABLE::CONV_FIELD_PAD_TOP];
  pad_right_ = field_val_[TABLE::CONV_FIELD_PAD_RIGHT];
  pad_bottom_ = field_val_[TABLE::CONV_FIELD_PAD_BOTTOM];
  bank_addr_out_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_OUT];
  channel_offset_ = field_val_[TABLE::CONV_FIELD_CHANNEL_OFFSET];
  bank_addr_weights_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_WEIGHTS];
  length_ = field_val_[TABLE::CONV_FIELD_LENGTH];
  bank_addr_bias_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_BIAS];
  bank_id_in_ = field_val_[TABLE::CONV_FIELD_BANK_ID_IN];
  bank_id_out_ = field_val_[TABLE::CONV_FIELD_BANK_ID_OUT];

  bank_addr_in_1_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_IN_1];
  bank_addr_in_2_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_IN_2];
  bank_addr_in_3_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_IN_3];

  // convinit value
  using CONVINIT = ConvInit<DPUVersion::DPUV3E>;
  kernel_w_ = CONVINIT::CUROBJ->get_kernel_w();
  kernel_h_ = CONVINIT::CUROBJ->get_kernel_h();
  shift_bias_ = CONVINIT::CUROBJ->get_shift_bias();
  shift_cut_ = CONVINIT::CUROBJ->get_shift_cut();
  act_type_ = CONVINIT::CUROBJ->get_act_type();
  stride_w_ = CONVINIT::CUROBJ->get_stride_w();
  stride_h_ = CONVINIT::CUROBJ->get_stride_h();
  stride_out_ = CONVINIT::CUROBJ->get_stride_out();
  jump_read_ = CONVINIT::CUROBJ->get_jump_read();
  jump_read_endl_ = CONVINIT::CUROBJ->get_jump_read_endl();
  jump_write_ = CONVINIT::CUROBJ->get_jump_write();
  jump_write_endl_ = CONVINIT::CUROBJ->get_jump_write_endl();
  stride_offset_in_ = CONVINIT::CUROBJ->get_stride_offset_in();
  stride_offset_out_ = CONVINIT::CUROBJ->get_stride_offset_out();
  valid_pixel_parallel_ = CONVINIT::CUROBJ->get_valid_pixel_parallel();

  // constraints from hardware implementation
  if (shift_bias_ < 32) {
    UNI_LOG_CHECK(shift_bias_ <= 20, SIM_PARAMETER_FAILED)
        << "invalid shift_bias";
  }

  // self defined
  vpp_ = valid_pixel_parallel_;
  pp_ = ArchCfg::Instance().get_param().conv_engine().pixel_parallel();
  // cp_ = SimCfg::Instance().get_channel_parallel(SimCfg::CP_INPUT);
  icp_ = ArchCfg::Instance().get_param().conv_engine().input_channel_parallel();
  ocp_ =
      ArchCfg::Instance().get_param().conv_engine().output_channel_parallel();
  cg_ = channel_group_;
  co_ = channel_offset_;
  ic_ = cg_ * icp_;  // input_channel is channel_group multiply channel_parallel
  oc_ = ocp_;        // output_channel is channel_parallel
  src_h_ = (pp_ - 1) * stride_h_ + kernel_h_;
  real_src_h_ = (vpp_ - 1) * stride_h_ + kernel_h_;
  src_w_ = (length_ - 1) * stride_w_ + kernel_w_;
  dst_h_ = pp_;
  real_dst_h_ = vpp_;
  dst_w_ = length_;
  src_size_ = src_h_ * src_w_ * ic_;
  group_id_out_ = bank_id_out_ / pp_;

  // resize related buffer size
  img_.resize(src_h_ * src_w_ * ic_);
  weights_.resize(oc_ * kernel_h_ * kernel_w_ * ic_);
  bias_.resize(oc_);
  rlt_s64_.resize(dst_h_ * dst_w_ * oc_);
  rlt_dtype_.resize(dst_h_ * dst_w_ * oc_);
}

template <>
Conv<DPUVersion::DPUV3ME>::Conv(int inst_type, int instid,
                                vector<string>& inst_str,
                                vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  // debug
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_CONV);
  Helper<DPUVersion::DPUV3ME>::InstInit(inst_type_, root_debug_path_, debug_,
                                        opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPUV3ME>::inst_table;
  channel_group_ = field_val_[TABLE::CONV_FIELD_CHANNEL_GROUP];
  bank_addr_in_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_IN];
  pad_left_ = field_val_[TABLE::CONV_FIELD_PAD_LEFT];
  pad_top_ = field_val_[TABLE::CONV_FIELD_PAD_TOP];
  pad_right_ = field_val_[TABLE::CONV_FIELD_PAD_RIGHT];
  pad_bottom_ = field_val_[TABLE::CONV_FIELD_PAD_BOTTOM];
  bank_addr_out_cb_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_OUT_CB];
  bank_addr_out_hbm_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_OUT_HBM];
  channel_offset_ = field_val_[TABLE::CONV_FIELD_CHANNEL_OFFSET];
  output_channel_offset_ = field_val_[TABLE::CONV_FIELD_OUTPUT_CHANNEL_OFFSET];
  bank_addr_weights_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_WEIGHTS];
  length_ = field_val_[TABLE::CONV_FIELD_LENGTH];
  bank_addr_bias_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_BIAS];
  bank_id_in_ = field_val_[TABLE::CONV_FIELD_BANK_ID_IN];
  bank_id_out_cb_ = field_val_[TABLE::CONV_FIELD_BANK_ID_OUT_CB];
  bank_id_out_hbm_ = field_val_[TABLE::CONV_FIELD_BANK_ID_OUT_HBM];
  dest_mode_ = field_val_[TABLE::CONV_FIELD_DEST_MODE];

  using CONVINIT = ConvInit<DPUVersion::DPUV3ME>;
  shift_bias_ = CONVINIT::CUROBJ->get_shift_bias();
  shift_relusix_ = CONVINIT::CUROBJ->get_shift_relusix();
  shift_cut_ = CONVINIT::CUROBJ->get_shift_cut();
  act_type_ = CONVINIT::CUROBJ->get_act_type();
  stride_w_ = CONVINIT::CUROBJ->get_stride_w();
  stride_h_ = CONVINIT::CUROBJ->get_stride_h();
  jump_write_cb_ = CONVINIT::CUROBJ->get_jump_write_cb();
  jump_write_hbm_ = CONVINIT::CUROBJ->get_jump_write_hbm();
  stride_offset_in_ = CONVINIT::CUROBJ->get_stride_offset_in();
  valid_pixel_parallel_ = CONVINIT::CUROBJ->get_valid_pixel_parallel();
  kernel_w_ = CONVINIT::CUROBJ->get_kernel_w();
  kernel_h_ = CONVINIT::CUROBJ->get_kernel_h();
  stride_out_cb_ = CONVINIT::CUROBJ->get_stride_out_cb();
  jump_read_ = CONVINIT::CUROBJ->get_jump_read();
  jump_read_endl_ = CONVINIT::CUROBJ->get_jump_read_endl();
  jump_write_endl_cb_ = CONVINIT::CUROBJ->get_jump_write_endl_cb();
  jump_write_endl_hbm_ = CONVINIT::CUROBJ->get_jump_write_endl_hbm();
  stride_offset_out_cb_ = CONVINIT::CUROBJ->get_stride_offset_out_cb();
  stride_out_hbm_ = CONVINIT::CUROBJ->get_stride_out_hbm();
  stride_offset_out_hbm_ = CONVINIT::CUROBJ->get_stride_offset_out_hbm();

  // constraints from hardware implementation
  if (shift_bias_ < 32) {
    UNI_LOG_CHECK(shift_bias_ <= 20, SIM_PARAMETER_FAILED)
        << "invalid shift_bias";
  }

  // self defined
  vpp_ = valid_pixel_parallel_;
  pp_ = ArchCfg::Instance().get_param().conv_engine().pixel_parallel();
  // cp_ = SimCfg::Instance().get_channel_parallel(SimCfg::CP_INPUT);
  icp_ = ArchCfg::Instance().get_param().conv_engine().input_channel_parallel();
  ocp_ =
      ArchCfg::Instance().get_param().conv_engine().output_channel_parallel();
  cg_ = channel_group_;
  co_ = channel_offset_;
  ic_ = cg_ * icp_;  // input_channel is channel_group multiply channel_parallel
  oc_ = ocp_;        // output_channel is channel_parallel
  src_h_ = (pp_ - 1) * stride_h_ + kernel_h_;
  real_src_h_ = (vpp_ - 1) * stride_h_ + kernel_h_;
  src_w_ = (length_ - 1) * stride_w_ + kernel_w_;
  dst_h_ = pp_;
  real_dst_h_ = vpp_;
  dst_w_ = length_;
  src_size_ = src_h_ * src_w_ * ic_;
  group_id_out_ = bank_id_out_ / pp_;

  // resize related buffer size
  img_.resize(src_h_ * src_w_ * ic_);
  weights_.resize(oc_ * kernel_h_ * kernel_w_ * ic_);
  bias_.resize(oc_);
  rlt_s64_.resize(dst_h_ * dst_w_ * oc_);
  rlt_dtype_.resize(dst_h_ * dst_w_ * oc_);

  UNI_LOG_CHECK(stride_offset_in_ < stride_h_, SIM_PARAMETER_FAILED)
      << "invalid stride_offset_in";
  // write CB check
  // dest_mode_ encoding:
  // 0: none
  // 1: write HBM
  // 2: write CB
  // 3: write both HBM and CB
  int fmap_bank_group = SimCfg::Instance().get_fmap_bank_group();
  int group_id_out_cb = bank_id_out_cb_ / pp_;
  if ((dest_mode_ == 2) || (dest_mode_ == 3)) {
    UNI_LOG_CHECK(group_id_out_cb == (fmap_bank_group - 1),
                  SIM_PARAMETER_FAILED)
        << "invalid bank_id for write CB";
  }
  // write HBM check
  int group_id_out_hbm = bank_id_out_hbm_ / pp_;
  if ((dest_mode_ == 1) || (dest_mode_ == 3)) {
    UNI_LOG_CHECK(group_id_out_hbm < (fmap_bank_group - 1),
                  SIM_PARAMETER_FAILED)
        << "invalid bank_id for write HBM";
  }
  // input bank id check
  int group_id_in = bank_id_in_ / pp_;
  UNI_LOG_CHECK(group_id_in < (fmap_bank_group - 1), SIM_PARAMETER_FAILED)
      << "invalid input bank id";
  UNI_LOG_CHECK(vpp_ <= pp_, SIM_PARAMETER_FAILED)
      << "invalid valid_pixel_parallel field";
}

template <>
Conv<DPUVersion::XVDPU>::Conv(int inst_type, int instid,
                              vector<string>& inst_str,
                              vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  // debug
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_CONV);
  Helper<DPUVersion::XVDPU>::InstInit(inst_type_, root_debug_path_, debug_,
                                      opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::XVDPU>::inst_table;
  // get conv instruction field value
  bank_addr_in_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_IN];
  pad_left_ = field_val_[TABLE::CONV_FIELD_PAD_LEFT];
  pad_top_ = field_val_[TABLE::CONV_FIELD_PAD_TOP];
  pad_right_ = field_val_[TABLE::CONV_FIELD_PAD_RIGHT];
  pad_bottom_ = field_val_[TABLE::CONV_FIELD_PAD_BOTTOM];
  bank_addr_out_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_OUT];
  bank_addr_weights_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_WEIGHTS];
  bank_addr_bias_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_BIAS];
  bank_id_in_ = field_val_[TABLE::CONV_FIELD_BANK_ID_IN];
  bank_id_out_ = field_val_[TABLE::CONV_FIELD_BANK_ID_OUT];

  bank_addr_in_1_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_IN_1];
  bank_addr_in_2_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_IN_2];
  bank_addr_in_3_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_IN_3];

  valid_pixel_parallel_ = field_val_[TABLE::CONV_FIELD_VALID_PIXEL_PARALLEL];
  ocg_offset_ = field_val_[TABLE::CONV_FIELD_OCG_OFFSET];

  // convinit value
  using CONVINIT = ConvInit<DPUVersion::XVDPU>;
  kernel_w_ = CONVINIT::CUROBJ->get_kernel_w();
  kernel_h_ = CONVINIT::CUROBJ->get_kernel_h();
  shift_bias_ = CONVINIT::CUROBJ->get_shift_bias();
  shift_cut_ = CONVINIT::CUROBJ->get_shift_cut();
  act_type_ = CONVINIT::CUROBJ->get_act_type();
  stride_w_ = CONVINIT::CUROBJ->get_stride_w();
  stride_h_ = CONVINIT::CUROBJ->get_stride_h();
  stride_out_ = CONVINIT::CUROBJ->get_stride_out();
  jump_read_ = CONVINIT::CUROBJ->get_jump_read();
  jump_read_endl_ = CONVINIT::CUROBJ->get_jump_read_endl();
  jump_write_ = CONVINIT::CUROBJ->get_jump_write();
  jump_write_endl_ = CONVINIT::CUROBJ->get_jump_write_endl();
  stride_offset_in_ = CONVINIT::CUROBJ->get_stride_offset_in();
  stride_offset_out_ = CONVINIT::CUROBJ->get_stride_offset_out();

  conv_num_ = CONVINIT::CUROBJ->get_conv_num();
  tile_icg_ = CONVINIT::CUROBJ->get_tile_icg();
  ic_iter_ = CONVINIT::CUROBJ->get_ic_iter();
  icg_offset_ = CONVINIT::CUROBJ->get_icg_offset();
  channel_offset_ = CONVINIT::CUROBJ->get_channel_offset();
  tile_ocg_ = CONVINIT::CUROBJ->get_tile_ocg();
  tile_owg_ = CONVINIT::CUROBJ->get_tile_owg();
  ow_iter_ = CONVINIT::CUROBJ->get_ow_iter();
  ow_offset_ = CONVINIT::CUROBJ->get_ow_offset();
  jump_read_weights_ = CONVINIT::CUROBJ->get_jump_read_weights();

  // constraints from hardware implementation
  UNI_LOG_CHECK(shift_bias_ <= 20, SIM_EXCEED_HARDWARE_LIMIT)
      << "invalid shift_bias";

  auto remain = SimCfg::Instance().get_xvdpu_conv_remain();
  xvdpu_needParamPkts_ = (conv_num_ == remain) ? true : false;
  UNI_LOG_CHECK(remain-- > 0, SIM_PARAMETER_FAILED)
      << "real Convs which all follow one ConvInit is larger than conv_num "
         "in that ConvInit!";
  SimCfg::Instance().set_xvdpu_conv_remain(remain);

  // self defined
  vpp_ = valid_pixel_parallel_;
  pp_ = ArchCfg::Instance().get_param().conv_engine().pixel_parallel();
  UNI_LOG_CHECK(pp_ == 8, SIM_EXCEED_HARDWARE_LIMIT)
      << "xvdpu only run at pixel_parallel=8 now, ArchCfg is " << pp_;
  icp_ = ArchCfg::Instance().get_param().conv_engine().input_channel_parallel();
  ocp_ =
      ArchCfg::Instance().get_param().conv_engine().output_channel_parallel();
  // height
  dst_h_ = pp_;
  real_dst_h_ = vpp_;
  instr_stride_h_ = stride_h_;  // for read()
  stride_h_ = std::min(kernel_h_, stride_h_);
  src_h_ = (dst_h_ - 1) * stride_h_ + kernel_h_;
  real_src_h_ = (real_dst_h_ - 1) * stride_h_ + kernel_h_;
  // width
  auto owp = 4;
  length_ = ow_iter_ * tile_owg_ * owp;
  dst_w_ = length_;
  instr_stride_w_ = stride_w_;  // for read()
  stride_w_ = std::min(kernel_w_, stride_w_);
  src_w_ = (length_ - 1) * stride_w_ + kernel_w_;
  // channel
  cg_ = 2 * ic_iter_ * tile_icg_;
  co_ = channel_offset_;
  ic_ = cg_ * icp_;  // input_channel is channel_group multiply channel_parallel
  oc_ = ocp_ * tile_ocg_;  // output_channel is channel_parallel

  src_size_ = src_h_ * src_w_ * ic_;
  group_id_out_ = bank_id_out_ / pp_;

  // resize related buffer size
  img_.resize(src_h_ * src_w_ * ic_);
  weights_.resize(oc_ * kernel_h_ * kernel_w_ * ic_);
  bias_.resize(oc_);
  rlt_s64_.resize(dst_h_ * dst_w_ * oc_);
  rlt_dtype_.resize(dst_h_ * dst_w_ * oc_);
  xvdpu_sum_dtype_.resize(dst_h_ * dst_w_ * oc_);
  if (SimCfg::Instance().get_gen_aie_data()) {
    Util::ChkFolder(MECfg::data_path, false);
    xvdpu_data_dir_ =
        MECfg::data_path + "/conv_inst_" + to_string(instid_) + "/";
    Util::ChkFolder(xvdpu_data_dir_, true);
  }
  if (ocp_ == 16) {
    MECfg::Instance().Setimg_port_width(2);
  } else if (ocp_ == 32 || ocp_ == 64) {
    MECfg::Instance().Setimg_port_width(4);
  } else {
    UNI_LOG_FATAL(SIM_EXCEED_HARDWARE_LIMIT)
        << "Not support this OCP: " << ocp_;
  }
  xvdpu_num_wgt_port_ = ocp_ / 4;
  MECfg::Instance().Setnum_weights_port(xvdpu_num_wgt_port_);

  UNI_LOG_CHECK(ocp_ == xvdpu_num_wgt_port_ * 4, SIM_PARAMETER_FAILED)
      << " output_channel_parall not equal weights port number" << endl;
  xvdpu_OCpg_ = ocp_ / 8;

  // get bank_num of weights bank group
  auto wgt_bank_group_name =
      ArchCfg::Instance().get_param().conv_engine().weight_bank();
  auto bank_group_size = ArchCfg::Instance().get_param().bank_group_size();
  for (auto idx_bank_group = 0; idx_bank_group < bank_group_size;
       idx_bank_group++) {
    auto name =
        ArchCfg::Instance().get_param().bank_group(idx_bank_group).name();
    if (wgt_bank_group_name == name) {
      wgt_bank_num_ =
          ArchCfg::Instance().get_param().bank_group(idx_bank_group).bank_num();
      break;
    }
  }
}

template <>
Conv<DPUVersion::DPU4F>::Conv(int inst_type, int instid,
                              vector<string>& inst_str,
                              vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  // debug
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_CONV);
  Helper<DPUVersion::DPU4F>::InstInit(inst_type_, root_debug_path_, debug_,
                                      opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPU4F>::inst_table;
  // get conv instruction field value
  channel_group_ = field_val_[TABLE::CONV_FIELD_CHANNEL_GROUP];
  bank_addr_in_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_IN];
  pad_left_ = field_val_[TABLE::CONV_FIELD_PAD_LEFT];
  pad_top_ = field_val_[TABLE::CONV_FIELD_PAD_TOP];
  pad_right_ = field_val_[TABLE::CONV_FIELD_PAD_RIGHT];
  pad_bottom_ = field_val_[TABLE::CONV_FIELD_PAD_BOTTOM];
  bank_addr_out_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_OUT];
  channel_offset_ = field_val_[TABLE::CONV_FIELD_CHANNEL_OFFSET];
  bank_addr_weights_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_WEIGHTS];
  length_ = field_val_[TABLE::CONV_FIELD_LENGTH];
  bank_addr_bias_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_BIAS];
  bank_id_in_ = field_val_[TABLE::CONV_FIELD_BANK_ID_IN];
  bank_id_out_ = field_val_[TABLE::CONV_FIELD_BANK_ID_OUT];

  bank_addr_in_1_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_IN_1];
  bank_addr_in_2_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_IN_2];
  bank_addr_in_3_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_IN_3];

  // convinit value
  using CONVINIT = ConvInit<DPUVersion::DPU4F>;
  kernel_w_ = CONVINIT::CUROBJ->get_kernel_w();
  kernel_h_ = CONVINIT::CUROBJ->get_kernel_h();
  shift_bias_ = CONVINIT::CUROBJ->get_shift_bias();
  shift_cut_ = CONVINIT::CUROBJ->get_shift_cut();
  quant_lth_ = CONVINIT::CUROBJ->get_quant_lth();
  calt_mode_ = CONVINIT::CUROBJ->get_calt_mode();
  act_type_ = CONVINIT::CUROBJ->get_act_type();
  stride_w_ = CONVINIT::CUROBJ->get_stride_w();
  stride_h_ = CONVINIT::CUROBJ->get_stride_h();
  stride_out_ = CONVINIT::CUROBJ->get_stride_out();
  jump_read_ = CONVINIT::CUROBJ->get_jump_read();
  jump_read_endl_ = CONVINIT::CUROBJ->get_jump_read_endl();
  jump_write_ = CONVINIT::CUROBJ->get_jump_write();
  jump_write_endl_ = CONVINIT::CUROBJ->get_jump_write_endl();
  stride_offset_in_ = CONVINIT::CUROBJ->get_stride_offset_in();
  stride_offset_out_ = CONVINIT::CUROBJ->get_stride_offset_out();
  valid_pixel_parallel_ = CONVINIT::CUROBJ->get_valid_pixel_parallel();

  // constraints from hardware implementation
  UNI_LOG_CHECK(shift_bias_ <= 20, SIM_PARAMETER_FAILED)
      << "invalid shift_bias";

  // self defined
  vpp_ = valid_pixel_parallel_;
  pp_ = ArchCfg::Instance().get_param().conv_engine().pixel_parallel();
  icp_ = ArchCfg::Instance().get_param().conv_engine().input_channel_parallel();
  ocp_ =
      ArchCfg::Instance().get_param().conv_engine().output_channel_parallel();
  cg_ = channel_group_;
  co_ = channel_offset_;
  ic_ = cg_ * icp_;  // input_channel is channel_group multiply channel_parallel
  oc_ = ocp_;        // output_channel is channel_parallel
  src_h_ = (pp_ - 1) * stride_h_ + kernel_h_;
  real_src_h_ = (vpp_ - 1) * stride_h_ + kernel_h_;
  src_w_ = (length_ - 1) * stride_w_ + kernel_w_;
  dst_h_ = pp_;
  real_dst_h_ = vpp_;
  dst_w_ = length_;
  src_size_ = src_h_ * src_w_ * ic_;
  group_id_out_ = bank_id_out_ / pp_;

  // resize related buffer size
  img_.resize(src_h_ * src_w_ * ic_);
  weights_.resize(oc_ * kernel_h_ * kernel_w_ * ic_);
  bias_.resize(oc_);
  rlt_s64_.resize(dst_h_ * dst_w_ * oc_);
  rlt_dtype_.resize(dst_h_ * dst_w_ * oc_);
}

template <DPUVersion T>
Conv<T>::~Conv() {}

// public funcs
template <DPUVersion T>
void Conv<T>::Exec() {
  // read img, weights, bias data from bank
  read();
  debug_read_pos_tick();
  debug_input_fmap("read");
  debug_input_tick("read");
  // padding for img data, also handle channel
  // offset
  pad();
  debug_input_fmap("pad");
  debug_input_tick("pad");
  // do conv operation of the whole feature map
  conv();
  debug_output_fmap(0, "conv_rlt_fmap");
  // add bias to conv result
  bias();
  debug_output_fmap(0, "bias_rlt_fmap");
  // do shift, trunc operation
  transform();
  debug_output_fmap(1, "transform_rlt_fmap");
  debug_output_tick("conv_result_tick");
  gen_aie_data();
  // save result to bank
  save();
}

template <>
void Conv<DPUVersion::DPUV3ME>::Exec() {
  // read img, weights, bias data from bank
  read();
  debug_input_fmap("read");
  debug_input_tick("read");
  // padding for img data, also handle channel offset
  pad();
  debug_input_fmap("pad");
  debug_input_tick("pad");
  // do conv operation of the whole feature map
  conv();
  debug_output_fmap(0, "conv_rlt_fmap");
  // add bias to conv result
  bias();
  debug_output_fmap(0, "bias_rlt_fmap");
  // do shift, trunc operation
  transform();
  debug_output_fmap(1, "transform_rlt_fmap");

  // save output
  if ((dest_mode_ == 1) || (dest_mode_ == 3)) {
    debug_output_tick_hbm("conv_result_tick_hbm");
    save_to_hbm();
  }
  if ((dest_mode_ == 2) || (dest_mode_ == 3)) {
    debug_output_tick_cb("conv_result_tick_cb");
    save_to_cb();
  }
}

// private funcs
template <DPUVersion dv>
void Conv<dv>::read() {
  // UNI_LOG_INFO << endl;
  // UNI_LOG_INFO << "read start..." << endl;

  // read img data from bank
  // UNI_LOG_INFO << "reading img..." << endl;

  UNI_LOG_CHECK(
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_in_) != nullptr,
      SIM_OUT_OF_RANGE)
      << "bank_id " << bank_id_in_ << " out of range!" << endl;
  auto bank_depth =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_in_)->GetH();
  UNI_LOG_CHECK(stride_offset_in_ < stride_h_, SIM_PARAMETER_FAILED)
      << "invalid stride_offset_in";
  auto stride_offset = stride_offset_in_;
  auto img_bank_id = bank_id_in_;
  auto base_bank_id = (bank_id_in_ / pp_) * pp_;
  auto addr_group = 0;
  for (auto idx_src_h = 0; idx_src_h < src_h_; idx_src_h++) {
    for (auto idx_src_w = 0; idx_src_w < src_w_; idx_src_w++) {
      auto ddr_addr = (idx_src_h * src_w_ + idx_src_w) * ic_;
      auto bank_addr_group_base = get_group_base_addr(addr_group);
      auto bank_addr = (bank_addr_group_base + stride_offset * jump_read_endl_ +
                        idx_src_w * jump_read_) %
                       bank_depth;
      auto bank = Buffer<DPU_DATA_TYPE>::Instance().GetBank(img_bank_id);
      UNI_LOG_CHECK(bank != nullptr, SIM_OUT_OF_RANGE)
          << "bank_id " << img_bank_id << " out of range!" << endl;
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
      ArchCfg::Instance().get_param().conv_engine().weight_bank();
  auto bank_depth_wgt =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(wgt_bank_group, 0)->GetH();
  for (int i = 0; i < oc_; i++) {
    int bank_id = i % ocp_;
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
        ArchCfg::Instance().get_param().conv_engine().bias_bank();
    auto bank = Buffer<DPU_DATA_TYPE>::Instance().GetBank(bias_bank_group, 0);
    bank->Read(bank_addr_bias_, oc_, bias_.data());
  }
  // UNI_LOG_INFO << "read finish!" << endl;
}

template <>
void Conv<DPUVersion::XVDPU>::read() {
  UNI_LOG_CHECK(
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_in_) != nullptr,
      SIM_OUT_OF_RANGE)
      << "bank_id " << bank_id_in_ << " out of range!" << endl;
  auto bank_depth =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_in_)->GetH();
  UNI_LOG_CHECK(stride_offset_in_ < instr_stride_h_, SIM_PARAMETER_FAILED)
      << "invalid stride_offset_in";

  auto bank_id_base = (bank_id_in_ / pp_) * pp_;
  auto bank_id_base_offset = bank_id_in_ - bank_id_base;

  // read image form bank
  auto real_h_offset = std::min(kernel_h_, instr_stride_h_);
  auto src_w_offset = instr_stride_w_;
  auto real_w_offset = std::min(kernel_w_, instr_stride_w_);
  auto idx_ddr_h{0};
  for (auto idx_dst_h = 0; idx_dst_h < dst_h_; idx_dst_h++) {
    // set last kernel of dst_h
    auto max_inner_h = (idx_dst_h == dst_h_ - 1) ? kernel_h_ : real_h_offset;
    for (auto idx_inner_h = 0; idx_inner_h < max_inner_h; idx_inner_h++) {
      auto idx_ddr_w{0};
      for (auto idx_dst_w = 0; idx_dst_w < dst_w_; idx_dst_w++) {
        // set last kernel of dst_w
        auto max_inner_w =
            (idx_dst_w == dst_w_ - 1) ? kernel_w_ : real_w_offset;
        for (auto idx_inner_w = 0; idx_inner_w < max_inner_w; idx_inner_w++) {
          auto idx_src_w = idx_dst_w * src_w_offset + idx_inner_w;
          auto ddr_addr = (idx_ddr_h * src_w_ + idx_ddr_w) * ic_;
          // img bank id
          auto bank_offset =
              bank_id_base_offset + idx_dst_h +
              (stride_offset_in_ + idx_inner_h) / instr_stride_h_;
          auto img_bank_id = bank_id_base + bank_offset % pp_;
          // img bank addr
          auto addr_group = bank_offset / pp_;
          auto bank_addr_group_base = get_group_base_addr(addr_group);
          auto stride_offset =
              (stride_offset_in_ + idx_inner_h) % instr_stride_h_;
          auto bank_addr =
              (bank_addr_group_base + stride_offset * jump_read_endl_ +
               idx_src_w * jump_read_) %
              bank_depth;

          auto bank = Buffer<DPU_DATA_TYPE>::Instance().GetBank(img_bank_id);
          UNI_LOG_CHECK(bank != nullptr, SIM_OUT_OF_RANGE)
              << "bank_id " << img_bank_id << " out of range!" << endl;
          bank->Read(bank_addr, ic_, img_.data() + ddr_addr);
          idx_ddr_w++;
        }
      }
      idx_ddr_h++;
    }
  }
  // read weights from bank
  auto wgt_bank_group =
      ArchCfg::Instance().get_param().conv_engine().weight_bank();
  auto bank_depth_wgt =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(wgt_bank_group, 0)->GetH();
  auto real_channel_group = ic_iter_ * tile_icg_ * 2 - icg_offset_;
  for (int i = 0; i < oc_; i++) {
    int bank_id = i % wgt_bank_num_;
    for (int j = 0; j < kernel_h_; j++) {
      for (int k = 0; k < kernel_w_; k++) {
        int ddr_addr =
            i * kernel_h_ * kernel_w_ * ic_ + j * kernel_w_ * ic_ + k * ic_;
        int bank_addr =
            (bank_addr_weights_ + (i / wgt_bank_num_) * jump_read_weights_ +
             j * kernel_w_ * real_channel_group + k * real_channel_group) %
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
        ArchCfg::Instance().get_param().conv_engine().bias_bank();
    auto bank = Buffer<DPU_DATA_TYPE>::Instance().GetBank(bias_bank_group, 0);
    bank->Read(bank_addr_bias_, oc_, bias_.data());
  }
}

template <>
void Conv<DPUVersion::DPU4F>::read() {
  UNI_LOG_CHECK(
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_in_) != nullptr,
      SIM_OUT_OF_RANGE)
      << "bank_id " << bank_id_in_ << " out of range!" << endl;
  auto bank_depth =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_in_)->GetH();
  UNI_LOG_CHECK(stride_offset_in_ < stride_h_, SIM_PARAMETER_FAILED)
      << "invalid stride_offset_in";
  auto stride_offset = stride_offset_in_;
  auto img_bank_id = bank_id_in_;
  auto base_bank_id = (bank_id_in_ / pp_) * pp_;
  auto addr_group = 0;
  for (auto idx_src_h = 0; idx_src_h < src_h_; idx_src_h++) {
    for (auto idx_src_w = 0; idx_src_w < src_w_; idx_src_w++) {
      auto ddr_addr = (idx_src_h * src_w_ + idx_src_w) * ic_;
      auto bank_addr_group_base = get_group_base_addr(addr_group);
      auto bank_addr = (bank_addr_group_base + stride_offset * jump_read_endl_ +
                        idx_src_w * jump_read_) %
                       bank_depth;
      if (icp_ != ocp_)
        UNI_LOG_CHECK((bank_addr % 2 == 0), SIM_PARAMETER_FAILED)
            << "bank_addr = " << bank_addr << ". All bank_addr must be even."
            << endl;

      BankShell::read(false, img_bank_id, ic_, bank_addr,
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
  // get 4-bit unsigned image
  for (auto& tmp_img : img_) {
    if (calt_mode_ == 0) {
      unsigned_data tmp = {*reinterpret_cast<uint8_t*>(&tmp_img)};
      tmp_img = static_cast<DPU_DATA_TYPE>(tmp.lo);
    } else {
      signed_data tmp = {*reinterpret_cast<int8_t*>(&tmp_img)};
      tmp_img = static_cast<DPU_DATA_TYPE>(tmp.lo);
    }
  }

  // read weights from bank
  auto wgt_bank_group =
      ArchCfg::Instance().get_param().conv_engine().weight_bank();
  auto bank_depth_wgt =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(wgt_bank_group, 0)->GetH();
  for (int i = 0; i < oc_; i++) {
    int bank_id = i % ocp_;
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
  // get 4-bit signed weights
  for (auto& tmp_wgt : weights_) {
    signed_data tmp = {tmp_wgt};
    tmp_wgt = static_cast<DPU_DATA_TYPE>(tmp.lo);
  }

  // if the even_th oc is less than 0, the pre_odd_th oc will add 1
  for (int i = 0; i < oc_; i += 2) {
    for (int j = 0; j < kernel_h_; j++) {
      for (int k = 0; k < kernel_w_; k++) {
        for (int idx_ic = 0; idx_ic < ic_; idx_ic++) {
          int ddr_addr_pre = i * kernel_h_ * kernel_w_ * ic_ +
                             j * kernel_w_ * ic_ + k * ic_ + idx_ic;
          int ddr_addr_post = (i + 1) * kernel_h_ * kernel_w_ * ic_ +
                              j * kernel_w_ * ic_ + k * ic_ + idx_ic;
          if (weights_[ddr_addr_pre] < 0) {
            if (weights_[ddr_addr_post] == 7)
              weights_[ddr_addr_post] = -8;
            else
              weights_[ddr_addr_post] += 1;
          }
        }
      }
    }
  }

  // read bias & shift_bias & shift_cut from bank
  {
    auto bias_bank_group =
        ArchCfg::Instance().get_param().conv_engine().bias_bank();
    auto bank = Buffer<DPU_DATA_TYPE>::Instance().GetBank(bias_bank_group, 0);
    bank->Read(bank_addr_bias_, oc_, bias_.data());
  }
}

#include "./xvdpu/meConv.imp"

template <>
void Conv<DPUVersion::XVDPU>::Exec() {
  // read img, weights, bias data from bank
  read();
  debug_input_fmap("read");
  debug_input_tick("read");
  // padding for img data, also handle channel
  // offset
  pad();
  xvdpu_pad();
  debug_input_fmap("pad");
  debug_input_tick("pad");
  // do conv operation of the whole feature map
  conv();
  xvdpu_conv();
  debug_output_fmap(0, "conv_rlt_fmap");
  // add bias to conv result
  bias();
  debug_output_fmap(0, "bias_rlt_fmap");
  // do shift, trunc operation
  transform();
  xvdpu_transform();
  debug_output_fmap(1, "transform_rlt_fmap");
  debug_output_tick("conv_result_tick");
  // save result to bank
  save();
}

template <DPUVersion dv>
int Conv<dv>::get_group_base_addr(int addr_group) {
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
    UNI_LOG_FATAL(SIM_OUT_OF_RANGE)
        << "bank group addr in is not "
           "enough, needed at least "
        << addr_group << ", infact it privide 4" << endl;
  }
  return ret;
}

template <>
int Conv<DPUVersion::DPUV3ME>::get_group_base_addr(int addr_group) {
  return bank_addr_in_ + addr_group * stride_h_ * jump_read_endl_;
}

template <DPUVersion dv>
void Conv<dv>::pad() {
  // UNI_LOG_INFO << endl;
  // UNI_LOG_INFO << "pad start..." << endl;
  // padding left
  // UNI_LOG_INFO << "padding left..." << endl;
  for (int i = 0; i < src_h_; i++) {
    for (int j = 0; j < pad_left_ && j < src_w_; j++) {
      for (int k = 0; k < ic_; k++) {
        unsigned int addr = i * src_w_ * ic_ + j * ic_ + k;
        UNI_LOG_CHECK(addr < img_.size(), SIM_OUT_OF_RANGE)
            << addr << " >= " << img_.size();
        img_[addr] = 0;
      }
    }
  }

  // padding top
  // UNI_LOG_INFO << "padding top..." << endl;
  for (int i = 0; i < pad_top_ && i < src_h_; i++) {
    for (int j = 0; j < src_w_; j++) {
      for (int k = 0; k < ic_; k++) {
        unsigned int addr = i * src_w_ * ic_ + j * ic_ + k;
        UNI_LOG_CHECK(addr < img_.size(), SIM_OUT_OF_RANGE)
            << addr << " >= " << img_.size();
        img_[addr] = 0;
      }
    }
  }

  // padding right
  // UNI_LOG_INFO << "padding right..." << endl;
  for (int i = 0; i < src_h_; i++) {
    for (int j = src_w_ - kernel_w_ + pad_right_ + 1; j < src_w_; j++) {
      for (int k = 0; k < ic_; k++) {
        unsigned int addr = i * src_w_ * ic_ + j * ic_ + k;
        UNI_LOG_CHECK(addr < img_.size(), SIM_OUT_OF_RANGE)
            << addr << " >= " << img_.size();
        img_[addr] = 0;
      }
    }
  }

  // padding bottom, it's different from padding
  // right UNI_LOG_INFO << "padding bottom..." <<
  // endl;
  for (int i = real_src_h_ - kernel_h_ + pad_bottom_ + 1; i < src_h_; i++) {
    for (int j = 0; j < src_w_; j++) {
      for (int k = 0; k < ic_; k++) {
        unsigned int addr = i * src_w_ * ic_ + j * ic_ + k;
        UNI_LOG_CHECK(addr < img_.size(), SIM_OUT_OF_RANGE)
            << addr << " >= " << img_.size();
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
void Conv<dv>::conv() {
  // UNI_LOG_INFO << endl;
  // UNI_LOG_INFO << "conv start..." << endl;
  for (int i = 0; i + kernel_h_ <= src_h_; i += stride_h_) {
    for (int j = 0; j + kernel_w_ <= src_w_; j += stride_w_) {
      for (int k = 0; k < oc_; k++) {
        conv_one(i, j, k);
      }
    }
  }

  // UNI_LOG_INFO << "conv finish!" << endl;
}

template <DPUVersion dv>
void Conv<dv>::conv_one(int idx_src_h, int idx_src_w, int idx_oc) {
  int64_t result = 0;

  // calculate conv value
  for (int i = 0; i < kernel_h_; i++) {
    for (int j = 0; j < kernel_w_; j++) {
      for (int k = 0; k < ic_; k++) {
        int img_addr =
            (idx_src_h + i) * src_w_ * ic_ + (idx_src_w + j) * ic_ + k;
        int weights_addr = idx_oc * kernel_h_ * kernel_w_ * ic_ +
                           i * kernel_w_ * ic_ + j * ic_ + k;
        result += img_[img_addr] * weights_[weights_addr];
      }
    }
  }

  // assign conv value to rlt_s64_ var
  int idx_dst_h = idx_src_h / stride_h_;
  int idx_dst_w = idx_src_w / stride_w_;
  int rlt_addr = idx_dst_h * dst_w_ * oc_ + idx_dst_w * oc_ + idx_oc;
  rlt_s64_[rlt_addr] = result;
}

template <DPUVersion dv>
void Conv<dv>::bias() {
  shift_bias_ = (shift_bias_ >= 32) ? (32 - shift_bias_) : shift_bias_;
  for (int i = 0; i < dst_h_; i++) {
    for (int j = 0; j < dst_w_; j++) {
      for (int k = 0; k < oc_; k++) {
        int rlt_addr = i * dst_w_ * oc_ + j * oc_ + k;
        rlt_s64_[rlt_addr] *= 2;
        rlt_s64_[rlt_addr] +=
            floor((double)bias_[k] * 2.0 * pow(2, shift_bias_));
      }
    }
  }
}

template <>
void Conv<DPUVersion::DPUV2>::bias() {
  for (int i = 0; i < dst_h_; i++) {
    for (int j = 0; j < dst_w_; j++) {
      for (int k = 0; k < oc_; k++) {
        int rlt_addr = i * dst_w_ * oc_ + j * oc_ + k;
        rlt_s64_[rlt_addr] *= 2;
        rlt_s64_[rlt_addr] += (double)bias_[k] * pow(2, shift_bias_);
      }
    }
  }
}

template <>
void Conv<DPUVersion::XVDPU>::bias() {
  for (int i = 0; i < dst_h_; i++) {
    for (int j = 0; j < dst_w_; j++) {
      for (int k = 0; k < oc_; k++) {
        int rlt_addr = i * dst_w_ * oc_ + j * oc_ + k;
        rlt_s64_[rlt_addr] *= 2;
        rlt_s64_[rlt_addr] += (double)bias_[k] * pow(2, shift_bias_);
      }
    }
  }
}

template <>
void Conv<DPUVersion::DPU4F>::bias() {
  for (int i = 0; i < dst_h_; i++) {
    for (int j = 0; j < dst_w_; j++) {
      for (int k = 0; k < oc_; k++) {
        int rlt_addr = i * dst_w_ * oc_ + j * oc_ + k;
        // real valid bit inside 21
        int32_t tmp_bias;
        if (((static_cast<int32_t>(bias_[k]) << shift_bias_) & 0x00100000) !=
            0x00100000) {
          tmp_bias =
              0x001FFFFF & (static_cast<int32_t>(bias_[k]) << shift_bias_);
        } else {
          tmp_bias =
              (0x001FFFFF & (static_cast<int32_t>(bias_[k]) << shift_bias_)) |
              0xFFE00000;
        }

        rlt_s64_[rlt_addr] *= 2;
        rlt_s64_[rlt_addr] += tmp_bias;
      }
    }
  }
}

template <DPUVersion dv>
void Conv<dv>::transform() {
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
void Conv<DPUVersion::DPU4F>::transform() {
  for (int i = 0; i < dst_h_; i++) {
    for (int j = 0; j < dst_w_; j++) {
      for (int k = 0; k < oc_; k++) {
        int addr = i * dst_w_ * oc_ + j * oc_ + k;

        double tmp = rlt_s64_[addr];
        tmp /= pow(2, shift_cut_ + 1);

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
        // rlt_dtype_[addr] = Calc::DPURound<DPU_DATA_TYPE>(tmp);
        if (quant_lth_) {
          rlt_dtype_[addr] = Calc::DPURound<DPU_DATA_TYPE>(tmp);
        } else {
          if (act_type_ == Calc::RELU_TYPE_RELU)
            if (tmp > 15) {
              rlt_dtype_[addr] = 15;
            } else if (tmp < 0) {
              rlt_dtype_[addr] = 0;
            } else if (tmp < 0 && (tmp - floor(tmp)) == 0.5) {
              rlt_dtype_[addr] = static_cast<DPU_DATA_TYPE>(ceil(tmp));
            } else {
              rlt_dtype_[addr] = static_cast<DPU_DATA_TYPE>(round(tmp));
            }
          else if (tmp > 7) {
            rlt_dtype_[addr] = 7;
          } else if (tmp < -8) {
            rlt_dtype_[addr] = -8;
          } else if (tmp < 0 && (tmp - floor(tmp)) == 0.5) {
            rlt_dtype_[addr] = static_cast<DPU_DATA_TYPE>(ceil(tmp));
          } else {
            rlt_dtype_[addr] = static_cast<DPU_DATA_TYPE>(round(tmp));
          }
        }
      }
    }
  }
}

template <DPUVersion T>
void Conv<T>::save() {
  // UNI_LOG_INFO << endl;
  // UNI_LOG_INFO << "conv save start..." << endl;
  UNI_LOG_CHECK(
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_out_) != nullptr,
      SIM_OUT_OF_RANGE)
      << "bank_id " << bank_id_out_ << " out of range!" << endl;

  auto bank_depth =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_out_)->GetH();
  auto bank_id_out_base = bank_id_out_ - bank_id_out_ % pp_;
  // bank id of the first bank in this bank_group
  auto bank_id_out_offset = bank_id_out_ % pp_;
  auto oc_align = oc_ - ocg_offset_ * ocp_;

  for (auto idx_h = 0; idx_h < real_dst_h_; idx_h++) {
    auto bank_offset = static_cast<uint32_t>(
        floor((float)(idx_h + stride_offset_out_) / stride_out_));
    // how many times does it change the bank
    auto bank_group_offset = static_cast<uint32_t>(
        floor((float)(bank_id_out_offset + bank_offset) / pp_));
    // how many times does it change the bank group
    auto bank_id_this_row =
        bank_id_out_base + (bank_id_out_offset + bank_offset) % pp_;
    auto bank_addr_this_row =
        bank_addr_out_                                        // base_addr
        + bank_group_offset * stride_out_ * jump_write_endl_  // change bank
        + ((idx_h + stride_offset_out_) % stride_out_) *
              jump_write_endl_  // change stride
                                // offset
        ;
    auto bank = Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_this_row);
    UNI_LOG_CHECK(bank != nullptr, SIM_OUT_OF_RANGE)
        << "bank_id " << bank_id_this_row << " out of range!" << endl;
    for (int idx_w = 0; idx_w < dst_w_ - ow_offset_; idx_w++) {
      auto bank_addr_this_pixel =
          (bank_addr_this_row + idx_w * jump_write_) % bank_depth;
      auto rlt_addr_this_pixel = idx_h * dst_w_ * oc_ + idx_w * oc_;
      bank->Write(bank_addr_this_pixel, oc_align,
                  rlt_dtype_.data() + rlt_addr_this_pixel);
    }
  }
  // UNI_LOG_INFO << "conv save finish!" << endl;
}

template <>
void Conv<DPUVersion::DPU4F>::save() {
  UNI_LOG_CHECK(
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_out_) != nullptr,
      SIM_OUT_OF_RANGE)
      << "bank_id " << bank_id_out_ << " out of range!" << endl;

  auto bank_depth =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_out_)->GetH();
  auto bank_id_out_base = bank_id_out_ - bank_id_out_ % pp_;
  // bank id of the first bank in this bank_group
  auto bank_id_out_offset = bank_id_out_ % pp_;
  for (auto idx_h = 0; idx_h < real_dst_h_; idx_h++) {
    auto bank_offset = static_cast<uint32_t>(
        floor((float)(idx_h + stride_offset_out_) / stride_out_));
    // how many times does it change the bank
    auto bank_group_offset = static_cast<uint32_t>(
        floor((float)(bank_id_out_offset + bank_offset) / pp_));
    // how many times does it change the bank group
    auto bank_id_this_row =
        bank_id_out_base + (bank_id_out_offset + bank_offset) % pp_;
    auto bank_addr_this_row =
        bank_addr_out_                                        // base_addr
        + bank_group_offset * stride_out_ * jump_write_endl_  // change bank
        + ((idx_h + stride_offset_out_) % stride_out_) *
              jump_write_endl_  // change stride
                                // offset
        ;
    auto bank = Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_this_row);
    UNI_LOG_CHECK(bank != nullptr, SIM_OUT_OF_RANGE)
        << "bank_id " << bank_id_this_row << " out of range!" << endl;
    for (int idx_w = 0; idx_w < dst_w_; idx_w++) {
      auto bank_addr_this_pixel =
          (bank_addr_this_row + idx_w * jump_write_) % bank_depth;
      auto rlt_addr_this_pixel = idx_h * dst_w_ * oc_ + idx_w * oc_;
      BankShell::write(quant_lth_, bank_id_this_row, oc_, bank_addr_this_pixel,
                       rlt_dtype_.data() + rlt_addr_this_pixel);
    }
  }
}

template <DPUVersion T>
void Conv<T>::save_to_cb() {
  UNI_LOG_CHECK(
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_out_cb_) != nullptr,
      SIM_OUT_OF_RANGE)
      << "bank_id " << bank_id_out_cb_ << " out of range!" << endl;

  auto bank_depth =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_out_cb_)->GetH();
  auto bank_id_out_base = bank_id_out_cb_ - bank_id_out_cb_ % pp_;
  auto bank_id_out_offset = bank_id_out_cb_ % pp_;
  for (auto idx_h = 0; idx_h < real_dst_h_; idx_h++) {
    // bank id
    auto bank_shift = (idx_h + stride_offset_out_cb_) / stride_out_cb_;
    auto addr_group = (bank_id_out_offset + bank_shift) / pp_;
    auto bank_id = bank_id_out_base + (bank_id_out_offset + bank_shift) % pp_;
    auto bank = Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id);
    // bank address
    auto bank_addr_base = bank_addr_out_cb_ +
                          addr_group * stride_out_cb_ * jump_write_endl_cb_ +
                          ((idx_h + stride_offset_out_cb_) % stride_out_cb_) *
                              jump_write_endl_cb_;

    for (int idx_w = 0; idx_w < dst_w_; idx_w++) {
      auto bank_addr = (bank_addr_base + idx_w * jump_write_cb_) % bank_depth;
      auto addr_offset = idx_h * dst_w_ * oc_ + idx_w * oc_;
      bank->Write(bank_addr, oc_, rlt_dtype_.data() + addr_offset);
    }
  }
}

template <DPUVersion T>
void Conv<T>::save_to_hbm() {
  UNI_LOG_CHECK(
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_out_hbm_) != nullptr,
      SIM_OUT_OF_RANGE)
      << "bank_id " << bank_id_out_hbm_ << " out of range!" << endl;

  auto bank_depth =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_out_hbm_)->GetH();
  auto bank_id_out_base = bank_id_out_hbm_ - bank_id_out_hbm_ % pp_;
  auto bank_id_out_offset = bank_id_out_hbm_ % pp_;
  for (auto idx_h = 0; idx_h < real_dst_h_; idx_h++) {
    // bank id
    auto bank_shift = (idx_h + stride_offset_out_hbm_) / stride_out_hbm_;
    auto addr_group = (bank_id_out_offset + bank_shift) / pp_;
    auto bank_id = bank_id_out_base + (bank_id_out_offset + bank_shift) % pp_;
    auto bank = Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id);
    // bank address
    auto bank_addr_base = bank_addr_out_hbm_ +
                          addr_group * stride_out_hbm_ * jump_write_endl_hbm_ +
                          ((idx_h + stride_offset_out_hbm_) % stride_out_hbm_) *
                              jump_write_endl_hbm_;
    for (int idx_w = 0; idx_w < dst_w_; idx_w++) {
      auto bank_addr = (bank_addr_base + idx_w * jump_write_hbm_) % bank_depth;
      auto addr_offset = idx_h * dst_w_ * oc_ + idx_w * oc_;
      if (output_channel_offset_ < icp_) {
        bank->Write(bank_addr, oc_, rlt_dtype_.data() + addr_offset);
      } else {
        bank->Write(bank_addr, icp_, rlt_dtype_.data() + addr_offset);
      }
    }
  }
}

template <DPUVersion T>
void Conv<T>::debug_input_fmap(string prefix) {
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
  for (int i = 0; i < oc_; i++) {
    // save output_channel title to file
    ss.clear();
    ss.str("");
    ss << "output_channel_" << i << "-------------------------------------";
    ss >> head;
    head += "\n";
    Util::SaveBin(save_name_weights, head.data(), head.size(), SM_APPEND);

    vector<DPU_DATA_TYPE> weights(ic_ * kernel_h_ * kernel_w_);
    Calc::HWC2CHW(kernel_h_, kernel_w_, ic_,
                  weights_.data() + i * ic_ * kernel_h_ * kernel_w_,
                  weights.data());
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

template <DPUVersion T>
void Conv<T>::debug_read_pos_tick() {
  if (!debug_) {
    return;
  }
  string save_name;
  stringstream ss;
  ss.clear();
  ss.str("");
  ss << debug_path_ << "conv_read_pos_tick." << instid_;
  ss >> save_name;
  ofstream fout(save_name);
  auto bank_depth =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_in_)->GetH();
  UNI_LOG_CHECK(stride_offset_in_ < stride_h_, SIM_PARAMETER_FAILED)
      << "invalid stride_offset_in";
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
          for (auto idx_pixel_parallel = 0; idx_pixel_parallel < pp_;
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
void Conv<T>::debug_input_tick(string prefix) {
  if (!debug_) return;

  string save_name_img;
  string save_name_weights;
  string save_name_bias;

  // make save file name
  stringstream ss;
  ss.clear();
  ss.str("");
  ss << debug_path_ << "img_" << prefix << "_tick." << instid_classify_;
  ss >> save_name_img;
  ss.clear();
  ss.str("");
  ss << debug_path_ << "weights_" << prefix << "_tick." << instid_classify_;
  ss >> save_name_weights;
  ss.clear();
  ss.str("");
  ss << debug_path_ << "bias_" << prefix << "_tick." << instid_classify_;
  ss >> save_name_bias;

  // save bias data in smallend mode
  auto bias_bank_group =
      ArchCfg::Instance().get_param().conv_engine().bias_bank();
  auto bias_bank =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bias_bank_group, 0);
  auto bank_id_bias = bias_bank->GetID();
  auto bank_depth_bias = bias_bank->GetH();
  auto bank_width_bias = bias_bank->GetW();
  Util::SaveHexContSmallEndBankAddr(
      save_name_bias, reinterpret_cast<const char*>(bias_.data()),
      bias_.size() * sizeof(DPU_DATA_TYPE),
      bank_width_bias * sizeof(DPU_DATA_TYPE), bank_depth_bias, bank_id_bias,
      bank_addr_bias_, SM_APPEND);
  // save img and weights data in smallend mode
  // img bank
  auto img_bank = Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_in_);
  auto bank_depth_img = img_bank->GetH();
  auto bank_width_img = img_bank->GetW();
  // weights bank
  auto wgt_bank_group =
      ArchCfg::Instance().get_param().conv_engine().weight_bank();
  auto wgt_bank = Buffer<DPU_DATA_TYPE>::Instance().GetBank(wgt_bank_group, 0);
  auto bank_depth_wgt = wgt_bank->GetH();
  auto bank_width_wgt = wgt_bank->GetW();
  for (int i = 0; i < src_w_ - kernel_w_ + 1; i += stride_w_) {
    for (int j = 0; j < cg_; j++) {
      for (int n = 0; n < kernel_h_; n++) {
        for (int m = 0; m < kernel_w_; m++) {
          // one tick, do following operation
          // get cp_*pp_ img data
          // get cp_*cp_ weights data
          // vector<DPU_DATA_TYPE> img_weights(cp_
          // * pp_ + cp_ * cp_);
          //          for (int k = 0; k < cp_; k++)
          //          {
          for (int l = 0; l < src_h_ - kernel_h_ + 1; l += stride_h_) {
            // important: must divide stride_h_
            // here
            int bank_offset =
                bank_id_in_ % pp_ + (stride_offset_in_ + l + n) / stride_h_;
            int id = bank_offset % pp_;
            int cycles = bank_offset / pp_;
            int bank_id_img = (bank_id_in_ / pp_) * pp_ + id;
            auto bank_addr_group_base = get_group_base_addr(cycles);
            int bank_addr_img =
                (bank_addr_group_base +
                 ((stride_offset_in_ + l + n) % stride_h_) * jump_read_endl_ +
                 (i + m) * jump_read_ + j) %
                bank_depth_img;
            // int cur_img_addr = k * pp_ + l /
            // stride_h_; img data order hwc:
            // pp_/src_w_/cg_*cp_
            int ori_img_addr =
                (l + n) * src_w_ * cg_ * icp_ + (i + m) * cg_ * icp_ + j * icp_;
            Util::SaveHexContSmallEndBankAddr(
                save_name_img,
                reinterpret_cast<const char*>(img_.data() + ori_img_addr), icp_,
                bank_width_img, bank_depth_img, bank_id_img, bank_addr_img,
                SM_APPEND);
            // img_weights[cur_img_addr] =
            // img_[ori_img_addr];
          }
          for (int r = 0; r < ocp_; r++) {  // input_channel
            // weights data order ohwi:
            // cp_/kernel_h_/kernel_w_/cg_*cp_

            int bank_id_weights = wgt_bank->GetID() + (r % oc_);
            int bank_addr_weights_base =
                bank_addr_weights_ + n * cg_ + m * kernel_h_ * cg_ + j;
            int bank_addr_weights = bank_addr_weights_base % bank_depth_wgt;

            int ori_weights_addr = r * kernel_h_ * kernel_w_ * cg_ * icp_ +
                                   n * kernel_w_ * cg_ * icp_ + m * cg_ * icp_ +
                                   j * icp_;
            Util::SaveHexContSmallEndBankAddr(
                save_name_weights,
                reinterpret_cast<const char*>(weights_.data() +
                                              ori_weights_addr),
                icp_, bank_width_wgt, bank_depth_wgt, bank_id_weights,
                bank_addr_weights, SM_APPEND);
          }
        }
      }
    }
  }
}

template <>
void Conv<DPUVersion::XVDPU>::debug_input_tick(string prefix) {
  if (!debug_) return;

  string save_name_img;
  string save_name_weights;
  string save_name_bias;

  // make save file name
  stringstream ss;
  ss.clear();
  ss.str("");
  ss << debug_path_ << "img_" << prefix << "_tick." << instid_;
  ss >> save_name_img;
  ss.clear();
  ss.str("");
  ss << debug_path_ << "weights_" << prefix << "_tick." << instid_;
  ss >> save_name_weights;
  ss.clear();
  ss.str("");
  ss << debug_path_ << "bias_" << prefix << "_tick." << instid_;
  ss >> save_name_bias;

  // save bias data in smallend mode
  auto bias_bank_group =
      ArchCfg::Instance().get_param().conv_engine().bias_bank();
  auto bias_bank =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bias_bank_group, 0);
  auto bank_id_bias = bias_bank->GetID();
  auto bank_depth_bias = bias_bank->GetH();
  auto bank_width_bias = bias_bank->GetW();
  Util::SaveHexContSmallEndBankAddr(
      save_name_bias, reinterpret_cast<const char*>(bias_.data()),
      bias_.size() * sizeof(DPU_DATA_TYPE),
      bank_width_bias * sizeof(DPU_DATA_TYPE), bank_depth_bias, bank_id_bias,
      bank_addr_bias_, SM_APPEND);
  // save img and weights data in smallend mode
  // img bank
  auto img_bank = Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_in_);
  auto bank_depth_img = img_bank->GetH();
  auto bank_width_img = img_bank->GetW();
  // weights bank
  auto wgt_bank_group =
      ArchCfg::Instance().get_param().conv_engine().weight_bank();
  auto wgt_bank = Buffer<DPU_DATA_TYPE>::Instance().GetBank(wgt_bank_group, 0);
  auto bank_depth_wgt = wgt_bank->GetH();
  auto bank_width_wgt = wgt_bank->GetW();
  // image data order iwhiw:
  // pp/ice/tile_iw/kernel_h/icp*tile_icg/ow_iter
  auto bank_id_base = (bank_id_in_ / pp_) * pp_;
  auto bank_id_base_offset = bank_id_in_ % pp_;
  auto owp{4};
  auto tile_ow = tile_owg_ * owp;
  auto real_w_offset = std::min(kernel_w_, stride_w_);
  for (auto idx_ow_iter = 0; idx_ow_iter < ow_iter_; idx_ow_iter++) {
    for (auto idx_cg = 0; idx_cg < cg_; idx_cg += 2) {
      for (auto idx_kh = 0; idx_kh < kernel_h_; idx_kh++) {
        for (auto idx_tile_ow = 0; idx_tile_ow < tile_ow; idx_tile_ow++) {
          auto max_tile_iw =
              (idx_tile_ow == tile_ow - 1) ? kernel_w_ : real_w_offset;
          for (auto idx_iw_offset = 0; idx_iw_offset < max_tile_iw;
               idx_iw_offset++) {
            for (auto idx_ice = 0U; idx_ice < 2; idx_ice++) {
              for (auto idx_ohp = 0; idx_ohp < dst_h_; idx_ohp++) {
                auto bank_offset =
                    bank_id_base_offset + idx_ohp +
                    (stride_offset_in_ + idx_kh) / instr_stride_h_;
                auto bank_id_img = bank_id_base + bank_offset % pp_;
                auto cycles = bank_offset / pp_;
                auto bank_addr_group_base = get_group_base_addr(cycles);
                auto idx_iw =
                    (idx_ow_iter * tile_ow + idx_tile_ow) * instr_stride_w_ +
                    idx_iw_offset;
                auto bank_addr_img =
                    (bank_addr_group_base +
                     ((stride_offset_in_ + idx_kh) % instr_stride_h_) *
                         jump_read_endl_ +
                     idx_iw * jump_read_ + (idx_cg + idx_ice)) %
                    bank_depth_img;

                auto idx_ori_iw =
                    (idx_ow_iter * tile_ow + idx_tile_ow) * stride_w_ +
                    idx_iw_offset;
                auto ori_img_addr =
                    (idx_ohp * stride_h_ + idx_kh) * src_w_ * cg_ * icp_ +
                    idx_ori_iw * cg_ * icp_ + (idx_cg + idx_ice) * icp_;

                Util::SaveHexContSmallEndBankAddr(
                    save_name_img,
                    reinterpret_cast<const char*>(img_.data() + ori_img_addr),
                    icp_, bank_width_img, bank_depth_img, bank_id_img,
                    bank_addr_img, SM_APPEND);
              }
            }
          }
        }
      }
    }
  }
  // weights data order oiwhio:
  // ocp/ice/kernel_w/kernel_h/icp*tile_icg/tile_ocg
  auto real_channel_group = ic_iter_ * tile_icg_ * 2 - icg_offset_;
  for (auto idx_ow_iter = 0; idx_ow_iter < ow_iter_; idx_ow_iter++)
    for (auto idx_ocg = 0; idx_ocg < tile_ocg_; idx_ocg++) {
      for (int j = 0; j < cg_; j += 2) {
        for (int n = 0; n < kernel_h_; n++) {
          for (int m = 0; m < kernel_w_; m++) {
            for (auto idx_ice = 0U; idx_ice < 2; idx_ice++) {
              for (auto idx_ocp = 0; idx_ocp < ocp_;
                   idx_ocp++) {  // input_channel
                auto idx_oc = idx_ocg * ocp_ + idx_ocp;
                auto bank_id_weights =
                    wgt_bank->GetID() + (idx_oc % wgt_bank_num_);
                auto bank_addr_wgt_base =
                    bank_addr_weights_ +
                    (idx_oc / wgt_bank_num_) * jump_read_weights_;
                auto bank_addr_wgt_offset = n * kernel_w_ * real_channel_group +
                                            m * real_channel_group +
                                            (j + idx_ice);
                auto bank_addr_wgt =
                    (bank_addr_wgt_base + bank_addr_wgt_offset) %
                    bank_depth_wgt;

                auto ori_weights_addr =
                    idx_oc * kernel_h_ * kernel_w_ * cg_ * icp_ +
                    n * kernel_w_ * cg_ * icp_ + m * cg_ * icp_ +
                    (j + idx_ice) * icp_;
                Util::SaveHexContSmallEndBankAddr(
                    save_name_weights,
                    reinterpret_cast<const char*>(weights_.data() +
                                                  ori_weights_addr),
                    icp_, bank_width_wgt, bank_depth_wgt, bank_id_weights,
                    bank_addr_wgt, SM_APPEND);
              }
            }
          }
        }
      }
    }
}
template <>
void Conv<DPUVersion::XVDPU>::debug_output_tick(string save_name) {
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
    for (int j = 0; j < oc_; j++) {
      for (int k = 0; k < pp_; k++) {
        int addr_tmp = i * oc_ * pp_ + j * pp_ + k;
        int addr_ori = k * length_ * oc_ + i * oc_ + j;

        rlt_tmp[addr_tmp] = rlt_dtype_[addr_ori];
      }
    }
  }

  // save data using upper data order
  auto bank_depth =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_out_)->GetH();
  // auto *p = reinterpret_cast<const char
  // *>(rlt_tmp.data());
  auto* p = reinterpret_cast<const char*>(rlt_dtype_.data());
  // int line_size = cp_ * pp_ *
  // sizeof(DPU_DATA_TYPE);
  auto line_size = ocp_ * sizeof(DPU_DATA_TYPE);
  // auto bank_id_out_ = group_id_out_ * pp_;
  auto bank_id_out_base = bank_id_out_ - bank_id_out_ % pp_;
  // bank id of the first bank in this bank_group
  auto bank_id_out_offset = bank_id_out_ % pp_;
  auto ocg_num = tile_ocg_ - ocg_offset_;

  for (int idx_owg = 0; idx_owg < ow_iter_; idx_owg++) {
    int idx_w_base = idx_owg * 4;
    for (auto idx_ocg = 0; idx_ocg < ocg_num; idx_ocg++) {
      for (int idx_owp = 0; idx_owp < 4; idx_owp++) {
        int idx_w = idx_w_base + idx_owp;
        if (idx_w >= (length_ - ow_offset_)) break;
        for (auto idx_h = 0; idx_h < real_dst_h_; idx_h++) {
          auto bank_offset = static_cast<uint32_t>(
              floor((float)(idx_h + stride_offset_out_) / stride_out_));
          // how many times does it change the bank
          auto bank_group_offset = static_cast<uint32_t>(
              floor((float)(bank_id_out_offset + bank_offset) / pp_));
          // how many times does it change the bank group
          auto bank_id_this_row =
              bank_id_out_base + (bank_id_out_offset + bank_offset) % pp_;
          auto bank_addr_this_row =
              bank_addr_out_  // base_addr
              +
              bank_group_offset * stride_out_ * jump_write_endl_  // change bank
              + ((idx_h + stride_offset_out_) % stride_out_) *
                    jump_write_endl_  // change stride
                                      // offset
              ;
          auto bank_addr_this_pixel =
              (bank_addr_this_row + idx_w * jump_write_ + idx_ocg) % bank_depth;
          auto offset = (idx_w + idx_h * length_) * line_size + idx_ocg * ocp_;
          Util::SaveHexContSmallEndBankAddr(
              save_name, p + offset, line_size, line_size, bank_depth,
              bank_id_this_row, bank_addr_this_pixel, SM_APPEND);
        }
      }
    }
  }
}

template <DPUVersion T>
void Conv<T>::debug_output_fmap(int type, string save_name) {
  if (!InstBase::debug_) return;

  // make save file name
  stringstream ss;
  ss.clear();
  ss.str("");
  ss << InstBase::debug_path_ << save_name << "." << InstBase::instid_;
  ss >> save_name;

  // save img data
  vector<int64_t> rlt_s64(oc_ * dst_h_ * dst_w_);
  vector<DPU_DATA_TYPE> rlt_s8(oc_ * dst_h_ * dst_w_);
  if (type == 0) {
    Calc::HWC2CHW(dst_h_, dst_w_, oc_, rlt_s64_.data(), rlt_s64.data());
  } else {
    Calc::HWC2CHW(dst_h_, dst_w_, oc_, rlt_dtype_.data(), rlt_s8.data());
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
        Util::SaveDec(save_name, rlt_s8.data() + addr, dst_w_, dst_w_,
                      SM_APPEND);
      }
    }
  }
}

template <DPUVersion T>
void Conv<T>::debug_output_tick(string save_name) {
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
    for (int j = 0; j < ocp_; j++) {
      for (int k = 0; k < pp_; k++) {
        int addr_tmp = i * ocp_ * pp_ + j * pp_ + k;
        int addr_ori = k * length_ * ocp_ + i * ocp_ + j;

        rlt_tmp[addr_tmp] = rlt_dtype_[addr_ori];
      }
    }
  }

  // save data using upper data order
  auto bank_depth =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_out_)->GetH();
  // auto *p = reinterpret_cast<const char
  // *>(rlt_tmp.data());
  auto* p = reinterpret_cast<const char*>(rlt_dtype_.data());
  // int line_size = cp_ * pp_ *
  // sizeof(DPU_DATA_TYPE);
  auto line_size = ocp_ * sizeof(DPU_DATA_TYPE);
  // auto bank_id_out_ = group_id_out_ * pp_;
  auto bank_id_out_base = bank_id_out_ - bank_id_out_ % pp_;
  // bank id of the first bank in this bank_group
  auto bank_id_out_offset = bank_id_out_ % pp_;
  for (int idx_w = 0; idx_w < length_ - ow_offset_; idx_w++) {
    for (auto idx_h = 0; idx_h < real_dst_h_; idx_h++) {
      auto bank_offset = static_cast<uint32_t>(
          floor((float)(idx_h + stride_offset_out_) / stride_out_));
      // how many times does it change the bank
      auto bank_group_offset = static_cast<uint32_t>(
          floor((float)(bank_id_out_offset + bank_offset) / pp_));
      // how many times does it change the bank
      // group
      auto bank_id_this_row =
          bank_id_out_base + (bank_id_out_offset + bank_offset) % pp_;
      auto bank_addr_this_row =
          bank_addr_out_                                        // base_addr
          + bank_group_offset * stride_out_ * jump_write_endl_  // change bank
          + ((idx_h + stride_offset_out_) % stride_out_) *
                jump_write_endl_  // change stride
                                  // offset
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
void Conv<T>::debug_output_tick_cb(string save_name) {
  if (!debug_) return;

  // make save file name
  stringstream ss;
  ss.clear();
  ss.str("");
  ss << debug_path_ << save_name << "." << instid_;
  ss >> save_name;

  auto bank_depth =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_out_cb_)->GetH();
  auto* p = reinterpret_cast<const char*>(rlt_dtype_.data());
  auto line_size = oc_ * sizeof(DPU_DATA_TYPE);
  auto bank_id_out_base = bank_id_out_cb_ - bank_id_out_cb_ % pp_;
  auto bank_id_out_offset = bank_id_out_cb_ % pp_;
  for (int idx_w = 0; idx_w < length_; idx_w++) {
    for (auto idx_h = 0; idx_h < real_dst_h_; idx_h++) {
      // bank id
      auto bank_shift = (idx_h + stride_offset_out_cb_) / stride_out_cb_;
      auto addr_group = (bank_id_out_offset + bank_shift) / pp_;
      auto bank_id = bank_id_out_base + (bank_id_out_offset + bank_shift) % pp_;

      // bank address
      auto stride_offset = (idx_h + stride_offset_out_cb_) % stride_out_cb_;
      auto bank_addr =
          (bank_addr_out_cb_ +
           addr_group * stride_out_cb_ * jump_write_endl_cb_ +
           stride_offset * jump_write_endl_cb_ + idx_w * jump_write_cb_) %
          bank_depth;

      auto addr_offset = (idx_h * length_ + idx_w) * line_size;
      Util::SaveHexContSmallEndBankAddr(save_name, p + addr_offset, line_size,
                                        line_size, bank_depth, bank_id,
                                        bank_addr, SM_APPEND);
    }
  }
}

template <DPUVersion T>
void Conv<T>::debug_output_tick_hbm(string save_name) {
  if (!debug_) return;

  // make save file name
  stringstream ss;
  ss.clear();
  ss.str("");
  ss << debug_path_ << save_name << "." << instid_;
  ss >> save_name;

  auto bank_depth =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_out_hbm_)->GetH();
  auto* p = reinterpret_cast<const char*>(rlt_dtype_.data());
  auto line_size = oc_ * sizeof(DPU_DATA_TYPE);
  auto bank_id_out_base = bank_id_out_hbm_ - bank_id_out_hbm_ % pp_;
  auto bank_id_out_offset = bank_id_out_hbm_ % pp_;

  for (int idx_w = 0; idx_w < length_; idx_w++) {
    for (auto idx_h = 0; idx_h < real_dst_h_; idx_h++) {
      // bank id
      auto bank_shift = (idx_h + stride_offset_out_hbm_) / stride_out_hbm_;
      auto addr_group = (bank_id_out_offset + bank_shift) / pp_;
      auto bank_id = bank_id_out_base + (bank_id_out_offset + bank_shift) % pp_;

      // bank address
      auto stride_offset = (idx_h + stride_offset_out_hbm_) % stride_out_hbm_;
      auto bank_addr =
          (bank_addr_out_hbm_ +
           addr_group * stride_out_hbm_ * jump_write_endl_hbm_ +
           stride_offset * jump_write_endl_hbm_ + idx_w * jump_write_hbm_) %
          bank_depth;

      auto addr_offset = (idx_h * length_ + idx_w) * line_size;
      if (output_channel_offset_ < icp_) {
        Util::SaveHexContSmallEndBankAddr(
            save_name, p + addr_offset, line_size, icp_ * sizeof(DPU_DATA_TYPE),
            bank_depth, bank_id, bank_addr, SM_APPEND);
      } else {
        Util::SaveHexContSmallEndBankAddr(
            save_name, p + addr_offset,
            (line_size - icp_ * sizeof(DPU_DATA_TYPE)),
            icp_ * sizeof(DPU_DATA_TYPE), bank_depth, bank_id, bank_addr,
            SM_APPEND);
      }
    }
  }
}

template <DPUVersion T>
void Conv<T>::GenInstStr(int inst_fmt) {
  ac_ = Helper<T>::GetInstStr(inst_type_, inst_fmt, dpdon_, dpdby_, field_str_);
}

// dpuv4e partial instantiation
template <>
Conv<DPUVersion::DPUV4E>::Conv(int inst_type, int instid,
                               vector<string>& inst_str,
                               vector<uint32_t>& inst_val)
    : InstBase(inst_type, instid, inst_str, inst_val) {
  // debug
  debug_ = debug_ && SimCfg::Instance().get_debug_instr(SimCfg::DBG_INSTR_CONV);
  Helper<DPUVersion::DPUV4E>::InstInit(inst_type_, root_debug_path_, debug_,
                                       opcode_, debug_path_);
  using TABLE = TableInterface<DPUVersion::DPUV4E>::inst_table;
  // get conv instruction field value
  channel_group_ = field_val_[TABLE::CONV_FIELD_CHANNEL_GROUP];
  bank_addr_in_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_IN];
  pad_left_ = field_val_[TABLE::CONV_FIELD_PAD_LEFT];
  pad_top_ = field_val_[TABLE::CONV_FIELD_PAD_TOP];
  pad_right_ = field_val_[TABLE::CONV_FIELD_PAD_RIGHT];
  pad_bottom_ = field_val_[TABLE::CONV_FIELD_PAD_BOTTOM];
  bank_addr_out_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_OUT];
  channel_offset_ = field_val_[TABLE::CONV_FIELD_CHANNEL_OFFSET];
  bank_addr_weights_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_WEIGHTS];
  length_ = field_val_[TABLE::CONV_FIELD_LENGTH];
  bank_addr_bias_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_BIAS];
  bank_id_in_ = field_val_[TABLE::CONV_FIELD_BANK_ID_IN];
  bank_id_out_ = field_val_[TABLE::CONV_FIELD_BANK_ID_OUT];

  bank_addr_in_1_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_IN_1];
  bank_addr_in_2_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_IN_2];
  bank_addr_in_3_ = field_val_[TABLE::CONV_FIELD_BANK_ADDR_IN_3];

  // convinit value
  using CONVINIT = ConvInit<DPUVersion::DPUV4E>;
  kernel_w_ = CONVINIT::CUROBJ->get_kernel_w();
  kernel_h_ = CONVINIT::CUROBJ->get_kernel_h();
  shift_bias_ = CONVINIT::CUROBJ->get_shift_bias();
  shift_cut_ = CONVINIT::CUROBJ->get_shift_cut();
  act_type_ = CONVINIT::CUROBJ->get_act_type();
  stride_w_ = CONVINIT::CUROBJ->get_stride_w();
  stride_h_ = CONVINIT::CUROBJ->get_stride_h();
  stride_out_ = CONVINIT::CUROBJ->get_stride_out();
  jump_read_ = CONVINIT::CUROBJ->get_jump_read();
  jump_read_endl_ = CONVINIT::CUROBJ->get_jump_read_endl();
  jump_write_ = CONVINIT::CUROBJ->get_jump_write();
  jump_write_endl_ = CONVINIT::CUROBJ->get_jump_write_endl();
  stride_offset_in_ = CONVINIT::CUROBJ->get_stride_offset_in();
  stride_offset_out_ = CONVINIT::CUROBJ->get_stride_offset_out();
  valid_pixel_parallel_ = CONVINIT::CUROBJ->get_valid_pixel_parallel();

  // constraints from hardware implementation
  if (shift_bias_ < 32) {
    UNI_LOG_CHECK(shift_bias_ <= 20, SIM_PARAMETER_FAILED)
        << "invalid shift_bias";
  }
  // LSB of bank_id_out suggested virtual bank
  if (bank_id_out_ >= 32) {
    auto bank_group_name =
        ArchCfg::Instance().get_param().eltwise_engine().input_bank()[0];
    Buffer<DPU_DATA_TYPE>::Instance().virtual_bank_acquire(bank_group_name);
    bank_id_out_ = Buffer<DPU_DATA_TYPE>::Instance().virtual_bank_mapping(
        bank_id_out_, bank_group_name);
  }

  // self defined
  vpp_ = valid_pixel_parallel_;
  pp_ = ArchCfg::Instance().get_param().conv_engine().pixel_parallel();
  icp_ = ArchCfg::Instance().get_param().conv_engine().input_channel_parallel();
  ocp_ =
      ArchCfg::Instance().get_param().conv_engine().output_channel_parallel();
  cg_ = channel_group_;
  co_ = channel_offset_;
  ic_ = cg_ * icp_;  // input_channel is channel_group multiply channel_parallel
  oc_ = ocp_;        // output_channel is channel_parallel

  src_h_ = (pp_ - 1) * stride_h_ + kernel_h_;
  real_src_h_ = (vpp_ - 1) * stride_h_ + kernel_h_;
  dst_h_ = pp_;
  real_dst_h_ = vpp_;
  AIE_W_ = 14;
  wg_ = (length_ + AIE_W_ - 1) / AIE_W_;
  align_length_ = wg_ * AIE_W_;
  src_w_ = (length_ - 1) * stride_w_ + kernel_w_;
  align_src_w_ = (align_length_ - 1) * stride_w_ + kernel_w_;
  dst_w_ = length_;
  align_dst_w_ = align_length_;
  group_id_out_ = bank_id_out_ / pp_;

  // resize related buffer size
  img_.resize(src_h_ * align_src_w_ * ic_);
  weights_.resize(oc_ * kernel_h_ * kernel_w_ * ic_);
  bias_.resize(oc_);
  orig_weights_.resize(oc_ * kernel_h_ * kernel_w_ * ic_);
  orig_bias_.resize(oc_);
  rlt_s64_.resize(dst_h_ * align_dst_w_ * oc_);
  rlt_dtype_.resize(dst_h_ * align_dst_w_ * oc_);
}

template <>
void Conv<DPUVersion::DPUV4E>::wgt_shuffle_back(vector<DPU_DATA_TYPE>& weight) {
  // 8x16 data index pattern is as follow
  int pos[8][16] = {
      {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30},
      {1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31},
      {64, 66, 68, 70, 72, 74, 76, 78, 80, 82, 84, 86, 88, 90, 92, 94},
      {65, 67, 69, 71, 73, 75, 77, 79, 81, 83, 85, 87, 89, 91, 93, 95},
      {32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62},
      {33, 35, 37, 39, 41, 43, 45, 47, 49, 51, 53, 55, 57, 59, 61, 63},
      {96, 98, 100, 102, 104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124,
       126},
      {97, 99, 101, 103, 105, 107, 109, 111, 113, 115, 117, 119, 121, 123, 125,
       127}};

  vector<DPU_DATA_TYPE> temp(weight);
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 16; j++) {
      int index = pos[i][j];
      weight[i * 16 + j] = temp[index];
    }
  }
}

template <>
void Conv<DPUVersion::DPUV4E>::read() {
  // UNI_LOG_INFO << endl;
  // UNI_LOG_INFO << "read start..." << endl;

  // read img data from bank
  // UNI_LOG_INFO << "reading img..." << endl;

  UNI_LOG_CHECK(stride_offset_in_ < stride_h_, SIM_PARAMETER_FAILED)
      << "invalid stride_offset_in";
  auto stride_offset = stride_offset_in_;
  auto img_bank_id = bank_id_in_;
  auto bank_depth =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_in_)->GetH();
  auto base_bank_id = (bank_id_in_ / pp_) * pp_;
  auto addr_group = 0;
  for (auto idx_src_h = 0; idx_src_h < src_h_; idx_src_h++) {
    for (auto idx_src_w = 0; idx_src_w < align_src_w_; idx_src_w++) {
      auto ddr_addr = (idx_src_h * align_src_w_ + idx_src_w) * ic_;
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
      ArchCfg::Instance().get_param().conv_engine().weight_bank();
  auto bank_depth_wgt =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(wgt_bank_group, 0)->GetH();
  for (int i = 0; i < oc_; i++) {
    for (int j = 0; j < kernel_h_; j++) {
      for (int k = 0; k < kernel_w_; k++) {
        int ddr_addr =
            i * kernel_h_ * kernel_w_ * ic_ + j * kernel_w_ * ic_ + k * ic_;
        int bank_addr = (bank_addr_weights_ + j * kernel_w_ * cg_ + k * cg_) %
                        bank_depth_wgt;
        auto bank =
            Buffer<DPU_DATA_TYPE>::Instance().GetBank(wgt_bank_group, i);
        bank->Read(bank_addr, ic_, weights_.data() + ddr_addr);
      }
    }
  }

  // weight shuffle back
  for (int i = 0; i < oc_ / 8; i++) {
    for (int j = 0; j < kernel_h_; j++) {
      for (int k = 0; k < kernel_w_; k++) {
        for (int m = 0; m < ic_ / 64; m++) {
          for (int n = 0; n < 4; n++) {
            vector<DPU_DATA_TYPE> temp;
            for (int p = 0; p < 8; p++) {
              for (int q = 0; q < 2; q++) {
                for (int l = 0; l < 8; l++) {
                  int src = (i * 8 + p) * kernel_h_ * kernel_w_ * ic_ +
                            j * kernel_w_ * ic_ + k * ic_ + m * 64 + n * 8 +
                            q * 32 + l;
                  temp.push_back(weights_[src]);
                }
              }
            }
            wgt_shuffle_back(temp);
            for (int p = 0; p < 8; p++) {
              for (int q = 0; q < 16; q++) {
                int dest = (i * 8 + p) * kernel_h_ * kernel_w_ * ic_ +
                           j * kernel_w_ * ic_ + k * ic_ + m * 64 + n * 16 + q;
                orig_weights_[dest] = temp[p * 16 + q];
              }
            }
          }
        }
      }
    }
  }

  // read bias from bank
  // UNI_LOG_INFO << "reading bias..." << endl;
  {
    auto bias_bank_group =
        ArchCfg::Instance().get_param().conv_engine().bias_bank();
    auto bank = Buffer<DPU_DATA_TYPE>::Instance().GetBank(bias_bank_group, 0);
    bank->Read(bank_addr_bias_, oc_, bias_.data());
  }

  // bias shuffle back
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      for (int k = 0; k < 4; k++) {
        for (int m = 0; m < 8; m++) {
          int src = ((i * 2 + j) * 4 + k) * 8 + m;
          int dest = i * 64 + j * 8 + k * 16 + m;
          orig_bias_[dest] = bias_[src];
        }
      }
    }
  }
  // UNI_LOG_INFO << "read finish!" << endl;
}

template <>
void Conv<DPUVersion::DPUV4E>::pad() {
  // UNI_LOG_INFO << endl;
  // UNI_LOG_INFO << "pad start..." << endl;
  // padding left
  // UNI_LOG_INFO << "padding left..." << endl;
  for (int i = 0; i < src_h_; i++) {
    for (int j = 0; j < pad_left_ && j < align_src_w_; j++) {
      for (int k = 0; k < ic_; k++) {
        unsigned int addr = i * align_src_w_ * ic_ + j * ic_ + k;
        UNI_LOG_CHECK(addr < img_.size(), SIM_OUT_OF_RANGE)
            << addr << " >= " << img_.size();
        img_[addr] = 0;
      }
    }
  }

  // padding top
  // UNI_LOG_INFO << "padding top..." << endl;
  for (int i = 0; i < pad_top_ && i < src_h_; i++) {
    for (int j = 0; j < align_src_w_; j++) {
      for (int k = 0; k < ic_; k++) {
        unsigned int addr = i * align_src_w_ * ic_ + j * ic_ + k;
        UNI_LOG_CHECK(addr < img_.size(), SIM_OUT_OF_RANGE)
            << addr << " >= " << img_.size();
        img_[addr] = 0;
      }
    }
  }

  // padding right
  // UNI_LOG_INFO << "padding right..." << endl;
  for (int i = 0; i < src_h_; i++) {
    for (int j = src_w_ - kernel_w_ + pad_right_ + 1; j < src_w_; j++) {
      for (int k = 0; k < ic_; k++) {
        unsigned int addr = i * align_src_w_ * ic_ + j * ic_ + k;
        UNI_LOG_CHECK(addr < img_.size(), SIM_OUT_OF_RANGE)
            << addr << " >= " << img_.size();
        img_[addr] = 0;
      }
    }
  }

  // UNI_LOG_INFO << "padding for length alignment..." << endl;
  for (int i = 0; i < src_h_; i++) {
    for (int j = src_w_; j < align_src_w_; j++) {
      for (int k = 0; k < ic_; k++) {
        unsigned int addr = i * align_src_w_ * ic_ + j * ic_ + k;
        UNI_LOG_CHECK(addr < img_.size(), SIM_OUT_OF_RANGE)
            << addr << " >= " << img_.size();
        img_[addr] = 0;
      }
    }
  }

  // padding bottom, it's different from padding right
  // UNI_LOG_INFO << "padding bottom..." << endl;
  for (int i = real_src_h_ - kernel_h_ + pad_bottom_ + 1; i < src_h_; i++) {
    for (int j = 0; j < align_src_w_; j++) {
      for (int k = 0; k < ic_; k++) {
        unsigned int addr = i * align_src_w_ * ic_ + j * ic_ + k;
        UNI_LOG_CHECK(addr < img_.size(), SIM_OUT_OF_RANGE)
            << addr << " >= " << img_.size();
        img_[addr] = 0;
      }
    }
  }
  // UNI_LOG_INFO << "pad finish!" << endl;

  // handle channel offset
  for (int i = 0; i < real_src_h_; i++) {
    for (int j = 0; j < align_src_w_; j++) {
      int ddr_addr = i * align_src_w_ * ic_ + j * ic_ + ic_ - co_;
      for (int k = 0; k < co_; k++) {
        img_[ddr_addr + k] = 0;
      }
    }
  }
}

template <>
void Conv<DPUVersion::DPUV4E>::conv_one(int idx_src_h, int idx_src_w,
                                        int idx_oc) {
  int64_t result = 0;

  // calculate conv value
  for (int i = 0; i < kernel_h_; i++) {
    for (int j = 0; j < kernel_w_; j++) {
      for (int k = 0; k < ic_; k++) {
        int img_addr =
            (idx_src_h + i) * align_src_w_ * ic_ + (idx_src_w + j) * ic_ + k;
        int weights_addr = idx_oc * kernel_h_ * kernel_w_ * ic_ +
                           i * kernel_w_ * ic_ + j * ic_ + k;
        result += img_[img_addr] * orig_weights_[weights_addr];
      }
    }
  }

  // assign conv value to rlt_s64_ var
  int idx_dst_h = idx_src_h / stride_h_;
  int idx_dst_w = idx_src_w / stride_w_;
  int rlt_addr = idx_dst_h * align_dst_w_ * oc_ + idx_dst_w * oc_ + idx_oc;
  rlt_s64_[rlt_addr] = result;
}

template <>
void Conv<DPUVersion::DPUV4E>::conv() {
  // UNI_LOG_INFO << endl;
  // UNI_LOG_INFO << "conv start..." << endl;
  for (int i = 0; i + kernel_h_ <= src_h_; i += stride_h_) {
    for (int j = 0; j + kernel_w_ <= align_src_w_; j += stride_w_) {
      for (int k = 0; k < oc_; k++) {
        conv_one(i, j, k);
      }
    }
  }

  // UNI_LOG_INFO << "conv finish!" << endl;
}

template <>
void Conv<DPUVersion::DPUV4E>::bias() {
  shift_bias_ = (shift_bias_ >= 32) ? (32 - shift_bias_) : shift_bias_;
  for (int i = 0; i < dst_h_; i++) {
    for (int j = 0; j < align_dst_w_; j++) {
      for (int k = 0; k < oc_; k++) {
        int rlt_addr = i * align_dst_w_ * oc_ + j * oc_ + k;
        rlt_s64_[rlt_addr] *= 2;
        rlt_s64_[rlt_addr] +=
            floor((double)orig_bias_[k] * 2.0 * pow(2, shift_bias_));
      }
    }
  }
}

template <>
void Conv<DPUVersion::DPUV4E>::transform() {
  // UNI_LOG_INFO << endl;
  // UNI_LOG_INFO << "transform start..." << endl;
  for (int i = 0; i < dst_h_; i++) {
    for (int j = 0; j < align_dst_w_; j++) {
      for (int k = 0; k < oc_; k++) {
        int addr = i * align_dst_w_ * oc_ + j * oc_ + k;

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
void Conv<DPUVersion::DPUV4E>::save() {
  // UNI_LOG_INFO << endl;
  // UNI_LOG_INFO << "conv save start..." << endl;
  auto bank_id_out_base = bank_id_out_ - bank_id_out_ % pp_;
  // bank id of the first bank in this bank_group
  auto bank_id_out_offset = bank_id_out_ % pp_;
  for (int idx_w = 0; idx_w < dst_w_; idx_w++) {
    for (auto idx_h = 0; idx_h < real_dst_h_; idx_h++) {
      auto bank_offset = static_cast<uint32_t>(
          floor((float)(idx_h + stride_offset_out_) / stride_out_));
      // how many times does it change the bank
      auto bank_group_offset = static_cast<uint32_t>(
          floor((float)(bank_id_out_offset + bank_offset) / pp_));
      // how many times does it change the bank group
      auto bank_id_this_row =
          bank_id_out_base + (bank_id_out_offset + bank_offset) % pp_;
      auto bank_addr_this_row =
          bank_addr_out_                                        // base_addr
          + bank_group_offset * stride_out_ * jump_write_endl_  // change bank
          + ((idx_h + stride_offset_out_) % stride_out_) *
                jump_write_endl_  // change stride offset
          ;
      auto bank = Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_this_row);

      auto bank_addr_this_pixel =
          (bank_addr_this_row + idx_w * jump_write_) %
          Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id_this_row)->GetH();
      auto rlt_addr_this_pixel = idx_h * align_dst_w_ * oc_ + idx_w * oc_;
      bank->Write(bank_addr_this_pixel, oc_,
                  rlt_dtype_.data() + rlt_addr_this_pixel);
    }
  }
  // UNI_LOG_INFO << "conv save finish!" << endl;
}

template <>
void Conv<DPUVersion::DPUV4E>::debug_input_fmap(string prefix) {
  if (!debug_) return;

  string head;
  string save_name_img;
  string save_name_weights;
  string save_name_bias;

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

  // save img data
  vector<DPU_DATA_TYPE> img(ic_ * src_h_ * align_src_w_);
  Calc::HWC2CHW(src_h_, align_src_w_, ic_, img_.data(), img.data());
  for (int i = 0; i < ic_; i++) {
    // save input_channel title to file
    ss.clear();
    ss.str("");
    ss << "channel_" << i << ":";
    ss >> head;
    head += "\n";
    Util::SaveBin(save_name_img, head.data(), head.size(), SM_APPEND);
    for (int j = 0; j < src_h_; j++) {
      int addr = i * src_h_ * align_src_w_ + j * align_src_w_;
      Util::SaveDec(save_name_img, img.data() + addr, align_src_w_,
                    align_src_w_, SM_APPEND);
    }
  }

  // save weights data
  for (int i = 0; i < oc_; i++) {
    // save output_channel title to file
    ss.clear();
    ss.str("");
    ss << "output_channel_" << i << "-------------------------------------";
    ss >> head;
    head += "\n";
    Util::SaveBin(save_name_weights, head.data(), head.size(), SM_APPEND);

    vector<DPU_DATA_TYPE> weights(ic_ * kernel_h_ * kernel_w_);
    Calc::HWC2CHW(kernel_h_, kernel_w_, ic_,
                  orig_weights_.data() + i * ic_ * kernel_h_ * kernel_w_,
                  weights.data());
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
    Util::SaveDec(save_name_bias, orig_bias_.data() + i, 1, 1, SM_APPEND);
  }
}

// calculation order:
// for an iteration (a block): C(64) -> H(2) -> W(14) -> Hg
// outer iteration: Cg -> Kh -> Kw -> Wg
template <>
void Conv<DPUVersion::DPUV4E>::debug_read_pos_tick() {
  if (!debug_) return;

  string save_name;
  stringstream ss;
  ss.clear();
  ss.str("");
  ss << debug_path_ << "conv_read_pos_tick." << instid_;
  ss >> save_name;
  ofstream fout(save_name);

  auto bank_id_group_base = (bank_id_in_ / pp_) * pp_;
  auto bank_id_offset = bank_id_in_ - bank_id_group_base;

  for (auto idx_w = 0; idx_w < wg_; idx_w++) {
    for (auto idx_kernel_w = 0; idx_kernel_w < kernel_w_; idx_kernel_w++) {
      for (auto idx_kernel_h = 0; idx_kernel_h < kernel_h_; idx_kernel_h++) {
        for (auto idx_channel_group = 0; idx_channel_group < cg_;
             idx_channel_group++) {
          for (auto idx_hg = 0; idx_hg < pp_ / 2; idx_hg++) {
            for (auto idx_length = 0; idx_length < AIE_W_; idx_length++) {
              // bank_id
              auto stride_offset =
                  (stride_offset_in_ + idx_kernel_h) % stride_h_;
              auto bank_shift = (stride_offset_in_ + idx_kernel_h) / stride_h_;
              auto bank_id_start = (bank_id_offset + bank_shift) % pp_;
              auto addr_group_base = (bank_id_offset + bank_shift) / pp_;

              for (auto idx_h = 0; idx_h < 2; idx_h++) {
                int idx_pp = idx_hg * 2 + idx_h;
                auto bank_id =
                    bank_id_group_base + (bank_id_start + idx_pp) % pp_;
                auto addr_group =
                    addr_group_base + (bank_id_start + idx_pp) / pp_;

                // bank address
                auto bank_addr =
                    (get_group_base_addr(addr_group) +
                     (idx_length + idx_w * AIE_W_) * stride_w_ * jump_read_ +
                     stride_offset * jump_read_endl_ +
                     idx_kernel_w * jump_read_ + idx_channel_group) %
                    Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id)->GetH();

                fout << setw(2) << setfill('0') << bank_id << "-" << setw(4)
                     << setfill('0') << bank_addr << " ";
              }
              fout << endl;
            }
          }
        }
      }
    }
  }
}

template <>
void Conv<DPUVersion::DPUV4E>::debug_input_tick(string prefix) {
  if (!debug_) return;

  string save_name_img;
  string save_name_weights;
  string save_name_bias;

  // make save file name
  stringstream ss;
  ss.clear();
  ss.str("");
  ss << debug_path_ << "img_" << prefix << "_tick." << instid_;
  ss >> save_name_img;
  ss.clear();
  ss.str("");
  ss << debug_path_ << "weights_" << prefix << "_tick." << instid_;
  ss >> save_name_weights;
  ss.clear();
  ss.str("");
  ss << debug_path_ << "bias_" << prefix << "_tick." << instid_;
  ss >> save_name_bias;

  // save bias data in smallend mode
  auto bias_bank_group =
      ArchCfg::Instance().get_param().conv_engine().bias_bank();
  auto bias_bank =
      Buffer<DPU_DATA_TYPE>::Instance().GetBank(bias_bank_group, 0);
  auto bank_id_bias = bias_bank->GetID();
  auto bank_depth_bias = bias_bank->GetH();
  auto bank_width_bias = bias_bank->GetW();
  Util::SaveHexContSmallEndBankAddr(
      save_name_bias, reinterpret_cast<const char*>(bias_.data()),
      bias_.size() * sizeof(DPU_DATA_TYPE),
      bank_width_bias * sizeof(DPU_DATA_TYPE), bank_depth_bias, bank_id_bias,
      bank_addr_bias_, SM_APPEND);

  /* save img and weights data in smallend mode
   * for an iteration (a block): C(64) -> H(2) -> W(14) -> Hg
   * outer iteration: Cg -> Kh -> Kw -> Wg
   */
  auto bank_id_group_base = (bank_id_in_ / pp_) * pp_;
  auto bank_id_offset = bank_id_in_ - bank_id_group_base;
  for (auto idx_w = 0; idx_w < wg_; idx_w++) {
    for (auto idx_kernel_w = 0; idx_kernel_w < kernel_w_; idx_kernel_w++) {
      for (auto idx_kernel_h = 0; idx_kernel_h < kernel_h_; idx_kernel_h++) {
        for (auto idx_channel_group = 0; idx_channel_group < cg_;
             idx_channel_group++) {
          // feature map
          for (auto idx_hg = 0; idx_hg < pp_ / 2; idx_hg++) {
            for (auto idx_length = 0; idx_length < AIE_W_; idx_length++) {
              // bank_id
              auto stride_offset =
                  (stride_offset_in_ + idx_kernel_h) % stride_h_;
              auto bank_shift = (stride_offset_in_ + idx_kernel_h) / stride_h_;
              auto bank_id_start = (bank_id_offset + bank_shift) % pp_;
              auto addr_group_base = (bank_id_offset + bank_shift) / pp_;

              for (auto idx_h = 0; idx_h < 2; idx_h++) {
                int idx_pp = idx_hg * 2 + idx_h;
                auto bank_id =
                    bank_id_group_base + (bank_id_start + idx_pp) % pp_;
                auto addr_group =
                    addr_group_base + (bank_id_start + idx_pp) / pp_;

                // bank address
                auto bank_addr =
                    (get_group_base_addr(addr_group) +
                     (idx_length + idx_w * AIE_W_) * stride_w_ * jump_read_ +
                     stride_offset * jump_read_endl_ +
                     idx_kernel_w * jump_read_ + idx_channel_group) %
                    Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id)->GetH();

                int ori_img_addr =
                    ((idx_hg * 2 + idx_h) * stride_h_ + idx_kernel_h) *
                        align_src_w_ * ic_ +
                    ((idx_w * AIE_W_ + idx_length) * stride_w_ + idx_kernel_w) *
                        ic_ +
                    idx_channel_group * icp_;

                Util::SaveHexContSmallEndBankAddr(
                    save_name_img,
                    reinterpret_cast<const char*>(img_.data() + ori_img_addr),
                    icp_, icp_,
                    Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id)->GetH(),
                    bank_id, bank_addr, SM_APPEND);
              }
            }
          }

          // weight
          auto wgt_bank_group =
              ArchCfg::Instance().get_param().conv_engine().weight_bank();
          auto bank_depth_wgt = Buffer<DPU_DATA_TYPE>::Instance()
                                    .GetBank(wgt_bank_group, 0)
                                    ->GetH();
          for (int p = 0; p < 2; p++) {
            for (int r = 0; r < oc_ / 8; r++) {
              for (int k = 0; k < 4; k++) {
                int bank_offset = p * 64 + k * 16 + r;
                // int bank_id =
                //     Buffer<DPU_DATA_TYPE>::Instance().GetBank(SimCfg::BANK_TYPE_WEIGHTS,
                //     0)->GetID() + bank_offset;
                int bank_addr =
                    (bank_addr_weights_ + idx_kernel_h * kernel_w_ * cg_ +
                     idx_kernel_w * cg_ + idx_channel_group) %
                    bank_depth_wgt;

                int ori_weights_addr =
                    bank_offset * kernel_h_ * kernel_w_ * ic_ +
                    idx_kernel_h * kernel_w_ * ic_ + idx_kernel_w * ic_ +
                    idx_channel_group * icp_;
                Util::SaveHexContSmallEndBankAddr(
                    save_name_weights,
                    reinterpret_cast<const char*>(weights_.data() +
                                                  ori_weights_addr),
                    icp_, icp_, bank_depth_wgt, bank_offset, bank_addr,
                    SM_APPEND);
              }
            }
          }
        }
      }
    }
  }
}

template <>
void Conv<DPUVersion::DPUV4E>::debug_output_fmap(int type, string save_name) {
  if (!debug_) return;

  // make save file name
  stringstream ss;
  ss.clear();
  ss.str("");
  ss << debug_path_ << save_name << "." << instid_;
  ss >> save_name;

  // save img data
  vector<int64_t> rlt_s64(oc_ * dst_h_ * align_dst_w_);
  vector<DPU_DATA_TYPE> rlt_s8(oc_ * dst_h_ * align_dst_w_);
  if (type == 0) {
    Calc::HWC2CHW(dst_h_, align_dst_w_, oc_, rlt_s64_.data(), rlt_s64.data());
  } else {
    Calc::HWC2CHW(dst_h_, align_dst_w_, oc_, rlt_dtype_.data(), rlt_s8.data());
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
      int addr = i * dst_h_ * align_dst_w_ + j * align_dst_w_;
      if (type == 0) {
        Util::SaveDec(save_name, rlt_s64.data() + addr, align_dst_w_,
                      align_dst_w_, SM_APPEND);
      } else {
        Util::SaveDec(save_name, rlt_s8.data() + addr, align_dst_w_,
                      align_dst_w_, SM_APPEND);
      }
    }
  }
}

template <>
void Conv<DPUVersion::DPUV4E>::debug_output_tick(string save_name) {
  if (!debug_) return;

  // make save file name
  stringstream ss;
  ss.clear();
  ss.str("");
  ss << debug_path_ << save_name << "." << instid_;
  ss >> save_name;

  auto* p = reinterpret_cast<const char*>(rlt_dtype_.data());
  auto line_size = oc_ * sizeof(DPU_DATA_TYPE);
  auto bank_id_out_base = bank_id_out_ - bank_id_out_ % pp_;
  auto bank_id_out_offset = bank_id_out_ % pp_;

  for (auto idx_w = 0; idx_w < align_length_; idx_w++) {
    for (auto idx_h = 0; idx_h < dst_h_; idx_h++) {
      // bank id
      auto bank_shift = (idx_h + stride_offset_out_) / stride_out_;
      auto addr_group = (bank_id_out_offset + bank_shift) / pp_;
      auto bank_id = bank_id_out_base + (bank_id_out_offset + bank_shift) % pp_;

      // bank address
      auto stride_offset = (idx_h + stride_offset_out_) % stride_out_;
      auto bank_addr =
          (bank_addr_out_ + addr_group * stride_out_ * jump_write_endl_ +
           stride_offset * jump_write_endl_ + (idx_w * jump_write_)) %
          Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id)->GetH();

      auto addr_offset = (idx_h * align_length_ + idx_w) * line_size;
      Util::SaveHexContSmallEndBankAddr(
          save_name, p + addr_offset, line_size, line_size,
          Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id)->GetH(), bank_id,
          bank_addr, SM_APPEND);
    }
  }
}

template <DPUVersion dv>
void Conv<dv>::gen_aie_data() {}

template <>
void Conv<DPUVersion::DPUV4E>::gen_aie_data() {
  if (!debug_) return;

  for (int owi = 0; owi < wg_; owi++) {
    int start_ow = owi * AIE_W_;
    for (int khi = 0; khi < kernel_h_; khi++) {
      for (int kwi = 0; kwi < kernel_w_; kwi++) {
        for (int ici = 0; ici < cg_; ici++) {
          int start_ic = ici * icp_;

          // tile the input files
          string img_path = debug_path_ + "image_" + std::to_string(instid_);
          Util::ChkFolder(img_path);

          for (int i = 0; i < 4; i++) {    // column
            for (int j = 0; j < 2; j++) {  // row
              string file = img_path + "/column" + to_string(i) + "_in" +
                            to_string(j) + ".txt";
              ofstream ofd;
              ofd.open(file, std::ios::app);

              for (int k = 0; k < 4; k++) {  // height
                int ih = khi + (k * 2 + j) * stride_h_;
                for (int m = 0; m < AIE_W_; m++) {  // width
                  int iw = start_ow * stride_w_ + kwi + m * stride_w_;
                  for (int n = 0; n < 16; n++) {  // input channel
                    int addr =
                        (ih * align_src_w_ + iw) * ic_ + start_ic + i * 16 + n;
                    ofd << setw(5) << (int)img_[addr];
                    if (n == 15) ofd << "\n";
                  }
                }
              }
              ofd.close();
            }
          }

          // tile the weights
          string wgt_path = debug_path_ + "weight_" + std::to_string(instid_);
          Util::ChkFolder(wgt_path);

          for (int i = 0; i < 4; i++) {    // column
            for (int j = 0; j < 4; j++) {  // row
              string file = wgt_path + "/column" + to_string(i) + "_weight" +
                            to_string(j) + ".txt";
              ofstream ofd;
              ofd.open(file, std::ios::app);

              vector<DPU_DATA_TYPE> temp;
              for (int r = 0; r < 2; r++) {     // kernel group in time locality
                for (int n = 0; n < 16; n++) {  // kernels
                  int kernel_id = j * 16 + r * 64 + n;

                  for (int t = 0; t < 2; t++) {
                    for (int k = 0; k < 8; k++) {
                      int addr =
                          ((kernel_id * kernel_h_ + khi) * kernel_w_ + kwi) *
                              ic_ +
                          start_ic + i * 8 + t * 32 + k;
                      temp.push_back(weights_[addr]);
                    }
                  }
                }
              }
              for (unsigned ctr = 0; ctr < temp.size(); ctr++) {
                ofd << setw(5) << (int)temp[ctr];
                if ((ctr % 8) == 7) ofd << "\n";
              }
              ofd.close();
            }
          }
        }
      }
    }

    // tile the bias
    string bias_path = debug_path_ + "bias_" + std::to_string(instid_);
    Util::ChkFolder(bias_path);

    for (int i = 0; i < 4; i++) {  // column
      string file = bias_path + "/cluster" + to_string(i) + "_param.txt";
      ofstream ofd;
      ofd.open(file, std::ios::app);

      // 16x4byte parameter header, layout:
      // word0: iteration number
      // word1: shift cut
      // word2: act_type
      // word2-7: reserved
      // word8-11: 0x00010001
      // word12-15: reserved
      int iter_num = kernel_h_ * kernel_w_ * cg_;
      int shift_cut = shift_cut_;
      int act_type = act_type_;
      ofd << setw(6) << iter_num;
      ofd << setw(6) << shift_cut;
      ofd << "\n";
      ofd << setw(6) << act_type;
      ofd << setw(6) << 0;
      ofd << "\n";
      for (int m = 0; m < 4; m++) {
        ofd << setw(6) << 0;
        if (m % 2) ofd << "\n";
      }
      for (int m = 0; m < 4; m++) {
        ofd << setw(6) << 0x00010001;
        if (m % 2) ofd << "\n";
      }
      for (int m = 0; m < 4; m++) {
        ofd << setw(6) << 0;
        if (m % 2) ofd << "\n";
      }

      int addr = i * 8;
      for (int t = 0; t < 4; t++) {
        for (int n = 0; n < 8; n++) {
          int value = floor((double)bias_[addr + t * 32 + n] * 2.0 *
                            pow(2, shift_bias_));
          double tmp = (double)value / 2.0;
          int rlt;
          if (shift_cut == 0) {
            if ((tmp < 0) && ((tmp - floor(tmp)) == 0.5))
              rlt = (int)ceil(tmp);
            else
              rlt = (int)round(tmp);
          } else {
            rlt = floor(tmp);
          }
          ofd << setw(6) << rlt;
          if (n % 2) ofd << "\n";
        }
      }
      ofd.close();
    }

    // tile the output
    string out_path = debug_path_ + "output_" + std::to_string(instid_);
    Util::ChkFolder(out_path);

    for (int i = 0; i < 4; i++) {    // cluster
      for (int j = 0; j < 2; j++) {  // height group
        string file = out_path + "/cluster" + to_string(i) + "_out" +
                      to_string(j) + ".txt";
        ofstream ofd;
        ofd.open(file, std::ios::app);

        for (int m = 0; m < AIE_W_; m++) {  // width
          int ow = start_ow + m;
          for (int k = 0; k < 4; k++) {  // height
            int oh = k * 2 + j;
            for (int s = 0; s < 2; s++) {
              for (int n = 0; n < 16; n++) {  // out channel
                int addr = (oh * align_dst_w_ + ow) * oc_ + i * 16 + s * 64 + n;
                ofd << setw(5) << (int)rlt_dtype_[addr];
                if ((n % 16) == 15) ofd << "\n";
              }
            }
          }
        }
        ofd.close();
      }
    }
  }
}
