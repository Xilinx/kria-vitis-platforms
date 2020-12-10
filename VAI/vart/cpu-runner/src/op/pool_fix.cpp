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

#include "pool_fix.hpp"

#include "pair_hash.hpp"

namespace {
struct ApproximateParam {
  ApproximateParam(int t_kh, int t_kw, int t_N, int t_M, float t_alpha)
      : kh(t_kh), kw(t_kw), N(t_N), M(t_M), alpha(t_alpha) {
    scale = (float)N / powf(2, M);
  }

  // formula: N/2^M = alpha * 1 / (kh * kw)
  int kh;  // pool kernel height
  int kw;  // pool kernel width
  int N;
  int M;
  float alpha;

  float scale;
};

std::unordered_map<uint64_t, ApproximateParam> ap = {
    {vart::cpu::make_key(1, 1), ApproximateParam{1, 1, 1, 0, 1.0000000000}},
    {vart::cpu::make_key(2, 2), ApproximateParam{2, 2, 1, 2, 1.0000000000}},
    {vart::cpu::make_key(3, 3), ApproximateParam{3, 3, 7, 6, 1.0019531250}},
    {vart::cpu::make_key(4, 4), ApproximateParam{4, 4, 1, 4, 1.0000000000}},
    {vart::cpu::make_key(5, 5), ApproximateParam{5, 5, 10, 8, 1.0009765625}},
    {vart::cpu::make_key(6, 6), ApproximateParam{6, 6, 7, 8, 1.0019531250}},
    {vart::cpu::make_key(7, 7), ApproximateParam{7, 7, 21, 10, 1.0048828125}},
    {vart::cpu::make_key(14, 14),
     ApproximateParam{14, 14, 21, 12, 1.0048828125}},
};
}  // anonymous namespace

