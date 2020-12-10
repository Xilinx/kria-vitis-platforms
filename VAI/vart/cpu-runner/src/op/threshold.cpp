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

#include "threshold.hpp"

#define THRESHOLD_MASK 0x00FFFFFF
#define BASE_MASK 0x00FFF800
#define DELTA_MASK 0x000007FE
#define SIGNAL_MASK 0x00000001

namespace vart {
namespace cpu {

template <typename DType>
const vector<string> Threshold<DType>::ITName = {"input", "threshold"};

// constructor and deconstructor
template <typename DType>
Threshold<DType>::Threshold(const xir::Subgraph* subg, const xir::Op* op,
                            IMapTBs_t inputs, CPUTBPtr_t output)
    : CPUOPBase(subg, op, inputs, output) {
  GET_INPUT_DIM4_FMAP(fmap_i_, input);
  GET_INPUT_DIM1_FMAP(fmap_t_, threshold);
  GET_OUTPUT_DIM4_FMAP(fmap_o_);
}

template <typename DType>
void Threshold<DType>::run() {
  // // do necessary check
  // print_param();
  // check_param();

  // // read data
  // read();

  // do threshold
  parse_threshold();
  threshold();

  // // do save, debug...
  // save();
}

template <typename DType>
void Threshold<DType>::print_param() {
  fmap_i_.print_param("fmap_i");
  fmap_t_.print_param("fmap_t");
  fmap_o_.print_param("fmap_o");
}

template <typename DType>
void Threshold<DType>::check_param() {
  UNI_LOG_CHECK(inputs_.size() == 2, VART_SIZE_ERROR);

  UNI_LOG_CHECK(fmap_i_.c == fmap_t_.c, VART_SIZE_ERROR);
}

// read data from DDR
template <typename DType>
void Threshold<DType>::read() {
  // read img data
  auto* data_in_tb = inputs_.at(ITName[INPUT]).at(0);
  const auto tb_input_num = data_in_tb->get_data_num();
  UNI_LOG_CHECK((int)tb_input_num == fmap_i_.num(), VART_SIZE_ERROR);
  data_in_ = GET_CPUTB_DType_PTR(DType, data_in_tb);

  // read threshold data, not check now
  auto* threshold_tb = inputs_.at(ITName[THRESHOLD]).at(0);
  const auto tb_threshold_num = threshold_tb->get_data_num();
  UNI_LOG_CHECK((int)tb_threshold_num == fmap_i_.c, VART_SIZE_ERROR);
  threshold_ptr_ = GET_CPUTB_DType_PTR(DType, threshold_tb);

  // handle output buffer
  data_out_ = GET_CPUTB_DType_PTR(DType, output_);
}

template <typename DType>
uint64_t Threshold<DType>::get_workload() {
  // TODO
  return 0;
}

template <typename DType>
void Threshold<DType>::parse_threshold() {
  threshold_.resize(fmap_i_.c);

  for (auto i = 0; i < fmap_i_.c; i++) {
    auto* p = threshold_ptr_ + i;
    threshold_[i].base = (int32_t)(((threshold_bit_t<int32_t>*)p)->base);
    threshold_[i].delta = (uint32_t)(((threshold_bit_t<uint32_t>*)p)->delta);
    threshold_[i].signal = (bool)(((threshold_bit_t<uint32_t>*)p)->signal);
  }
}

template <typename DType>
void Threshold<DType>::threshold() {
  for (auto i = 0; i < fmap_o_.num(); i++) {
    auto j = i % fmap_o_.c;

    if (threshold_[j].delta) {
      int tmp = ceil((static_cast<double>(threshold_[j].base) -
                      static_cast<double>(data_in_[i]) * 4.0) /
                     threshold_[j].delta);
      if (tmp > 8)
        data_out_[i] = 0;
      else if (tmp < -7)
        data_out_[i] = 15;
      else
        data_out_[i] = 8 - tmp;
    } else {
      if (threshold_[j].base <= data_in_[i] * 4.0)
        data_out_[i] = 15;
      else
        data_out_[i] = 0;
    }

    if (threshold_[j].signal) data_out_[i] = 15 - data_out_[i];

    if (if_signed_) data_out_[i] -= 8;
  }
}

INSTANTIATE_TPCLASS(Threshold);
REG_OP_INSTANCE_FUNC("threshold", Threshold);

}  // namespace cpu
}  // namespace vart
