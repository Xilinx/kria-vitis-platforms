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
#ifndef __INSTBASE_HPP__
#define __INSTBASE_HPP__

#include "ArchCfg.hpp"
#include "Buffer.hpp"
#include "Calc.hpp"
#include "Helper.hpp"
#include "InstTable.hpp"
#include "SimCfg.hpp"
#include "bank_shell.hpp"

#include "common.hpp"
class InstBase {
 public:
  // const static uint32_t INST_WORD_BIT_NUM = 32;
  // const static uint32_t MAX_INST_FIELD_NUM = 64;
  // const static uint32_t FIELD_START_POS = 3;

 public:  // global static variables
  static void StaticInit(const string debug_path);
  static void SetNetID(int netid) { netid_ = netid; }
  static int GetNetID() { return netid_; }
  static void SetLayerID(int layerid) { layerid_ = layerid; }
  static int GetLayerID() { return layerid_; }
  static int GetInstNum(int inst_type) { return inst_num_[inst_type]; }

 public:
  // real constructor
  explicit InstBase(int inst_type, int instid, vector<string>& inst_str,
                    vector<uint32_t>& inst_val);
  virtual ~InstBase();

 public:
  // used to parse depends between insts
  void SetDpdOn(int inst_classify) { dpdon_.set(inst_classify, true); }
  void SetDpdBy(int inst_classify) { dpdby_.set(inst_classify, true); }
  string GetDpdOn() const { return dpdon_.to_string(); }
  string GetDpdBy() const { return dpdby_.to_string(); }
  bitset<static_cast<uint32_t>(Category::INST_CATEGORY_MAX)> GetDpdOnBitset()
      const {
    return dpdon_;
  }
  bitset<static_cast<uint32_t>(Category::INST_CATEGORY_MAX)> GetDpdByBitset()
      const {
    return dpdby_;
  }

  string GetInstStr(int inst_fmt = INST_FMT_AC_DESP_YES) {
    // init ac_
    ac_ = "";
    GenInstStr(inst_fmt);
    return ac_;
  }

  // funcs to get opcode, inst, instid, dpdon, dpdby
  virtual uint32_t GetOpCode() const { return opcode_; }
  virtual uint32_t GetInstType() const { return inst_type_; }

  virtual void Exec() {}
  virtual void GenInstStr(int inst_fmt) {}
  virtual void PrintInst();

 protected:
  // instid is instruction ID
  int inst_type_;
  int instid_;
  int instid_classify_;

  // field_str_: {"LOAD", "0001", "0001", "0", "2", "0", "0", "1", "64",
  // "1000000"}
  vector<string> field_str_;
  vector<uint32_t> field_val_;

  string ac_;
  uint32_t opcode_;
  bitset<static_cast<uint32_t>(Category::INST_CATEGORY_MAX)> dpdon_;
  bitset<static_cast<uint32_t>(Category::INST_CATEGORY_MAX)> dpdby_;

  bool debug_;
  string debug_path_;

 protected:
  static int netid_;
  static int layerid_;
  static int hp_width_;
  static string root_debug_path_;
  static uint32_t reserved_;

  static vector<uint32_t> inst_num_;
};

#endif /* __INSTBASE_HPP__ */
