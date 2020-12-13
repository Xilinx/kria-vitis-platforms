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
#ifndef __ELEWINIT_HPP__
#define __ELEWINIT_HPP__

#include "pub/InstBase.hpp"

template <DPUVersion dv> class ElewInit : public InstBase {
public:
  explicit ElewInit(int inst_type, int instid, vector<string> &inst_str,
                    vector<uint32_t> &inst_val);
  ~ElewInit();

public:
  // excute load instruction
  void Exec() override final;
  void GenInstStr(int inst_fmt) override final;

  // other public funcs
public:
  // other public funcs
  static int get_id(int i);
  static int get_bank_id_in(int i);
  static int get_shift_read(int i);
  static int get_jump_read(int i);
  static int get_bank_addr_in(int i);
  static int get_jump_read_endl(int i);

private:
  static deque<int> id_;
  static deque<int> bank_id_in_;
  static deque<int> shift_read_;
  static deque<int> jump_read_;
  static deque<int> bank_addr_in_;
  static deque<int> jump_read_endl_;

  int local_id_;
  int local_bank_id_in_;
  int local_shift_read_;
  int local_jump_read_;
  int local_bank_addr_in_;
  int local_jump_read_endl_;
};

#endif /* __ELEWINIT_HPP__ */
