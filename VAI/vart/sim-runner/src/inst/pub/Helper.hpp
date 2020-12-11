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
#ifndef __HELPER_HPP__
#define __HELPER_HPP__

#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>
#include "InstTable.hpp"
#include "SimCfg.hpp"
#include "Util.hpp"
#include "common.hpp"

template <DPUVersion T>
class Helper {
 public:
  Helper() = delete;
  ~Helper() = delete;

 public:
  static void InstInit(const int32_t inst_type, const string root_debug_path,
                       const bool debug, uint32_t& opcode, string& debug_path);
  static void ReadInstInit(vector<vector<bool>>& minus_mask,
                           vector<vector<uint32_t>>& field_mask);
  static void GenInst(const string& inst, const int32_t inst_fmt,
                      const vector<vector<bool>>& minus_mask,
                      const vector<vector<uint32_t>>& field_mask,
                      vector<string>& field_str, vector<uint32_t>& field_val,
                      vector<uint32_t>& v_inst_mc);
  static string GetInstStr(
      const int32_t inst_type, const int32_t inst_fmt,
      const bitset<static_cast<uint32_t>(Category::INST_CATEGORY_MAX)>& dpdon,
      const bitset<static_cast<uint32_t>(Category::INST_CATEGORY_MAX)>& dpdby,
      const vector<string>& field_str);

 private:
  static void set_opcode(const int32_t inst_type, uint32_t& opcode);
  static void set_debug_path_and_folder(const int32_t inst_type,
                                        const string root_debug_path,
                                        string& debug_path);
  static vector<bool> get_minus_mask(const int32_t inst_type);
  static vector<uint32_t> get_field_mask(const int32_t inst_type);
  static void get_field_value(
      const int32_t inst_type, const uint32_t opcode,
      const bitset<static_cast<uint32_t>(Category::INST_CATEGORY_MAX)>& dpdon,
      const bitset<static_cast<uint32_t>(Category::INST_CATEGORY_MAX)>& dpdby,
      const vector<string>& field_str, vector<uint32_t>& field_val);
  static void ChkField(const int32_t inst_type, const vector<bool>& minus_mask,
                       const vector<uint32_t>& field_val);
  static vector<string> GenInstStr(int32_t& inst_type, const int32_t inst_fmt,
                                   const string& inst_str);
  static vector<uint32_t> GenInstVal(const int32_t& inst_type,
                                     const vector<string>& field_str);
  static void GenMC(const int32_t inst_type, const vector<bool>& minus_mask,
                    const vector<uint32_t>& field_mask,
                    const vector<uint32_t>& field_val,
                    vector<uint32_t>& mc_vec);
};

template <DPUVersion T>
void Helper<T>::set_opcode(const int32_t inst_type, uint32_t& opcode) {
  opcode = TableInterface<T>::inst_table::OPCode.at(inst_type);
};

template <DPUVersion T>
void Helper<T>::set_debug_path_and_folder(const int32_t inst_type,
                                          const string root_debug_path,
                                          string& debug_path) {
  // set debug path
  debug_path = root_debug_path + "/" +
               TableInterface<T>::inst_table::InstNameLower.at(inst_type) + "/";
  // set debug folder
  auto inst_name =
      TableInterface<T>::inst_table::InstNameLower.at(inst_type) + "/";
  Util::ChkFolder(root_debug_path + "/" + inst_name, false);
}

// initial minus mask
template <DPUVersion T>
vector<bool> Helper<T>::get_minus_mask(const int32_t inst_type) {
  auto field_num = TableInterface<T>::inst_table::FieldNum[inst_type];
  vector<bool> minus_mask(field_num);
  for (auto idx_field = 0U; idx_field < field_num; idx_field++) {
    if (TableInterface<T>::inst_table::InstFieldMinus.at(inst_type).at(
            idx_field) == 1)
      minus_mask.at(idx_field) = true;
  }
  return minus_mask;
}

