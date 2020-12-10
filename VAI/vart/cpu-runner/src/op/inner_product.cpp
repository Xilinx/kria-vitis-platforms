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

#include "inner_product.hpp"

namespace vart {
namespace cpu {

template <typename DType, typename TmpDType>
const vector<string> InnerProduct<DType, TmpDType>::ITName = {
    "input",
    "weights",
    "bias",
};

// constructor and deconstructor
template <typename DType, typename TmpDType>
InnerProduct<DType, TmpDType>::InnerProduct(const xir::Subgraph* subg,
                                            const xir::Op* op, IMapTBs_t inputs,
                                            CPUTBPtr_t output)
    : CPUOPBase(subg, op, inputs, output) {
    
  GET_INPUT_DIMX_FMAP(fmap_i_, input, 6);
  GET_INPUT_DIMX_FMAP(fmap_w_, weights, 6);
  GET_OUTPUT_DIMX_FMAP(fmap_o_, 6);

  has_bias_ = xir_op_->get_input_num(ITName[BIAS]) > 0 ? true : false;
  if (has_bias_) {
    GET_INPUT_DIM1_FMAP(fmap_b_, bias);
  }

  in_shape_ = op->get_input_ops("input")[0]->get_output_tensor()->get_shape();
  weights_shape_ = op->get_input_ops("weights")[0]->get_output_tensor()->get_shape();
  axis_ = op->get_attr<int>("axis");

  // print param value, used to debug
  print_param();
}

template <typename DType, typename TmpDType>
void InnerProduct<DType, TmpDType>::run() {
  // // do necessary check
  // check_param();

  // // read data
  // read();

  // calc inner_product
  inner_product();

  // add bias to inner_product result
  bias();

  // // do save, debug...
  // save();
}

template <typename DType, typename TmpDType>
void InnerProduct<DType, TmpDType>::print_param() {
  fmap_i_.print_param("fmap_i");
  fmap_w_.print_param("fmap_w");
  UNI_LOG_DEBUG_INFO << "has_bias: " << Bool2Str(has_bias_) << endl;
  fmap_b_.print_param("fmap_b");
  fmap_o_.print_param("fmap_o");

}

template <typename DType, typename TmpDType>
void InnerProduct<DType, TmpDType>::check_param() {
  // It is correct. Otherwise, the xmodel would not be passed to VART.
}

// read data from DDR
template <typename DType, typename TmpDType>
void InnerProduct<DType, TmpDType>::read() {
  // read img data
  auto* img_tb = inputs_.at(ITName[INPUT]).at(0);
  img_ = GET_CPUTB_DType_PTR(DType, img_tb);
  const auto img_data_num = img_tb->get_data_num();
  UNI_LOG_CHECK((int)img_data_num == fmap_i_.num(), VART_SIZE_ERROR)
      << ", " << img_data_num << " != " << fmap_i_.num();

  // read weights data
  // weights order after read: oc, h, w, c
  auto* weights_tb = inputs_.at(ITName[WEIGHTS]).at(0);
  weights_ = GET_CPUTB_DType_PTR(DType, weights_tb);
  const auto weights_data_num = weights_tb->get_data_num();
  UNI_LOG_CHECK((int)weights_data_num == fmap_w_.num(), VART_SIZE_ERROR)
      << ", " << weights_data_num << " != " << fmap_w_.num();

  // read bias data
  if (has_bias_) {
    auto* bias_tb = inputs_.at(ITName[BIAS]).at(0);
    bias_ = GET_CPUTB_DType_PTR(DType, bias_tb);
    const auto bias_data_num = bias_tb->get_data_num();
    UNI_LOG_CHECK((int)bias_data_num == fmap_b_.num(), VART_SIZE_ERROR)
        << ", " << bias_data_num << " != " << fmap_b_.num();
  }

  // handle output buffer
  rlt_ = GET_CPUTB_DType_PTR(DType, output_);
}

template <typename DType, typename TmpDType>
uint64_t InnerProduct<DType, TmpDType>::get_workload() {
  // TODO
  return 0;
}

template <typename DType, typename TmpDType>
void InnerProduct<DType, TmpDType>::inner_product() {
  int k = 1, in = 1, w = 1;
  for (uint i = axis_; i < in_shape_.size(); i++)
    k *= in_shape_[i];
  for (uint i = 0; i < in_shape_.size(); i++)
    in *= in_shape_[i];
  for (uint i = 0; i < weights_shape_.size(); i++)
    w *= weights_shape_[i];
  int outter = in / k;
  int oc = w / k;
  for (auto i = 0; i < outter; i++) {
    for (auto j = 0; j < oc; j++) {
      auto addr = i * oc + j;
      for (auto p = 0; p < k; p++) {
        auto img_addr = i * k + p;
        auto weights_addr = j * k + p;
        rlt_[addr] += img_[img_addr] * weights_[weights_addr];
      }
    }
  }
  if (!has_bias_) return;
  for (auto i = 0; i < outter; i++)
    for (auto j = 0; j < oc; j++)
      rlt_[i * oc + j] += bias_[j];
}

template <typename DType, typename TmpDType>
void InnerProduct<DType, TmpDType>::inner_product_one(int idx_dst_n,
                                                      int idx_dst_h,
                                                      int idx_dst_w,
                                                      int idx_oc) {
// nothing
}

template <typename DType, typename TmpDType>
void InnerProduct<DType, TmpDType>::bias() {
  // nothing
}

INSTANTIATE_TPCLASS(InnerProduct);
REG_OP_INSTANCE_FUNC("inner-product", InnerProduct);

}  // namespace cpu
}  // namespace vart
