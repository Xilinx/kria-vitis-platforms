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

#include "shape.hpp"

namespace vart {
namespace cpu {

template <typename DType>
const vector<string> Shape<DType>::ITName = {
    "input",
};

// constructor and deconstructor
template <typename DType>
Shape<DType>::Shape(const xir::Subgraph* subg, const xir::Op* op,
                    IMapTBs_t inputs, CPUTBPtr_t output)
    : CPUOPBase(subg, op, inputs, output) {
  GET_INPUT_DIMX_FMAP(fmap_i_, input, 4);
  GET_OUTPUT_DIMX_FMAP(fmap_o_, 1);
}

template <typename DType>
void Shape<DType>::run() {
  // // do necessary check
  // print_param();
  // check_param();

  // // read data
  // read();

  // do shape
  shape();

  // // do save, debug...
  // save();
}

template <typename DType>
void Shape<DType>::print_param() {
  UNI_LOG_DEBUG_INFO << "fmap_i: " << Vec2Str(fmap_i_, ", ") << endl;
  UNI_LOG_DEBUG_INFO << "fmap_o: " << Vec2Str(fmap_o_, ", ") << endl;
}

template <typename DType>
void Shape<DType>::check_param() {
  UNI_LOG_CHECK(inputs_.size() == 1, VART_SIZE_ERROR);

  // check shape validation
  UNI_LOG_CHECK((int)fmap_i_.size() == fmap_o_[0], VART_INVALID_VALUE)
      << ", " << fmap_i_.size() << " != " << fmap_o_[0];
}

// read data from DDR
template <typename DType>
void Shape<DType>::read() {
  // NOTE: not need to read img data

  // handle output buffer
  rlt_ = GET_CPUTB_DType_PTR(DType, output_);
}

template <typename DType>
uint64_t Shape<DType>::get_workload() {
  // TODO
  return 0;
}

template <typename DType>
void Shape<DType>::shape() {
  // just assign value, nothing to do else
  for (auto i = 0; i < fmap_o_[0]; i++) {
    rlt_[i] = fmap_i_[i];
  }
}

INSTANTIATE_TPCLASS(Shape);
REG_OP_INSTANCE_FUNC("shape", Shape);

}  // namespace cpu
}  // namespace vart