// initial field mask
template <DPUVersion T>
vector<uint32_t> Helper<T>::get_field_mask(const int32_t inst_type) {
  auto word_num = TableInterface<T>::inst_table::WordNum[inst_type];
  auto field_num = TableInterface<T>::inst_table::FieldNum[inst_type];
  vector<uint32_t> field_mask(field_num);
  for (auto idx_word = 0U; idx_word < word_num; idx_word++) {
    auto& FieldPosMap =
        TableInterface<T>::inst_table::InstFieldPos.at(inst_type).at(idx_word);
    auto& FieldLenMap =
        TableInterface<T>::inst_table::InstFieldLen.at(inst_type).at(idx_word);
    for (auto it = FieldPosMap.begin(); it != FieldPosMap.end(); it++) {
      auto field_id = it->first;
      auto field_pos = FieldPosMap.at(field_id);
      auto field_len = FieldLenMap.at(field_id);

      for (auto k = 0U; k < field_len; k++) {
        field_mask.at(field_id) |= (1 << (field_pos + k));
      }
    }
  }
  return field_mask;
}

template <DPUVersion T>
void Helper<T>::InstInit(const int32_t inst_type, const string root_debug_path,
                         const bool debug, uint32_t& opcode,
                         string& debug_path) {
  set_opcode(inst_type, opcode);
  if (debug) set_debug_path_and_folder(inst_type, root_debug_path, debug_path);
}

template <DPUVersion T>
void Helper<T>::ReadInstInit(vector<vector<bool>>& minus_mask,
                             vector<vector<uint32_t>>& field_mask) {
  auto inst_num = TableInterface<T>::inst_table::INST_TYPE_MAX;
  for (auto idx_inst = 0; idx_inst < inst_num; idx_inst++) {
    minus_mask.emplace_back(get_minus_mask(idx_inst));
    field_mask.emplace_back(get_field_mask(idx_inst));
  }
}

template <DPUVersion T>
void Helper<T>::ChkField(const int32_t inst_type,
                         const vector<bool>& minus_mask,
                         const vector<uint32_t>& field_val) {
  auto word_num = TableInterface<T>::inst_table::WordNum[inst_type];
  auto field_num = TableInterface<T>::inst_table::FieldNum.at(inst_type);
  auto field_name = TableInterface<T>::inst_table::InstFieldName.at(inst_type);
  auto field_len = TableInterface<T>::inst_table::InstFieldLen.at(inst_type);
  auto inst_num = TableInterface<T>::inst_table::INST_TYPE_MAX;

  UNI_LOG_CHECK(field_val.size() == field_num, SIM_PARAMETER_FAILED)
      << ": inst fields number error, "
      << "it is " << field_val.size() << ", it should be " << field_num;

  for (auto idx_word = 0U; idx_word < word_num; idx_word++) {
    for (auto it = field_len[idx_word].begin(); it != field_len[idx_word].end();
         it++) {
      auto id = it->first;
      auto len = it->second;
      auto val = field_val[id];
      auto name = field_name[id];

      if (id < 3)  // for OPCODE, DPDON, DPDBY
        continue;

      if (inst_type >= inst_num - 3)
        if (id == 3) continue;

      auto min = static_cast<uint64_t>(0);
      auto max = static_cast<uint64_t>(pow(2, len));

      if (minus_mask.at(id)) {
        UNI_LOG_CHECK(static_cast<uint64_t>(val) <= max &&
                          static_cast<uint64_t>(val) > min,
                      SIM_OUT_OF_RANGE)
            << ": field_name = " << name << ", field_id = " << id
            << ", field_val = " << val;
      } else {
        UNI_LOG_CHECK(static_cast<uint64_t>(val) < max &&
                          static_cast<uint64_t>(val) >= min,
                      SIM_OUT_OF_RANGE)
            << ": field_name = " << name << ", field_id = " << id
            << ", field_val = " << val;
      }
    }
  }
}

