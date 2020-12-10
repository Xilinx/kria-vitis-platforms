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

#include "stack.hpp"

namespace vart {
namespace cpu {

template <typename DType>
const vector<string> Stack<DType>::ITName = {
    "input",
};

// constructor and deconstructor
template <typename DType>
Stack<DType>::Stack(const xir::Subgraph* subg, const xir::Op* op,
                    IMapTBs_t inputs, CPUTBPtr_t output)
    : CPUOPBase(subg, op, inputs, output) {
  // get input tensor's fmap
  auto xir_input_tensors = xir_op_->get_input_tensors(ITName[INPUT]);
  input_num_ = xir_input_tensors.size();
  UNI_LOG_CHECK(input_num_ > 1, VART_SIZE_ERROR)
      << ", " << input_num_ << " <= 1";
  fmap_i_.resize(input_num_);
  for (auto i = 0; i < input_num_; i++) {
    auto* xir_tensor_i = xir_input_tensors[i];
    UNI_LOG_CHECK(xir_tensor_i != nullptr, VART_NULL_PTR);
    UNI_LOG_CHECK(
        xir_tensor_i->get_data_type() == xir_input_tensors[0]->get_data_type(),
        VART_INVALID_VALUE)
        << ", " << xir_tensor_i->get_data_type().to_string()
        << " != " << xir_input_tensors[0]->get_data_type().to_string();

    fmap_i_[i] = xir_tensor_i->get_shape();
  }

  // get output tensor's fmap
  auto* xir_tensor_o = xir_op_->get_output_tensor();
  UNI_LOG_CHECK(xir_tensor_o != nullptr, VART_NULL_PTR);
  fmap_o_ = xir_tensor_o->get_shape();

  // get axis
  axis_ = xir_op_->get_attr<int>("axis");
  if (axis_ < 0) {
    axis_ += fmap_i_[0].size();
  }

  // print param value, used to debug
  print_param();
}

template <typename DType>
void Stack<DType>::run() {
  // // do necessary check
  // check_param();

  // // read data
  // read();

  // calc stack
  stack();

  // // do save, debug...
  // save();
}

template <typename DType>
void Stack<DType>::print_param() {
  for (auto i = 0; i < input_num_; i++) {
    UNI_LOG_DEBUG_INFO << "fmap_i[" << i << "]: " << Vec2Str(fmap_i_[i], ", ")
                       << endl;
  }
  UNI_LOG_DEBUG_INFO << "fmap_o: " << Vec2Str(fmap_o_, ", ") << endl;

  UNI_LOG_DEBUG_INFO << "axis: " << axis_ << endl;
}

template <typename DType>
void Stack<DType>::check_param() {
  UNI_LOG_CHECK(inputs_.size() >= 1, VART_SIZE_ERROR)
      << ", Err: inputs size is " << inputs_.size();

  for (auto i = 1U; i < fmap_i_.size(); i++) {
    UNI_LOG_CHECK(fmap_i_[0].size() == fmap_i_[i].size(), VART_SIZE_ERROR)
        << ", " << fmap_i_[0].size() << " != " << fmap_i_[i].size();
    for (auto j = 0U; j < fmap_i_[0].size(); j++) {
      UNI_LOG_CHECK(fmap_i_[0][j] == fmap_i_[i][j], VART_INVALID_VALUE)
          << ", " << fmap_i_[0][j] << " != " << fmap_i_[i][j];
    }
  }
}

// read data from DDR
template <typename DType>
void Stack<DType>::read() {
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
uint64_t Stack<DType>::get_workload() {
  // TODO
  return 0;
}

template <typename DType>
void Stack<DType>::stack() {
  auto dim = fmap_i_[0].size();
  if (dim == 1) {
    stack_dim_1();
  } else if (dim == 2) {
    stack_dim_2();
  } else if (dim == 3) {
    stack_dim_3();
  } else {
    UNI_LOG_ERROR(VART_NOT_SUPPORT) << "Not support dim " << dim << endl;
    abort();
  }
}

template <typename DType>
void Stack<DType>::stack_dim_1() {
  // check validation
  UNI_LOG_CHECK(axis_ < 1, VART_INVALID_VALUE) << ", " << axis_ << " >= 1";
  UNI_LOG_CHECK(fmap_o_.size() == 2, VART_SIZE_ERROR)
      << ", " << fmap_o_.size() << " != 2";
  UNI_LOG_CHECK(fmap_o_[0] == input_num_, VART_INVALID_VALUE)
      << ", " << fmap_o_[0] << " != " << input_num_;
  UNI_LOG_CHECK(fmap_o_[1] == fmap_i_[0][0], VART_INVALID_VALUE)
      << ", " << fmap_o_[1] << " != " << fmap_i_[0][0];

  // reorganize input data into output data
  for (auto i = 0; i < fmap_o_[0]; i++) {  // axis is 0
    for (auto j = 0; j < fmap_o_[1]; j++) {
      auto src_addr = j;
      auto dst_addr = i * fmap_o_[1] + j;

      data_out_[dst_addr] = data_in_[i][src_addr];
    }
  }
}

template <typename DType>
void Stack<DType>::stack_dim_2() {
  // check validation
  UNI_LOG_CHECK(fmap_o_.size() == 3, VART_SIZE_ERROR)
      << ", " << fmap_o_.size() << " != 3";
  if (axis_ == 0) {
    UNI_LOG_CHECK(fmap_o_[0] == input_num_, VART_INVALID_VALUE)
        << ", " << fmap_o_[0] << " != " << input_num_;
    UNI_LOG_CHECK(fmap_o_[1] == fmap_i_[0][0], VART_INVALID_VALUE)
        << ", " << fmap_o_[1] << " != " << fmap_i_[0][0];
    UNI_LOG_CHECK(fmap_o_[2] == fmap_i_[0][1], VART_INVALID_VALUE)
        << ", " << fmap_o_[2] << " != " << fmap_i_[0][1];
  } else if (axis_ == 1) {
    UNI_LOG_CHECK(fmap_o_[0] == fmap_i_[0][0], VART_INVALID_VALUE)
        << ", " << fmap_o_[0] << " != " << fmap_i_[0][0];
    UNI_LOG_CHECK(fmap_o_[1] == input_num_, VART_INVALID_VALUE)
        << ", " << fmap_o_[1] << " != " << input_num_;
    UNI_LOG_CHECK(fmap_o_[2] == fmap_i_[0][1], VART_INVALID_VALUE)
        << ", " << fmap_o_[2] << " != " << fmap_i_[0][1];
  } else {
    UNI_LOG_ERROR(VART_NOT_SUPPORT) << "Not support axis " << axis_ << endl;
    abort();
  }

  // reorganize input data into output data
  if (axis_ == 0) {
    for (auto i = 0; i < fmap_o_[0]; i++) {  // axis is 0
      for (auto j = 0; j < fmap_o_[1]; j++) {
        for (auto k = 0; k < fmap_o_[2]; k++) {
          auto src_addr = j * fmap_o_[2] + k;
          auto dst_addr = i * fmap_o_[1] * fmap_o_[2] + j * fmap_o_[2] + k;

          data_out_[dst_addr] = data_in_[i][src_addr];
        }
      }
    }
  } else if (axis_ == 1) {
    for (auto i = 0; i < fmap_o_[0]; i++) {
      for (auto j = 0; j < fmap_o_[1]; j++) {  // axis is 1
        for (auto k = 0; k < fmap_o_[2]; k++) {
          auto src_addr = i * fmap_o_[2] + k;
          auto dst_addr = i * fmap_o_[1] * fmap_o_[2] + j * fmap_o_[2] + k;

          data_out_[dst_addr] = data_in_[i][src_addr];
        }
      }
    }
  }
}

template <typename DType>
void Stack<DType>::stack_dim_3() {
  // check validation
  UNI_LOG_CHECK(fmap_o_.size() == 4, VART_SIZE_ERROR)
      << ", " << fmap_o_.size() << " != 4";
  if (axis_ == 0) {
    UNI_LOG_CHECK(fmap_o_[0] == input_num_, VART_INVALID_VALUE)
        << ", " << fmap_o_[0] << " != " << input_num_;
    UNI_LOG_CHECK(fmap_o_[1] == fmap_i_[0][0], VART_INVALID_VALUE)
        << ", " << fmap_o_[1] << " != " << fmap_i_[0][0];
    UNI_LOG_CHECK(fmap_o_[2] == fmap_i_[0][1], VART_INVALID_VALUE)
        << ", " << fmap_o_[2] << " != " << fmap_i_[0][1];
    UNI_LOG_CHECK(fmap_o_[3] == fmap_i_[0][2], VART_INVALID_VALUE)
        << ", " << fmap_o_[3] << " != " << fmap_i_[0][2];
  } else if (axis_ == 1) {
    UNI_LOG_CHECK(fmap_o_[0] == fmap_i_[0][0], VART_INVALID_VALUE)
        << ", " << fmap_o_[0] << " != " << fmap_i_[0][0];
    UNI_LOG_CHECK(fmap_o_[1] == input_num_, VART_INVALID_VALUE)
        << ", " << fmap_o_[1] << " != " << input_num_;
    UNI_LOG_CHECK(fmap_o_[2] == fmap_i_[0][1], VART_INVALID_VALUE)
        << ", " << fmap_o_[2] << " != " << fmap_i_[0][1];
    UNI_LOG_CHECK(fmap_o_[3] == fmap_i_[0][2], VART_INVALID_VALUE)
        << ", " << fmap_o_[3] << " != " << fmap_i_[0][2];
  } else if (axis_ == 2) {
    UNI_LOG_CHECK(fmap_o_[0] == fmap_i_[0][0], VART_INVALID_VALUE)
        << ", " << fmap_o_[0] << " != " << fmap_i_[0][0];
    UNI_LOG_CHECK(fmap_o_[1] == fmap_i_[0][1], VART_INVALID_VALUE)
        << ", " << fmap_o_[1] << " != " << fmap_i_[0][1];
    UNI_LOG_CHECK(fmap_o_[2] == input_num_, VART_INVALID_VALUE)
        << ", " << fmap_o_[2] << " != " << input_num_;
    UNI_LOG_CHECK(fmap_o_[3] == fmap_i_[0][2], VART_INVALID_VALUE)
        << ", " << fmap_o_[3] << " != " << fmap_i_[0][2];
  } else {
    UNI_LOG_ERROR(VART_NOT_SUPPORT) << "Not support axis " << axis_ << endl;
    abort();
  }

  // reorganize input data into output data
  if (axis_ == 0) {
    for (auto i = 0; i < fmap_o_[0]; i++) {  // axis is 0
      for (auto j = 0; j < fmap_o_[1]; j++) {
        for (auto k = 0; k < fmap_o_[2]; k++) {
          for (auto m = 0; m < fmap_o_[3]; m++) {
            auto src_addr = j * fmap_o_[2] * fmap_o_[3] + k * fmap_o_[3] + m;
            auto dst_addr = i * fmap_o_[1] * fmap_o_[2] * fmap_o_[3] +
                            j * fmap_o_[2] * fmap_o_[3] + k * fmap_o_[3] + m;

            data_out_[dst_addr] = data_in_[i][src_addr];
          }
        }
      }
    }
  } else if (axis_ == 1) {
    for (auto i = 0; i < fmap_o_[0]; i++) {
      for (auto j = 0; j < fmap_o_[1]; j++) {  // axis is 1
        for (auto k = 0; k < fmap_o_[2]; k++) {
          for (auto m = 0; m < fmap_o_[3]; m++) {
            auto src_addr = i * fmap_o_[2] * fmap_o_[3] + k * fmap_o_[3] + m;
            auto dst_addr = i * fmap_o_[1] * fmap_o_[2] * fmap_o_[3] +
                            j * fmap_o_[2] * fmap_o_[3] + k * fmap_o_[3] + m;

            data_out_[dst_addr] = data_in_[i][src_addr];
          }
        }
      }
    }
  } else if (axis_ == 2) {
    for (auto i = 0; i < fmap_o_[0]; i++) {
      for (auto j = 0; j < fmap_o_[1]; j++) {
        for (auto k = 0; k < fmap_o_[2]; k++) {  // axis is 2
          for (auto m = 0; m < fmap_o_[3]; m++) {
            auto src_addr = i * fmap_o_[1] * fmap_o_[3] + j * fmap_o_[3] + m;
            auto dst_addr = i * fmap_o_[1] * fmap_o_[2] * fmap_o_[3] +
                            j * fmap_o_[2] * fmap_o_[3] + k * fmap_o_[3] + m;

            data_out_[dst_addr] = data_in_[i][src_addr];
          }
        }
      }
    }
  }
}

INSTANTIATE_TPCLASS(Stack);
REG_OP_INSTANCE_FUNC("stack", Stack);

}  // namespace cpu
}  // namespace vart
