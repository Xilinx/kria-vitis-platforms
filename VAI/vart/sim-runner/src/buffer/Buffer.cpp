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
#include "Buffer.hpp"
#include <sstream>
#include "SimCfg.hpp"
#include "conf/ArchCfg.hpp"
#include "util/Util.hpp"

template <typename DType>
Buffer<DType>::Buffer() {
  UNI_LOG_INFO << "Buffer initializing...";
  bank_init_ = SimCfg::Instance().get_bank_init();
  bank_init_type_ = SimCfg::Instance().get_bank_init_type();
  bank_init_file_ = SimCfg::Instance().get_bank_init_file();
  auto bank_group_size = ArchCfg::Instance().get_param().bank_group_size();
  // group_map_.resize(bank_group_size);
  bank_num_ = 0;

  for (auto idx_bank_group = 0; idx_bank_group < bank_group_size;
       idx_bank_group++) {
    auto name =
        ArchCfg::Instance().get_param().bank_group(idx_bank_group).name();
    auto base_id =
        ArchCfg::Instance().get_param().bank_group(idx_bank_group).base_id();
    auto num =
        ArchCfg::Instance().get_param().bank_group(idx_bank_group).bank_num();
    auto height =
        ArchCfg::Instance().get_param().bank_group(idx_bank_group).bank_depth();
    auto width =
        ArchCfg::Instance().get_param().bank_group(idx_bank_group).bank_width();
    bank_id_max_ = (bank_id_max_ < (base_id + num - 1)) ? (base_id + num - 1)
                                                        : bank_id_max_;
    for (auto idx_bank = 0U; idx_bank < num; idx_bank++) {
      group_map_[name].push_back(
          std::make_shared<Bank<DType>>(base_id, height, width));
      bank_map_[base_id] = group_map_[name][idx_bank];
      base_id++;
    }
  }
  bank_num_ = bank_map_.size();

  init_bank();
  if (bank_init_) {
    SaveAllBank(DATA_FMT_HEX_CONT_SMALLEND_BANKADDR, 0);
  }
}

template <typename DType>
void Buffer<DType>::virtual_bank_acquire(
    const std::string src_bank_group_name) {
  auto dst_bank_group_name = "Virtual_" + src_bank_group_name;
  if (group_map_.find(dst_bank_group_name) != group_map_.end()) return;

  auto bank_group_size = ArchCfg::Instance().get_param().bank_group_size();
  for (auto idx_bank_group = 0; idx_bank_group < bank_group_size;
       idx_bank_group++) {
    auto name =
        ArchCfg::Instance().get_param().bank_group(idx_bank_group).name();
    if (src_bank_group_name == name) {
      auto base_id =
          virtual_bank_id_offset +
          ArchCfg::Instance().get_param().bank_group(idx_bank_group).base_id();
      auto num =
          ArchCfg::Instance().get_param().bank_group(idx_bank_group).bank_num();
      auto height = ArchCfg::Instance()
                        .get_param()
                        .bank_group(idx_bank_group)
                        .bank_depth();
      auto width = ArchCfg::Instance()
                       .get_param()
                       .bank_group(idx_bank_group)
                       .bank_width();

      for (auto idx_bank = 0U; idx_bank < num; idx_bank++) {
        group_map_[dst_bank_group_name].push_back(
            std::make_shared<Bank<DType>>(base_id, height, width));
        bank_map_[base_id] = group_map_[dst_bank_group_name][idx_bank];
        base_id++;
      }
      return;
    }
  }
  UNI_LOG_FATAL(SIM_OUT_OF_RANGE)
      << "source bank group could not be found, bank_group_name: "
      << src_bank_group_name << endl;
}

