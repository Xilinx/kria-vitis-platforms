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
#ifndef __BANK_SHELL_HPP__
#define __BANK_SHELL_HPP__

#include "Buffer.hpp"
#include "common.hpp"

class BankShell {
 public:
  BankShell() = delete;
  ~BankShell() = delete;

 public:
  static void read(bool is_8_bit, int32_t bank_id, int32_t channel,
                   int32_t addr, DPU_DATA_TYPE* data) {
    auto real_size = is_8_bit ? 2 * channel : channel;
    auto real_addr = is_8_bit ? 2 * addr : addr;
    vector<DPU_DATA_TYPE> data_buf(real_size);
    auto bank = Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id);
    UNI_LOG_CHECK(bank != nullptr, SIM_OUT_OF_RANGE)
        << "bank_id " << bank_id << " out of range!" << endl;

    // read
    bank->Read(real_addr, real_size, data_buf.data());

    for (auto idx_channel = 0; idx_channel < channel; idx_channel++) {
      DPU_DATA_TYPE tmp;
      if (is_8_bit) {
        auto lo = static_cast<uint8_t>(data_buf[2 * idx_channel]);
        auto hi = static_cast<uint8_t>(data_buf[2 * idx_channel + 1]);
        tmp = static_cast<DPU_DATA_TYPE>((hi << 4) | (0x0F & lo));

      } else {
        auto lo = static_cast<uint8_t>(data_buf[idx_channel]);
        tmp = static_cast<DPU_DATA_TYPE>(0x0F & lo);
      }
      data[idx_channel] = tmp;
    }
  }

  static void write(bool is_8_bit, int32_t bank_id, int32_t channel,
                    int32_t addr, DPU_DATA_TYPE* data) {
    auto real_size = is_8_bit ? 2 * channel : channel;
    auto real_addr = is_8_bit ? 2 * addr : addr;
    vector<DPU_DATA_TYPE> data_buf(real_size);
    auto bank = Buffer<DPU_DATA_TYPE>::Instance().GetBank(bank_id);
    UNI_LOG_CHECK(bank != nullptr, SIM_OUT_OF_RANGE)
        << "bank_id " << bank_id << " out of range!" << endl;
    // write
    for (auto idx_channel = 0; idx_channel < channel; idx_channel++) {
      DPU_DATA_TYPE tmp = static_cast<uint8_t>(data[idx_channel]);
      if (is_8_bit) {
        data_buf[2 * idx_channel] = static_cast<DPU_DATA_TYPE>(0x0F & tmp);
        data_buf[2 * idx_channel + 1] = static_cast<DPU_DATA_TYPE>(tmp >> 4);
        if ((0x08 & data_buf[2 * idx_channel]) == 0x08)
          data_buf[2 * idx_channel] =
              static_cast<DPU_DATA_TYPE>(0xF0 | data_buf[2 * idx_channel]);
      } else {
        data_buf[idx_channel] = static_cast<DPU_DATA_TYPE>(0x0F & tmp);
        if ((0x08 & data_buf[idx_channel]) == 0x08)
          data_buf[idx_channel] =
              static_cast<DPU_DATA_TYPE>(0xF0 | data_buf[idx_channel]);
      }
    }
    bank->Write(real_addr, real_size, data_buf.data());
  }
};

#endif /* __BANK_SHELL_HPP__ */
