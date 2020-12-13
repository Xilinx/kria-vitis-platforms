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
#ifndef __CBLOAD_HPP__
#define __CBLOAD_HPP__

#include "Helper.hpp"
#include "InstBase.hpp"

class CBLoad : public InstBase {
 public:
  explicit CBLoad(int inst_type, int instid, vector<string>& inst_str,
                  vector<uint32_t>& inst_val);
  ~CBLoad();

 public:
  // excute load instruction
  void Exec() override final;
  void GenInstStr(int inst_fmt) override final;

 private:
  int32_t length_;
  int32_t channel_group_;
  int32_t group_id_in_;
  int32_t bank_addr_in_;
  int32_t bank_addr_out_;
  int32_t cb_sel_;
};

#endif /* __CBLOAD_HPP__ */
