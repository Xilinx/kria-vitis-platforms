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

#include "pool_base.hpp"
#include "fast_pad.hpp"
#include "align_buf_mgr.hpp"

namespace vart {
namespace cpu {

template <typename DType>
const vector<string> PoolBase<DType>::ITName = {
    "input",
};

// constructor and deconstructor
template <typename DType>
PoolBase<DType>::PoolBase(const xir::Subgraph* subg,
    const xir::Op* op, IMapTBs_t inputs, CPUTBPtr_t output)
    : CPUOPBase(subg, op, inputs, output) {

  GET_INPUT_DIM4_FMAP(raw_fmap_i_, input);
  fmap_i_ = raw_fmap_i_;
  GET_OUTPUT_DIM4_FMAP(fmap_o_);

  is_global_ = false;
  if(xir_op_->has_attr("global"))
    is_global_ = xir_op_->get_attr<bool>("global");

  if (is_global_) {
    auto* xir_tensor_i = xir_op_->get_input_tensor(ITName[INPUT]);
    UNI_LOG_CHECK(xir_tensor_i != nullptr, VART_NULL_PTR);
    auto xir_tensor_i_dims = xir_tensor_i->get_shape();
    kernel_ = Kernel_t{xir_tensor_i_dims[1], xir_tensor_i_dims[2]};
    stride_ = Stride_t{1, 1};
    pad_ = Pad_t{0, 0, 0, 0, 0};
  } else {
    // get kernel info
    auto xir_kernel = xir_op_->get_attr<vector<int>>("kernel");
    kernel_ = Kernel_t{xir_kernel[1], xir_kernel[0]};

    // get stride info
    auto xir_stride = xir_op_->get_attr<vector<int>>("stride");
    stride_ = Stride_t{xir_stride[1], xir_stride[0]};

    // get pad info
    std::string xir_pad_mode = "FLOOR";
    if(xir_op_->has_attr("pad_mode"))
      xir_pad_mode = xir_op_->get_attr<std::string>("pad_mode");
    if (xir_op_->has_attr("pad")) {
      auto xir_pad = xir_op_->get_attr<vector<int>>("pad");
      raw_pad_ = Pad_t{xir_pad_mode, xir_pad[0], xir_pad[2], xir_pad[1], xir_pad[3]};
      pad_ = raw_pad_;
    } else {
      raw_pad_ = Pad_t{xir_pad_mode, 0, 0, 0, 0};
      pad_ = raw_pad_;
    }
  }

  pad_ = calc_pad(fmap_i_, fmap_o_, kernel_, stride_, pad_);
  fmap_i_.h += pad_.t + pad_.b;
  fmap_i_.w += pad_.l + pad_.r;

  FMAP_SIZE = fmap_o_.num();
  THREAD_NUM = (CPU_NUM==1) ? 1 : ceil(std::log(CPU_NUM));
  THREAD_WORKLOAD = ceil((float)FMAP_SIZE / THREAD_NUM);
}

template <typename DType>
void PoolBase<DType>::print_param() {
  UNI_LOG_DEBUG_INFO << "pool_type = "
    << ((pool_type_==MAX_POOL) ? "MAX_POOL" : "AVG_POOL")
    << endl;
  raw_fmap_i_.print_param("raw_fmap_i");
  fmap_i_.print_param("fmap_i");
  fmap_o_.print_param("fmap_o");

  raw_pad_.print_param("raw_pad");
  pad_.print_param("pad");
  kernel_.print_param("kernel");
  stride_.print_param("stride");

  UNI_LOG_DEBUG_INFO << "is_global = "
    << Bool2Str(is_global_) << endl;

  if(CPU_RUN_MODE == CPURunMode::NORMAL_THREAD
      || CPU_RUN_MODE == CPURunMode::GEMM_THREAD) {
    UNI_LOG_DEBUG_INFO << "THREAD_NUM = " << THREAD_NUM << endl;
    UNI_LOG_DEBUG_INFO << "THREAD_WORKLOAD = " << THREAD_WORKLOAD << endl;
  }
}


// private funcs
template <typename DType>
void PoolBase<DType>::check_param() {
  UNI_LOG_CHECK(inputs_.size() == 1, VART_SIZE_ERROR)
      << ", Err: inputs size is " << inputs_.size();

  if (pad_.mode == PadAttr::PAD_MODE_VALID || is_global_) {
    UNI_LOG_CHECK(pad_.l == 0, VART_INVALID_VALUE);
    UNI_LOG_CHECK(pad_.t == 0, VART_INVALID_VALUE);
    UNI_LOG_CHECK(pad_.r == 0, VART_INVALID_VALUE);
    UNI_LOG_CHECK(pad_.b == 0, VART_INVALID_VALUE);
  }

  // check fmap_out
  if (is_global_) {
    UNI_LOG_CHECK(fmap_o_.h == 1, VART_INVALID_VALUE)
        << ", " << fmap_o_.h << " != 1";
    UNI_LOG_CHECK(fmap_o_.w == 1, VART_INVALID_VALUE)
        << ", " << fmap_o_.w << " != 1";
    UNI_LOG_CHECK(fmap_i_.c == fmap_o_.c, VART_INVALID_VALUE)
        << ", " << fmap_i_.c << " != " << fmap_o_.c;
  } else {
    int dst_h = 0;
    int dst_w = 0;
    if (pad_.mode == PadAttr::PAD_MODE_FLOOR) {
      dst_h = floor(1.0 * (fmap_i_.h - kernel_.h + stride_.h) / stride_.h);
      dst_w = floor(1.0 * (fmap_i_.w - kernel_.w + stride_.w) / stride_.w);
    } else if (pad_.mode == PadAttr::PAD_MODE_CEIL) {
      dst_h = ceil(1.0 * (fmap_i_.h - kernel_.h + stride_.h) / stride_.h);
      dst_w = ceil(1.0 * (fmap_i_.w - kernel_.w + stride_.w) / stride_.w);
    } else if (pad_.mode == PadAttr::PAD_MODE_SAME) {
      dst_h = ceil(1.0 * raw_fmap_i_.h / stride_.h);
      dst_w = ceil(1.0 * raw_fmap_i_.w / stride_.w);
    } else if (pad_.mode == PadAttr::PAD_MODE_VALID) {
      dst_h = ceil(1.0 * (fmap_i_.h - kernel_.h + 1.0) / stride_.h);
      dst_w = ceil(1.0 * (fmap_i_.w - kernel_.w + 1.0) / stride_.w);
    } else {
      UNI_LOG_ERROR(VART_NOT_SUPPORT)
          << "Not support pad mode " << pad_.mode << endl;
      abort();
    }

    UNI_LOG_CHECK(dst_h == fmap_o_.h, VART_INVALID_VALUE)
        << ", " << dst_h << " != " << fmap_o_.h;
    UNI_LOG_CHECK(dst_w == fmap_o_.w, VART_INVALID_VALUE)
        << ", " << dst_w << " != " << fmap_o_.w;
    UNI_LOG_CHECK(fmap_i_.c == fmap_o_.c, VART_INVALID_VALUE)
        << ", " << fmap_i_.c << " != " << fmap_o_.c;
  }
}

// read data from DDR
template <typename DType>
void PoolBase<DType>::read() {
  // handle output buffer
  data_out_ptr_ = GET_CPUTB_DType_PTR(DType, output_);

  // read img data
  auto* data_in_tb = inputs_.at(ITName[INPUT]).at(0);
  auto* tb_input_ptr = GET_CPUTB_DType_PTR(DType, data_in_tb);

  if(pad_.l==0 && pad_.t==0 && pad_.r==0 && pad_.b==0) {
    data_in_ptr_ = tb_input_ptr;
    return;
  }

  if(pool_type_ == MAX_POOL) {
    pad_value_ = std::numeric_limits<DType>::lowest();
  } else {
    pad_value_ = 0;
  }

  if (VART_DEBUG) {
    UNI_LOG_DEBUG_INFO << "pad_value = " << pad_value_ << endl;
  }

  data_in_ptr_ = reinterpret_cast<DType*>(AlignBufMgr::Instance()
    ->allocate(fmap_i_.num() * sizeof(DType), ALIGN_SIZE));
  FastPad(raw_fmap_i_, fmap_i_, pad_)
    .transform<DType>(tb_input_ptr, data_in_ptr_, pad_value_);
}

template <typename DType>
uint64_t PoolBase<DType>::get_workload() {
  // not consider batch
  ops_ = fmap_o_.h * fmap_o_.w * fmap_o_.c * kernel_.h * kernel_.w;

  return ops_;
}

INSTANTIATE_TPCLASS(PoolBase);

}  // namespace cpu
}  // namespace vart
