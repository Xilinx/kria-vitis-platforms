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

#include "bn.hpp"


namespace vart {
namespace cpu {

template <typename DType>
const vector<string> BN<DType>::ITName = {
    "input",
    "gamma",
    "beta",
    "moving_mean",
    "moving_var",
};

// constructor and deconstructor
template <typename DType>
BN<DType>::BN(const xir::Subgraph* subg,
    const xir::Op* op, IMapTBs_t inputs, CPUTBPtr_t output)
    : CPUOPBase(subg, op, inputs, output) {

  // get i/o fmap
  GET_INPUT_DIMX_FMAP(raw_fmap_i_, input, 2);
  fmap_i_ = raw_fmap_i_;

  GET_INPUT_DIM1_FMAP(fmap_gamma_, gamma);
  GET_INPUT_DIM1_FMAP(fmap_beta_, beta);
  GET_INPUT_DIM1_FMAP(fmap_mm_, moving_mean);
  GET_INPUT_DIM1_FMAP(fmap_mv_, moving_var);

  GET_OUTPUT_DIMX_FMAP(fmap_o_, 2);

  epsilon_ = CPUOPBase::xir_op_->get_attr<float>("epsilon");
}

template <typename DType>
void BN<DType>::print_param() {
  if(!CPUCfg::Instance().debug())
    return;

  raw_fmap_i_.print_param("raw_fmap_i");
  fmap_i_.print_param("fmap_i");

  fmap_gamma_.print_param("fmap_gamma");
  fmap_beta_.print_param("fmap_beta");
  fmap_mm_.print_param("fmap_mm");
  fmap_mv_.print_param("fmap_mv");

  fmap_o_.print_param("fmap_o");

  UNI_LOG_DEBUG_INFO << "epsilon = " << epsilon_ << endl;
}

template <typename DType>
void BN<DType>::check_param() {

}

template <typename DType>
uint64_t BN<DType>::get_workload() {
  return 0;
}

template <typename DType>
void BN<DType>::read() {
  read_input();
  read_gamma();
  read_beta();
  read_moving_mean();
  read_moving_var();

  rlt_ = GET_CPUTB_DType_PTR(DType, output_);
}

template <typename DType>
void BN<DType>::read_input() {
  auto* cputb = inputs_.at(ITName[INPUT]).at(0);
  img_ = GET_CPUTB_DType_PTR(DType, cputb);
}

template <typename DType>
void BN<DType>::read_gamma() {
  auto* gamma_tb = inputs_.at(ITName[GAMMA]).at(0);
  gamma_ = GET_CPUTB_DType_PTR(DType, gamma_tb);
}

template <typename DType>
void BN<DType>::read_beta() {
  auto* beta_tb = inputs_.at(ITName[BETA]).at(0);
  beta_ = GET_CPUTB_DType_PTR(DType, beta_tb);
}

template <typename DType>
void BN<DType>::read_moving_mean() {
  auto* moving_mean_tb = inputs_.at(ITName[MOVING_MEAN]).at(0);
  moving_mean_ = GET_CPUTB_DType_PTR(DType, moving_mean_tb);
}

template <typename DType>
void BN<DType>::read_moving_var() {
  auto* moving_var_tb = inputs_.at(ITName[MOVING_VAR]).at(0);
  moving_var_ = GET_CPUTB_DType_PTR(DType, moving_var_tb);
}

template <typename DType>
void BN<DType>::bn() {
  for (auto n = 0; n < fmap_o_.n; n++)
    for (auto h = 0; h < fmap_o_.h; h++)
      for (auto w = 0; w < fmap_o_.w; w++) {
        for (auto c = 0; c < fmap_o_.c; c++) {
          int idx = ((n * fmap_o_.h + h) * fmap_o_.w + w) * fmap_o_.c + c;
          // rlt_[idx] = (img_[idx] - moving_mean_[c]) /
          //                 sqrt(moving_var_[c] + epsilon_) * gamma_[c] +
          //             beta_[c];
          DType inv_var = 1 / std::sqrt(moving_var_[c] +
                                        static_cast<DType>(epsilon_));
          DType alpha = inv_var * gamma_[c];
          DType beta = beta_[c] - moving_mean_[c] * inv_var * gamma_[c];
          rlt_[idx] = img_[idx] * alpha + beta;
        }
      }
}

template <typename DType>
void BN<DType>::run() {
  read();
  bn();
}

INSTANTIATE_TPCLASS(BN);
REG_OP_INSTANCE_FUNC("batchnorm", BN);
REG_OP_INSTANCE_FUNC("batchnorm2d", BN);

}  // namespace cpu
}  // namespace vart
