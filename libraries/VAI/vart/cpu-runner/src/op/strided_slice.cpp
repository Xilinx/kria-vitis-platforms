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

#include "strided_slice.hpp"

namespace vart {
namespace cpu {

template <typename DType>
const vector<string> StridedSlice<DType>::ITName = {
    "input",
};

// constructor and deconstructor
template <typename DType>
StridedSlice<DType>::StridedSlice(const xir::Subgraph* subg, const xir::Op* op,
                                  IMapTBs_t inputs, CPUTBPtr_t output)
    : CPUOPBase(subg, op, inputs, output) {
  GET_INPUT_DIMX_FMAP(fmap_i_, input, 4);
  GET_OUTPUT_DIMX_FMAP(fmap_o_, 4);

  // get param
  begin_ = xir_op_->get_attr<vector<int>>("begin");
  end_ = xir_op_->get_attr<vector<int>>("end");
  strides_ = xir_op_->get_attr<vector<int>>("strides");
  // begin_mask_ = xir_op_->get_attr<int>("begin_mask");
  // end_mask_ = xir_op_->get_attr<int>("end_mask");
  // ellipsis_mask_ = xir_op_->get_attr<int>("ellipsis_mask");
  // new_axis_mask_ = xir_op_->get_attr<int>("new_axis_mask");
  // shrink_axis_mask_ = xir_op_->get_attr<int>("shrink_axis_mask");
}

template <typename DType>
void StridedSlice<DType>::run() {
  // // do necessary check
  // print_param();
  // check_param();

  // // read data
  // read();

  // do strided_slice
  strided_slice();

  // // do save, debug...
  // save();
}

template <typename DType>
void StridedSlice<DType>::print_param() {
  UNI_LOG_DEBUG_INFO << "fmap_i: " << Vec2Str(fmap_i_, ", ") << endl;
  UNI_LOG_DEBUG_INFO << "fmap_o: " << Vec2Str(fmap_o_, ", ") << endl;

  UNI_LOG_DEBUG_INFO << "begin: " << Vec2Str(begin_, ", ") << endl;
  UNI_LOG_DEBUG_INFO << "end: " << Vec2Str(end_, ", ") << endl;
  UNI_LOG_DEBUG_INFO << "strides: " << Vec2Str(strides_, ", ") << endl;

  UNI_LOG_DEBUG_INFO << "begin_mask: " << begin_mask_ << endl;
  UNI_LOG_DEBUG_INFO << "end_mask: " << end_mask_ << endl;
  UNI_LOG_DEBUG_INFO << "ellipsis_mask: " << ellipsis_mask_ << endl;
  UNI_LOG_DEBUG_INFO << "new_axis_mask: " << new_axis_mask_ << endl;
  UNI_LOG_DEBUG_INFO << "shrink_axis_mask: " << shrink_axis_mask_ << endl;
}

template <typename DType>
void StridedSlice<DType>::check_param() {
  UNI_LOG_CHECK(inputs_.size() == 1, VART_SIZE_ERROR);

  // check begin/end/stride
  UNI_LOG_CHECK(begin_.size() == end_.size(), VART_SIZE_ERROR)
      << ", " << begin_.size() << " != " << end_.size();
  UNI_LOG_CHECK(begin_.size() == strides_.size(), VART_SIZE_ERROR)
      << ", " << begin_.size() << " != " << strides_.size();

  UNI_LOG_CHECK(begin_.size() <= fmap_i_.size(), VART_SIZE_ERROR)
      << ", " << begin_.size() << " > " << fmap_i_.size();

  // not supported param
  UNI_LOG_CHECK(begin_mask_ == 0, VART_INVALID_VALUE)
      << ", " << begin_mask_ << " != 0";
  UNI_LOG_CHECK(end_mask_ == 0, VART_INVALID_VALUE)
      << ", " << end_mask_ << " != 0";
  UNI_LOG_CHECK(ellipsis_mask_ == 0, VART_INVALID_VALUE)
      << ", " << ellipsis_mask_ << " != 0";
  UNI_LOG_CHECK(new_axis_mask_ == 0, VART_INVALID_VALUE)
      << ", " << new_axis_mask_ << " != 0";
  // UNI_LOG_CHECK(shrink_axis_mask_ == 0, VART_INVALID_VALUE)
  //     << ", " << shrink_axis_mask_ << " != 0";
}

// read data from DDR
template <typename DType>
void StridedSlice<DType>::read() {
  // read img data
  auto* cputb = inputs_.at(ITName[INPUT]).at(0);
  data_in_ = GET_CPUTB_DType_PTR(DType, cputb);

  // handle output buffer
  data_out_ = GET_CPUTB_DType_PTR(DType, output_);
}

template <typename DType>
uint64_t StridedSlice<DType>::get_workload() {
  // TODO
  return 0;
}

template <typename DType>
void StridedSlice<DType>::strided_slice() {
  // copy input data into tmp buf
  auto data_in_size = get_vec_mul(fmap_i_);
  vector<DType> data_tmp(data_in_size);
  std::copy_n(data_in_, data_in_size, data_tmp.data());
  auto fmap_tmp = fmap_i_;

  for (auto i = 0U; i < begin_.size(); i++) {
    if (shrink_axis_mask_ == (1 << i)) {
      end_[i] = begin_[i] + 1;
      strides_[i] = 1;
    }

    auto pos = i;
    auto begin = begin_[i];
    auto end = end_[i];
    auto stride = strides_[i];
    UNI_LOG_CHECK(begin >= 0, VART_INVALID_VALUE) << ", " << begin << " < 0";
    UNI_LOG_CHECK(end >= 0, VART_INVALID_VALUE) << ", " << end << " < 0";
    UNI_LOG_CHECK(stride >= 0, VART_INVALID_VALUE) << ", " << stride << " < 0";
    UNI_LOG_CHECK(begin < end, VART_INVALID_VALUE)
        << ", " << begin << " >= " << end;

    if (fmap_tmp.size() == 1) {
      draw_dim1(pos, begin, end, stride, fmap_tmp, data_tmp);
    } else if (fmap_tmp.size() == 2) {
      draw_dim2(pos, begin, end, stride, fmap_tmp, data_tmp);
    } else if (fmap_tmp.size() == 3) {
      draw_dim3(pos, begin, end, stride, fmap_tmp, data_tmp);
    } else if (fmap_tmp.size() == 4) {
      draw_dim4(pos, begin, end, stride, fmap_tmp, data_tmp);
    } else {
      UNI_LOG_ERROR(VART_NOT_SUPPORT)
          << "Not support fmap size " << fmap_tmp.size() << endl;
      abort();
    }
  }

  UNI_LOG_CHECK(fmap_tmp == fmap_o_, VART_INVALID_VALUE)
      << ", " << Vec2Str(fmap_tmp, ", ") << " != " << Vec2Str(fmap_o_, ", ");

  std::copy_n(data_tmp.data(), get_vec_mul(fmap_tmp), data_out_);
}

template <typename DType>
void StridedSlice<DType>::draw_dim1(int pos, int begin, int end, int stride,
                                    vector<int>& fmap, vector<DType>& data) {
  UNI_LOG_CHECK(pos == 0, VART_INVALID_VALUE) << ", " << pos << " != 0";
  UNI_LOG_CHECK(fmap.size() == 1, VART_SIZE_ERROR)
      << ", " << fmap.size() << " != 1";

  vector<DType> data_tmp;
  for (auto i = begin; i < end; i += stride) {
    data_tmp.push_back(data[i]);
  }

  fmap[pos] = (end - begin) / stride;
  data = data_tmp;
}

template <typename DType>
void StridedSlice<DType>::draw_dim2(int pos, int begin, int end, int stride,
                                    vector<int>& fmap, vector<DType>& data) {
  UNI_LOG_CHECK(pos < 2, VART_INVALID_VALUE) << ", " << pos << " >= 2";
  UNI_LOG_CHECK(fmap.size() == 2, VART_SIZE_ERROR)
      << ", " << fmap.size() << " != 2";

  vector<DType> data_tmp;
  if (pos == 0) {
    for (auto i = begin; i < end; i += stride) {
      for (auto j = 0; j < fmap[1]; j++) {
        auto addr = i * fmap[1] + j;
        data_tmp.push_back(data[addr]);
      }
    }
  } else if (pos == 1) {
    for (auto i = 0; i < fmap[0]; i++) {
      for (auto j = begin; j < end; j += stride) {
        auto addr = i * fmap[1] + j;
        data_tmp.push_back(data[addr]);
      }
    }

  } else {
    UNI_LOG_ERROR(VART_IDX_ERROR) << "Error pos: " << pos << endl;
    abort();
  }

  fmap[pos] = (end - begin) / stride;
  data = data_tmp;
}

template <typename DType>
void StridedSlice<DType>::draw_dim3(int pos, int begin, int end, int stride,
                                    vector<int>& fmap, vector<DType>& data) {
  UNI_LOG_CHECK(pos < 3, VART_INVALID_VALUE) << ", " << pos << " >= 3";
  UNI_LOG_CHECK(fmap.size() == 3, VART_SIZE_ERROR)
      << ", " << fmap.size() << " != 3";

  vector<DType> data_tmp;
  if (pos == 0) {
    for (auto i = begin; i < end; i += stride) {
      for (auto j = 0; j < fmap[1]; j++) {
        for (auto k = 0; k < fmap[2]; k++) {
          auto addr = i * fmap[1] * fmap[2] + j * fmap[2] + k;
          data_tmp.push_back(data[addr]);
        }
      }
    }
  } else if (pos == 1) {
    for (auto i = 0; i < fmap[0]; i++) {
      for (auto j = begin; j < end; j += stride) {
        for (auto k = 0; k < fmap[2]; k++) {
          auto addr = i * fmap[1] * fmap[2] + j * fmap[2] + k;
          data_tmp.push_back(data[addr]);
        }
      }
    }
  } else if (pos == 2) {
    for (auto i = 0; i < fmap[0]; i++) {
      for (auto j = 0; j < fmap[1]; j++) {
        for (auto k = begin; k < end; k += stride) {
          auto addr = i * fmap[1] * fmap[2] + j * fmap[2] + k;
          data_tmp.push_back(data[addr]);
        }
      }
    }
  } else {
    UNI_LOG_ERROR(VART_IDX_ERROR) << "Error pos: " << pos << endl;
    abort();
  }

  fmap[pos] = (end - begin) / stride;
  data = data_tmp;
}

template <typename DType>
void StridedSlice<DType>::draw_dim4(int pos, int begin, int end, int stride,
                                    vector<int>& fmap, vector<DType>& data) {
  UNI_LOG_CHECK(pos < 4, VART_INVALID_VALUE) << ", " << pos << " >= 4";
  UNI_LOG_CHECK(fmap.size() == 4, VART_SIZE_ERROR)
      << ", " << fmap.size() << " != 4";

  vector<DType> data_tmp;
  if (pos == 0) {
    for (auto i = begin; i < end; i += stride) {
      for (auto j = 0; j < fmap[1]; j++) {
        for (auto m = 0; m < fmap[2]; m++) {
          for (auto n = 0; n < fmap[3]; n++) {
            auto addr = i * fmap[1] * fmap[2] * fmap[3] +
                        j * fmap[2] * fmap[3] + m * fmap[3] + n;
            data_tmp.push_back(data[addr]);
          }
        }
      }
    }
  } else if (pos == 1) {
    for (auto i = 0; i < fmap[0]; i++) {
      for (auto j = begin; j < end; j += stride) {
        for (auto m = 0; m < fmap[2]; m++) {
          for (auto n = 0; n < fmap[3]; n++) {
            auto addr = i * fmap[1] * fmap[2] * fmap[3] +
                        j * fmap[2] * fmap[3] + m * fmap[3] + n;
            data_tmp.push_back(data[addr]);
          }
        }
      }
    }
  } else if (pos == 2) {
    for (auto i = 0; i < fmap[0]; i++) {
      for (auto j = 0; j < fmap[1]; j++) {
        for (auto m = begin; m < end; m += stride) {
          for (auto n = 0; n < fmap[3]; n++) {
            auto addr = i * fmap[1] * fmap[2] * fmap[3] +
                        j * fmap[2] * fmap[3] + m * fmap[3] + n;
            data_tmp.push_back(data[addr]);
          }
        }
      }
    }
  } else if (pos == 3) {
    for (auto i = 0; i < fmap[0]; i++) {
      for (auto j = 0; j < fmap[1]; j++) {
        for (auto m = 0; m < fmap[2]; m++) {
          for (auto n = begin; n < end; n += stride) {
            auto addr = i * fmap[1] * fmap[2] * fmap[3] +
                        j * fmap[2] * fmap[3] + m * fmap[3] + n;
            data_tmp.push_back(data[addr]);
          }
        }
      }
    }
  } else {
    UNI_LOG_ERROR(VART_IDX_ERROR) << "Error pos: " << pos << endl;
    abort();
  }

  fmap[pos] = (end - begin) / stride;
  data = data_tmp;
}

INSTANTIATE_TPCLASS(StridedSlice);
REG_OP_INSTANCE_FUNC("strided_slice", StridedSlice);

}  // namespace cpu
}  // namespace vart
