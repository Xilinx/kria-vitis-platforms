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

#include "cpu_std_inc.hpp"
#include "cpu_types.hpp"

namespace vart {
namespace cpu {

using Dilation_t = Stride_t;

class FastPad {
public:
  FastPad(const FMap_t& fmap_src, const FMap_t& fmap_dst, const PadAttr& pad, const Dilation_t& dilation) :
      fmap_src_(fmap_src), fmap_dst_(fmap_dst), pad_(pad), dilation_(dilation) {

    check();

    SRC_SLICE_SIZE = fmap_src_.w * fmap_src_.c;
    DST_SLICE_SIZE = fmap_dst_.w * fmap_dst_.c;

    SRC_DICE_SIZE = fmap_src_.h * SRC_SLICE_SIZE;
    DST_DICE_SIZE = fmap_dst_.h * DST_SLICE_SIZE;
  }
  FastPad(const FMap_t& fmap_src, const FMap_t& fmap_dst, const PadAttr& pad) :
      fmap_src_(fmap_src), fmap_dst_(fmap_dst), pad_(pad) {

    check();

    SRC_SLICE_SIZE = fmap_src_.w * fmap_src_.c;
    DST_SLICE_SIZE = fmap_dst_.w * fmap_dst_.c;

    SRC_DICE_SIZE = fmap_src_.h * SRC_SLICE_SIZE;
    DST_DICE_SIZE = fmap_dst_.h * DST_SLICE_SIZE;
  }

public:
  template<typename T>
  void transform(const T* src, T* dst, T pad_value = T(0)) {
    for(auto i=0; i<fmap_src_.n; i++) {
      pad_one_batch(src + i*SRC_DICE_SIZE, dst + i*DST_DICE_SIZE, pad_value);
    }
  }

private:
  template<typename T>
  void pad_one_batch(const T* src, T* dst, T pad_value = T(0)) {
    if(pad_.l==0 && pad_.t==0 && pad_.r==0 && pad_.b==0 && dilation_.h==1 && dilation_.w==1) {
      std::copy_n(src, SRC_DICE_SIZE, dst);
      return;
    }

    handle_pad_around(src, dst, pad_value);

    // copy rest data
    // NOTE: YES, if not considering dilation,
    // the first if statement is enough,
    // but ehhhhh........
    if(dilation_.h==1 && dilation_.w==1) {
      handle_dilation_11(src, dst);
    } else if(dilation_.h==1 && dilation_.w!=1) {
      handle_dilation_1w(src, dst);
    } else if(dilation_.h!=1 && dilation_.w==1) {
      handle_dilation_h1(src, dst);
    } else {
      handle_dilation_hw(src, dst);
    }
  }

  template<typename T>
  void handle_pad_around(const T* src, T* dst, T pad_value = T(0)) {
    // pad top and bottom first
    if(pad_.t > 0) {
      std::fill_n(dst, pad_.t * DST_SLICE_SIZE, pad_value);
    }
    if(pad_.b > 0) {
      std::fill_n(dst + (fmap_dst_.h - pad_.b) * DST_SLICE_SIZE,
        pad_.b * DST_SLICE_SIZE, pad_value);
    }

    // pad left and right
    if(pad_.l > 0) {
      for(auto h=pad_.t; h<fmap_dst_.h-pad_.b; h++) {
        auto offset = h * DST_SLICE_SIZE;
        std::fill_n(dst + offset, pad_.l * fmap_dst_.c, pad_value);
      }
    }
    if(pad_.r > 0) {
      for(auto h=pad_.t; h<fmap_dst_.h-pad_.b; h++) {
        auto offset = h * DST_SLICE_SIZE + (fmap_dst_.w - pad_.r) * fmap_dst_.c;
        std::fill_n(dst + offset, pad_.r * fmap_dst_.c, pad_value);
      }
    }
  }

  template<typename T>
  void handle_dilation_11(const T* src, T* dst) {
    for(auto h=pad_.t; h<fmap_dst_.h-pad_.b; h++) {
      auto src_offset = (h-pad_.t) * SRC_SLICE_SIZE;
      auto dst_offset = h * DST_SLICE_SIZE + pad_.l * fmap_dst_.c;
      std::copy_n(src + src_offset, SRC_SLICE_SIZE, dst + dst_offset);
    }
  }

