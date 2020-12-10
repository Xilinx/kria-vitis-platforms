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

#include "leaky_relu.hpp"

namespace vart {
namespace cpu {

template <typename DType>
LeakyRelu<DType>::LeakyRelu(const xir::Subgraph* subg, const xir::Op* op,
                            IMapTBs_t inputs, CPUTBPtr_t output)
    : ReluBase<DType>(subg, op, inputs, output) {
  alpha_ = CPUOPBase::xir_op_->get_attr<float>("alpha");
}

template <typename DType>
void LeakyRelu<DType>::run() {
  // // do necessary check
  // print_param();
  // this->check_param();

  // // read data
  // this->read();

  // do leaky_relu
  leaky_relu();

  // // do save, debug...
  // this->save();
}

template <typename DType>
void LeakyRelu<DType>::print_param() {
  ReluBase<DType>::print_param();

  UNI_LOG_DEBUG_INFO << "alpha: " << alpha_ << endl;
}

template <typename DType>
void LeakyRelu<DType>::leaky_relu() {
  copy_n(data_in_ptr_, fmap_o_.num(), data_out_ptr_);

  for (auto i = 0; i < fmap_o_.num(); i++) {
    if (data_out_ptr_[i] < 0) {
      data_out_ptr_[i] *= alpha_;
    }
  }
}

INSTANTIATE_TPCLASS(LeakyRelu);
REG_OP_INSTANCE_FUNC("leaky-relu", LeakyRelu);

}  // namespace cpu
}  // namespace vart
