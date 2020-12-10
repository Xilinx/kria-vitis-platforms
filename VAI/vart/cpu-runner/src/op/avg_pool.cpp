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

#include "avg_pool.hpp"

namespace vart {
namespace cpu {

template <typename DType>
AvgPool<DType>::AvgPool(const xir::Subgraph* subg, const xir::Op* op,
                        IMapTBs_t inputs, CPUTBPtr_t output)
    : PoolBase<DType>(subg, op, inputs, output) {
  pool_type_ = PoolBase<DType>::AVG_POOL;
  count_include_pad_ =
    GET_OP_ATTR_WITH_DFT<bool>(op, "count_include_pad", true);
  count_include_invalid_ =
    GET_OP_ATTR_WITH_DFT<bool>(op, "count_include_invalid", true);
}

template <typename DType>
void AvgPool<DType>::run() {
  // // do necessary check
  // print_param();
  // this->check_param();

  // // read data
  // this->read();

  // // calc avgpool
  acc_pool();
  avg_pool();

  // // do save, debug...
  // this->save();
}

template <typename DType>
void AvgPool<DType>::print_param() {
  PoolBase<DType>::print_param();

  UNI_LOG_DEBUG_INFO << "count_include_pad = " << Bool2Str(count_include_pad_)
                     << endl;
  UNI_LOG_DEBUG_INFO << "count_include_invalid = " << Bool2Str(count_include_invalid_)
                     << endl;
}

template <typename DType>
void AvgPool<DType>::acc_pool() {
  if (CPU_RUN_MODE == CPURunMode::NORMAL_THREAD ||
      CPU_RUN_MODE == CPURunMode::GEMM_THREAD) {
    acc_pool_thread();
  } else {
    acc_pool_normal();
  }
}

template <typename DType>
void AvgPool<DType>::acc_pool_normal() {
  for (auto n = 0; n < fmap_o_.n; n++) {
    for (auto h = 0; h < fmap_o_.h; h++) {
      for (auto w = 0; w < fmap_o_.w; w++) {
        acc_pool_one(n, h, w);
      }
    }
  }
}

template <typename DType>
void AvgPool<DType>::acc_pool_thread() {
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
                                acc_pool_one(fmap.n, fmap.h, fmap.w);
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
void AvgPool<DType>::acc_pool_one(int idx_dst_n, int idx_dst_h, int idx_dst_w) {
  // init result value
  int idx_src_n = idx_dst_n;
  int idx_src_h = idx_dst_h * stride_.h;
  int idx_src_w = idx_dst_w * stride_.w;

  auto dst_base = idx_dst_n * fmap_o_.ncod() + idx_dst_h * fmap_o_.hcod() +
                  idx_dst_w * fmap_o_.wcod();
  std::fill_n(data_out_ptr_ + dst_base, fmap_o_.c, 0);

  // accumulate one pool kernel
  for (auto h = 0; h < kernel_.h; h++) {
    for (auto w = 0; w < kernel_.w; w++) {
      // be in special_pad range, not handle
      if (idx_src_h + h >= fmap_i_.h || idx_src_w + w >= fmap_i_.w) continue;

      auto src_base = idx_src_n * fmap_i_.ncod() +
                      (idx_src_h + h) * fmap_i_.hcod() +
                      (idx_src_w + w) * fmap_i_.wcod();

      for (auto c = 0; c < fmap_o_.c; c++) {
        data_out_ptr_[dst_base + c] += data_in_ptr_[src_base + c];
      }
    }
  }
}

template <typename DType>
void AvgPool<DType>::avg_pool() {
  if (!std::is_floating_point<DType>::value) {
    UNI_LOG_ERROR(VART_NOT_SUPPORT)
        << "Only support float data type in avgpool!" << endl;
    abort();
  }
  avg_pool_normal();


  // if (CPU_RUN_MODE == CPURunMode::NORMAL_THREAD ||
  //     CPU_RUN_MODE == CPURunMode::GEMM_THREAD) {
  //   avg_pool_thread();
  // } else {
  // }
}

template <typename DType>
void AvgPool<DType>::avg_pool_normal() {
  for (auto n = 0; n < fmap_o_.n; n++) {
    for (auto h = 0; h < fmap_o_.h; h++) {
      for (auto w = 0; w < fmap_o_.w; w++) {
        avg_pool_one(n, h, w);
      }
    }
  }
}

template <typename DType>
void AvgPool<DType>::avg_pool_thread() {
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
                                avg_pool_one(fmap.n, fmap.h, fmap.w);
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
void AvgPool<DType>::avg_pool_one(int idx_dst_n, int idx_dst_h, int idx_dst_w) {
  // init result value
  int idx_src_h = idx_dst_h * stride_.h;
  int idx_src_w = idx_dst_w * stride_.w;

  double factor;
  if (count_include_pad_) {
    int fw = kernel_.w, fh = kernel_.h;
    if (!count_include_invalid_) {
      if ((idx_src_w + kernel_.w) > (raw_fmap_i_.w + pad_.l + pad_.r))
        fw = raw_fmap_i_.w + pad_.l + pad_.r - idx_src_w;
      if ((idx_src_h + kernel_.h) > (raw_fmap_i_.h + pad_.t + pad_.b))
        fh = raw_fmap_i_.h + pad_.t + pad_.b - idx_src_h;
    }
    factor = 1.0 / fw / fh;
  } else {
    int fw = kernel_.w, fh = kernel_.h;
    if (idx_src_w < pad_.l)
      fw = kernel_.w - (pad_.l - idx_src_w);
    else if ((idx_src_w + kernel_.w) > (raw_fmap_i_.w + pad_.l))
      fw = raw_fmap_i_.w + pad_.l - idx_src_w;
    if (idx_src_h < pad_.t)
      fh = kernel_.h - (pad_.t - idx_src_h);
    else if ((idx_src_h + kernel_.h) > (raw_fmap_i_.h + pad_.t))
      fh = raw_fmap_i_.h + pad_.t - idx_src_h;
    factor = 1.0 / fw / fh;
  }

  auto addr_base = idx_dst_n * fmap_o_.ncod() + idx_dst_h * fmap_o_.hcod() +
                   idx_dst_w * fmap_o_.wcod();
  for (auto i = 0; i < fmap_o_.c; i++) {
    data_out_ptr_[addr_base + i] *= factor;
  }
}

INSTANTIATE_TPCLASS(AvgPool);

REG_OP_INSTANCE_FUNC("avgpool2d", AvgPool);

}  // namespace cpu
}  // namespace vart
