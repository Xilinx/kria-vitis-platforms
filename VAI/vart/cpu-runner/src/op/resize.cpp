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

#include "resize.hpp"

namespace vart {
namespace cpu {

template <typename DType>
const vector<string> Resize<DType>::ITName = {
    "input",
    "size",
};

// constructor and deconstructor
template <typename DType>
Resize<DType>::Resize(const xir::Subgraph* subg, const xir::Op* op,
                      IMapTBs_t inputs, CPUTBPtr_t output)
    : CPUOPBase(subg, op, inputs, output) {
  GET_INPUT_DIM4_FMAP(fmap_i_, input);
  GET_OUTPUT_DIM4_FMAP(fmap_o_);

  mode_ = xir_op_->get_attr<std::string>("mode");
  align_corners_ = xir_op_->get_attr<bool>("align_corners");
  half_pixel_centers_ = xir_op_->get_attr<bool>("half_pixel_centers");

  // resize related buffer size
  img_.resize(fmap_i_.n * fmap_i_.h * fmap_i_.w * fmap_i_.c);
  rlt_.resize(fmap_o_.n * fmap_o_.h * fmap_o_.w * fmap_o_.c);
}

template <typename DType>
void Resize<DType>::run() {
  // // do necessary check
  // print_param();
  // check_param();

  // // read data
  // read();

  // calc resize
  resize();
  for (uint i = 0; i < rlt_.size(); i++)
    data_out_ptr_[i] = static_cast<DType>(rlt_[i]);

  // // do save, debug...
  // save();
}

template <typename DType>
void Resize<DType>::print_param() {
  raw_fmap_i_.print_param("raw_fmap_i");
  fmap_i_.print_param("fmap_i");
  fmap_o_.print_param("fmap_o");
}

template <typename DType>
void Resize<DType>::check_param() {
  UNI_LOG_CHECK(inputs_.size() >= 1 && inputs_.size() <= 2, VART_SIZE_ERROR)
      << ", Err: inputs size is " << inputs_.size();
}

// read data from DDR
template <typename DType>
void Resize<DType>::read() {
  // read img data
  auto* img_tb = inputs_.at(ITName[INPUT]).at(0);
  data_in_ptr_ = GET_CPUTB_DType_PTR(DType, img_tb);
  const auto img_data_num = img_tb->get_data_num();
  for (uint i = 0; i < img_data_num; i++) img_[i] = data_in_ptr_[i];

  // handle output buffer
  data_out_ptr_ = GET_CPUTB_DType_PTR(DType, output_);
}

template <typename DType>
uint64_t Resize<DType>::get_workload() {
  // TODO
  return 0;
}

struct CachedInterpolation {
  int lower;
  int upper;
  float lerp;
};

template <typename DType>
void Resize<DType>::resize() {
  auto cal_scale = [](std::int32_t in, std::int32_t out,
                      bool align_corners) -> float {
    return (align_corners && out > 1) ? (in - 1) / static_cast<float>(out - 1)
                                      : in / static_cast<float>(out);
  };
  auto scaler = [](std::int32_t out, float scale,
                   bool half_pixel_centers) -> float {
    return (half_pixel_centers)
               ? (static_cast<float>(out) + 0.5f) * scale - 0.5f
               : static_cast<float>(out) * scale;
  };
  if (mode_ == "NEAREST") {
    auto h_scale = cal_scale(fmap_i_.h, fmap_o_.h, align_corners_);
    auto w_scale = cal_scale(fmap_i_.w, fmap_o_.w, align_corners_);
    for (int n = 0; n < fmap_o_.n; n++)
      for (int h = 0; h < fmap_o_.h; h++)
        for (int w = 0; w < fmap_o_.w; w++)
          for (int c = 0; c < fmap_o_.c; c++) {
            auto idx = ((n * fmap_o_.h + h) * fmap_o_.w + w) * fmap_o_.c + c;
            auto h_scaler = scaler(h, h_scale, half_pixel_centers_);
            auto w_scaler = scaler(w, w_scale, half_pixel_centers_);
            auto h_idx = std::min(
                (align_corners_) ? static_cast<int>(std::round(h_scaler))
                                 : static_cast<int>(std::floor(h_scaler)),
                fmap_i_.h - 1);
            auto w_idx = std::min(
                (align_corners_) ? static_cast<int>(std::round(w_scaler))
                                 : static_cast<int>(std::floor(w_scaler)),
                fmap_i_.w - 1);
            auto i_idx =
                ((n * fmap_i_.h + h_idx) * fmap_i_.w + w_idx) * fmap_i_.c + c;
            rlt_[idx] = img_[i_idx];
          }
  } else if (mode_ == "BILINEAR") {
    std::vector<CachedInterpolation> xs(fmap_o_.w + 1);
    std::vector<CachedInterpolation> ys(fmap_o_.h + 1);
    auto h_scale = cal_scale(fmap_i_.h, fmap_o_.h, align_corners_);
    auto w_scale = cal_scale(fmap_i_.w, fmap_o_.w, align_corners_);
    auto compute_interpolation_weights =
        [&](const int out_size, const int in_size, const float scale,
            CachedInterpolation* interpolation) {
          interpolation[out_size].lower = 0;
          interpolation[out_size].upper = 0;
          for (int i = out_size - 1; i >= 0; --i) {
            const float in = scaler(i, scale, half_pixel_centers_);
            interpolation[i].lower =
                std::max(static_cast<int>(std::floor(in)),
                         static_cast<int>(0));
            interpolation[i].upper =
                std::min(static_cast<int>(std::ceil(in)),
                         static_cast<int>(in_size - 1));
            interpolation[i].lerp = in - interpolation[i].lower;
          }
        };
    compute_interpolation_weights(fmap_o_.h, fmap_i_.h, h_scale, ys.data());
    compute_interpolation_weights(fmap_o_.w, fmap_i_.w, w_scale, xs.data());
    for (uint i = 0; i < xs.size(); ++i) {
      xs[i].lower *= fmap_i_.c;
      xs[i].upper *= fmap_i_.c;
    }
    const int in_row_size = fmap_i_.w * fmap_i_.c;
    const int in_batch_num_values = fmap_i_.h * in_row_size;
    const int out_row_size = fmap_o_.w * fmap_i_.c;

    auto compute_lerp = [&](const float top_left, const float top_right,
                            const float bottom_left, const float bottom_right,
                            const float x_lerp, const float y_lerp) -> float {
      const float top = top_left + (top_right - top_left) * x_lerp;
      const float bottom = bottom_left + (bottom_right - bottom_left) * x_lerp;
      return top + (bottom - top) * y_lerp;
    };
    DType* in_ptr = img_.data();
    int idx_start = 0;
    for (int b = 0; b < fmap_o_.n; ++b) {
      for (int y = 0; y < fmap_o_.h; ++y) {
        const DType* ys_input_lower_ptr = in_ptr + ys[y].lower * in_row_size;
        const DType* ys_input_upper_ptr = in_ptr + ys[y].upper * in_row_size;
        const float ys_lerp = ys[y].lerp;
        for (int x = 0; x < fmap_o_.w; ++x) {
          auto xs_lower = xs[x].lower;
          auto xs_upper = xs[x].upper;
          auto xs_lerp = xs[x].lerp;
          for (int c = 0; c < fmap_i_.c; ++c) {
            const float top_left(ys_input_lower_ptr[xs_lower + c]);
            const float top_right(ys_input_lower_ptr[xs_upper + c]);
            const float bottom_left(ys_input_upper_ptr[xs_lower + c]);
            const float bottom_right(ys_input_upper_ptr[xs_upper + c]);
            rlt_[x * fmap_i_.c + c + idx_start] =
                compute_lerp(top_left, top_right, bottom_left, bottom_right,
                             xs_lerp, ys_lerp);
          }
        }
        idx_start += out_row_size;
      }
      in_ptr += in_batch_num_values;
    }
  } else {
    // to be updated
  }
}

INSTANTIATE_TPCLASS(Resize);
REG_OP_INSTANCE_FUNC("resize", Resize);
REG_OP_INSTANCE_FUNC("upsample", Resize);

}  // namespace cpu
}  // namespace vart
