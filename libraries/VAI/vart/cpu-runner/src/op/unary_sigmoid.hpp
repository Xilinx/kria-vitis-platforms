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

#include "unary_base.hpp"

namespace vart {
namespace cpu {

template<typename DType>
class UnarySigmoid : public UnaryBase<DType> {
public:
  UnarySigmoid(const xir::Subgraph* subg, const xir::Op* op,
      IMapTBs_t inputs, CPUTBPtr_t output)
    : UnaryBase<DType>(subg, op, inputs, output) {
      UnaryBase<DType>::unary_type_ = UnaryBase<DType>::SIGMOID;
  }
  virtual ~UnarySigmoid() = default;

protected:
  virtual void calculate() override final;

protected:
  using UnaryBase<DType>::fmap_i_;
  using UnaryBase<DType>::fmap_o_;
  using UnaryBase<DType>::data_in_ptr_;
  using UnaryBase<DType>::data_out_ptr_;
};

} // namespace cpu
} // namespace vart


