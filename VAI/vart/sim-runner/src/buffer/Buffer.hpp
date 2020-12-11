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

#include <map>
#include <memory>
#include <string>
#include <vector>
#include "buffer/Bank.hpp"

/**
 * @brief on-chip buffer data structure
 * @details on-chip buffer is arranged as multi-bank structure,
 * for each type of data, the bank configuration is different,
 * which is indicated in the SimCfg.txt file
 */
template <typename DType>
class Buffer {
 public:
  static Buffer& Instance() {
    static Buffer buf;
    return buf;
  }

  void Touch() {
    // UNI_LOG_INFO << "Buffer Instantiating Over!!! ";
  }

 public:
  int32_t GetBankNum() { return bank_num_; }
  int32_t GetBankNum(const std::string bank_group_name);
  int32_t GetBankH(const std::string bank_group_name);
  int32_t GetBankW(const std::string bank_group_name);
  int32_t GetBankSize();
  int32_t GetBankSizeByGroup(const std::string bank_group_name);
  int32_t GetBaseID(const std::string bank_group_name);
  std::shared_ptr<Bank<DType>> GetBank(const std::string bank_group_name,
                                       const int32_t idx);

  std::shared_ptr<Bank<DType>> GetBank(int id);
  uint32_t GetBankIDMax() { return bank_id_max_; }
  void SaveAllBank(int fmt, int inst_no);
  // virtual bank
  void virtual_bank_acquire(const std::string src_bank_group_name);
  int32_t virtual_bank_mapping(const int32_t virtual_bank_id,
                               const std::string bank_group_name);

 private:
  Buffer();
  Buffer(const Buffer&) = delete;
  Buffer& operator=(const Buffer&) = delete;
  void init_bank();

 private:
  int32_t bank_num_;
  uint32_t bank_id_max_;
  bool bank_init_;
  int bank_init_type_;
  int32_t virtual_bank_id_offset{200};
  std::string bank_init_file_;
  std::map<int, std::shared_ptr<Bank<DType>>> bank_map_;
  std::map<std::string, std::vector<std::shared_ptr<Bank<DType>>>> group_map_;
};
