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
#ifndef __SAVE_HPP__
#define __SAVE_HPP__

#include "buffer/DDR.hpp"
#include "pub/InstBase.hpp"

template <DPUVersion T>
class Save : public InstBase {
 public:
  explicit Save(int inst_type, int instid, vector<string>& inst_str,
                vector<uint32_t>& inst_val);
  ~Save();

 public:
  // excute load instruction
  void Exec() override final;
  void GenInstStr(int inst_fmt) override final;

 private:
  void debug_tick();
  void combine(bool is_8_bit, vector<DPU_DATA_TYPE> data_in,
               DPU_DATA_TYPE* data_out);

 private:
  int32_t hp_id_;
  int32_t bank_id_;
  int32_t bank_addr_;
  int32_t jump_write_;
  int32_t jump_read_;
  int32_t length_;
  int32_t channel_;
  int32_t reg_id_;
  uint32_t ddr_addr_;

  int32_t quant_lth_;
};

template <>
Save<DPUVersion::DPUV2>::Save(int inst_type, int instid,
                             vector<string>& inst_str,
                             vector<uint32_t>& inst_val);
template <>
Save<DPUVersion::DPUV3E>::Save(int inst_type, int instid,
                              vector<string>& inst_str,
                              vector<uint32_t>& inst_val);
template <>
Save<DPUVersion::DPUV3ME>::Save(int inst_type, int instid,
                             vector<string>& inst_str,
                             vector<uint32_t>& inst_val);
template <>
Save<DPUVersion::XVDPU>::Save(int inst_type, int instid,
                              vector<string>& inst_str,
                              vector<uint32_t>& inst_val);
template <>
Save<DPUVersion::DPUV4E>::Save(int inst_type, int instid,
                              vector<string>& inst_str,
                              vector<uint32_t>& inst_val);
template <>
Save<DPUVersion::DPU4F>::Save(int inst_type, int instid,
                                vector<string>& inst_str,
                                vector<uint32_t>& inst_val);
template <>
void Save<DPUVersion::DPU4F>::Exec();
template <>
void Save<DPUVersion::DPU4F>::debug_tick();

#endif /* __SAVE_HPP__ */
