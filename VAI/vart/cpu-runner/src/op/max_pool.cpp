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

#include "max_pool.hpp"

#include <limits>

namespace vart {
namespace cpu {

template <typename DType>
MaxPool<DType>::MaxPool(const xir::Subgraph* subg, const xir::Op* op,
                        IMapTBs_t inputs, CPUTBPtr_t output)
    : PoolBase<DType>(subg, op, inputs, output) {
  pool_type_ = PoolBase<DType>::MAX_POOL;
}

template <typename DType>
void MaxPool<DType>::run() {
  max_pool();
}

template <typename DType>
void MaxPool<DType>::max_pool() {
  if (CPU_RUN_MODE == CPURunMode::NORMAL_THREAD ||
      CPU_RUN_MODE == CPURunMode::GEMM_THREAD) {
    max_pool_thread();
  } else {
    max_pool_normal();
  }
}

template <typename DType>
void MaxPool<DType>::max_pool_normal() {
  for (auto n = 0; n < fmap_o_.n; n++) {
    for (auto h = 0; h < fmap_o_.h; h++) {
      for (auto w = 0; w < fmap_o_.w; w++) {
        max_pool_one(n, h, w);
      }
    }
  }
}

template <typename DType>
void MaxPool<DType>::max_pool_thread() {
  vector<std::future<int>> thr_fut(THREAD_NUM);

  for (auto i = 0U; i < THREAD_NUM; i++) {
    // thr_fut[i] = std::async(std::launch::async | std::launch::deferred,
    thr_fut[i] = std::async(std::launch::async,
                            [this](decltype(i) i) {
                              auto BASE_POS = i * THREAD_WORKLOAD;
                              for (auto j = 0U; j < THREAD_WORKLOAD; j++) {
                                auto pos = BASE_POS + j;
                                if (pos % fmap_o_.c != 0) continue;
                                if (pos >= FMAP_SIZE) break;

                                auto fmap = fmap_o_.pos2coord(pos);
                                max_pool_one(fmap.n, fmap.h, fmap.w);
                              }

                              return 0;
                            },
                            i);
  }

  for (auto i = 0U; i < THREAD_NUM; i++) {
    thr_fut[i].wait();
  }
}

template <typename DType>
void MaxPool<DType>::max_pool_one(int idx_dst_n, int idx_dst_h, int idx_dst_w) {
  auto idx_src_n = idx_dst_n;
  auto idx_src_h = idx_dst_h * stride_.h;
  auto idx_src_w = idx_dst_w * stride_.w;

  auto dst_base = idx_dst_n * fmap_o_.ncod() + idx_dst_h * fmap_o_.hcod() +
                  idx_dst_w * fmap_o_.wcod();

  std::fill_n(data_out_ptr_ + dst_base, fmap_o_.c,
              std::numeric_limits<DType>::lowest());

  for (auto h = 0; h < kernel_.h; h++) {
    for (auto w = 0; w < kernel_.w; w++) {
      // be in special_pad range, not handle
      if (idx_src_h + h >= fmap_i_.h || idx_src_w + w >= fmap_i_.w) continue;

      auto src_base = idx_src_n * fmap_i_.ncod() +
                      (idx_src_h + h) * fmap_i_.hcod() +
                      (idx_src_w + w) * fmap_i_.wcod();
      for (auto c = 0; c < fmap_o_.c; c++) {
        if (data_out_ptr_[dst_base + c] < data_in_ptr_[src_base + c])
          data_out_ptr_[dst_base + c] = data_in_ptr_[src_base + c];
      }
    }
  }
}

INSTANTIATE_TPCLASS(MaxPool);

REG_OP_INSTANCE_FUNC("maxpool2d", MaxPool);

}  // namespace cpu
}  // namespace vart
