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

#include <functional>
#include "cpu_std_inc.hpp"

namespace vart {
namespace cpu {

constexpr uint64_t make_key(uint32_t size, uint32_t align) {
  uint64_t k = size;

  return (k << 32 | align);
}

struct PairHash {
public:
  template<typename T, typename U>
  size_t operator()(const std::pair<T, U> &x) const {
    auto h1 = std::hash<T>{}(x.first);
    auto h2 = std::hash<U>{}(x.second);

    return h1 ^ h2;
  }

};

}  // namespace cpu
}  // namespace vart
