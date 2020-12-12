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
#ifndef __POOL_HPP__
#define __POOL_HPP__

#include "pub/InstBase.hpp"

template <DPUVersion dv>
class Pool : public InstBase {
 public:
  const static DPU_DATA_TYPE MIN_DTYPE_VALUE;

 public:
  explicit Pool(int inst_type, int instid, vector<string>& inst_str,
                vector<uint32_t>& inst_val);
  ~Pool();

 public:
  // excute load instruction
  void Exec() override final;
  void GenInstStr(int inst_fmt) override final;

 public:
  // read img, weights, bias data from bank
  void read();
  int get_group_base_addr(int addr_group);
  // padding for img data
  void pad();
  // do pool operation of the whole feature map
  void pool();
  // do one pool kernel's pool operation
  void pool_one(int idx_src_h, int idx_src_w);
  // do shift, trunc operation
  void transform();
  // save result
  void save();

  // debug funcs
  // for debug
  void debug_var();
  // debug img/weights/bias input value
  void debug_input_fmap(string prefix);
  void debug_input_tick();
  void debug_output_fmap();
  void debug_output_tick();

 private:
  // pool instruction fields
  int bank_id_in_;
  int bank_id_out_;
  int bank_addr_in_;
  int pad_left_;
  int pad_top_;
  int pad_right_;
  int pad_bottom_;
  int bank_addr_out_;
  int length_;
  int channel_group_;
  int jump_read_endl_;
  int jump_write_endl_;
  int bank_addr_in_1_;
  int bank_addr_in_2_;
  int bank_addr_in_3_;
  int downsample_kernel_h_;
  int downsample_kernel_w_;

  // PoolInit instruction field
  int pool_type_;
  int stride_h_;
  int stride_w_;
  int stride_offset_in_;
  int valid_pixel_parallel_;
  int stride_offset_out_;
  int stride_out_;
  int jump_read_;
  int jump_write_;
  int kernel_h_;
  int kernel_w_;
  int shift_cut_;

  // others
  // hardware config value
  int pp_;
  int cp_;
  int vpp_;
  int pp_pool_;

  // self defined
  int cg_;
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

  // img
  vector<DPU_DATA_TYPE> img_;        // src_h_ * src_w_ * ic_
  vector<DPU_DATA_TYPE> rlt_dtype_;  // dst_h_ * dst_w_ * oc_
};

template <>
Pool<DPUVersion::DPUV2>::Pool(int inst_type, int instid,
                             vector<string>& inst_str,
                             vector<uint32_t>& inst_val);
template <>
Pool<DPUVersion::DPUV3E>::Pool(int inst_type, int instid,
                              vector<string>& inst_str,
                              vector<uint32_t>& inst_val);
template <>
Pool<DPUVersion::DPUV4E>::Pool(int inst_type, int instid,
                              vector<string>& inst_str,
                              vector<uint32_t>& inst_val);
template <>
Pool<DPUVersion::DPUV3ME>::Pool(int inst_type, int instid,
                             vector<string>& inst_str,
                             vector<uint32_t>& inst_val);
template <>
Pool<DPUVersion::XVDPU>::Pool(int inst_type, int instid,
                              vector<string>& inst_str,
                              vector<uint32_t>& inst_val);
template <>
Pool<DPUVersion::DPU4F>::Pool(int inst_type, int instid,
                                vector<string>& inst_str,
                                vector<uint32_t>& inst_val);

#endif /* __POOL_HPP__ */
