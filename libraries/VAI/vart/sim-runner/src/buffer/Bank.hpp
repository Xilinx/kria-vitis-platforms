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
#include <vector>

/**
 * @brief bank data structure
 */
template <typename DType>
class Bank {
 public:
  Bank(int bank_id, int bank_h, int bank_w)
      : bank_id_(bank_id),
        bank_h_(bank_h),
        bank_w_(bank_w),
        data_(bank_h_ * bank_w_) {}
  ~Bank(){};

 public:
  int GetID() const { return bank_id_; }
  int GetH() const { return bank_h_; }
  int GetW() const { return bank_w_; }

  void Read(int bank_addr, int ele_num, DType* data);
  void Write(int bank_addr, int ele_num, DType* data, int32_t offset = 0);
  void Save(int inst_no, int fmt);

 private:
  int bank_id_;
  int bank_h_;
  int bank_w_;
  std::vector<DType> data_;
};
