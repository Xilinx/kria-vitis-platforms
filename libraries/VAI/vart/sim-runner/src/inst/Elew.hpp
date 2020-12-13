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
#ifndef __ELEW_HPP__
#define __ELEW_HPP__

#include "pub/InstBase.hpp"

template <DPUVersion dv>
class Elew : public InstBase {
 public:
  const static int MAX_ELEW_NUM = 8;

 public:
  explicit Elew(int inst_type, int instid, vector<string>& inst_str,
                vector<uint32_t>& inst_val);
  ~Elew();

 public:
  // excute load instruction
  void Exec() override final;
  void GenInstStr(int inst_fmt) override final;

 public:
  int get_num() const;
  int get_bank_id_out() const;
  int get_shift_write() const;
  int get_jump_write() const;
  int get_channel_group() const;
  int get_length() const;
  int get_bank_addr_out() const;

 private:
  // functional funcs
  void read();
  void calc();
  void save();

  // debug funcs
  void debug_input_fmap();
  void debug_input_tick();
  void debug_output_fmap();
  void debug_output_tick();

 private:
  // inst field related
  int num_;
  int bank_id_out_;
  int shift_write_;
  int jump_write_;
  int channel_group_;
  int length_;
  int bank_addr_out_;
  int act_type_;
  int jump_write_endl_;
  int valid_pixel_parallel_;
  int stride_out_;
  int stride_offset_out_;
  int elew_type_{0};

  // InstElewInit params
  array<int, MAX_ELEW_NUM> id_;
  array<int, MAX_ELEW_NUM> bank_id_in_;
  array<int, MAX_ELEW_NUM> shift_read_;
  array<int, MAX_ELEW_NUM> jump_read_endl_;
  array<int, MAX_ELEW_NUM> jump_read_;
  array<int, MAX_ELEW_NUM> bank_addr_in_;

  // others
  int cg_;
  int cp_;
  int ic_;
  int src_h_;
  int src_w_;
  vector<DPU_DATA_TYPE> data_;
  vector<DPU_DATA_TYPE> rlt_dtype_;

  int vpp_;
  int pp_conv_;
};

template <>
Elew<DPUVersion::DPUV2>::Elew(int inst_type, int inst_id,
                              vector<string>& inst_str,
                              vector<uint32_t>& inst_val);
template <>
Elew<DPUVersion::DPUV3E>::Elew(int inst_type, int inst_id,
                               vector<string>& inst_str,
                               vector<uint32_t>& inst_val);
template <>
Elew<DPUVersion::DPUV3ME>::Elew(int inst_type, int inst_id,
                                vector<string>& inst_str,
                                vector<uint32_t>& inst_val);
template <>
Elew<DPUVersion::XVDPU>::Elew(int inst_type, int inst_id,
                              vector<string>& inst_str,
                              vector<uint32_t>& inst_val);
template <>
Elew<DPUVersion::DPUV4E>::Elew(int inst_type, int inst_id,
                               vector<string>& inst_str,
                               vector<uint32_t>& inst_val);
#endif /* __ELEW_HPP__ */
