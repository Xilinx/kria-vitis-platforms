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

#include "eltwise_fix.hpp"

namespace vart {
namespace cpu {

template <typename DType>
EltwiseFix<DType>::EltwiseFix(const xir::Subgraph* subg, const xir::Op* op,
                              IMapTBs_t inputs, CPUTBPtr_t output)
    : Eltwise<DType>(subg, op, inputs, output) {
  auto nonlinear_type_str = op->get_attr<string>("nonlinear");
  if (nonlinear_type_str == "NONE" || nonlinear_type_str == "")
    nonlinear_type_ = 0;
  else if (nonlinear_type_str == "RELU")
    nonlinear_type_ = 1;
  else if (nonlinear_type_str == "PRELU")
    nonlinear_type_ = 2;
  else if (nonlinear_type_str == "LEAKYRELU")
    nonlinear_type_ = 3;
  else if (nonlinear_type_str == "RELU6")
    nonlinear_type_ = 4;
  else
    UNI_LOG_FATAL(VART_NOT_SUPPORT)
      << "Unsupported nonlinear type: " << nonlinear_type_str << ".";

  for (auto i = 0; i < input_num_; i++) {
    auto* xir_tensor_i = CPUOPBase::xir_op_->get_input_tensor("input", i);
    UNI_LOG_CHECK(xir_tensor_i != nullptr, VART_NULL_PTR);
    auto fp = xir_tensor_i->get_attr<int>("fix_point");
    fp_inputs_.push_back(fp);
  }

  auto* xir_tensor_o = CPUOPBase::xir_op_->get_output_tensor();
  UNI_LOG_CHECK(xir_tensor_o != nullptr, VART_NULL_PTR);
  fp_output_ = xir_tensor_o->get_attr<int>("fix_point");

  int fp_min = fp_inputs_[0];
  for (auto i = 0; i < input_num_; i++) {
    if (fp_min > fp_inputs_[i]) fp_min = fp_inputs_[i];
  }
  for (auto i = 0; i < input_num_; i++) {
    shift_read_.push_back(fp_inputs_[i] - fp_min);
  }
  shift_write_ = fp_min - fp_output_;

  // calc shift related factor to speed up calculating time
  shift_read_factor_.resize(input_num_);
  for (auto i = 0; i < input_num_; i++) {
    shift_read_factor_[i] = pow(2, shift_read_[i] - 2);
  }
  shift_write_factor_ = pow(2, shift_write_ + 2);
}

template <typename DType>
void EltwiseFix<DType>::run() {
  if (CPU_RUN_MODE == CPURunMode::NORMAL_THREAD ||
      CPU_RUN_MODE == CPURunMode::GEMM_THREAD) {
    eltwise_fix_thread();
  } else {
    eltwise_fix_normal();
  }
}

template <typename DType>
void EltwiseFix<DType>::print_param() {
  Eltwise<DType>::print_param();

  UNI_LOG_DEBUG_INFO << "nonlinear_type = " << nonlinear_type_ << endl;
  UNI_LOG_DEBUG_INFO << "bit_width = " << CPUOPBase::bit_width_ << endl;
  for (auto i = 0; i < input_num_; i++) {
    UNI_LOG_DEBUG_INFO << "fp_input" << i << " = " << fp_inputs_[i] << endl;
  }
  UNI_LOG_DEBUG_INFO << "fp_output = " << fp_output_ << endl;

  for (auto i = 0; i < input_num_; i++) {
    UNI_LOG_DEBUG_INFO << "shift_read" << i << " = " << shift_read_[i] << endl;
  }
  UNI_LOG_DEBUG_INFO << "shift_write = " << shift_write_ << endl;
}

template <typename DType>
void EltwiseFix<DType>::eltwise_fix_normal() {
  auto FMAP_SIZE = fmap_o_.num();
  vector<float> tmp_buf(FMAP_SIZE, 0);

  for (auto id = 0; id < input_num_; id++) {
    if (channel_wise_[id]) {
      auto N = fmap_i_[id].num();
      for (auto i = 0; i < FMAP_SIZE; i++) {
        tmp_buf[i] += floor(data_in_[id][i % N] / shift_read_factor_[id]);
      }
    } else {
      for (auto i = 0; i < FMAP_SIZE; i++) {
        tmp_buf[i] += floor(data_in_[id][i] / shift_read_factor_[id]);
      }
    }
  }

  for (auto i = 0; i < FMAP_SIZE; i++) {
    tmp_buf[i] /= shift_write_factor_;

    if (nonlinear_type_ == NONLINEAR_RELU) {
      if (tmp_buf[i] < 0) tmp_buf[i] = 0;
    } else if (nonlinear_type_ == NONLINEAR_LEAKY_RELU) {
      // Use 26/256 instead of 0.1 on hardware
      if (tmp_buf[i] < 0) tmp_buf[i] *= 26.f / 256.f;
    }

    data_out_[i] =
        DPURound<DType>(tmp_buf[i], CPUOPBase::data_min_, CPUOPBase::data_max_);
  }
}

template <typename DType>
void EltwiseFix<DType>::eltwise_fix_thread() {
  vector<std::future<int>> fut(THREAD_NUM);

  for (auto tid = 0U; tid < THREAD_NUM; tid++) {
    fut[tid] = std::async(
        std::launch::async,
        [this](decltype(tid) tid) {
          vector<float> tmp_buf(THREAD_WORKLOAD, 0);
          auto BASE_POS = tid * THREAD_WORKLOAD;
          auto END_POS =
              std::min<uint32_t>(fmap_o_.num(), BASE_POS + THREAD_WORKLOAD);

          for (auto id = 0; id < input_num_; id++) {
            if (channel_wise_[id]) {
              auto N = fmap_i_[id].num();
              for (auto i = 0U, pos = BASE_POS; pos < END_POS; i++, pos++) {
                tmp_buf[i] +=
                    floor(data_in_[id][pos % N] / shift_read_factor_[id]);
              }
            } else {
              for (auto i = 0U, pos = BASE_POS; pos < END_POS; i++, pos++) {
                tmp_buf[i] += floor(data_in_[id][pos] / shift_read_factor_[id]);
              }
            }
          }

          for (auto i = 0U, pos = BASE_POS; pos < END_POS; i++, pos++) {
            tmp_buf[i] /= shift_write_factor_;

            if (nonlinear_type_ == NONLINEAR_RELU) {
              if (tmp_buf[i] < 0) tmp_buf[i] = 0;
            } else if (nonlinear_type_ == NONLINEAR_LEAKY_RELU) {
              // Use 26/256 instead of 0.1 on hardware
              if (tmp_buf[i] < 0) tmp_buf[i] *= 26.f / 256.f;
            }

            data_out_[pos] = DPURound<DType>(tmp_buf[i], CPUOPBase::data_min_,
                                             CPUOPBase::data_max_);
          }

          return 0;
        },
        tid);
  }

  for (auto tid = 0U; tid < THREAD_NUM; tid++) {
    fut[tid].wait();
  }
}

INSTANTIATE_TPCLASS(EltwiseFix);
REG_OP_INSTANCE_FUNC("eltwise-fix", EltwiseFix);

}  // namespace cpu
}  // namespace vart
