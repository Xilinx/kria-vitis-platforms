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
#include "vart/tensor_buffer.hpp"

namespace vart {
namespace sim {

template <typename DType>
class SimTensorBuffer : public TensorBuffer {
 public:
  explicit SimTensorBuffer(const xir::Tensor* tensor);
  virtual ~SimTensorBuffer() = default;
  SimTensorBuffer(const SimTensorBuffer& other) = delete;
  SimTensorBuffer& operator=(const SimTensorBuffer& rhs) = delete;

 public:
  virtual std::pair<std::uint64_t, std::size_t> data(
      const std::vector<std::int32_t> index = {}) override final;

 private:
  std::vector<DType> buf_;
};

}  // namespace sim
}  // namespace vart