template <typename DType>
int32_t Buffer<DType>::GetBaseID(const std::string bank_group_name) {
  auto bank_group_size = ArchCfg::Instance().get_param().bank_group_size();
  for (auto idx_bank_group = 0; idx_bank_group < bank_group_size;
       idx_bank_group++) {
    auto name =
        ArchCfg::Instance().get_param().bank_group(idx_bank_group).name();
    if (bank_group_name == name) {
      auto base_id =
          ArchCfg::Instance().get_param().bank_group(idx_bank_group).base_id();
      return base_id;
    }
  }
  UNI_LOG_FATAL(SIM_OUT_OF_RANGE)
      << "Bank group could not be found, bank_group_name: " << bank_group_name
      << endl;
}

template <typename DType>
int32_t Buffer<DType>::virtual_bank_mapping(
    const int32_t virtual_bank_id, const std::string src_bank_group_name) {
  auto bank_num = GetBankNum(src_bank_group_name);
  auto bank_id = virtual_bank_id % bank_num;
  auto base_id = GetBaseID(src_bank_group_name);
  bank_id += virtual_bank_id_offset + base_id;
  return bank_id;
}

template <typename DType>
std::shared_ptr<Bank<DType>> Buffer<DType>::GetBank(int id) {
  if (bank_map_.find(id) == bank_map_.end()) {
    return nullptr;
    // UNI_LOG_FATAL(SIM_OUT_OF_RANGE) << "not found bank id: " << id;
  }
  std::shared_ptr<Bank<DType>> bank = bank_map_.at(id);
  return bank;
}

template <typename DType>
void Buffer<DType>::SaveAllBank(int fmt, int inst_no) {
  string save_name = SimCfg::Instance().get_debug_path() + "/bank_all_inst_" +
                     to_string(inst_no) + Util::GetFileNameSuffix(fmt);
  for (auto idx = 0U; idx <= bank_id_max_; idx++) {
    if (bank_map_.find(idx) == bank_map_.end()) continue;

    auto bank = GetBank(idx);
    auto width = bank->GetW();
    auto depth = bank->GetH();
    auto size = depth * width;

    vector<DType> buf(size);
    bank->Read(0, size, buf.data());
    if (fmt <= DATA_FMT_HEX_CONT_BIGEND) {
      Util::SaveData(fmt, save_name, buf.data(), buf.size(), width, SM_APPEND);
    } else if (fmt == DATA_FMT_HEX_CONT_SMALLEND_BANKADDR) {
      Util::SaveHexContSmallEndBankAddr(
          save_name, reinterpret_cast<char*>(buf.data()),
          buf.size() * sizeof(DType), width * sizeof(DType), depth, idx, 0,
          SM_APPEND);
    } else if (fmt == DATA_FMT_HEX_CONT_BIGEND_BANKADDR) {
      Util::SaveHexContBigEndBankAddr(
          save_name, reinterpret_cast<char*>(buf.data()),
          buf.size() * sizeof(DType), width * sizeof(DType), depth, idx, 0,
          SM_APPEND);
    } else {
      UNI_LOG_FATAL(SIM_OUT_OF_RANGE) << "not supported file format";
    }
  }
}

