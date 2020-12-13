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

#include "pad.hpp"
#include "fast_pad.hpp"

namespace vart {
namespace cpu {

template <typename DType>
const vector<string> Pad<DType>::ITName = {
    "input",
};

// constructor and deconstructor
template <typename DType>
Pad<DType>::Pad(const xir::Subgraph* subg, const xir::Op* op, IMapTBs_t inputs,
                CPUTBPtr_t output)
    : CPUOPBase(subg, op, inputs, output) {
  // get input/output tensor's fmap
  GET_INPUT_DIM4_FMAP(raw_fmap_i_, input);
  fmap_i_ = raw_fmap_i_;
  GET_OUTPUT_DIM4_FMAP(fmap_o_);

  // get pad info
  std::string xir_pad_mode = xir_op_->get_attr<std::string>("mode");
  auto xir_pad = xir_op_->get_attr<vector<int>>("paddings");
  if (xir_pad.size() == 8)
    pad_ = Pad_t{xir_pad_mode, xir_pad[4], xir_pad[2], xir_pad[5], xir_pad[3]};
  else
    UNI_LOG_FATAL(VART_NOT_SUPPORT)
      << "vart only support 4d pad currentlly;"
      << endl;
  // constant_values_ = xir_op_->get_attr<vector<int>>("constant_values");

  // update input height and width according to padding info
  fmap_i_.h += pad_.t + pad_.b;
  fmap_i_.w += pad_.l + pad_.r;
}

template <typename DType>
void Pad<DType>::run() {
  // // do necessary check
  // print_param();
  // check_param();

  // // init buffer related
  // read();

  // do padding
  pad();

  // // do save, debug...
  // save();
}

template <typename DType>
void Pad<DType>::print_param() {
  raw_fmap_i_.print_param("raw_fmap_i");
  fmap_i_.print_param("fmap_i");
  fmap_o_.print_param("fmap_o");

  pad_.print_param("pad");
}

template <typename DType>
void Pad<DType>::check_param() {
  UNI_LOG_CHECK(inputs_.size() == 1, VART_SIZE_ERROR)
      << ", Err: inputs size is " << inputs_.size();

  UNI_LOG_CHECK(fmap_i_.n == fmap_o_.n, VART_INVALID_VALUE)
      << "," << fmap_i_.n << " != " << fmap_o_.n;
  UNI_LOG_CHECK(fmap_i_.h == fmap_o_.h, VART_INVALID_VALUE)
      << "," << fmap_i_.h << " != " << fmap_o_.h;
  UNI_LOG_CHECK(fmap_i_.w == fmap_o_.w, VART_INVALID_VALUE)
      << "," << fmap_i_.w << " != " << fmap_o_.w;
  UNI_LOG_CHECK(fmap_i_.c == fmap_o_.c, VART_INVALID_VALUE)
      << "," << fmap_i_.c << " != " << fmap_o_.c;
}

// read data from DDR
template <typename DType>
void Pad<DType>::read() {
  // read img data
  auto* cputb = inputs_.at(ITName[INPUT]).at(0);
  img_ = GET_CPUTB_DType_PTR(DType, cputb);

  // handle output buffer
  rlt_ = GET_CPUTB_DType_PTR(DType, output_);
}

template <typename DType>
uint64_t Pad<DType>::get_workload() {
  // TODO
  return 0;
}

template <typename DType>
void Pad<DType>::pad() {
  if (pad_.mode == PadAttr::PAD_MODE_CONSTANT) {
    FastPad(raw_fmap_i_, fmap_o_, pad_).transform<DType>(img_, rlt_);
  } else if (pad_.mode == PadAttr::PAD_MODE_SYMMETRIC) {
    for (int n = 0; n < raw_fmap_i_.n; n++)
      for (int h = 0; h < raw_fmap_i_.h + pad_.t + pad_.b; h++)
        for (int w = 0; w < raw_fmap_i_.w + pad_.l + pad_.r; w++)
          for (int c = 0; c < raw_fmap_i_.c; c++) {
            int h_idx = h - pad_.t;
            int w_idx = w - pad_.l;
            if (h < pad_.t)
              h_idx = pad_.t - 1 - std::min(h, pad_.t - 1);
            else if (h >= raw_fmap_i_.h + pad_.t)
              h_idx = raw_fmap_i_.h - 1 - std::min(h - pad_.t - raw_fmap_i_.h, raw_fmap_i_.h - 1);
            if (w < pad_.l)
              w_idx = pad_.l - 1 - std::min(w, pad_.l - 1);
            else if (w >= raw_fmap_i_.w + pad_.l)
              w_idx = raw_fmap_i_.w - 1 - std::min(w - pad_.l - raw_fmap_i_.w, raw_fmap_i_.w - 1);
            auto out_idx = ((n * fmap_o_.h + h) * fmap_o_.w + w) * fmap_o_.c + c;
            auto in_idx = ((n * raw_fmap_i_.h + h_idx) * raw_fmap_i_.w + w_idx) * raw_fmap_i_.c + c;
            rlt_[out_idx] = img_[in_idx];
          }
  }
}

INSTANTIATE_TPCLASS(Pad);
REG_OP_INSTANCE_FUNC("pad", Pad);
REG_OP_INSTANCE_FUNC("pad-fix", Pad);

}  // namespace cpu
}  // namespace vart
