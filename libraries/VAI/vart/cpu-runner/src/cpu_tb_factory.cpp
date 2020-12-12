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

#include "cpu_tb_factory.hpp"

namespace vart {
namespace cpu {

CPUTBPtr_t CPUTBFactory::create_or_get(const xir::Op* op) {
  auto op_name = op->get_name();

  std::lock_guard<std::recursive_mutex> lock(mtx_);

  // if existed, return directly
  auto *lookup_ptr = get_by_op_name(op_name);
  if(lookup_ptr != nullptr)
    return lookup_ptr;

  auto* tensor = op->get_output_tensor();
  auto tensor_name = tensor->get_name();

  // create new cpu tensor buffer
  // and update three related data structure
  auto tb = CPUTensorBuffer::make(op, tensor);
  lookup_ptr = tb.get();

  tbs_.push_back(std::move(tb));
  op_name_map_[op_name] = lookup_ptr;
  tensor_name_map_[tensor_name] = lookup_ptr;

  if (VART_DEBUG) {
    UNI_LOG_DEBUG_INFO << endl;
    UNI_LOG_DEBUG_INFO << "Created op " << op_name
                       << "\'s output CPUTensorBuffer:" << endl;
    PRINT_DIVIDING_LINE();
    print_cpu_tb(lookup_ptr);
  }

  return lookup_ptr;
}

} // namespace cpu
} // namespace vart