template <DPUVersion T>
vector<string> Helper<T>::GenInstStr(int32_t& inst_type, const int32_t inst_fmt,
                                     const string& inst_str) {
  vector<string> inst_field;

  if (inst_fmt == INST_FMT_AC_DESP_YES) {
    vector<string> split = Util::Split(inst_str);

    // push opcode/dpdon/dpdby
    inst_field.push_back(split[0]);
    inst_field.push_back(split[1]);
    inst_field.push_back(split[2]);

    // get inst's basic info
    inst_type = TableInterface<T>::inst_table::InstName2InstType.at(split[0]);
    auto field_num = TableInterface<T>::inst_table::FieldNum.at(inst_type);
    auto& field_name =
        TableInterface<T>::inst_table::InstFieldName.at(inst_type);

    // check field num
    UNI_LOG_CHECK((split.size() - 3) % 2 == 0, SIM_PARAMETER_FAILED);
    UNI_LOG_CHECK((split.size() - 3) / 2 + 3 == field_num, SIM_PARAMETER_FAILED)
        << " : Field number error, " << (split.size() - 3) / 2 + 3
        << " != " << field_num;

    // save inst key/value into a unordered_map
    unordered_map<string, string> inst_map;
    for (auto i = 3U; i < split.size(); i += 2) {
      inst_map[split[i]] = split[i + 1];
    }

    // save all inst value into inst_field
    for (auto i = 3U; i < field_num; i++) {
      UNI_LOG_CHECK(inst_map.find(field_name[i]) != inst_map.end(),
                    SIM_OUT_OF_RANGE)
          << " : Not found field " << field_name[i] << " in " << inst_str;
      inst_field.push_back(inst_map.at(field_name[i]));
    }
  } else if (inst_fmt == INST_FMT_AC_DESP_NO) {
    inst_field = Util::Split(inst_str);
  } else {
    UNI_LOG_FATAL(SIM_OUT_OF_RANGE)
        << "Not supported inst fmt " << inst_fmt << endl;
  }

  return inst_field;
}

template <DPUVersion T>
vector<uint32_t> Helper<T>::GenInstVal(const int32_t& inst_type,
                                       const vector<string>& field_str) {
  // get & check field type and number
  auto& field_type =
      TableInterface<T>::inst_table::InstFieldDataType.at(inst_type);
  auto field_num = TableInterface<T>::inst_table::FieldNum.at(inst_type);

  UNI_LOG_CHECK(field_num == field_str.size(), SIM_PARAMETER_FAILED)
      << field_num << " != " << field_str.size() << endl;

  // set field value
  vector<uint32_t> field_val(field_num);
  const bitset<static_cast<uint32_t>(Category::INST_CATEGORY_MAX)> dpdon{
      field_str[1]};
  const bitset<static_cast<uint32_t>(Category::INST_CATEGORY_MAX)> dpdby{
      field_str[2]};

  for (auto i = 0U; i < field_num; i++) {
    // special for opcode dpdon and dpdby
    if (i == 0)
      field_val[i] = TableInterface<T>::inst_table::OPCode.at(inst_type);
    else if (i == 1) {
      field_val[i] = dpdon.to_ulong();
      // cout << field_val[i] << endl;
    } else if (i == 2) {
      field_val[i] = dpdby.to_ulong();
      // cout << field_val[i] << endl;
    } else if (field_type.at(i) ==
               TableInterface<T>::inst_table::
                   INST_FIELD_DATA_TYPE_UINT)  // inst_type does not
                                               // need
                                               // to be specialized
      field_val[i] = stoul(field_str[i]);
    else if (field_type.at(i) ==
             TableInterface<T>::inst_table::INST_FIELD_DATA_TYPE_STRING)
      field_val[i] = 0xFFFFFFFF;
    else {
      UNI_LOG_FATAL(SIM_OUT_OF_RANGE)
          << "Not supported field type " << field_type.at(i) << endl;
    }
  }
  return field_val;
}

