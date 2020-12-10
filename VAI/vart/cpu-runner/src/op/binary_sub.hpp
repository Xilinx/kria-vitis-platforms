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

#include "binary_base.hpp"

namespace vart {
namespace cpu {

template<typename DType>
class BinarySub : public BinaryBase<DType> {
public:
  BinarySub(const xir::Subgraph* subg, const xir::Op* op,
      IMapTBs_t inputs, CPUTBPtr_t output)
    : BinaryBase<DType>(subg, op, inputs, output) {
      BinaryBase<DType>::binary_type_ = BinaryBase<DType>::SUB;
  }
  virtual ~BinarySub() = default;

protected:
  virtual void calculate() override final;

protected:
  using BinaryBase<DType>::fmap_i_;
  using BinaryBase<DType>::fmap_o_;
  using BinaryBase<DType>::data_in_ptr_;
  using BinaryBase<DType>::data_out_ptr_;
};

} // namespace cpu
} // namespace vart


