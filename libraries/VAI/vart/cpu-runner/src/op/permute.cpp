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

#include "permute.hpp"
#include "cpu_types.hpp"

namespace vart {
namespace cpu {

template <typename DType>
const vector<string> Permute<DType>::ITName = {
    "input",
};

// constructor and deconstructor
template <typename DType>
Permute<DType>::Permute(const xir::Subgraph* subg, const xir::Op* op,
                        IMapTBs_t inputs, CPUTBPtr_t output)
    : CPUOPBase(subg, op, inputs, output) {
  GET_INPUT_DIMX_FMAP(fmap_i_, input, 4);
  GET_OUTPUT_DIMX_FMAP(fmap_o_, 4);

  // get order attr
  order_ = xir_op_->get_attr<vector<int>>("order");
  UNI_LOG_CHECK(order_.size() == 4, VART_INVALID_VALUE);

  // print param value, used to debug
  print_param();
}

template <typename DType>
void Permute<DType>::run() {
  // // do necessary check
  check_param();

  // // read data
  read();

  // do permute
  permute();

  // // do save, debug...
  // save();
}

template <typename DType>
void Permute<DType>::print_param() {
  fmap_i_.print_param("fmap_i");
  fmap_o_.print_param("fmap_o");
  UNI_LOG_DEBUG_INFO << "order: " << Vec2Str(order_, ", ") << endl;
}

template <typename DType>
void Permute<DType>::check_param() {
  UNI_LOG_CHECK(inputs_.size() == 1, VART_SIZE_ERROR);

  // check fmap_i_ and fmap_o_'s size
  UNI_LOG_CHECK(fmap_i_.num() == fmap_o_.num(),
                VART_INVALID_VALUE)
      << ", " << fmap_i_.num() << " != " << fmap_o_.num();
}

// read data from DDR
template <typename DType>
void Permute<DType>::read() {
  // read img data
  auto* cputb = inputs_.at(ITName[INPUT]).at(0);
  data_in_ = GET_CPUTB_DType_PTR(DType, cputb);

  // handle output buffer
  data_out_ = GET_CPUTB_DType_PTR(DType, output_);
}

template <typename DType>
uint64_t Permute<DType>::get_workload() {
  // TODO
  return 0;
}

template <typename DType>
void Permute<DType>::permute() {
  // TODO: Support other dst format other than CHW & HWC.
  if ((order_[0] == 0) && (order_[1] == 3)        // CHW
       && (order_[2] == 1) && (order_[3] == 2)) {
    HWC2CHW(fmap_i_.h, fmap_i_.w, fmap_i_.c, data_in_, data_out_);
  } else if ((order_[0] == 0) && (order_[1] == 1) // HWC
       && (order_[2] == 2) && (order_[3] == 3)) {
    std::copy_n(data_in_, fmap_i_.num(), data_out_);
  } else {
    UNI_LOG_FATAL(VART_NOT_SUPPORT)
       << "Not supported order [" << order_[0] << "," << order_[1] << ","
       << order_[2] << "," << order_[3] << "], for permute " << xir_op_->get_name();
  }

}

INSTANTIATE_TPCLASS(Permute);
REG_OP_INSTANCE_FUNC("permute", Permute);
REG_OP_INSTANCE_FUNC("transpose", Permute);

}  // namespace cpu
}  // namespace vart
