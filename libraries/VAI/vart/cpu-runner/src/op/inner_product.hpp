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

#include "cpu_op_base.hpp"

namespace vart {
namespace cpu {


template<typename DType, typename TmpDType =
  std::enable_if_t<std::is_arithmetic<DType>::value && !std::is_same<bool, DType>::value,
    typename std::conditional<
      std::is_floating_point<DType>::value,
      DType,
      typename std::conditional<
        std::is_signed<DType>::value,
        int64_t,
        uint64_t
      >::type
    >::type
  >
>
class InnerProduct : public CPUOPBase {
public:
  enum InputTensorType {
    INPUT,
    WEIGHTS,
    BIAS,
  };
  // input tensor name
  const static vector<string> ITName;

public:
  InnerProduct(const xir::Subgraph* subg, const xir::Op* op,
      IMapTBs_t inputs, CPUTBPtr_t output);
  ~InnerProduct() = default;

  virtual void run() override final;

  virtual void print_param() override final;
  virtual void check_param() override final;

  virtual void read() override final;

  virtual uint64_t get_workload() override final;

private:
  void inner_product();
  void inner_product_one(int idx_dst_n, int idx_dst_h, int idx_dst_w, int idx_oc);
  void bias();

private:
  FMap_t fmap_i_;
  FMap_t fmap_w_;
  FMap_t fmap_b_;
  FMap_t fmap_o_;

  std::vector<int> in_shape_;
  std::vector<int> weights_shape_;
  bool has_bias_;
  int axis_;

  // caculate buffer
  // DDR and Bank are all with stride, but caculate buffer don't have
  DType* img_{nullptr};
  DType* weights_{nullptr};
  DType* bias_{nullptr};
  DType* rlt_{nullptr};
};

} // namespace cpu
} // namespace vart

