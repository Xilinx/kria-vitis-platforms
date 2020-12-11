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
#ifndef __POOLINITL_HPP__
#define __POOLINITL_HPP__

#include "pub/InstBase.hpp"

template <DPUVersion dv>
class PoolInit : public InstBase {
 public:
  explicit PoolInit(int inst_type, int instid, vector<string>& inst_str,
                    vector<uint32_t>& inst_val);
  ~PoolInit();

  static PoolInit* CUROBJ;

 public:
  // excute PoolInit instruction
  void Exec() override final;
  void GenInstStr(int inst_fmt) override final;

 public:
  int get_pool_type() { return pool_type_; }
  int get_stride_h() { return stride_h_; }
  int get_stride_w() { return stride_w_; }
  int get_stride_offset_in() { return stride_offset_in_; }
  int get_valid_pixel_parallel() { return valid_pixel_parallel_; }
  int get_stride_offset_out() { return stride_offset_out_; }
  int get_stride_out() { return stride_out_; }
  int get_jump_read() { return jump_read_; }
  int get_jump_write() { return jump_write_; }
  int get_kernel_h() { return kernel_h_; }
  int get_kernel_w() { return kernel_w_; }
  int get_shift_cut() { return shift_cut_; }

 private:
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
};

template <>
PoolInit<DPUVersion::DPUV2>::PoolInit(int inst_type, int instid,
                                     vector<string>& inst_str,
                                     vector<uint32_t>& inst_val);
template <>
PoolInit<DPUVersion::DPUV3E>::PoolInit(int inst_type, int instid,
                                      vector<string>& inst_str,
                                      vector<uint32_t>& inst_val);
template <>
PoolInit<DPUVersion::DPUV4E>::PoolInit(int inst_type, int instid,
                                      vector<string>& inst_str,
                                      vector<uint32_t>& inst_val);
template <>
PoolInit<DPUVersion::DPUV3ME>::PoolInit(int inst_type, int instid,
                                     vector<string>& inst_str,
                                     vector<uint32_t>& inst_val);
template <>
PoolInit<DPUVersion::XVDPU>::PoolInit(int inst_type, int instid,
                                      vector<string>& inst_str,
                                      vector<uint32_t>& inst_val);
template <>
PoolInit<DPUVersion::DPU4F>::PoolInit(int inst_type, int instid,
                                        vector<string>& inst_str,
                                        vector<uint32_t>& inst_val);

#endif /* __CONVINITL_HPP__ */
