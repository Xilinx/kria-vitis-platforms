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

#include "fix.hpp"

namespace vart {
namespace cpu {

template <typename DType, typename FixType>
const vector<string> Fix<DType, FixType>::ITName = {
    "input",
};

// constructor and deconstructor
template <typename DType, typename FixType>
Fix<DType, FixType>::Fix(const xir::Subgraph* subg, const xir::Op* op,
                         IMapTBs_t inputs, CPUTBPtr_t output)
    : CPUOPBase(subg, op, inputs, output) {
  GET_INPUT_DIMX_FMAP(fmap_i_, input, 6);
  GET_OUTPUT_DIMX_FMAP(fmap_o_, 6);

  // make sure input op's type
  auto* input_op = xir_op_->get_input_op(ITName[INPUT]);
  UNI_LOG_CHECK(input_op != nullptr, VART_NULL_PTR);
  const_flag_ = false;
  if (input_op->get_type() == "const" || input_op->get_type() == "const-fix")
    const_flag_ = true;

  // get fix_point and bit_width
  if (xir_op_->has_attr("if_signed")) {
    if_signed_ = xir_op_->get_attr<bool>("if_signed");
  } else {
    if_signed_ = true;
  }
  fix_point_ = xir_op_->get_attr<int>("fix_point");
  bit_width_ = xir_op_->get_attr<int>("bit_width");

  if (xir_op_->has_attr("round_mode")) {
    round_mode_ = xir_op_->get_attr<string>("round_mode");
  } else {
    if (const_flag_) {
      round_mode_ = "STD_ROUND";
    } else {
      round_mode_ = "DPU_ROUND";
    }
  }

  step_ = pow(2, fix_point_);
  lower_bound_base_ = if_signed_ ? -pow(2, bit_width_ - 1) : 0;
  upper_bound_base_ =
      if_signed_ ? pow(2, bit_width_ - 1) - 1 : pow(2, bit_width_) - 1;
  lower_bound_ = lower_bound_base_ / step_;
  upper_bound_ = upper_bound_base_ / step_;

  FMAP_SIZE = get_vec_mul(fmap_o_);
  THREAD_NUM = (CPU_NUM == 1) ? 1 : ceil(std::log(CPU_NUM));
  THREAD_WORKLOAD = ceil((float)FMAP_SIZE / THREAD_NUM);
}

template <typename DType, typename FixType>
void Fix<DType, FixType>::run() {
  // // do necessary check
  // print_param();
  // check_param();

  // // read data
  // read();

  // do fix
  float2fix();
  fix2float();

  // // do save, debug...
  // save();
}

template <typename DType, typename FixType>
void Fix<DType, FixType>::print_param() {
  UNI_LOG_DEBUG_INFO << "fmap_i = " << Vec2Str(fmap_i_, ", ") << endl;
  UNI_LOG_DEBUG_INFO << "fmap_o = " << Vec2Str(fmap_o_, ", ") << endl;

  UNI_LOG_DEBUG_INFO << "if_signed = " << Bool2Str(if_signed_) << endl;
  UNI_LOG_DEBUG_INFO << "fix_point = " << fix_point_ << endl;
  UNI_LOG_DEBUG_INFO << "bit_width = " << bit_width_ << endl;

  UNI_LOG_DEBUG_INFO << "round_mode = " << round_mode_ << endl;

  UNI_LOG_DEBUG_INFO << "step = " << step_ << endl;
  UNI_LOG_DEBUG_INFO << "lower_bound_base = " << lower_bound_base_ << endl;
  UNI_LOG_DEBUG_INFO << "upper_bound_base = " << upper_bound_base_ << endl;
  UNI_LOG_DEBUG_INFO << "lower_bound = " << lower_bound_ << endl;
  UNI_LOG_DEBUG_INFO << "upper_bound = " << upper_bound_ << endl;

  if (CPU_RUN_MODE == CPURunMode::NORMAL_THREAD ||
      CPU_RUN_MODE == CPURunMode::GEMM_THREAD) {
    UNI_LOG_DEBUG_INFO << "THREAD_NUM = " << THREAD_NUM << endl;
    UNI_LOG_DEBUG_INFO << "THREAD_WORKLOAD = " << THREAD_WORKLOAD << endl;
  }
}

template <typename DType, typename FixType>
void Fix<DType, FixType>::check_param() {
  UNI_LOG_CHECK(inputs_.size() == 1, VART_SIZE_ERROR);

  UNI_LOG_CHECK(fmap_i_ == fmap_o_, VART_INVALID_VALUE)
      << ", " << Vec2Str(fmap_i_, ", ") << " != " << Vec2Str(fmap_o_, ", ");
}

// read data from DDR
template <typename DType, typename FixType>
void Fix<DType, FixType>::read() {
  // read img data
  auto* cputb = inputs_.at(ITName[INPUT]).at(0);
  data_in_ptr_ = GET_CPUTB_DType_PTR(DType, cputb);

  // handle output buffer
  data_out_ptr_ = GET_CPUTB_DType_PTR(DType, output_);
  fix_rlt_ptr_ = output_->get_fix_data_ptr();
}

template <typename DType, typename FixType>
uint64_t Fix<DType, FixType>::get_workload() {
  // TODO
  return 0;
}

template <typename DType, typename FixType>
void Fix<DType, FixType>::float2fix() {
  if (CPU_RUN_MODE == CPURunMode::NORMAL) {
    float2fix_normal();
  } else if (CPU_RUN_MODE == CPURunMode::NORMAL_THREAD) {
    float2fix_thread();
  } else {
    float2fix_normal();
  }
}

template <typename DType, typename FixType>
void Fix<DType, FixType>::fix2float() {
  if (CPU_RUN_MODE == CPURunMode::NORMAL) {
    fix2float_normal();
  } else if (CPU_RUN_MODE == CPURunMode::NORMAL_THREAD) {
    fix2float_thread();
  } else {
    fix2float_normal();
  }
}

template <typename DType, typename FixType>
void Fix<DType, FixType>::float2fix_normal() {
  if (round_mode_ == "STD_ROUND") {
    for (auto i = 0U; i < FMAP_SIZE; i++) {
      float2fix_std_round(i);
    }
  } else if (round_mode_ == "DPU_ROUND") {
    for (auto i = 0U; i < FMAP_SIZE; i++) {
      float2fix_dpu_round(i);
    }
  } else if (round_mode_ == "PY3_ROUND") {
    for (auto i = 0U; i < FMAP_SIZE; i++) {
      float2fix_py3_round(i);
    }
  } else {
    UNI_LOG_ERROR(VART_NOT_SUPPORT)
        << "Not supported round mode " << round_mode_ << endl;
    abort();
  }
}

template <typename DType, typename FixType>
void Fix<DType, FixType>::float2fix_thread() {
  vector<std::future<int>> thr_fut(THREAD_NUM);

  for (auto i = 0U; i < THREAD_NUM; i++) {
    // thr_fut[i] = std::async(std::launch::async | std::launch::deferred,
    thr_fut[i] = std::async(std::launch::async,
                            [this](decltype(i) i) {
                              auto BASE_POS = i * THREAD_WORKLOAD;
                              if (round_mode_ == "STD_ROUND") {
                                for (auto j = 0U; j < THREAD_WORKLOAD; j++) {
                                  auto pos = BASE_POS + j;
                                  if (pos >= FMAP_SIZE) break;

                                  float2fix_std_round(pos);
                                }
                              } else if (round_mode_ == "DPU_ROUND") {
                                for (auto j = 0U; j < THREAD_WORKLOAD; j++) {
                                  auto pos = BASE_POS + j;
                                  if (pos >= FMAP_SIZE) break;

                                  float2fix_dpu_round(pos);
                                }
                              } else if (round_mode_ == "PY3_ROUND") {
                                for (auto j = 0U; j < THREAD_WORKLOAD; j++) {
                                  auto pos = BASE_POS + j;
                                  if (pos >= FMAP_SIZE) break;

                                  float2fix_py3_round(pos);
                                }
                              } else {
                                UNI_LOG_ERROR(VART_NOT_SUPPORT)
                                    << "Not supported round mode "
                                    << round_mode_ << endl;
                                abort();
                              }

                              return 0;
                            },
                            i);
  }

  for (auto i = 0U; i < THREAD_NUM; i++) {
    thr_fut[i].wait();
  }
}

template <typename DType, typename FixType>
void Fix<DType, FixType>::fix2float_normal() {
  if (round_mode_ == "STD_ROUND") {
    for (auto i = 0U; i < FMAP_SIZE; i++) {
      fix2float_std_round(i);
    }
  } else if (round_mode_ == "DPU_ROUND") {
    for (auto i = 0U; i < FMAP_SIZE; i++) {
      fix2float_dpu_round(i);
    }
  } else if (round_mode_ == "PY3_ROUND") {
    for (auto i = 0U; i < FMAP_SIZE; i++) {
      fix2float_py3_round(i);
    }
  } else {
    UNI_LOG_ERROR(VART_NOT_SUPPORT)
        << "Not supported round mode " << round_mode_ << endl;
    abort();
  }
}

template <typename DType, typename FixType>
void Fix<DType, FixType>::fix2float_thread() {
  vector<std::future<int>> thr_fut(THREAD_NUM);

  for (auto i = 0U; i < THREAD_NUM; i++) {
    // thr_fut[i] = std::async(std::launch::async | std::launch::deferred,
    thr_fut[i] = std::async(std::launch::async,
                            [this](decltype(i) i) {
                              auto BASE_POS = i * THREAD_WORKLOAD;
                              if (round_mode_ == "STD_ROUND") {
                                for (auto j = 0U; j < THREAD_WORKLOAD; j++) {
                                  auto pos = BASE_POS + j;
                                  if (pos >= FMAP_SIZE) break;

                                  fix2float_std_round(pos);
                                }
                              } else if (round_mode_ == "DPU_ROUND") {
                                for (auto j = 0U; j < THREAD_WORKLOAD; j++) {
                                  auto pos = BASE_POS + j;
                                  if (pos >= FMAP_SIZE) break;

                                  fix2float_dpu_round(pos);
                                }
                              } else if (round_mode_ == "PY3_ROUND") {
                                for (auto j = 0U; j < THREAD_WORKLOAD; j++) {
                                  auto pos = BASE_POS + j;
                                  if (pos >= FMAP_SIZE) break;

                                  fix2float_py3_round(pos);
                                }
                              } else {
                                UNI_LOG_ERROR(VART_NOT_SUPPORT)
                                    << "Not supported round mode "
                                    << round_mode_ << endl;
                                abort();
                              }

                              return 0;
                            },
                            i);
  }

  for (auto i = 0U; i < THREAD_NUM; i++) {
    thr_fut[i].wait();
  }
}

INSTANTIATE_TPCLASS(Fix);
REG_OP_INSTANCE_FUNC("fix", Fix);

}  // namespace cpu
}  // namespace vart
