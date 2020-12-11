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
#include "buffer/Bank.hpp"
#include "SimCfg.hpp"
#include "UniLog/UniLog.hpp"
#include "util/Util.hpp"

template <typename DType>
void Bank<DType>::Read(int bank_addr, int ele_num, DType* data) {
  UNI_LOG_CHECK(data != nullptr, SIM_NULL_PTR)
      << "Bank read: the target data ptr is null!";

  int ele_start = (bank_addr % bank_h_) * bank_w_;
  uint32_t bank_size = bank_h_ * bank_w_;
  for (auto i = 0; i < ele_num; i++) {
    data[i] = data_[(ele_start + i) % bank_size];
  }
}

template <typename DType>
void Bank<DType>::Write(int bank_addr, int ele_num, DType* data,
                        int32_t offset) {
  UNI_LOG_CHECK(data != nullptr, SIM_NULL_PTR)
      << "Bank write: the source data ptr is null!";
  UNI_LOG_CHECK(offset <= bank_w_, SIM_OUT_OF_RANGE)
      << "Bank write: offset is larger than bank width!";

  uint32_t ele_start = (bank_addr % bank_h_) * bank_w_ + offset;
  uint32_t bank_size = bank_h_ * bank_w_;
  for (auto i = 0; i < ele_num; i++) {
    data_[ele_start++ % bank_size] = data[i];
  }
  uint32_t align_size = (bank_w_ - (ele_num + offset) % bank_w_) % bank_w_;
  //  cout << ele_num << ", " << offset << ", " << bank_w_ << ", " << align_size
  //   << endl;
  for (auto i = 0U; i < align_size; i++) {
    data_[(ele_start++) % bank_size] = 0;
  }
}

template <typename DType>
void Bank<DType>::Save(int inst_no, int fmt) {
  UNI_LOG_CHECK(fmt >= DATA_FMT_MIN && fmt < DATA_FMT_MAX, SIM_OUT_OF_RANGE)
      << "Bank save: not supported file format!";

  string save_name = SimCfg::Instance().get_debug_path() + "bank_" +
                     to_string(bank_id_) + "_inst_" + to_string(inst_no) +
                     Util::GetFileNameSuffix(fmt);

  if (fmt <= DATA_FMT_HEX_CONT_BIGEND) {
    Util::SaveData(fmt, save_name, data_.data(), data_.size(), bank_w_);
  } else if (fmt == DATA_FMT_HEX_CONT_SMALLEND_BANKADDR) {
    Util::SaveHexContSmallEndBankAddr(
        save_name, reinterpret_cast<const char*>(data_.data()),
        data_.size() * sizeof(DType), bank_w_ * sizeof(DType));
  } else if (fmt == DATA_FMT_HEX_CONT_BIGEND_BANKADDR) {
    Util::SaveHexContBigEndBankAddr(
        save_name, reinterpret_cast<const char*>(data_.data()),
        data_.size() * sizeof(DType), bank_w_ * sizeof(DType));
  }
}

template class Bank<DPU_DATA_TYPE>;
