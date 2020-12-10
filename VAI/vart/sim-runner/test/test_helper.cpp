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

//#include "sim/inst/XInstTable.hpp"
#include <iostream>
#include <memory>
#include "inst/pub/Helper.hpp"

using namespace std;

/*
static uint32_t mc_array[] = {
    // LOAD 0010 0000 hp_id 0 bank_id 33 bank_addr 0 pad_start 0 pad_end 0
    // pad_idx 1 jump_read 64 jump_write 4 length 1 mode_avg 0 channel 64 reg_id
    // 0 ddr_addr 11916264
    0x02021000, 0x0000003f, 0x0300003f, 0x00b5d3e8,
    // save
    0x46008000, 0x0000003f, 0x030dc03f, 0x20119d00,
    // CONVINIT 0100 0100 act_type 1 shift_cut 6 jump_read 4 stride_out 1
    // shift_bias 6 jump_read_endl 1 stride_h 1 kernel_h 1 valid_pixel_parallel
    // 8 stride_offset_in 0 jump_write 4 stride_w 1 kernel_w 1 stride_offset_out
    // 0 jump_write_endl 1

    0x94411803, 0x00001800, 0x0000e003, 0x00000000,
    // conv
    0x85400000, 0x0300f000, 0x0dc00000, 0x00008000, 0x00000000,
    // poolinit
    0x6880000f, 0x01e00824,
    // pool
    0xc9b0f000, 0x000003c8, 0x000006c0, 0x00000000, 0x00000000
    // dwinit
    // dptwise
    // elewinit
    // elew
    // end
    // dumpbank
    // dumpddr
    // dumpddrslice
};

template <DPUVersion T>
uint32_t* read_mc(int32_t inst_type) {
  uint32_t* ret = static_cast<uint32_t*>(mc_array);
  for (auto idx_inst = 0; idx_inst < inst_type; idx_inst++) {
    auto word_num = TableInterface<T>::inst_table::WordNum.at(inst_type);
    // cout << "Word number of " <<
    // TableInterface<T>::inst_table::InstName[idx_inst]<< " is:"
    // << word_num << endl;
    ret += word_num;
  }
  return ret;
}
//*/

int main(int argc, char* argv[]) {
  int32_t inst_num = 1;
  cout << inst_num << endl;
  string root_debug_path = ".";
  string debug_path;
  uint32_t opcode;
  Helper<DPUVersion::DPUV2>::InstInit(inst_num, root_debug_path, true, opcode,
                                     debug_path);
  cout << hex << opcode << " " << debug_path << endl;
  // TableInterface<DPUVersion::DPUV2>::inst_table::INST_TYPE_MAX;
  /*
  vector<uint32_t> mc_vec; // save mc for all test inst
  for (auto idx_inst = 0; idx_inst < inst_num; idx_inst++) {
    uint32_t opcode = 1;
    string debug_path = "./";
    vector<string> field_str = {"LOAD", "0010", "0000", "0",       "33", "0",
                                "0",    "0",    "1",    "64",      "4",  "1",
                                "0",    "64",   "0",    "11916264"};
    bitset<static_cast<uint32_t>(Category::INST_CATEGORY_MAX)> dpdon{
        field_str[1]};
    bitset<static_cast<uint32_t>(Category::INST_CATEGORY_MAX)> dpdby{
        field_str[2]};
    vector<uint32_t> field_val;
    vector<bool> minus_mask;
    vector<uint32_t> field_mask;
    cout << TableInterface<DPUVersion::DPUV2>::inst_table::InstName[idx_inst]
         << endl;
    // test StaticInit()
    Helper<DPUVersion::DPUV2>::InstInit(idx_inst, debug_path, opcode,
  debug_path);
    cout << "Set field_value as :" << endl;
    for (auto it = field_val.begin(); it != field_val.end(); it++) {
      auto idx_field = it - field_val.begin();
      cout << TableInterface<DPUVersion::DPUV2>::inst_table::InstFieldName
                  .at(idx_inst)
                  .at(idx_field)
           << " : " << *it << endl;
    }
    cout << "Set debug_path as : " << debug_path << endl;
    cout << "Set minus_mask as : " << endl;
    for (auto it = minus_mask.begin(); it != minus_mask.end(); it++) {
      auto idx_field = it - minus_mask.begin();
      cout << TableInterface<DPUVersion::DPUV2>::inst_table::InstFieldName
                  .at(idx_inst)
                  .at(idx_field)
           << " : " << *it << endl;
    }
    cout << "Set field_mask as:" << endl;
    for (auto it = field_mask.begin(); it != field_mask.end(); it++) {
      auto idx_field = it - field_mask.begin();
      cout << TableInterface<DPUVersion::DPUV2>::inst_table::InstFieldName
                  .at(idx_inst)
                  .at(idx_field)
           << " : " << setfill('0') << setw(8) << hex << *it << endl;
    }

    // test GetInstStr()
    auto inst_str = Helper<DPUVersion::DPUV2>::GetInstStr(
        idx_inst, INST_FMT_AC_DESP_YES, dpdon, dpdby, field_str);
    cout << "Get inst_str : " << endl << inst_str << endl;

    //  test GetInstFromWord()
    auto mc_ptr = read_mc<DPUVersion::DPUV2>(idx_inst);
    auto ac = Helper<DPUVersion::DPUV2>::GetInstFromWord(idx_inst, mc_ptr,
                                                        minus_mask, field_mask);
    cout << "read ac as:" << endl << ac << endl;

    // test GenMC()
    vector<uint32_t> mc;
    Helper<DPUVersion::DPUV2>::GenMC(idx_inst, minus_mask, field_mask, field_val,
                                    mc, mc_vec);
    cout << "Get "
         << TableInterface<DPUVersion::DPUV2>::inst_table::InstName[idx_inst]
         << " mc : " << endl;
    for (auto it = mc.begin(); it != mc.end(); it++) {
      auto idx_word = it - mc.begin();
      cout << "Word[" << idx_word << "] : " << setfill('0') << setw(8) << hex
           << *it << endl;
    }

    // test ChkField()
    Helper<DPUVersion::DPUV2>::ChkField(idx_inst, minus_mask, field_str,
                                       field_val);
  }
  cout << "Get all mc : " << endl;
  for (auto it = mc_vec.begin(); it != mc_vec.end(); it++) {
    auto idx_word = it - mc_vec.begin();
    cout << "Word[" << idx_word << "] : " << setfill('0') << setw(8) << hex
         << *it << endl;
         }*/
  return 0;
}
