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

#include "reshape.hpp"

namespace vart {
namespace cpu {

template <typename DType>
const vector<string> Reshape<DType>::ITName = {"input", "shape"};

// constructor and deconstructor
template <typename DType>
Reshape<DType>::Reshape(const xir::Subgraph* subg, const xir::Op* op,
                        IMapTBs_t inputs, CPUTBPtr_t output)
    : CPUOPBase(subg, op, inputs, output) {
  GET_INPUT_DIMX_FMAP(fmap_i_, input, 6);
  GET_OUTPUT_DIMX_FMAP(fmap_o_, 6);

  if (xir_op_->get_input_num(ITName[SHAPE]) == 1) {
    auto* xir_tensor_s = xir_op_->get_input_tensor(ITName[SHAPE]);
    UNI_LOG_CHECK(xir_tensor_s != nullptr, VART_NULL_PTR);
    fmap_s_ = xir_tensor_s->get_shape();
  } else {
    fmap_s_ = xir_op_->get_attr<vector<int>>(ITName[SHAPE]);
  }
}

template <typename DType>
void Reshape<DType>::run() {
  // // do necessary check
  // print_param();
  // check_param();

  // // read data
  // read();

  // do reshape
  reshape();

  // // do save, debug...
  // save();
}

template <typename DType>
void Reshape<DType>::print_param() {
  UNI_LOG_DEBUG_INFO << "fmap_i: " << Vec2Str(fmap_i_, ", ") << endl;
  UNI_LOG_DEBUG_INFO << "fmap_s: " << Vec2Str(fmap_s_, ", ") << endl;
  UNI_LOG_DEBUG_INFO << "fmap_o: " << Vec2Str(fmap_o_, ", ") << endl;
}

template <typename DType>
void Reshape<DType>::check_param() {
  UNI_LOG_CHECK(inputs_.size() == 1 || inputs_.size() == 2, VART_SIZE_ERROR);

  UNI_LOG_CHECK(fmap_i_.size() <= 6, VART_SIZE_ERROR)
      << ", " << fmap_i_.size() << " > 6";
  UNI_LOG_CHECK(fmap_o_.size() <= 6, VART_SIZE_ERROR)
      << ", " << fmap_o_.size() << " > 6";

  UNI_LOG_CHECK(get_vec_mul(fmap_i_) == get_vec_mul(fmap_o_), VART_SIZE_ERROR);
}

// read data from DDR
template <typename DType>
void Reshape<DType>::read() {
  // read img data
  auto* cputb = inputs_.at(ITName[INPUT]).at(0);
  data_in_ = GET_CPUTB_DType_PTR(DType, cputb);

  // NOTE: not read shape data

  // handle output buffer
  data_out_ = GET_CPUTB_DType_PTR(DType, output_);
}

template <typename DType>
uint64_t Reshape<DType>::get_workload() {
  // TODO
  return 0;
}

template <typename DType>
void Reshape<DType>::reshape() {
  // just assign value, nothing to do else
  std::copy_n(data_in_, get_vec_mul(fmap_i_), data_out_);
}

INSTANTIATE_TPCLASS(Reshape);
REG_OP_INSTANCE_FUNC("reshape", Reshape);
REG_OP_INSTANCE_FUNC("reshape-fix", Reshape);

}  // namespace cpu
}  // namespace vart
