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
#ifndef __DPT_WISE_HPP__
#define __DPT_WISE_HPP__

#include "pub/InstBase.hpp"

template <DPUVersion dv>
class DptWise : public InstBase {
 public:
  explicit DptWise(int inst_type, int instid, vector<string>& inst_str,
                   vector<uint32_t>& inst_val);
  ~DptWise();

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
  // do dwconv operation of the whole feature map
  void dw_conv();
  // do one dwconv kernel's dwconv operation
  void dw_conv_one(int idx_src_h, int idx_src_w, int idx_oc);
  // add bias to dwconv result
  void bias();
  // do shift, trunc operation
  void transform();
  // save result
  void save();

  // debug funcs
  // for debug
  void debug_var();
  // debug img/weights/bias input value
  void debug_input_fmap(string prefix);
  void debug_read_pos_tick();
  void debug_input_tick();
  void debug_input_tick(const string& prefix);
  // if type is 1, debug rlt_dtype_
  // if type is 0, debug rlt_s64_
  void debug_output_fmap(int type, string save_name);
  void debug_output_tick(string save_name);

 private:
  // dwconv instruction fields
  int channel_group_;
  int bank_addr_in_;
  int pad_left_;
  int pad_right_;
  int pad_top_;
  int pad_bottom_;
  int channel_offset_;
  int bank_id_in_;
  int bank_id_out_;
  int bank_addr_out_;
  int bank_addr_weights_;
  int length_;
  int bank_addr_bias_;
  int bank_addr_in_1_;
  int bank_addr_in_2_;
  int bank_addr_in_3_;
  int act_type_;

  // others
  // hardware config value
  int pp_;
  int pp_dwconv_;
  int cp_;

  // dwconvinit value
  int jump_read_;
  int jump_write_;
  int jump_read_endl_;
  int stride_offset_in_;
  int valid_pixel_parallel_;
  int stride_w_;
  int stride_h_;
  int kernel_w_;
  int kernel_h_;
  int jump_write_endl_;
  int stride_offset_out_;
  int stride_out_;
  int shift_bias_;
  int shift_cut_;

  int shift_relusix_;

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
  int group_id_in_;
  int group_id_out_;

  int scale_;  // output_channel/input_channel

  // img, weights, bias
  vector<DPU_DATA_TYPE> img_;             // src_h_ * src_w_ * ic_
  vector<DPU_DATA_TYPE> weights_;         // oc_ * kernel_h_ * kernel_w_
  vector<DPU_DATA_TYPE> bias_;            // oc_
  vector<DPU_DATA_TYPE> shift_bias_buf_;  // oc_
  vector<DPU_DATA_TYPE> shift_cut_buf_;   // oc_
  vector<int64_t> rlt_s64_;               // dst_h_ * dst_w_ * oc_
  vector<DPU_DATA_TYPE> rlt_dtype_;       // dst_h_ * dst_w_ * oc_
};

template <>
void DptWise<DPUVersion::DPU4F>::Exec();
template <>
void DptWise<DPUVersion::DPU4F>::read();
template <>
void DptWise<DPUVersion::DPU4F>::bias();
template <>
void DptWise<DPUVersion::DPU4F>::transform();
template <>
void DptWise<DPUVersion::DPU4F>::save();
template <>
void DptWise<DPUVersion::DPU4F>::debug_input_fmap(string prefix);
template <>
void DptWise<DPUVersion::XVDPU>::Exec();
template <>
void DptWise<DPUVersion::XVDPU>::bias();
template <>
void DptWise<DPUVersion::XVDPU>::debug_input_tick(const string& prefix);
#endif /* __DPT_WISE_HPP__ */