template <typename DType>
void Buffer<DType>::init_bank() {
  if (!bank_init_) return;

  UNI_LOG_CHECK((bank_init_type_ >= DATA_FMT_MIN), SIM_OUT_OF_RANGE)
      << "not supported bank init file type: " << bank_init_type_;

  vector<DType> buf(GetBankSize());

  if (bank_init_type_ >= DATA_FMT_MAX) {
    Util::Random(buf.data(), static_cast<size_t>(buf.size()),
                 static_cast<DType>(-16), static_cast<DType>(16), 123456);
    int offset = 0;
    for (auto idx = 0U; idx <= bank_id_max_; idx++) {
      if (bank_map_.find(idx) == bank_map_.end()) continue;
      auto bank = GetBank(idx);
      auto width = bank->GetW();
      auto depth = bank->GetH();
      auto size = depth * width;

      bank->Write(0, size, buf.data() + offset);
      offset += size;
    }
  } else {
    string line;
    std::fstream f(bank_init_file_);
    Util::ChkOpen(f, bank_init_file_);

    while (!f.eof()) {
      uint64_t line_idx = 0;
      for (auto idx = 0U; idx <= bank_id_max_; idx++) {
        if (bank_map_.find(idx) == bank_map_.end()) continue;
        auto bank = GetBank(idx);
        auto width = bank->GetW();
        auto depth = bank->GetH();

        for (auto k = 0; k < depth; k++) {
          getline(f, line);
          if (line.size()) {
            vector<DType> line_data(width);
            std::stringstream ss;
            ss.clear();
            ss.str("");
            string addr;
            char split;
            string value;
            ss << line;
            line_idx++;
            ss >> addr >> split >> value;
            UNI_LOG_CHECK(value.size() == (size_t)2 * width, SIM_OUT_OF_RANGE)
                << "bank initial line size is not equal to bank width";
            for (int m = 0; m < width; m++) {
              int pos = 0;
              if (bank_init_type_ == DATA_FMT_HEX_CONT_SMALLEND_BANKADDR) {
                pos = 2 * width - 2 * m - 2;
              } else if (bank_init_type_ == DATA_FMT_HEX_CONT_BIGEND_BANKADDR) {
                pos = 2 * m;
              } else {
                UNI_LOG_FATAL(SIM_OUT_OF_RANGE)
                    << " Not support bank_init_type_ " << bank_init_type_
                    << endl;
              }
              int i_val = std::stoi(string(value, pos, 2), nullptr, 16);
              line_data[m] = static_cast<char>(i_val);
            }
            bank->Write(k, width, line_data.data());
          }
        }
      }
    }
    f.close();
  }
}

template <typename DType>
int32_t Buffer<DType>::GetBankNum(const string bank_group_name) {
  UNI_LOG_CHECK(group_map_.find(bank_group_name) != group_map_.end(),
                SIM_OUT_OF_RANGE)
      << "not found bank group: " << bank_group_name;
  return group_map_[bank_group_name].size();
}

template <typename DType>
int32_t Buffer<DType>::GetBankH(const string bank_group_name) {
  UNI_LOG_CHECK(group_map_.find(bank_group_name) != group_map_.end(),
                SIM_OUT_OF_RANGE)
      << "not found bank group: " << bank_group_name;

  return group_map_[bank_group_name][0]->GetH();
}

template <typename DType>
int32_t Buffer<DType>::GetBankW(const string bank_group_name) {
  UNI_LOG_CHECK(group_map_.find(bank_group_name) != group_map_.end(),
                SIM_OUT_OF_RANGE)
      << "not found bank group: " << bank_group_name;
  return group_map_[bank_group_name][0]->GetW();
}

template <typename DType>
int32_t Buffer<DType>::GetBankSizeByGroup(const string bank_group_name) {
  UNI_LOG_CHECK(group_map_.find(bank_group_name) != group_map_.end(),
                SIM_OUT_OF_RANGE)
      << "not found bank group: " << bank_group_name;
  return GetBankNum(bank_group_name) * GetBankH(bank_group_name) *
         GetBankW(bank_group_name);
}

template <typename DType>
int32_t Buffer<DType>::GetBankSize() {
  int bank_size = 0;
  for (auto it = group_map_.begin(); it != group_map_.end(); it++) {
    bank_size += GetBankSizeByGroup(it->first);
  }
  return bank_size;
}

template <typename DType>
shared_ptr<Bank<DType>> Buffer<DType>::GetBank(const string bank_group_name,
                                               const int32_t idx) {
  UNI_LOG_CHECK(group_map_.find(bank_group_name) != group_map_.end(),
                SIM_OUT_OF_RANGE)
      << "not found bank group: " << bank_group_name;
  UNI_LOG_CHECK((unsigned)idx < group_map_[bank_group_name].size(),
                SIM_OUT_OF_RANGE)
      << "unsupported index " << idx << " in bank_group: " << bank_group_name;

  return group_map_[bank_group_name][idx];
}
template class Buffer<DPU_DATA_TYPE>;
