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
#ifndef __READ_INST_HPP__
#define __READ_INST_HPP__

#include "SimCfg.hpp"
#include "common.hpp"

class ReadInst {
public:
  explicit ReadInst(const vector<string> &inst_vec);
  ~ReadInst() = default;

public:
  int GetInstNum() const { return v_inst_val_.size(); }
  // get inst as field string
  vector<string> GetInstStr(int i) { return v_inst_str_[i]; }
  // get inst as field value
  vector<uint32_t> GetInstVal(int i) { return v_inst_val_[i]; }
  // get inst as machine code
  vector<uint32_t> GetMC() { return v_inst_mc_; };

private:
  void check_field(); // get inst value and check whether it is
                      // inside range
  vector<string> split_inst_str(const string &inst_str);

private:
  string inst_file_;
  int inst_fmt_;
  vector<vector<string>> v_inst_str_;
  vector<vector<uint32_t>> v_inst_val_;
  vector<uint32_t> v_inst_mc_;
};

#endif /* __READ_INST_HPP__ */
