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

namespace vart {
namespace cpu {

struct FMapAttr;
using fmap_coord_t = FMapAttr;

typedef struct FMapAttr {
  int n;
  int h;
  int w;
  int c;

  FMapAttr() : n(0), h(0), w(0), c(0) {}
  FMapAttr(int var_n, int var_h, int var_w, int var_c)
      : n(var_n), h(var_h), w(var_w), c(var_c) {}
  FMapAttr(const vector<int>& dims) {
    auto DIM_SIZE = dims.size();
    n = h = w = c = 1;

    if(DIM_SIZE>=1 && DIM_SIZE<=4) {
      c = dims[DIM_SIZE-1];
      if(DIM_SIZE >= 2) { w = dims[DIM_SIZE-2]; }
      if(DIM_SIZE >= 3) { h = dims[DIM_SIZE-3]; }
      if(DIM_SIZE >= 4) { n = dims[DIM_SIZE-4]; }
    } else {
      UNI_LOG_ERROR(VART_NOT_SUPPORT) << "Not support dim: "
        << Vec2Str(dims, ", ") << endl;
      abort();
    }
  }

  int num() const { return n * h * w * c; }
  int ncod() const { return h * w * c; }
  int hcod() const { return w * c; }
  int wcod() const { return c; }
  int ccod() const { return 1; }

  inline int coord2pos(const fmap_coord_t& coord) const {
    auto pos = 0;

    pos += coord.n * ncod();
    pos += coord.h * hcod();
    pos += coord.w * wcod();
    pos += coord.c * ccod();

    return pos;
  }
  inline fmap_coord_t pos2coord(int pos) const {
    fmap_coord_t coord;

    coord.n = pos/ncod();
    coord.h = (pos%ncod())/hcod();
    coord.w = ((pos%ncod())%hcod())/wcod();
    coord.c = (((pos%ncod())%hcod())%wcod())/ccod();

    return coord;
  }

  void print_param(const string& head = "fmap") const {
    UNI_LOG_DEBUG_INFO << head << ": [" << "n: " << n
      << ", h: " << h << ", w: " << w << ", c: " << c << "]" << endl;
  }

  bool operator==(const FMapAttr& other) {
    if (n == other.n
        && h == other.h
        && w == other.w
        && c == other.c) {
      return true;
    }

    return false;
  }
} FMap_t;

typedef struct KernelAttr {
  int ori_h;
  int ori_w;
  int dilation_h;
  int dilation_w;
  int h;
  int w;

  KernelAttr() : ori_h(1), ori_w(1), dilation_h(1), dilation_w(1), h(1), w(1) {}
  KernelAttr(int var_h, int var_w)
      : ori_h(var_h),
        ori_w(var_w),
        dilation_h(1),
        dilation_w(1),
        h(var_h),
        w(var_w) {}
  KernelAttr(int var_h, int var_w, int var_dilation)
      : ori_h(var_h),
        ori_w(var_w),
        dilation_h(var_dilation),
        dilation_w(var_dilation) {
    h = ori_h * dilation_h - (dilation_h - 1);
    w = ori_w * dilation_w - (dilation_w - 1);
  }
  KernelAttr(int var_h, int var_w, int var_dilation_h, int var_dilation_w)
      : ori_h(var_h),
        ori_w(var_w),
        dilation_h(var_dilation_h),
        dilation_w(var_dilation_w) {
    h = ori_h * dilation_h - (dilation_h - 1);
    w = ori_w * dilation_w - (dilation_w - 1);
  }

  void print_param(const string& head = "kernel") const {
    UNI_LOG_DEBUG_INFO << head << ": ["
                     << "ori_h: " << ori_h << ", ori_w: " << ori_w
                     << ", dilation_h: " << dilation_h
                     << ", dilation_w: " << dilation_w << ", h: " << h
                     << ", w: " << w << "]" << endl;
  }

  bool operator==(const KernelAttr& other) {
    if (ori_h == other.ori_h
        && ori_w == other.ori_w
        && dilation_h == other.dilation_h
        && dilation_w == other.dilation_w
        && h == other.h
        && w == other.w) {
      return true;
    }

    return false;
  }

} Kernel_t;

typedef struct StrideAttr {
  int h;
  int w;

  StrideAttr() : h(1), w(1) {}
  StrideAttr(int var_h, int var_w) : h(var_h), w(var_w) {}

  void print_param(const string& head = "stride") const {
    UNI_LOG_DEBUG_INFO << head << ": ["
                     << "h: " << h << ", w: " << w << "]" << endl;
  }
  bool operator==(const StrideAttr& other) {
    if (h == other.h && w == other.w) {
      return true;
    }

    return false;
  }

} Stride_t;

typedef struct ScaleAttr {
  int h;
  int w;

  ScaleAttr() : h(1), w(0) {}
  ScaleAttr(int var_h, int var_w) : h(var_h), w(var_w) {}

  void print_param(const string& head = "scale") const {
    UNI_LOG_DEBUG_INFO << head << ": ["
                     << "h: " << h << ", w: " << w << "]" << endl;
  }
  void check_same(const ScaleAttr& other) const {
    UNI_LOG_CHECK(h == other.h, VART_INVALID_VALUE) << ", " << h << " != " << other.h;
    UNI_LOG_CHECK(w == other.w, VART_INVALID_VALUE) << ", " << w << " != " << other.w;
  }
} Scale_t;

typedef struct PadAttr {
  enum {
    PAD_MODE_FLOOR,
    PAD_MODE_CEIL,
    PAD_MODE_SAME,   // make output with same width and height as input
    PAD_MODE_VALID,  // no padding
    PAD_MODE_CONSTANT,
    PAD_MODE_REFLECT,
    PAD_MODE_SYMMETRIC,
  };
  int mode;
  int l;  // left
  int t;  // top
  int r;  // right
  int b;  // bottom

  PadAttr() : mode(PAD_MODE_FLOOR), l(0), t(0), r(0), b(0) {}
  PadAttr(int var_l, int var_t, int var_r, int var_b)
      : mode(PAD_MODE_FLOOR), l(var_l), t(var_t), r(var_r), b(var_b) {}
  PadAttr(int var_mode, int var_l, int var_t, int var_r, int var_b)
      : mode(var_mode), l(var_l), t(var_t), r(var_r), b(var_b) {}
  PadAttr(std::string var_mode, int var_l, int var_t, int var_r, int var_b)
      : l(var_l), t(var_t), r(var_r), b(var_b) {
    if (var_mode == "FLOOR")
      mode = PAD_MODE_FLOOR;
    else if (var_mode == "CEIL")
      mode = PAD_MODE_CEIL;
    else if (var_mode == "SAME")
      mode = PAD_MODE_SAME;
    else if (var_mode == "VALID")
      mode = PAD_MODE_VALID;
    else if (var_mode == "CONSTANT")
      mode = PAD_MODE_CONSTANT;
    else if (var_mode == "REFLECT")
      mode = PAD_MODE_REFLECT;
    else if (var_mode == "SYMMETRIC")
      mode = PAD_MODE_SYMMETRIC;
    else
      UNI_LOG_FATAL(VART_NOT_SUPPORT)
        << "We only support pad_mode: FLOOR, CEIL, SAME, VALID. "
           "For Pad op, we support pad_mode: CONSTANT, REFLECT, SYMMETRIC"
        << endl;
  }

  void print_param(const string& head = "pad") const {
    UNI_LOG_DEBUG_INFO << head << ": ["
                     << "mode: " << mode << ", left: " << l << ", top: " << t
                     << ", right: " << r << ", bottom: " << b << "]" << endl;
  }

  bool operator==(const PadAttr& other) {
    if (mode == other.mode
        && l == other.l
        && t == other.t
        && r == other.r
        && b == other.b) {
      return true;
    }

    return false;
  }
} Pad_t;

}  // namespace cpu
}  // namespace vart
