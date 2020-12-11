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
#ifndef __CONV_HPP__
#define __CONV_HPP__

#include "pub/InstBase.hpp"

struct unsigned_data {
  unsigned lo : 4, hi : 4;
};

struct signed_data {
  signed lo : 4, hi : 4;
};

struct trans_wgt {
  DPU_DATA_TYPE real_wgt : 7;
};

struct trans_shift {
  uint8_t real_shift : 5;
};

template <DPUVersion dv>
class Conv : public InstBase {
 public:
  explicit Conv(int inst_type, int inst_id, vector<string>& inst_str,
                vector<uint32_t>& inst_val);
  ~Conv();

 public:
  // excute load instruction
  void Exec() override final;
  void GenInstStr(int inst_fmt) override final;

 private:
  // read img, weights, bias data from bank
  void read();
  int get_group_base_addr(int addr_group);
  // padding for img data
  void pad();
  // do conv operation of the whole feature map
  void conv();
  // do one conv kernel's conv operation
  void conv_one(int idx_src_h, int idx_src_w, int idx_oc);
  // add bias to conv result
  void bias();
  // do shift, trunc operation
  void transform();
  // save result
  void save();
  // save result
  void save_to_cb();
  void save_to_hbm();

 private:
  void wgt_shuffle_back(vector<DPU_DATA_TYPE>& weight);
  void gen_aie_data();

  // debug funcs
  // for debug
  void debug_var();
  // debug img/weights/bias input value
  void debug_input_fmap(string prefix);
  void debug_read_pos_tick();
  void debug_input_tick(string prefix);
  // if type is 0, debug rlt_s8_
  // if type is 1, debug rlt_s64_
  void debug_output_fmap(int type, string save_name);
  void debug_output_tick(string save_name);
  void debug_output_tick_cb(string save_name);
  void debug_output_tick_hbm(string save_name);

 private:
  // conv instruction fields
  int channel_group_;
  int bank_addr_in_;
  int pad_left_;
  int pad_top_;
  int pad_right_;
  int pad_bottom_;
  int bank_addr_out_;
  int channel_offset_;
  int bank_id_in_;
  int bank_id_out_;
  int group_id_out_;
  int bank_addr_weights_;
  int jump_read_endl_;
  int length_;
  int bank_addr_bias_;
  int jump_read_;
  int bank_addr_in_1_;
  int bank_addr_in_2_;
  int bank_addr_in_3_;
  int ocg_offset_{0};

  int output_channel_offset_;
  int bank_id_out_cb_;
  int bank_addr_out_cb_;
  int dest_mode_;
  int bank_id_out_hbm_;
  int bank_addr_out_hbm_;

  // others
  // hardware config value
  int pp_;
  int icp_;
  int ocp_;

  // convinit value
  int kernel_w_;
  int kernel_h_;
  int shift_bias_;
  int shift_cut_;
  int act_type_;
  int stride_w_;
  int stride_h_;
  int valid_pixel_parallel_;
  int jump_write_;
  int stride_offset_in_;
  int stride_out_;
  int stride_offset_out_;
  int jump_write_endl_;

  int jump_write_cb_;
  int stride_offset_out_cb_;
  int stride_out_cb_;
  int shift_relusix_;
  int jump_write_endl_cb_;
  int jump_write_hbm_;
  int jump_write_endl_hbm_;
  int stride_out_hbm_;
  int stride_offset_out_hbm_;

  int conv_num_;
  int tile_icg_;
  int ic_iter_;
  int icg_offset_;
  int tile_ocg_;
  int tile_owg_;
  int ow_iter_;
  int ow_offset_{0};
  int jump_read_weights_;

  int quant_lth_;
  int calt_mode_;

  // self defined
  int vpp_;
  int cg_;
  int co_;
  int ic_;  // input_channel is channel_parallel
  int oc_;  // output_channel is channel_parallel
  int src_h_;
  int real_src_h_;
  int src_w_;
  int dst_h_;
  int real_dst_h_;
  int dst_w_;
  int src_size_;

  int align_src_w_;
  int align_dst_w_;
  int AIE_W_;
  int align_length_;
  int wg_;

  int32_t wgt_bank_num_;

  // img, weights, bias
  vector<DPU_DATA_TYPE> img_;           // src_h_ * src_w_ * ic_
  vector<DPU_DATA_TYPE> weights_;       // oc_ * kernel_h_ * kernel_w_ * ic_
  vector<DPU_DATA_TYPE> bias_;          // oc_
  vector<int64_t> rlt_s64_;             // dst_h_ * dst_w_ * oc_
  vector<DPU_DATA_TYPE> rlt_dtype_;     // dst_h_ * dst_w_ * oc_
  vector<DPU_DATA_TYPE> orig_weights_;  // oc_ * kernel_h_ * kernel_w_ * ic_
  vector<DPU_DATA_TYPE> orig_bias_;     // oc_

 private:
  void xvdpu_read();
  void xvdpu_conv();
  void xvdpu_transform();
  void xvdpu_genWLut(int kernel_w, int kernel_h, int stride_w, int cut_icg,
                     int iw, int32_t* wlut);
  string xvdpu_getWgtfname(int g, int l);
  int32_t xvdpu_getExecType(unsigned int coreidx, int idx_iter, int iterNum,
                            int inner_count, int out_count);
  void xvdpu_trasWgtOri2ME(const int row, const int col,
                           const DPU_DATA_TYPE* mat, DPU_DATA_TYPE* mat_out);
  void xvdpu_pad();

  int32_t instr_stride_h_{0}, instr_stride_w_{0};
  vector<DPU_DATA_TYPE> xvdpu_sum_dtype_;  // dst_h_ * dst_w_ * oc_
  string xvdpu_data_dir_;
  int xvdpu_num_wgt_port_;
  int xvdpu_OCpg_;
  bool xvdpu_needParamPkts_;
};

template <>
void Conv<DPUVersion::XVDPU>::debug_input_tick(string prefix);
template <>
void Conv<DPUVersion::XVDPU>::debug_output_tick(string prefix);
template <>
void Conv<DPUVersion::XVDPU>::bias();
#endif /* __CONV_HPP__ */
