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

#include "cpu_tensor_buffer.hpp"

namespace vart {
namespace cpu {

class CPUTBFactory {
private:
  explicit CPUTBFactory() = default;
  VART_BIG_THREE_LAW(CPUTBFactory);

public:
  static CPUTBFactory& Instance() {
    static CPUTBFactory fac;
    return fac;
  }

  CPUTBPtr_t create_or_get(const xir::Op* op);

  inline CPUTBPtr_t get_by_op_name(const string& op_name) {
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    if (op_name_map_.find(op_name) != op_name_map_.end())
      return op_name_map_.at(op_name);
    return nullptr;
  }
  inline CPUTBPtr_t get_by_tensor(const xir::Tensor* tensor) {
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    auto tensor_name = tensor->get_name();
    return get_by_tensor_name(tensor_name);
  }
  inline CPUTBPtr_t get_by_tensor_name(const string& tensor_name) {
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    if (tensor_name_map_.find(tensor_name) != tensor_name_map_.end())
      return tensor_name_map_.at(tensor_name);
    return nullptr;
  }

private:
  // using mutex to make sure this class is thread-safe,
  // and in create_or_get func, it will call other routines,
  // so should use a recursive mutex to prevent dead-lock
  std::recursive_mutex mtx_;

  // use list to manage created TBs
  list<unique_ptr<CPUTensorBuffer>> tbs_;

  // key: op name, value is tbs_ related element's raw pointer
  map<string, CPUTensorBuffer*> op_name_map_;
  // key: tensor name, value is tbs_ related element's raw pointer
  map<string, CPUTensorBuffer*> tensor_name_map_;
};

} // namespace cpu
} // namespace vart
