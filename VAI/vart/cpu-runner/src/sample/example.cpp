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

#include "example.hpp"

namespace vart {
namespace cpu {

template <typename DType>
const vector<string> Example<DType>::ITName = {
    "input",
};

// constructor and deconstructor
template <typename DType>
Example<DType>::Example(const xir::Subgraph* subg, const xir::Op* op,
                        IMapTBs_t inputs, CPUTBPtr_t output)
    : CPUOPBase(subg, op, inputs, output) {
  /* STEP3: if op having multi-input,
   * should using following definition and get routine
   * \code
   *    vector<Dimension> fmap_i_;
   *    GET_INPUT_DIMX_FMAPS(fmap_i_, input, 4);
   * \endcode
   */
  GET_INPUT_DIMX_FMAP(fmap_i_, input, 4);
  GET_OUTPUT_DIMX_FMAP(fmap_o_, 4);

  /* STEP4: read reated attrs here
   * \code
   *    example_attr_a_ = xir_op_->get_attr<int>("attr_a");
   *    example_attr_b_ = xir_op_->get_attr<int>("attr_b");
   *    example_attr_c_ = xir_op_->get_attr<int>("attr_c");
   * \endcode
   */
}

template <typename DType>
void Example<DType>::run() {
  /* STEP5: implementation here,
   * NOTE: please don't call read/check_param/print_param/save routines
   * which have been called by scheduler.
   */
}

template <typename DType>
void Example<DType>::print_param() {
  fmap_i_.print_param("fmap_i");
  fmap_o_.print_param("fmap_o");

  // STEP6: print attribute vars here
  UNI_LOG_DEBUG_INFO << "example_attr_a: " << example_attr_a_ << endl;
  UNI_LOG_DEBUG_INFO << "example_attr_b: " << example_attr_b_ << endl;
  UNI_LOG_DEBUG_INFO << "example_attr_c: " << example_attr_c_ << endl;
}

template <typename DType>
void Example<DType>::check_param() {
  /* STEP7: add check code here
   * \code
   *    UNI_LOG_CHECK(inputs_.size() == 1, VART_SIZE_ERROR);
   * \endcode
   */
}

// read data from DDR
template <typename DType>
void Example<DType>::read() {
  /* STEP8: asign input pointer, if having multi-input,
   * you can refer to following code snippet
   * \code
   *    data_in_ptr_.resize(input_num_);
   *    for (auto idx = 0; idx < input_num_; idx++) {
   *      auto* cputb = inputs_.at(ITName[INPUT]).at(idx);
   *      data_in_ptr_[idx] = GET_CPUTB_DType_PTR(DType, cputb);
   *    }
   * \endcode
   * otherwise, refer to following code snippet
   */
  auto* cputb = inputs_.at(ITName[INPUT]).at(0);
  data_in_ptr_ = GET_CPUTB_DType_PTR(DType, cputb);

  // handle output buffer
  data_out_ptr_ = GET_CPUTB_DType_PTR(DType, output_);
}

template <typename DType>
uint64_t Example<DType>::get_workload() {
  // STEP9: calculate workload here
  return 0;
}

INSTANTIATE_TPCLASS(Example);
// STEP10: change xir op def name here
REG_OP_INSTANCE_FUNC("example", Example);

}  // namespace cpu
}  // namespace vart
