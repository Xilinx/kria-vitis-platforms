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

#include "eltwise.hpp"

namespace vart {
namespace cpu {

template <typename DType>
const vector<string> Eltwise<DType>::ITName = {
    "input",
};

// constructor and deconstructor
template <typename DType>
Eltwise<DType>::Eltwise(const xir::Subgraph* subg, const xir::Op* op,
                        IMapTBs_t inputs, CPUTBPtr_t output)
    : CPUOPBase(subg, op, inputs, output) {
  GET_INPUT_DIMX_FMAPS(fmap_i_, input, 4);
  GET_OUTPUT_DIMX_FMAP(fmap_o_, 4);

  input_num_ = fmap_i_.size();

  THREAD_NUM = (CPU_NUM == 1) ? 1 : ceil(std::log(CPU_NUM));
  THREAD_WORKLOAD = ceil((float)fmap_o_.num() / THREAD_NUM);
}

template <typename DType>
void Eltwise<DType>::run() {
  if (CPU_RUN_MODE == CPURunMode::NORMAL_THREAD ||
      CPU_RUN_MODE == CPURunMode::GEMM_THREAD) {
    eltwise_thread();
  } else {
    eltwise_normal();
  }
}

template <typename DType>
void Eltwise<DType>::print_param() {
  for (auto i = 0; i < input_num_; i++) {
    fmap_i_[i].print_param("fmap_i" + to_string(i));
  }
  fmap_o_.print_param("fmap_o");

  if (CPU_RUN_MODE == CPURunMode::NORMAL_THREAD ||
      CPU_RUN_MODE == CPURunMode::GEMM_THREAD) {
    UNI_LOG_DEBUG_INFO << "THREAD_NUM = " << THREAD_NUM << endl;
    UNI_LOG_DEBUG_INFO << "THREAD_WORKLOAD = " << THREAD_WORKLOAD << endl;
  }
}

template <typename DType>
void Eltwise<DType>::check_param() {
  UNI_LOG_CHECK(inputs_.size() >= 1, VART_SIZE_ERROR)
      << ", Err: inputs size is " << inputs_.size();
}

// read data from DDR
template <typename DType>
void Eltwise<DType>::read() {
  // handle input buffer
  data_in_.resize(input_num_);
  for (auto idx = 0; idx < input_num_; idx++) {
    auto* cputb = inputs_.at(ITName[INPUT]).at(idx);
    data_in_[idx] = GET_CPUTB_DType_PTR(DType, cputb);
  }

  // handle output buffer
  data_out_ = GET_CPUTB_DType_PTR(DType, output_);

  // decide channel wise flag
  channel_wise_.resize(input_num_);
  for (auto id = 0; id < input_num_; id++) {
    channel_wise_[id] = (fmap_i_[id].ndims() != fmap_o_.ndims());
  }
}

template <typename DType>
uint64_t Eltwise<DType>::get_workload() {
  return fmap_o_.num();
}

template <typename DType>
void Eltwise<DType>::eltwise_normal() {
  std::fill_n(data_out_, fmap_o_.num(), 0);

  for (auto id = 0; id < input_num_; id++) {
    if (channel_wise_[id]) {
      auto N = fmap_i_[id].num();
      for (auto i = 0; i < fmap_o_.num(); i++) {
        data_out_[i] += data_in_[id][i % N];
      }
    } else {
      for (auto i = 0; i < fmap_o_.num(); i++) {
        data_out_[i] += data_in_[id][i];
      }
    }
  }
}

template <typename DType>
void Eltwise<DType>::eltwise_thread() {
  std::fill_n(data_out_, fmap_o_.num(), 0);

  vector<std::future<int>> fut(THREAD_NUM);

  for (auto tid = 0U; tid < THREAD_NUM; tid++) {
    fut[tid] =
        std::async(std::launch::async,
                   [this](decltype(tid) tid) {
                     auto BASE_POS = tid * THREAD_WORKLOAD;
                     auto FMAP_SIZE = fmap_o_.num();
                     auto END_POS = std::min<uint32_t>(
                         FMAP_SIZE, BASE_POS + THREAD_WORKLOAD);

                     for (auto id = 0; id < input_num_; id++) {
                       auto N = fmap_i_[id].num();

                       if (channel_wise_[id]) {
                         for (auto pos = BASE_POS; pos < END_POS; pos++) {
                           data_out_[pos] += data_in_[id][pos % N];
                         }
                       } else {
                         for (auto pos = BASE_POS; pos < END_POS; pos++) {
                           data_out_[pos] += data_in_[id][pos];
                         }
                       }
                     }

                     return 0;
                   },
                   tid);
  }

  for (auto tid = 0U; tid < THREAD_NUM; tid++) {
    fut[tid].wait();
  }
}

INSTANTIATE_TPCLASS(Eltwise);
REG_OP_INSTANCE_FUNC("eltwise", Eltwise);

}  // namespace cpu
}  // namespace vart
