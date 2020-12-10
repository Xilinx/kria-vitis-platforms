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

#include "reorg.hpp"

namespace vart {
namespace cpu {

template <typename DType>
const vector<string> Reorg<DType>::ITName = {
    "input",
};

// constructor and deconstructor
template <typename DType>
Reorg<DType>::Reorg(const xir::Subgraph* subg, const xir::Op* op,
                    IMapTBs_t inputs, CPUTBPtr_t output)
    : CPUOPBase(subg, op, inputs, output) {
  GET_INPUT_DIM4_FMAP(fmap_i_, input);
  GET_OUTPUT_DIM4_FMAP(fmap_o_);

  // get attr
  auto scale_hw = xir_op_->get_attr<int>("scale");
  scale_ = ScaleAttr{scale_hw, scale_hw};
  reverse_ = xir_op_->get_attr<bool>("reverse");

  // resize related buffer size
  data_in_tmp_.resize(fmap_i_.n * fmap_i_.h * fmap_i_.w * fmap_i_.c);
  data_out_tmp_.resize(fmap_o_.n * fmap_o_.h * fmap_o_.w * fmap_o_.c);
}

template <typename DType>
void Reorg<DType>::run() {
  // // do necessary check
  // print_param();
  // check_param();

  // // read data
  // read();

  // calc reorg
  reorg();

  // // do save, debug...
  // save();
}

template <typename DType>
void Reorg<DType>::print_param() {
  fmap_i_.print_param("fmap_i");
  fmap_o_.print_param("fmap_o");

  scale_.print_param();
  UNI_LOG_DEBUG_INFO << "reverse_: " << Bool2Str(reverse_) << endl;
}

template <typename DType>
void Reorg<DType>::check_param() {
  UNI_LOG_CHECK(inputs_.size() == 1, VART_SIZE_ERROR)
      << ", Err: inputs size is " << inputs_.size();

  // check fmap_out height
  UNI_LOG_CHECK(fmap_i_.n == 1, VART_INVALID_VALUE)
      << ", " << fmap_i_.n << " != 1";
}

template <typename DType>
void Reorg<DType>::read() {
  // read img data
  auto* img_tb = inputs_.at(ITName[INPUT]).at(0);
  const auto img_data_num = img_tb->get_data_num();
  UNI_LOG_CHECK(img_data_num == data_in_tmp_.size(), VART_SIZE_ERROR)
      << ", " << img_data_num << " > " << data_in_tmp_.size();

  data_in_ = GET_CPUTB_DType_PTR(DType, img_tb);

  // handle output buffer
  data_out_ = GET_CPUTB_DType_PTR(DType, output_);
}

template <typename DType>
uint64_t Reorg<DType>::get_workload() {
  // TODO
  return 0;
}

template <typename DType>
void Reorg<DType>::reorg() {
  if (reverse_) {
    reorg_reverse_yes();
  } else {
    reorg_reverse_no();
  }
}

template <typename DType>
void Reorg<DType>::reorg_reverse_yes() {
  std::copy_n(data_in_, data_in_tmp_.size(), data_in_tmp_.data());

  for (auto i = 0; i < fmap_i_.h; i++) {
    for (auto j = 0; j < fmap_i_.w; j++) {
      reorg_one_reverse(i, j);
    }
  }

  // change tile's element order into reorg's element order
  for (auto i = 0; i < fmap_o_.h; i++) {
    for (auto j = 0; j < fmap_o_.w; j++) {
      for (auto k = 0; k < fmap_o_.c; k++) {
        auto src_pos = i * fmap_o_.w * fmap_o_.c + j * fmap_o_.c + k;

        auto h_unit = scale_.h * scale_.h;
        auto h_base = i / h_unit * h_unit;
        auto h_rest = i % h_unit;
        auto h_base2 = (h_rest % scale_.h) * scale_.h;
        auto h_rest2 = h_rest / scale_.h;
        auto dst_i = h_base + h_base2 + h_rest2;
        auto dst_j = j;
        auto dst_k = k;

        auto dst_pos =
            dst_i * fmap_o_.w * fmap_o_.c + dst_j * fmap_o_.c + dst_k;

        data_out_[dst_pos] = data_out_tmp_[src_pos];
      }
    }
  }
}

template <typename DType>
void Reorg<DType>::reorg_reverse_no() {
  // change reorg's element order into tile's element order
  for (auto i = 0; i < fmap_i_.h; i++) {
    for (auto j = 0; j < fmap_i_.w; j++) {
      for (auto k = 0; k < fmap_i_.c; k++) {
        auto src_pos = i * fmap_i_.w * fmap_i_.c + j * fmap_i_.c + k;

        auto h_unit = scale_.h * scale_.h;
        auto h_base = i / h_unit * h_unit;
        auto h_rest = i % h_unit;
        auto h_base2 = (h_rest % scale_.h) * scale_.h;
        auto h_rest2 = h_rest / scale_.h;
        auto dst_i = h_base + h_base2 + h_rest2;
        auto dst_j = j;
        auto dst_k = k;

        auto dst_pos =
            dst_i * fmap_i_.w * fmap_i_.c + dst_j * fmap_i_.c + dst_k;

        data_in_tmp_[dst_pos] = data_in_[src_pos];
      }
    }
  }

  // reorg one by one
  for (auto i = 0; i < fmap_i_.h; i += scale_.h) {
    for (auto j = 0; j < fmap_i_.w; j += scale_.w) {
      reorg_one(i, j);
    }
  }

  std::copy_n(data_out_tmp_.data(), data_out_tmp_.size(), data_out_);
}

template <typename DType>
void Reorg<DType>::reorg_one_reverse(int idx_h, int idx_w) {
  auto dst_idx_h_start = idx_h * scale_.h;
  auto dst_idx_w_start = idx_w * scale_.w;
  auto src_pos_start = idx_h * fmap_i_.w * fmap_i_.c + idx_w * fmap_i_.c;

  for (auto i = dst_idx_h_start; i < dst_idx_h_start + scale_.h; i++) {
    for (auto j = dst_idx_w_start; j < dst_idx_w_start + scale_.w; j++) {
      for (auto k = 0; k < fmap_o_.c; k++) {
        auto src_pos = src_pos_start +
                       (i - dst_idx_h_start) * scale_.w * fmap_o_.c +
                       (j - dst_idx_w_start) * fmap_o_.c + k;
        auto dst_pos = i * fmap_o_.w * fmap_o_.c + j * fmap_o_.c + k;

        data_out_tmp_[dst_pos] = data_in_tmp_[src_pos];
      }
    }
  }
}

template <typename DType>
void Reorg<DType>::reorg_one(int idx_h, int idx_w) {
  auto dst_idx_h = idx_h / scale_.h;
  auto dst_idx_w = idx_w / scale_.w;
  auto dst_pos_start =
      dst_idx_h * fmap_o_.w * fmap_o_.c + dst_idx_w * fmap_o_.c;

  for (auto i = idx_h; i < idx_h + scale_.h; i++) {
    for (auto j = idx_w; j < idx_w + scale_.w; j++) {
      for (auto k = 0; k < fmap_i_.c; k++) {
        auto src_pos = i * fmap_i_.w * fmap_i_.c + j * fmap_i_.c + k;
        auto dst_pos = dst_pos_start + (i - idx_h) * scale_.w * fmap_i_.c +
                       (j - idx_w) * fmap_i_.c + k;

        data_out_tmp_[dst_pos] = data_in_tmp_[src_pos];
      }
    }
  }
}

INSTANTIATE_TPCLASS(Reorg);
REG_OP_INSTANCE_FUNC("reorg", Reorg);

}  // namespace cpu
}  // namespace vart
