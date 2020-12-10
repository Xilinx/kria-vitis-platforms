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

#include "pool_base.hpp"

namespace vart {
namespace cpu {

template<typename DType>
class AvgPool : virtual public PoolBase<DType> {
public:
  AvgPool(const xir::Subgraph* subg, const xir::Op* op,
      IMapTBs_t inputs, CPUTBPtr_t output);
  virtual ~AvgPool() = default;

  virtual void run() override;

  virtual void print_param() override;

protected:

  void acc_pool();
  void acc_pool_normal();
  void acc_pool_thread();
  inline void acc_pool_one(int idx_dst_n, int idx_dst_h, int idx_dst_w);

  void avg_pool();
  void avg_pool_normal();
  void avg_pool_thread();
  inline void avg_pool_one(int idx_dst_n, int idx_dst_h, int idx_dst_w);

protected:
  bool count_include_pad_{true};
  bool count_include_invalid_{true};

  using PoolBase<DType>::pool_type_;
  using PoolBase<DType>::raw_fmap_i_;
  using PoolBase<DType>::fmap_i_;
  using PoolBase<DType>::fmap_o_;
  using PoolBase<DType>::raw_pad_;
  using PoolBase<DType>::pad_;
  using PoolBase<DType>::kernel_;
  using PoolBase<DType>::stride_;
  using PoolBase<DType>::is_global_;
  using PoolBase<DType>::data_in_ptr_;
  using PoolBase<DType>::data_out_ptr_;

  using PoolBase<DType>::THREAD_NUM;
  using PoolBase<DType>::THREAD_WORKLOAD;
  using PoolBase<DType>::FMAP_SIZE;
};

} // namespace cpu
} // namespace vart

