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
#ifndef __DUMPBANK_HPP__
#define __DUMPBANK_HPP__

#include "InstBase.hpp"

template <DPUVersion dv> class DumpBank : public InstBase {
public:
  // real constructor
  explicit DumpBank(int inst_type, int instid, vector<string> &inst_str,
                    vector<uint32_t> &inst_val);
  ~DumpBank();

public:
  // excute load instruction
  void Exec() override final;
  void GenInstStr(int inst_fmt) override final;

private:
  void dump_bank_range();

private:
  string save_name_;
  int save_fmt_;
  int bank_start_;
  int bank_dump_num_;
};

#endif /* __DUMPBANK_HPP__ */
