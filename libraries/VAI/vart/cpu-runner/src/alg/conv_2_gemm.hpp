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

#include "cpu_base_inc.hpp"

namespace vart {
namespace cpu {

class Conv2Gemm {
public:
  Conv2Gemm(const FMap_t& fmap_src, const FMap_t& fmap_dst,
      const Kernel_t kernel, const Stride_t& stride) :
      fmap_src_(fmap_src),
      fmap_dst_(fmap_dst),
      kernel_(kernel),
      stride_(stride) {

    if(fmap_src_.n != fmap_dst_.n ||
        fmap_dst_.c != kernel_.h * kernel_.w * fmap_src_.c) {
      std::cerr << "fmap err!" << endl;
      abort();
    }


    SRC_SLICE_SIZE = fmap_src_.w * fmap_src_.c;
    DST_SLICE_SIZE = fmap_dst_.w * fmap_dst_.c;

    SRC_BATCH_SIZE = fmap_src_.h * SRC_SLICE_SIZE;
    DST_BATCH_SIZE = fmap_dst_.h * DST_SLICE_SIZE;
  }

  VART_BIG_THREE_LAW(Conv2Gemm);

public:
  template<typename T>
  void transform(const T* src, T* dst) {
    for(auto n=0; n<fmap_dst_.n; n++) {
      trans_one_batch(src + n*SRC_BATCH_SIZE, dst + n*DST_BATCH_SIZE);
    }
  }

private:
  template<typename T>
  void trans_one_batch(const T* src, T* dst) {
    for(auto h=0; h<fmap_dst_.h; h++) {
      for(auto w=0; w<fmap_dst_.w; w++) {
        auto coord_src_h = h * stride_.h;
        auto coord_src_w = w * stride_.w;

        auto *src_addr_base = src + coord_src_h * SRC_SLICE_SIZE + coord_src_w * fmap_src_.c;
        auto *dst_addr_base = dst + h * DST_SLICE_SIZE + w * fmap_dst_.c;

        trans_one_kernel(src_addr_base, dst_addr_base, h, w);
      }
    }
  }

  template<typename T>
  void trans_one_kernel(const T* src, T* dst, int h, int w) {

    for(auto k=0; k<kernel_.h; k++) {
      auto *src_addr = src + k * SRC_SLICE_SIZE;
      auto *dst_addr = dst + k * kernel_.w * fmap_src_.c;

      std::copy_n(src_addr, kernel_.w * fmap_src_.c, dst_addr);
    }
  }

private:
  FMap_t fmap_src_;
  FMap_t fmap_dst_;
  Kernel_t kernel_;
  Stride_t stride_{1, 1};

  int SRC_SLICE_SIZE;
  int DST_SLICE_SIZE;

  int SRC_BATCH_SIZE;
  int DST_BATCH_SIZE;
};


} // namespace cpu
} // namespace vart
