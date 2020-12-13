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
#ifndef __LAYER_HPP__
#define __LAYER_HPP__

#include "Buffer.hpp"
#include "SimCfg.hpp"

#include "InstBase.hpp"

class Layer {
public:
  Layer(int netid, int layerid, string debug_path,
        const vector<string> &inst_vec);
  ~Layer();

public:
  // exec current layer's inst
  void Run();
  // save machine code into file
  void SaveMC(int fmt);

  // get Inst number of current layer
  // if inst_type is IT_MAX, return all inst num
  size_t GetInstNum(int inst_type);

private:
  // layer's init operation
  void init();

  // execute inst by order
  void exec_by_order();

  // add an instruction from a inst string
  void add_inst(int inst_id, vector<string> &inst_str,
                vector<uint32_t> &inst_val);

  // print all kinds of insts number
  void print_inst_num();

private:
  int netid_;
  int layerid_;
  string debug_path_;
  vector<string> inst_vec_;

  int inst_num_;
  vector<shared_ptr<InstBase>> v_inst_;
  vector<uint32_t> mc_vec_;

  // array<int, static_cast<uint32_t>(Category::INST_CATEGORY_MAX)> exec_id_;
};

#endif /* __LAYER_HPP__ */
