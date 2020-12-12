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

#include "downsample.hpp"

namespace vart {
namespace cpu {

template <typename DType>
const vector<string> DownSample<DType>::ITName = {
    "input",
};

// constructor and deconstructor
template <typename DType>
DownSample<DType>::DownSample(const xir::Subgraph* subg, const xir::Op* op,
                              IMapTBs_t inputs, CPUTBPtr_t output)
    : CPUOPBase(subg, op, inputs, output) {
  GET_INPUT_DIM4_FMAP(fmap_i_, input);
  GET_OUTPUT_DIM4_FMAP(fmap_o_);

  // get attr
  auto scale_vec = xir_op_->get_attr<vector<int>>("scale");
  scale_ = ScaleAttr{scale_vec[1], scale_vec[0]};

  mode_ = NEAREST;
  if (xir_op_->has_attr("mode")) {
    auto mode = xir_op_->get_attr<string>("mode");
    if (mode == "NEAREST")
      mode_ = NEAREST;
    else if (mode == "BILINEAR")
      mode_ = BILINEAR;
  }
}

template <typename DType>
void DownSample<DType>::run() {
  // // do necessary check
  // print_param();
  // check_param();

  // // read data
  // read();

  // calc upsample
  downsample();

  // // do save, debug...
  // save();
}

template <typename DType>
void DownSample<DType>::print_param() {
  fmap_i_.print_param("fmap_i");
  fmap_o_.print_param("fmap_o");

  scale_.print_param();
  UNI_LOG_DEBUG_INFO << "mode: " << mode_ << endl;
}

template <typename DType>
void DownSample<DType>::check_param() {
  UNI_LOG_CHECK(inputs_.size() == 1, VART_SIZE_ERROR)
      << ", Err: inputs size is " << inputs_.size();
}

template <typename DType>
void DownSample<DType>::read() {
  // read img data
  auto* img_tb = inputs_.at(ITName[INPUT]).at(0);
  const auto img_data_num = img_tb->get_data_num();
  UNI_LOG_CHECK((int)img_data_num == fmap_i_.num(), VART_SIZE_ERROR)
      << ", " << img_data_num << " > " << fmap_i_.num();
  data_in_ptr_ = GET_CPUTB_DType_PTR(DType, img_tb);

  // handle output buffer
  data_out_ptr_ = GET_CPUTB_DType_PTR(DType, output_);
}

template <typename DType>
uint64_t DownSample<DType>::get_workload() {
  // TODO
  return 0;
}

template <typename DType>
void DownSample<DType>::downsample() {
  for (auto n = 0; n < fmap_o_.n; n++) {
    for (auto h = 0; h < fmap_o_.h; h++) {
      for (auto w = 0; w < fmap_o_.w; w++) {
        downsample_one(n, h, w);
      }
    }
  }
}

template <typename DType>
void DownSample<DType>::downsample_one(int idx_n, int idx_h, int idx_w) {
  auto src_idx_h_start = idx_h * scale_.h;
  auto src_idx_w_start = idx_w * scale_.w;

  auto src_pos_start = idx_n * fmap_i_.h * fmap_i_.w * fmap_i_.c +
                       src_idx_h_start * fmap_i_.w * fmap_i_.c +
                       src_idx_w_start * fmap_i_.c;
  auto dst_pos_start = idx_n * fmap_o_.h * fmap_o_.w * fmap_o_.c +
                       idx_h * fmap_o_.w * fmap_o_.c + idx_w * fmap_o_.c;

  for (auto i = 0; i < fmap_o_.c; i++) {
    data_out_ptr_[dst_pos_start + i] = data_in_ptr_[src_pos_start + i];
  }
}

INSTANTIATE_TPCLASS(DownSample);
REG_OP_INSTANCE_FUNC("downsample", DownSample);

}  // namespace cpu
}  // namespace vart
