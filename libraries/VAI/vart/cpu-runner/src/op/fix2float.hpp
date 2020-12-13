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

#include "fix.hpp"

namespace vart {
namespace cpu {

class Fix2Float : public Fix<float, int32_t> {
public:
  Fix2Float(const xir::Subgraph* subg, const xir::Op* op,
      IMapTBs_t inputs, CPUTBPtr_t output);
  ~Fix2Float() = default;

  virtual void run() override final;

  virtual void read() override final;

private:
  using Fix<float, int32_t>::fix_rlt_ptr_;
  using Fix<float, int32_t>::data_out_ptr_;

};

} // namespace cpu
} // namespace vart


