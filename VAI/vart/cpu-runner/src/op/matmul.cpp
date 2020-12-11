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

#include "matmul.hpp"

namespace vart {
namespace cpu {

template <typename DType>
const vector<string> Matmul<DType>::ITName = {"input", "bias"};

// constructor and deconstructor
template <typename DType>
Matmul<DType>::Matmul(const xir::Subgraph* subg, const xir::Op* op,
                      IMapTBs_t inputs, CPUTBPtr_t output)
    : CPUOPBase(subg, op, inputs, output) {
  // get input tensor's fmap
  GET_INPUT_DIMX_FMAPS(fmap_i_, input, 6);
  GET_OUTPUT_DIMX_FMAP(fmap_o_, 6);
  UNI_LOG_CHECK(fmap_i_.size() == 2, VART_SIZE_ERROR)
      << ", " << fmap_i_.size() << " != 2";
  fmap_ia_ = fmap_i_[0];
  fmap_ib_ = fmap_i_[1];

  // get bias tensor's fmap
  has_bias_ = xir_op_->get_input_num(ITName[BIAS]) > 0 ? true : false;
  if (has_bias_) {
    GET_OUTPUT_DIMX_FMAP(fmap_b_, 6);
  }

  transpose_a_ = xir_op_->get_attr<bool>("transpose_a");
  transpose_b_ = xir_op_->get_attr<bool>("transpose_b");

  // must run this function at the end of ctor,
  // because it will use upper attributes.
  calc_param();
}

template <typename DType>
void Matmul<DType>::run() {
  for (auto i = 0; i < fmap_o_.num(); i++) {
    auto coord = fmap_o_.pos2coord(i);

    auto coord_a = coord;
    auto coord_b = coord;
    for (auto j = 0; j < fmap_o_.ndims(); j++) {
      if (fmap_ia_[j] == 1 || j == fmap_o_.ndims() - 1) {
        coord_a[j] = 0;
      }

      if (fmap_ib_[j] == 1 || j == fmap_o_.ndims() - 2) {
        coord_b[j] = 0;
      }
    }

    auto pos_a = fmap_ia_.coord2pos(coord_a);
    auto pos_b = fmap_ib_.coord2pos(coord_b);
    data_out_[i] = 0;
    for(auto j=0; j<K_; j++) {
      data_out_[i] += data_ina_[pos_a + j] * data_inb_[pos_b + j * N_];
    }

    if (has_bias_) {
      data_out_[i] += data_bias_[i % N_];
    }
  }
}

template <typename DType>
void Matmul<DType>::print_param() {
  fmap_i_[0].print_param("original fmap_ia");
  UNI_LOG_DEBUG_INFO << "transpose_a: " << Bool2Str(transpose_a_) << endl;
  fmap_ia_.print_param("transposed fmap_ia");
  UNI_LOG_DEBUG_INFO << "batch_a: " << batch_a_ << endl;

  fmap_i_[1].print_param("original fmap_ib");
  UNI_LOG_DEBUG_INFO << "transpose_b: " << Bool2Str(transpose_b_) << endl;
  fmap_ib_.print_param("transposed fmap_ib");
  UNI_LOG_DEBUG_INFO << "batch_b: " << batch_b_ << endl;

  if (has_bias_) fmap_b_.print_param("fmap_b");
  fmap_o_.print_param("fmap_o: ");
}

template <typename DType>
void Matmul<DType>::check_param() {
  CHECK(fmap_ia_.ndims() >= 2);
  CHECK(fmap_ib_.ndims() >= 2);
  CHECK(fmap_ia_.ndims() == fmap_ib_.ndims());
  CHECK(fmap_ia_.ndims() == fmap_o_.ndims());
  CHECK(M_ == fmap_o_.dim(fmap_o_.ndims() - 2));
  CHECK(N_ == fmap_o_.dim(fmap_o_.ndims() - 1));
}

template <typename DType>
void Matmul<DType>::read() {
  // read input data
  auto* data_ina_tb = inputs_.at(ITName[INPUT]).at(0);
  auto* data_inb_tb = inputs_.at(ITName[INPUT]).at(1);
  data_ina_ = GET_CPUTB_DType_PTR(DType, data_ina_tb);
  data_inb_ = GET_CPUTB_DType_PTR(DType, data_inb_tb);

  // do transpose for inputs
  if (transpose_a_) {
    for (auto i = 0; i < batch_a_; i++) {
      auto size_a = M_ * K_;
      auto* pa = data_ina_ + i * size_a;
      vector<DType> tmp_a(size_a);
      HW2WH(K_, M_, pa, tmp_a.data());
      std::copy_n(tmp_a.begin(), size_a, pa);
    }
  }

  if (transpose_b_) {
    for (auto i = 0; i < batch_b_; i++) {
      auto size_b = K_ * N_;
      auto* pb = data_inb_ + i * size_b;
      vector<DType> tmp_b(size_b);
      HW2WH(N_, K_, pb, tmp_b.data());
      std::copy_n(tmp_b.begin(), size_b, pb);
    }
  }

  // check bias dim and get bias ptr
  if (has_bias_) {
    auto* data_bias_tb = inputs_.at(ITName[BIAS]).at(0);
    data_bias_ = GET_CPUTB_DType_PTR(DType, data_bias_tb);
  }

  data_out_ = GET_CPUTB_DType_PTR(DType, output_);
}

template <typename DType>
uint64_t Matmul<DType>::get_workload() {
  uint64_t mkn = 2 * M_ * K_ * N_;
  uint64_t batch_out = fmap_o_.num() / M_ / N_;
  return batch_out * mkn;
}

template <typename DType>
void Matmul<DType>::calc_param() {
  if (transpose_a_) {
    M_ = fmap_ia_.dim(fmap_ia_.ndims() - 1);
    K_ = fmap_ia_.dim(fmap_ia_.ndims() - 2);
    batch_a_ = fmap_ia_.num() / M_ / K_;
    fmap_ia_.set_dim(fmap_ia_.ndims() - 2, M_);
    fmap_ia_.set_dim(fmap_ia_.ndims() - 1, K_);
  } else {
    M_ = fmap_ia_.dim(fmap_ia_.ndims() - 2);
    K_ = fmap_ia_.dim(fmap_ia_.ndims() - 1);
    batch_a_ = fmap_ia_.num() / M_ / K_;
  }

  if (transpose_b_) {
    N_ = fmap_ib_.dim(fmap_ib_.ndims() - 2);
    UNI_LOG_CHECK(fmap_ib_.dim(fmap_ib_.ndims() - 1) == K_, VART_SIZE_ERROR)
        << ", " << fmap_ib_.dim(fmap_ib_.ndims() - 1) << " != " << K_;
    batch_b_ = fmap_ib_.num() / K_ / N_;
    fmap_ib_.set_dim(fmap_ib_.ndims() - 2, K_);
    fmap_ib_.set_dim(fmap_ib_.ndims() - 1, N_);
  } else {
    N_ = fmap_ib_.dim(fmap_ib_.ndims() - 1);
    UNI_LOG_CHECK(fmap_ib_.dim(fmap_ib_.ndims() - 2) == K_, VART_SIZE_ERROR)
        << ", " << fmap_ib_.dim(fmap_ib_.ndims() - 2) << " != " << K_;
    batch_b_ = fmap_ib_.num() / K_ / N_;
  }

  // make fmap_ia_ and fmap_ib_ have the same dims
  if (fmap_ia_.ndims() > fmap_ib_.ndims()) {
    while (fmap_ia_.ndims() != fmap_ib_.ndims()) {
      fmap_ib_.insert_dim(0, 1);
    }
  } else if (fmap_ia_.ndims() < fmap_ib_.ndims()) {
    while (fmap_ia_.ndims() != fmap_ib_.ndims()) {
      fmap_ia_.insert_dim(0, 1);
    }
  }
}

INSTANTIATE_TPCLASS(Matmul);

REG_OP_INSTANCE_FUNC("matmul", Matmul);

}  // namespace cpu
}  // namespace vart
