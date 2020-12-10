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

#include "const.hpp"

namespace vart {
namespace cpu {

// constructor and deconstructor
template <typename DType>
Const<DType>::Const(const xir::Subgraph* subg, const xir::Op* op,
                    IMapTBs_t inputs, CPUTBPtr_t output)
    : CPUOPBase(subg, op, inputs, output) {
  GET_OUTPUT_DIMX_FMAP(fmap_o_, 6);

  data_in_vec_ = xir_op_->get_attr<vector<char>>("data");

  data_num_ = get_vec_mul(fmap_o_);
}

template <typename DType>
void Const<DType>::run() {
  // // do necessary check
  // print_param();
  // check_param();

  // // read data
  // read();

  process();

  // // do save, debug...
  // save();
}

// public funcs
template <typename DType>
void Const<DType>::print_param() {
  UNI_LOG_DEBUG_INFO << "fmap_o: " << Vec2Str(fmap_o_, ", ") << endl;
  UNI_LOG_DEBUG_INFO << "output dtype: " << get_data_type_str(output_tensor_)
                     << endl;
}

// private funcs
template <typename DType>
void Const<DType>::check_param() {
  UNI_LOG_CHECK(inputs_.size() == 0, VART_SIZE_ERROR);

  int data_size = data_in_vec_.size();

  UNI_LOG_CHECK(data_size == data_num_ * ((bit_width_ + 7) / 8),
                VART_SIZE_ERROR)
      << ", " << data_size << " != " << data_num_ << "*"
      << ((bit_width_ + 7) / 8);
}

// read data from op's attribute
template <typename DType>
void Const<DType>::read() {
  // handle output buffer
  data_out_ptr_ = GET_CPUTB_DType_PTR(DType, output_);

  data_in_ptr_ = data_in_vec_.data();
}

template <typename DType>
void Const<DType>::process() {
  if (data_type_ == xir::DataType::FLOAT) {
    std::copy_n((DType*)data_in_ptr_, data_num_, data_out_ptr_);
  } else {
    if ((data_type_ == xir::DataType::XINT ||
         data_type_ == xir::DataType::XUINT) &&
        bit_width_ == 24) {
      process_for_threshold();
    } else {
      process_normal();
    }
  }
}

template <typename DType>
void Const<DType>::process_normal() {
  for (auto i = 0; i < data_num_; i++) {
    data_out_ptr_[i] = static_cast<DType>(data_in_ptr_[i]);
  }
}

template <typename DType>
void Const<DType>::process_for_threshold() {
  UNI_LOG_CHECK(sizeof(DType) == 4, VART_SIZE_ERROR);

  struct threshold_src_t {
    char data0 : 8, data1 : 8, data2 : 8, data3 : 8;
  };
  struct threshold_dst_t {  // no use here, just for reference
    int32_t signal : 1, delta : 10, base : 13, unused : 8;
  };

  char* src_ptr = (char*)(data_in_ptr_);
  auto* dst_ptr = (threshold_src_t*)(output_->get_data_ptr());
  for (auto i = 0; i < data_num_; i++) {
    (dst_ptr + i)->data0 = *(src_ptr + 3 * i + 0);
    (dst_ptr + i)->data1 = *(src_ptr + 3 * i + 1);
    (dst_ptr + i)->data2 = *(src_ptr + 3 * i + 2);
    (dst_ptr + i)->data3 = 0;
  }
}

INSTANTIATE_TPCLASS(Const);

REG_OP_INSTANCE_FUNC("const", Const);
REG_OP_INSTANCE_FUNC("const-fix", Const);
}  // namespace cpu
}  // namespace vart
