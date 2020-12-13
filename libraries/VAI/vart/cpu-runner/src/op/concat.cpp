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

#include "concat.hpp"

namespace vart {
namespace cpu {

template <typename DType>
const vector<string> Concat<DType>::ITName = {
    "input",
};

// constructor and deconstructor
template <typename DType>
Concat<DType>::Concat(const xir::Subgraph* subg, const xir::Op* op,
                      IMapTBs_t inputs, CPUTBPtr_t output)
    : CPUOPBase(subg, op, inputs, output) {
  // get input/output tensor's fmap
  GET_INPUT_DIMX_FMAPS(fmap_i_, input, 4);
  GET_OUTPUT_DIMX_FMAP(fmap_o_, 4);

  input_num_ = fmap_i_.size();

  // get axis attr and transfer it to 4-dimension axis
  raw_axis_ = GET_OP_ATTR<int>(op, "axis");
  axis_ = raw_axis_;
  auto dim_num = xir_op_->get_output_tensor()->get_shape().size();
  if (axis_ < 0) axis_ += dim_num;
  THREAD_NUM = input_num_;
}

template <typename DType>
void Concat<DType>::run() {
  if (CPU_RUN_MODE == CPURunMode::NORMAL_THREAD ||
      CPU_RUN_MODE == CPURunMode::GEMM_THREAD) {
    concat_thread();
  } else {
    concat_normal();
  }
}

template <typename DType>
void Concat<DType>::print_param() {
  UNI_LOG_DEBUG_INFO << "input_num = " << input_num_ << endl;
  for (auto i = 0; i < input_num_; i++) {
    fmap_i_[i].print_param("fmap_i" + to_string(i));
  }
  fmap_o_.print_param("fmap_o");

  UNI_LOG_DEBUG_INFO << "raw_axis = " << raw_axis_ << endl;
  UNI_LOG_DEBUG_INFO << "axis = " << axis_ << endl;

  if (CPU_RUN_MODE == CPURunMode::NORMAL_THREAD ||
      CPU_RUN_MODE == CPURunMode::GEMM_THREAD) {
    UNI_LOG_DEBUG_INFO << "THREAD_NUM = " << THREAD_NUM << endl;
  }
}

template <typename DType>
void Concat<DType>::check_param() {
  UNI_LOG_CHECK(axis_ < fmap_o_.ndims(), VART_INVALID_VALUE)
      << ", Err: " << axis_ << " >= " << fmap_o_.ndims();

  for (auto i = 0; i < fmap_o_.ndims(); i++) {
    auto tmp_dim = 0;
    for (auto id = 0; id < input_num_; id++) {
      if (i != axis_) {
        UNI_LOG_CHECK(fmap_o_.dim(i) == fmap_i_[id].dim(i), VART_INVALID_VALUE)
            << ", Err: " << fmap_o_.dim(i) << " != " << fmap_i_[id].dim(i);
      } else {
        tmp_dim += fmap_i_[id].dim(i);
      }
    }

    if (i == axis_) {
      UNI_LOG_CHECK(fmap_o_.dim(i) == tmp_dim, VART_INVALID_VALUE)
          << ", Err: " << fmap_o_.dim(i) << " != " << tmp_dim;
    }
  }
}

// read data from DDR
template <typename DType>
void Concat<DType>::read() {
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
void Concat<DType>::concat_thread() {
  THREAD_NUM = input_num_;
  vector<std::future<int>> fut(THREAD_NUM);

  auto acc = 0;
  for (auto id = 0; id < input_num_; id++) {
    fut[id] = std::async(
        std::launch::async,
        [this](decltype(id) id, decltype(acc) acc) {
          auto inner_num = fmap_i_[id][axis_] * fmap_i_[id].cod(axis_);
          auto outer_num = fmap_i_[id].num() / inner_num;
          for (auto o = 0; o < outer_num; o++) {
            auto src_addr = o * inner_num;
            auto coord = fmap_i_[id].pos2coord(src_addr);
            coord[axis_] += acc;
            auto dst_addr = fmap_o_.coord2pos(coord);

            copy_n(&data_in_[id][src_addr], inner_num, &data_out_[dst_addr]);
          }

          return 0;
        },
        id, acc);

    acc += fmap_i_[id][axis_];
  }

  for (auto i = 0; i < THREAD_NUM; i++) {
    fut[i].wait();
  }
}

template <typename DType>
void Concat<DType>::concat_normal() {
  auto acc = 0;
  for (auto id = 0; id < input_num_; id++) {
    auto inner_num = fmap_i_[id][axis_] * fmap_i_[id].cod(axis_);
    auto outer_num = fmap_i_[id].num() / inner_num;
    for (auto o = 0; o < outer_num; o++) {
      auto src_addr = o * inner_num;
      auto coord = fmap_i_[id].pos2coord(src_addr);
      coord[axis_] += acc;
      auto dst_addr = fmap_o_.coord2pos(coord);

      copy_n(&data_in_[id][src_addr], inner_num, &data_out_[dst_addr]);
    }

    acc += fmap_i_[id][axis_];
  }
}

template <typename DType>
void Concat<DType>::concat_slow() {
  for (auto i = 0; i < fmap_o_.num(); i++) {
    auto coord = fmap_o_.pos2coord(i);
    auto input_id = get_input_id(coord);

    auto src_addr = fmap_i_[input_id].coord2pos(coord);
    data_out_[i] = data_in_[input_id][src_addr];
  }
}

template <typename DType>
int Concat<DType>::get_input_id(coord_t& coord) {
  for (auto i = 0; i < input_num_; i++) {
    if (coord[axis_] >= fmap_i_[i].dim(axis_)) {
      coord[axis_] -= fmap_i_[i].dim(axis_);
    } else {
      return i;
    }
  }

  UNI_LOG_FATAL(VART_INVALID_VALUE) << ": Err pos " << coord[axis_] << endl;
  abort();
}

INSTANTIATE_TPCLASS(Concat);
REG_OP_INSTANCE_FUNC("concat", Concat);

}  // namespace cpu
}  // namespace vart
