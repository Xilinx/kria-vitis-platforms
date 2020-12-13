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

#include "gstiling.hpp"

namespace vart {
namespace cpu {

template <typename DType>
const vector<string> GSTiling<DType>::ITName = {
    "input",
};

// constructor and deconstructor
template <typename DType>
GSTiling<DType>::GSTiling(const xir::Subgraph* subg, const xir::Op* op,
                          IMapTBs_t inputs, CPUTBPtr_t output)
    : CPUOPBase(subg, op, inputs, output) {
  GET_INPUT_DIM4_FMAP(fmap_i_, input);
  GET_OUTPUT_DIM4_FMAP(fmap_o_);

  // get attrs
  reverse_ = xir_op_->get_attr<bool>("reverse");
  auto st = xir_op_->get_attr<int>("stride");
  stride_ = StrideAttr{st, st};

  // print param value, used to debug
  print_param();
}

template <typename DType>
void GSTiling<DType>::run() {
  // // do necessary check
  // check_param();

  // // read data
  // read();

  // do gstiling
  gstiling();

  // // do save, debug...
  // save();
}

template <typename DType>
void GSTiling<DType>::print_param() {
  fmap_i_.print_param("fmap_i");
  fmap_o_.print_param("fmap_o");

  UNI_LOG_DEBUG_INFO << "reverse: " << Bool2Str(reverse_) << endl;
  stride_.print_param("stride");
}

template <typename DType>
void GSTiling<DType>::check_param() {
  UNI_LOG_CHECK(inputs_.size() == 1, VART_SIZE_ERROR);

  // check fmap validation
  UNI_LOG_CHECK(stride_.h == stride_.w, VART_INVALID_VALUE)
      << ", " << stride_.h << " != " << stride_.w;

  auto stride_sq = stride_.h * stride_.w;
  if (reverse_) {
    UNI_LOG_CHECK(fmap_o_.n == fmap_i_.n, VART_INVALID_VALUE);
    UNI_LOG_CHECK(fmap_o_.h == fmap_i_.h * stride_.h, VART_INVALID_VALUE);
    UNI_LOG_CHECK(fmap_o_.w == fmap_i_.w * stride_.w, VART_INVALID_VALUE);
    UNI_LOG_CHECK(fmap_o_.c == fmap_i_.c / stride_sq, VART_INVALID_VALUE);
    UNI_LOG_CHECK(fmap_i_.c % stride_sq == 0, VART_INVALID_VALUE);
  } else {
    UNI_LOG_CHECK(fmap_o_.n == fmap_i_.n, VART_INVALID_VALUE);
    UNI_LOG_CHECK(fmap_o_.h == fmap_i_.h / stride_.h, VART_INVALID_VALUE);
    UNI_LOG_CHECK(fmap_o_.w == fmap_i_.w / stride_.w, VART_INVALID_VALUE);
    UNI_LOG_CHECK(fmap_o_.c == fmap_i_.c * stride_sq, VART_INVALID_VALUE);
    UNI_LOG_CHECK(fmap_i_.h % stride_.h == 0, VART_INVALID_VALUE);
    UNI_LOG_CHECK(fmap_i_.w % stride_.w == 0, VART_INVALID_VALUE);
  }
}

// read data from DDR
template <typename DType>
void GSTiling<DType>::read() {
  // read img data
  auto* cputb = inputs_.at(ITName[INPUT]).at(0);
  data_in_ = GET_CPUTB_DType_PTR(DType, cputb);

  // handle output buffer
  data_out_ = GET_CPUTB_DType_PTR(DType, output_);
}

template <typename DType>
uint64_t GSTiling<DType>::get_workload() {
  // TODO
  return 0;
}

template <typename DType>
void GSTiling<DType>::gstiling() {
  auto* pin = data_in_;
  auto* pout = data_out_;
  int oh, ow, oc;

  for (auto n = 0; n < fmap_i_.n; n++) {
    for (auto ih = 0; ih < fmap_i_.h; ih++) {
      for (auto iw = 0; iw < fmap_i_.w; iw++) {
        for (auto ic = 0; ic < fmap_i_.c; ic++) {
          if (!reverse_) {
            oh = ih / stride_.h;
            ow = iw / stride_.w;
            oc = ((ih % stride_.h) * stride_.w + iw % stride_.w) * fmap_i_.c +
                 ic;
          } else {
            auto off = ic / fmap_o_.c;
            oh = ih * stride_.h + off / stride_.w;
            ow = iw * stride_.w + off % stride_.w;
            oc = ic % fmap_o_.c;
          }

          auto idx_in = n * fmap_i_.h * fmap_i_.w * fmap_i_.c +
                        ih * fmap_i_.w * fmap_i_.c + iw * fmap_i_.c + ic;
          auto idx_out = n * fmap_o_.h * fmap_o_.w * fmap_o_.c +
                         oh * fmap_o_.w * fmap_o_.c + ow * fmap_o_.c + oc;
          pout[idx_out] = pin[idx_in];
        }
      }
    }
  }
}

INSTANTIATE_TPCLASS(GSTiling);

REG_OP_INSTANCE_FUNC("gstiling", GSTiling);
REG_OP_INSTANCE_FUNC("tile-fix", GSTiling);

}  // namespace cpu
}  // namespace vart
