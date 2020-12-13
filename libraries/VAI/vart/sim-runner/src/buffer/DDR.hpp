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
#pragma once

#include <string>
#include <tuple>
#include <vector>
#include "SimCfg.hpp"
#include "util/LoadCfg.hpp"

namespace xir {
class Subgraph;
}

struct Reg {
  int id;
  uint64_t size;
  std::vector<char> data;
};

class DDR {
 public:
  static DDR& Instance() {
    static DDR ddr;
    return ddr;
  }
  void Initial();                           // init by config file
  void Initial(const xir::Subgraph* subg);  // init by xir subgraph

 public:
  char* GetAddr(int reg_id, uint64_t offset);
  uint64_t GetSize(int reg_id) const;
  void SaveReg(const std::string file);
  void SaveDDR(std::string save_name, int fmt, bool skip = false);
  void SaveDDR(std::string save_name,
               const std::vector<std::tuple<int32_t, int32_t, int32_t>>& regs,
               int fmt, bool skip = false);

 private:
  explicit DDR(){};
  DDR(const DDR&) = delete;
  DDR& operator=(const DDR&) = delete;
  int GetRegID(std::string reg_id);

 private:
  void RegConf(const string& regcfg_file = "./config/RegCfg.txt");
  void DDRConf(const string& ddrcfg_file = "./config/DDRCfg.txt");
  // random
  void init_ddr(const int32_t& id);
  void init_ddr(const string& init_file);

 private:
  std::vector<struct Reg> ddr_buf_;
  std::map<std::string, std::string> reg_id_to_context_type_;
  std::map<std::string, std::string> reg_id_to_shared_;
};