namespace vart {
namespace cpu {

template <typename DType>
PoolFix<DType>::PoolFix(const xir::Subgraph* subg, const xir::Op* op,
                        IMapTBs_t inputs, CPUTBPtr_t output)
    : PoolBase<DType>(subg, op, inputs, output),
      AvgPool<DType>(subg, op, inputs, output),
      MaxPool<DType>(subg, op, inputs, output) {
  auto type_str = op->get_attr<std::string>("type");
  if (type_str == "MAX")
    pool_type_ = 0;
  else if (type_str == "AVG")
    pool_type_ = 1;
  else
    UNI_LOG_FATAL(VART_NOT_SUPPORT) << "Unknown avgpool type: " << type_str;

  auto xir_tensor_i = CPUOPBase::xir_op_->get_input_tensor("input");
  auto xir_tensor_o = CPUOPBase::xir_op_->get_output_tensor();

  fp_input_ = xir_tensor_i->get_attr<int>("fix_point");
  fp_output_ = xir_tensor_o->get_attr<int>("fix_point");
  shift_pool_ = fp_output_ - fp_input_;
  pow_shift_ = pow(2.0, shift_pool_);
}

template <typename DType>
void PoolFix<DType>::run() {
  // // do necessary check
  // print_param();
  // this->check_param();

  // // read data
  // this->read();

  // calc avgpool
  if (pool_type_ == PoolBase<DType>::MAX_POOL) {
    this->max_pool();
    max_pool_fix();
  } else {
    this->acc_pool();
    avg_pool_fix();
  }

  // // do save, debug...
  // this->save();
}

template <typename DType>
void PoolFix<DType>::print_param() {
  PoolBase<DType>::print_param();

  UNI_LOG_DEBUG_INFO << "pooltype = " << pool_type_ << endl;
  UNI_LOG_DEBUG_INFO << "fix_width = " << fix_width_ << endl;
  UNI_LOG_DEBUG_INFO << "fp_input = " << fp_input_ << endl;
  UNI_LOG_DEBUG_INFO << "fp_output = " << fp_output_ << endl;
  UNI_LOG_DEBUG_INFO << "shift_pool = " << shift_pool_ << endl;
  UNI_LOG_DEBUG_INFO << "pow_shift = " << pow_shift_ << endl;
}

template <typename DType>
void PoolFix<DType>::max_pool_fix() {
  if (CPU_RUN_MODE == CPURunMode::NORMAL_THREAD ||
      CPU_RUN_MODE == CPURunMode::GEMM_THREAD) {
    max_pool_fix_thread();
  } else {
    max_pool_fix_normal();
  }
}

template <typename DType>
void PoolFix<DType>::max_pool_fix_normal() {
  for (auto i = 0; i < fmap_o_.num(); i++) {
    data_out_ptr_[i] =
        DPURound<DType>(data_out_ptr_[i] * pow_shift_, CPUOPBase::data_min_,
                        CPUOPBase::data_max_);
  }
}

template <typename DType>
void PoolFix<DType>::max_pool_fix_thread() {
  vector<std::future<int>> thr_fut(THREAD_NUM);

  for (auto i = 0U; i < THREAD_NUM; i++) {
    // thr_fut[i] = std::async(std::launch::async | std::launch::deferred,
    thr_fut[i] = std::async(std::launch::async,
                            [this](decltype(i) i) {
                              auto BASE_POS = i * THREAD_WORKLOAD;
                              for (auto j = 0U; j < THREAD_WORKLOAD; j++) {
                                auto pos = BASE_POS + j;
                                if (pos >= FMAP_SIZE) break;

                                data_out_ptr_[pos] = DPURound<DType>(
                                    data_out_ptr_[pos] * pow_shift_,
                                    CPUOPBase::data_min_, CPUOPBase::data_max_);
                              }

                              return 0;
                            },
                            i);
  }

  for (auto i = 0U; i < THREAD_NUM; i++) {
    thr_fut[i].wait();
  }
}

template <typename DType>
void PoolFix<DType>::avg_pool_fix() {
  if (CPU_RUN_MODE == CPURunMode::NORMAL_THREAD ||
      CPU_RUN_MODE == CPURunMode::GEMM_THREAD) {
    avg_pool_fix_thread();
  } else {
    avg_pool_fix_normal();
  }
}

template <typename DType>
void PoolFix<DType>::avg_pool_fix_normal() {
  for (auto i = 0; i < fmap_o_.num(); i++) {
    avg_pool_fix_one(i);
  }
}

template <typename DType>
void PoolFix<DType>::avg_pool_fix_thread() {
  vector<std::future<int>> thr_fut(THREAD_NUM);

  for (auto i = 0U; i < THREAD_NUM; i++) {
    // thr_fut[i] = std::async(std::launch::async | std::launch::deferred,
    thr_fut[i] = std::async(std::launch::async,
                            [this](decltype(i) i) {
                              auto BASE_POS = i * THREAD_WORKLOAD;
                              for (auto j = 0U; j < THREAD_WORKLOAD; j++) {
                                auto pos = BASE_POS + j;
                                if (pos >= FMAP_SIZE) break;

                                avg_pool_fix_one(pos);
                              }

                              return 0;
                            },
                            i);
  }

  for (auto i = 0U; i < THREAD_NUM; i++) {
    thr_fut[i].wait();
  }
}

template <typename DType>
void PoolFix<DType>::avg_pool_fix_one(int i) {
  auto tmp = 0.f;
  auto key = vart::cpu::make_key(kernel_.h, kernel_.w);

  if (ap.find(key) != ap.end()) {
    tmp = (float)data_out_ptr_[i] * ap.at(key).scale;
  } else {
    tmp = (float)data_out_ptr_[i] / kernel_.w / kernel_.h;
  }

  data_out_ptr_[i] = DPURound<DType>(tmp * pow_shift_, CPUOPBase::data_min_,
                                     CPUOPBase::data_max_);
}

INSTANTIATE_TPCLASS(PoolFix);
REG_OP_INSTANCE_FUNC("pool-fix", PoolFix);

}  // namespace cpu
}  // namespace vart
