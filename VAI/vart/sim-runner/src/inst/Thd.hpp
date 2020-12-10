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
#ifndef __THD_HPP__
#define __THD_HPP__

#include "Helper.hpp"
#include "InstBase.hpp"

struct data_dst {
  int32_t signal_bit : 1, delta : 10, base : 13, no_use : 8;
};

struct data_src {
  int32_t data0 : 8, data1 : 8, data2 : 8, no_use : 8;
};

struct trans_delta {
  uint16_t real_delta : 10;
};

union param_union {
  data_src src;
  data_dst dst;
};

class Thd : public InstBase {
 public:
  // real constructor
  explicit Thd(int inst_type, int instid, vector<string>& inst_str,
               vector<uint32_t>& inst_val);
  ~Thd();

 public:
  // excute load instruction
  void Exec() override final;
  void GenInstStr(int inst_fmt) override final;

 public:
  int32_t get_channel_group() const;
  int32_t get_bank_addr_in() const;

  int32_t get_signed_mode() const;
  int32_t get_valid_pixel_parallel() const;
  int32_t get_length() const;
  int32_t get_bank_addr_out() const;

  int32_t get_stride_out() const;
  int32_t get_bank_id_in() const;
  int32_t get_jump_read() const;
  int32_t get_jump_read_endl() const;

  int32_t get_stride_offset_out() const;
  int32_t get_bank_id_out() const;
  int32_t get_jump_write() const;
  int32_t get_jump_write_endl() const;

  int32_t get_param_addr() const;

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
  int32_t channel_group_;
  int32_t bank_addr_in_;

  int32_t signed_mode_;
  int32_t valid_pixel_parallel_;
  int32_t length_;
  int32_t bank_addr_out_;

  int32_t stride_out_;
  int32_t bank_id_in_;
  int32_t jump_read_;
  int32_t jump_read_endl_;

  int32_t stride_offset_out_;
  int32_t bank_id_out_;
  int32_t jump_write_;
  int32_t jump_write_endl_;

  int32_t param_addr_;

  // others
  int32_t cg_;
  int32_t cp_;
  int32_t ic_;
  int32_t vpp_;
  int32_t pp_conv_;
  int32_t src_w_;
  vector<DPU_DATA_TYPE> data_;
  vector<DPU_DATA_TYPE> rlt_dtype_;
  vector<DPU_DATA_TYPE> param_;
  vector<int16_t> base_;
  vector<uint16_t> delta_;
  vector<bool> signal_bit_;
};

#endif /* __THD_HPP__ */