template <DPUVersion T>
void Helper<T>::GenMC(const int32_t inst_type, const vector<bool>& minus_mask,
                      const vector<uint32_t>& field_mask,
                      const vector<uint32_t>& field_val,
                      vector<uint32_t>& mc_vec) {
  auto word_num = TableInterface<T>::inst_table::WordNum[inst_type];
  // cout << TableInterface<T>::inst_table::OPCode.at(inst_type) << endl;
  auto field_pos = TableInterface<T>::inst_table::InstFieldPos.at(inst_type);

  uint32_t mc = 0U;
  for (auto idx_word = 0U; idx_word < word_num; idx_word++) {
    for (auto it = field_pos[idx_word].begin(); it != field_pos[idx_word].end();
         it++) {
      auto id = it->first;
      auto pos = field_pos[idx_word].at(id);
      // set minus_mask
      auto field_value = minus_mask.at(id) ? field_val[id] - 1 : field_val[id];
      // set position
      mc |= static_cast<uint32_t>((static_cast<uint32_t>(field_value) << pos) &
                                  static_cast<uint32_t>(field_mask.at(id)));
      // cout << "word " << idx_word << ": pos " << pos << " value " <<
      // field_value << " ";
      // cout << setfill('0') << setw(8) << hex
      //     << static_cast<uint32_t>(
      //            (static_cast<uint32_t>(field_value) << pos) &
      //            static_cast<uint32_t>(field_mask.at(id)))
      //     << endl;
    }
    // cout << "mc: " << mc << endl;
    mc_vec.emplace_back(mc);
    mc = 0U;
  }
}

template <DPUVersion T>
void Helper<T>::GenInst(const string& inst, const int32_t inst_fmt,
                        const vector<vector<bool>>& minus_mask,
                        const vector<vector<uint32_t>>& field_mask,
                        vector<string>& field_str, vector<uint32_t>& field_val,
                        vector<uint32_t>& v_inst_mc) {
  // get inst_type from inst_field_str
  int32_t inst_type;
  field_str = GenInstStr(inst_type, inst_fmt, inst);
  field_val = GenInstVal(inst_type, field_str);
  auto inst_num = TableInterface<T>::inst_table::INST_TYPE_MAX;
  ChkField(inst_type, minus_mask[inst_type], field_val);
  if (inst_type < inst_num - 3) {
    GenMC(inst_type, minus_mask[inst_type], field_mask[inst_type], field_val,
          v_inst_mc);
  }
}

template <DPUVersion T>
string Helper<T>::GetInstStr(
    const int32_t inst_type, const int32_t inst_fmt,
    const bitset<static_cast<uint32_t>(Category::INST_CATEGORY_MAX)>& dpdon,
    const bitset<static_cast<uint32_t>(Category::INST_CATEGORY_MAX)>& dpdby,
    const vector<string>& field_str) {
  auto field_num = TableInterface<T>::inst_table::FieldNum.at(inst_type);
  auto field_name = TableInterface<T>::inst_table::InstFieldName.at(inst_type);

  // for OPCODE, DPDON, DPDBY
  string inst_ac = TableInterface<T>::inst_table::InstName.at(inst_type) + " ";
  inst_ac += dpdon.to_string() + " ";
  inst_ac += dpdby.to_string() + " ";

  // except OPCODE, DPDON, DPDBY
  for (auto idx_field = 3U; idx_field < field_num; idx_field++) {
    if (inst_fmt == INST_FMT_AC_DESP_YES) {
      inst_ac += field_name[idx_field] + " ";
    }

    inst_ac += field_str[idx_field];
    if (idx_field != field_num - 1) inst_ac += " ";
  }

  return inst_ac;
}

#endif
