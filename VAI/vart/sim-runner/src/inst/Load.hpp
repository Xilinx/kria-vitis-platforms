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
#ifndef __LOAD_HPP__
#define __LOAD_HPP__

#include "Helper.hpp"
#include "InstBase.hpp"
#include "buffer/DDR.hpp"

template <DPUVersion T>
class Load : public InstBase {
 public:
  const static int MODE_AVG_CHANNEL_LIMIT = 64;
  const static int MODE_AVG_MAX = 3;

  const static int MODE_LOAD_SINGLE_REG = 0;
  const static int MODE_LOAD_THREE_REG = 1;

 public:
  explicit Load(int inst_type, int instid, vector<string>& inst_str,
                vector<uint32_t>& inst_val);
  ~Load();

 public:
  // excute load instruction
  void Exec() override final;
  void GenInstStr(int inst_fmt) override final;

 private:
  void load_block(const vector<bool>& pad_mask, const int32_t block_ddr_addr,
                  const int32_t block_bank_id, const int32_t block_bank_addr);
  void debug_tick();
  void debug_tick(const int32_t ddr_addr);
  void debug_tick_mode_single();
  void exec_mode_single_reg(const vector<bool>& pad_mask);
  void debug_tick_mode_three();
  void exec_mode_three_reg(const vector<bool>& pad_mask);
  vector<DPU_DATA_TYPE> separate(bool is_8_bit, DPU_DATA_TYPE* data,
                                 int32_t num);
  void gen_PadMask(vector<bool>& pad_mask, int bank_width);
  void load_and_minusmean(DPU_DATA_TYPE* buf, shared_ptr<Bank<DPU_DATA_TYPE>> & bank, const vector<bool> &pad_mask);

 private:
  int32_t hp_id_;
  int32_t bank_id_;
  int32_t bank_addr_;
  int32_t pad_start_;
  int32_t pad_end_;
  int32_t pad_idx_;
  int32_t jump_read_;
  int32_t jump_write_;
  int32_t length_;
  int32_t mode_avg_;
  int32_t channel_;
  int32_t reg_id_;
  uint32_t ddr_addr_;
  int32_t jump_read_endl_;
  int32_t output_channel_num_;
  int32_t ddr_mode_;

  int32_t mode_;
  int32_t pad_top_;
  int32_t pad_bottom_;
  int32_t channel_offset_;
  int32_t reg_id_1_;
  uint32_t ddr_addr_1_;
  int32_t reg_id_2_;
  uint32_t ddr_addr_2_;

  int32_t quant_lth_;

  int32_t jump_write_endl_;
  int32_t block_num_;

  vector<DPU_DATA_TYPE> avg_reg_;

  bool is_img_bank;
  bool is_wgt_bank;
};

// v1.4.0
template <>
Load<DPUVersion::DPUV2>::Load(int inst_type, int instid,
                              vector<string>& inst_str,
                              vector<uint32_t>& inst_val);
// v1.4.0-multi-elew
template <>
Load<DPUVersion::DPUV3E>::Load(int inst_type, int instid,
                               vector<string>& inst_str,
                               vector<uint32_t>& inst_val);
// v1.7.0
template <>
Load<DPUVersion::DPUV3ME>::Load(int inst_type, int instid,
                                vector<string>& inst_str,
                                vector<uint32_t>& inst_val);
// xvdpu
template <>
Load<DPUVersion::XVDPU>::Load(int inst_type, int instid,
                              vector<string>& inst_str,
                              vector<uint32_t>& inst_val);
// dpuv4e
template <>
Load<DPUVersion::DPUV4E>::Load(int inst_type, int instid,
                               vector<string>& inst_str,
                               vector<uint32_t>& inst_val);

template <>
void Load<DPUVersion::DPUV3E>::Exec();
template <>
void Load<DPUVersion::DPUV3ME>::Exec();
template <>
void Load<DPUVersion::DPU4F>::Exec();
template <>
void Load<DPUVersion::DPUV4E>::Exec();
template <>
void Load<DPUVersion::DPU4F>::debug_tick();
template <>
void Load<DPUVersion::XVDPU>::Exec();

#endif /* __LOAD_HPP__ */
