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

#include "priorbox.hpp"

namespace vart {
namespace cpu {

template <typename DType>
const vector<string> PriorBox<DType>::ITName = {
    "input",
};

// constructor and deconstructor
template <typename DType>
PriorBox<DType>::PriorBox(const xir::Subgraph* subg, const xir::Op* op,
                          IMapTBs_t inputs, CPUTBPtr_t output)
    : CPUOPBase(subg, op, inputs, output) {
  GET_INPUT_DIMX_FMAPS(fmap_i_, input, 4);
  GET_OUTPUT_DIMX_FMAP(fmap_o_, 4);

  input_num_ = fmap_i_.size();

  // get axis attr and transfer it to 4-dimension axis
  min_sizes_ = xir_op_->get_attr<vector<float>>("min_sizes");
  max_sizes_ = xir_op_->get_attr<vector<float>>("max_sizes");
  aspect_ratio_ = xir_op_->get_attr<vector<float>>("aspect_ratio");
  flip_ = xir_op_->get_attr<bool>("flip");
  clip_ = xir_op_->get_attr<bool>("clip");
  variance_ = xir_op_->get_attr<vector<float>>("variance");
  step_ = xir_op_->get_attr<vector<float>>("step");
  offset_ = xir_op_->get_attr<float>("offset");

  // print param value, used to debug
  print_param();
}

template <typename DType>
void PriorBox<DType>::run() {
  // // do necessary check
  // check_param();

  // // read data
  // read();

  // calc priorbox
  priorbox();

  // // do save, debug...
  // save();
}

template <typename DType>
void PriorBox<DType>::print_param() {
  for (auto i = 0; i < input_num_; i++) {
    fmap_i_[i].print_param("fmap_i" + to_string(i));
  }
  fmap_o_.print_param("fmap_o");

  UNI_LOG_DEBUG_INFO << "min_sizes = " << Vec2Str(min_sizes_, ", ") << endl;
  UNI_LOG_DEBUG_INFO << "max_sizes = " << Vec2Str(max_sizes_, ", ") << endl;
  UNI_LOG_DEBUG_INFO << "aspect_ratio = " << Vec2Str(aspect_ratio_, ", ")
                     << endl;
  UNI_LOG_DEBUG_INFO << "flip = " << Bool2Str(flip_) << endl;
  UNI_LOG_DEBUG_INFO << "clip = " << Bool2Str(clip_) << endl;
  UNI_LOG_DEBUG_INFO << "variance = " << Vec2Str(variance_, ", ") << endl;
  UNI_LOG_DEBUG_INFO << "step = " << Vec2Str(step_, ", ") << endl;
  UNI_LOG_DEBUG_INFO << "offset = " << offset_ << endl;
}

template <typename DType>
void PriorBox<DType>::check_param() {
  // TODO
}

// read data from DDR
template <typename DType>
void PriorBox<DType>::read() {
  // handle input buffer
  data_in_.resize(input_num_);
  for (auto idx = 0; idx < input_num_; idx++) {
    auto* cputb = inputs_.at(ITName[INPUT]).at(idx);
    data_in_[idx] = GET_CPUTB_DType_PTR(DType, cputb);
  }

  // handle output buffer
  data_out_ = GET_CPUTB_DType_PTR(DType, output_);
}

template <typename DType>
uint64_t PriorBox<DType>::get_workload() {
  // TODO
  return 0;
}

template <typename DType>
void PriorBox<DType>::priorbox() {
  // TODO
}

INSTANTIATE_TPCLASS(PriorBox);
REG_OP_INSTANCE_FUNC("priorbox", PriorBox);

}  // namespace cpu
}  // namespace vart