  template<typename T>
  void handle_dilation_1w(const T* src, T* dst) {
    for(auto h=pad_.t; h<fmap_dst_.h-pad_.b; h++) {
      auto src_offset_base = (h-pad_.t)/dilation_.h * SRC_SLICE_SIZE;
      auto dst_offset_base = h * DST_SLICE_SIZE;
      for(auto w=pad_.l; w<fmap_dst_.w-pad_.r;) {
        if((w-pad_.l) % dilation_.w == 0) { // normal data
          auto src_offset = src_offset_base + (w-pad_.l) / dilation_.w * fmap_src_.c;
          auto dst_offset = dst_offset_base + w * fmap_dst_.c;
          std::copy_n(src + src_offset, fmap_src_.c, dst + dst_offset);
          w += 1;
        } else { // dilation data, it's 0
          auto offset = dst_offset_base + w * fmap_dst_.c;
          std::fill_n(dst + offset, (dilation_.w-1) * fmap_dst_.c, T(0));
          w += (dilation_.w-1);
        }
      }
    }
  }

  template<typename T>
  void handle_dilation_h1(const T* src, T* dst) {
    for(auto h=pad_.t; h<fmap_dst_.h-pad_.b; h++) {
      if((h-pad_.t) % dilation_.h == 0) { // normal data
        auto src_offset = (h-pad_.t) / dilation_.h * SRC_SLICE_SIZE;
        auto dst_offset = h * DST_SLICE_SIZE + pad_.l * fmap_dst_.c;
        std::copy_n(src + src_offset, SRC_SLICE_SIZE, dst + dst_offset);
      } else { // dilation data, it's 0
        auto offset = h * DST_SLICE_SIZE + pad_.l * fmap_dst_.c;
        std::fill_n(dst + offset, (fmap_dst_.w - pad_.l - pad_.r) * fmap_dst_.c, T(0));
      }
    }
  }

  template<typename T>
  void handle_dilation_hw(const T* src, T* dst) {
    for(auto h=pad_.t; h<fmap_dst_.h-pad_.b; h++) {
      if((h-pad_.t) % dilation_.h != 0) { // dilation data
        auto offset = h * DST_SLICE_SIZE + pad_.l * fmap_dst_.c;
        std::fill_n(dst + offset, (fmap_dst_.w - pad_.l - pad_.r) * fmap_dst_.c, T(0));
        continue;
      }

      auto src_offset_base = (h-pad_.t)/dilation_.h * SRC_SLICE_SIZE;
      auto dst_offset_base = h * DST_SLICE_SIZE;
      for(auto w=pad_.l; w<fmap_dst_.w-pad_.r;) {
        if((w-pad_.l) % dilation_.w == 0) { // normal data
          auto src_offset = src_offset_base + (w-pad_.l) / dilation_.w * fmap_src_.c;
          auto dst_offset = dst_offset_base + w * fmap_dst_.c;
          std::copy_n(src + src_offset, fmap_src_.c, dst + dst_offset);
          w += 1;
        } else { // dilation data, it's 0
          auto offset = dst_offset_base + w * fmap_dst_.c;
          std::fill_n(dst + offset, (dilation_.w-1) * fmap_dst_.c, T(0));
          w += (dilation_.w-1);
        }
      }
    }
  }

  void check() {
    if((fmap_src_.n != fmap_dst_.n) ||
        (fmap_src_.h * dilation_.h - (dilation_.h - 1) + pad_.t + pad_.b != fmap_dst_.h) ||
        (fmap_src_.w * dilation_.w - (dilation_.w - 1) + pad_.l + pad_.r != fmap_dst_.w) ||
        (fmap_src_.c != fmap_dst_.c)) {
      abort();
    }
  }

private:
  FMap_t fmap_src_;
  FMap_t fmap_dst_;
  Pad_t pad_;
  Dilation_t dilation_{1, 1};

  int SRC_SLICE_SIZE;
  int DST_SLICE_SIZE;

  int SRC_DICE_SIZE;
  int DST_DICE_SIZE;

};


}  // namespace cpu
}  // namespace vart
