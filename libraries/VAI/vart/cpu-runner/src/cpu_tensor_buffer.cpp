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

#include "cpu_tensor_buffer.hpp"
#include "cpu_op_base.hpp"
#include "vart/mm/host_flat_tensor_buffer.hpp"
#include "vart/util_4bit.hpp"

namespace vart {
namespace cpu {

static atomic<int> CPUTB_NEXT_ID(0);

CPUTensorBuffer::CPUTensorBuffer(const xir::Op* xir_op,
                                 const xir::Tensor* xir_tensor)
    : TensorBuffer(xir_tensor),
      xir_op_(xir_op),
      xir_tensor_(xir_tensor),
      dims_(xir_tensor->get_shape()),
      dtype_(xir_tensor_->get_data_type().type),
      bit_width_(xir_tensor_->get_data_type().bit_width),
      if_signed_(get_if_signed(dtype_)),
      use_internal_buf_(true),
      data_num_(get_vec_mul(dims_)),
      data_size_(data_num_ * one_ele_size()),
      internal_data_buf_(ALIGN_SIZE + data_size_, 0) {
  // NOTE: to simplify op inference procedure, we only use 3 types:
  // int32_t, float, double according to xir_tensor data type and bit width
  auto space = internal_data_buf_.size();
  void* p = internal_data_buf_.data();
  std::align(ALIGN_SIZE, data_size_, p, space);
  data_ptr_ = reinterpret_cast<char*>(p);

  if (xir_op_->get_type() == "fix") {
    fix_data_num_ = data_num_;
    fix_data_size_ = fix_data_num_ * sizeof(FixData_t);
    internal_fix_data_buf_.resize(ALIGN_SIZE + fix_data_size_);

    auto space = internal_fix_data_buf_.size();
    void* p = internal_fix_data_buf_.data();
    std::align(ALIGN_SIZE, fix_data_size_, p, space);
    fix_data_ptr_ = reinterpret_cast<FixData_t*>(p);
  }

  // make sure unique id for this tensor buffer
  unique_id_ = CPUTB_NEXT_ID.fetch_add(1);
}

std::pair<uint64_t, size_t> CPUTensorBuffer::data(
    const std::vector<std::int32_t> index) {
  if (index.size() == 0) {
    return {reinterpret_cast<uint64_t>(data_ptr_), data_num_};
  }
  UNI_LOG_CHECK(index.size() == dims_.size(), VART_SIZE_ERROR)
      << ", " << index.size() << " != " << dims_.size();

  uint64_t offset = 0;
  for (auto i = 0U; i < index.size(); i++) {
    UNI_LOG_CHECK(index[i] < dims_[i], VART_SIZE_ERROR)
        << ", " << index[i] << ">=" << dims_[i];

    uint64_t factor = 1;
    for (auto j = i + 1; j < index.size(); j++) {
      factor *= dims_[j];
    }

    offset += index[i] * factor;
  }

  UNI_LOG_CHECK(offset < data_num_, VART_SIZE_ERROR)
      << ", " << offset << " >= " << data_num_;
  auto* p = data_ptr_ + offset * one_ele_size();
  return {reinterpret_cast<uint64_t>(p), data_num_ - offset};
}

void CPUTensorBuffer::set_data_ptr(void* ptr) {
  UNI_LOG_CHECK(ptr != nullptr, VART_NULL_PTR);
  use_internal_buf_ = false;
  data_ptr_ = (char*)ptr;
}

static std::vector<int32_t> get_stride(const xir::Tensor* tensor,
                                       bool ignore_def = false) {
  auto shape = tensor->get_shape();
  auto stride = std::vector<int32_t>(shape.size());
  if (tensor->has_attr("strides") && !ignore_def) {
    UNI_LOG_FATAL(VART_CPU_TENSOR_BUFFER_OPERATION_FAIL)
        << "CPUTensorBuffer doesn't support strides currently!";
  } else if (tensor->has_attr("stride") && !ignore_def) {
    stride = tensor->get_attr<std::vector<int32_t>>("stride");
  } else {
    auto step = 1;
    for (int idx = shape.size() - 1; idx >= 0; idx--) {
      stride[idx] = step;
      step *= shape[idx];
    }
  }
  return stride;
}

void CPUTensorBuffer::copy_data_in(char* ptr) {
  // TODO: check data size

#define COPY_DATA_IN_BY_TYPE(inner_type, outer_type)                           \
  do {                                                                         \
    auto* ptr_outer = reinterpret_cast<outer_type*>(ptr);                      \
    auto* ptr_inner = reinterpret_cast<inner_type*>(data_ptr_);                \
    for (auto i = 0u; i < data_num_; i++) {                                    \
      ptr_inner[i] = static_cast<inner_type>(ptr_outer[i]);                    \
    }                                                                          \
  } while (0);

  if (dtype_ == xir::DataType::INT) {
    if (bit_width_ == 8) {
      COPY_DATA_IN_BY_TYPE(int32_t, int8_t);
    } else if (bit_width_ == 16) {
      COPY_DATA_IN_BY_TYPE(int32_t, int16_t);
    } else if (bit_width_ == 32) {
      COPY_DATA_IN_BY_TYPE(int32_t, int32_t);
    } else if (bit_width_ == 64) {
      COPY_DATA_IN_BY_TYPE(int32_t, int64_t);
    } else {
      UNI_LOG_FATAL(VART_NOT_SUPPORT)
          << "Not supported bit width " << bit_width_;
    }
  } else if (dtype_ == xir::DataType::UINT) {
    if (bit_width_ == 8) {
      COPY_DATA_IN_BY_TYPE(int32_t, uint8_t);
    } else if (bit_width_ == 16) {
      COPY_DATA_IN_BY_TYPE(int32_t, uint16_t);
    } else if (bit_width_ == 32) {
      COPY_DATA_IN_BY_TYPE(int32_t, uint32_t);
    } else if (bit_width_ == 64) {
      COPY_DATA_IN_BY_TYPE(int32_t, uint64_t);
    } else {
      UNI_LOG_FATAL(VART_NOT_SUPPORT)
          << "Not supported bit width " << bit_width_;
    }
  } else if (dtype_ == xir::DataType::XINT) {
    if (bit_width_ == 4) {
      // TODO
      int8_t* src = (int8_t*)ptr;
      int32_t* dst = (int32_t*)data_ptr_;
      signed4bit_2_dt<int32_t>(src, dst, data_num_, tensor_->get_shape(),
                               get_stride(tensor_));
    } else if (bit_width_ == 8) {
      COPY_DATA_IN_BY_TYPE(int32_t, int8_t);
    } else if (bit_width_ == 16) {
      COPY_DATA_IN_BY_TYPE(int32_t, int16_t);
    } else if (bit_width_ == 32) {
      COPY_DATA_IN_BY_TYPE(int32_t, int32_t);
    } else if (bit_width_ == 64) {
      COPY_DATA_IN_BY_TYPE(int32_t, int64_t);
    } else {
      UNI_LOG_FATAL(VART_NOT_SUPPORT)
          << "Not supported bit width " << bit_width_;
    }
  } else if (dtype_ == xir::DataType::XUINT) {
    if (bit_width_ == 4) {
      // TODO
      uint8_t* src = (uint8_t*)ptr;
      int32_t* dst = (int32_t*)data_ptr_;
      unsigned4bit_2_dt<int32_t>(src, dst, data_num_, tensor_->get_shape(),
                                 get_stride(tensor_));
    } else if (bit_width_ == 8) {
      COPY_DATA_IN_BY_TYPE(int32_t, uint8_t);
    } else if (bit_width_ == 16) {
      COPY_DATA_IN_BY_TYPE(int32_t, uint16_t);
    } else if (bit_width_ == 32) {
      COPY_DATA_IN_BY_TYPE(int32_t, uint32_t);
    } else if (bit_width_ == 64) {
      COPY_DATA_IN_BY_TYPE(int32_t, uint64_t);
    } else {
      UNI_LOG_FATAL(VART_NOT_SUPPORT)
          << "Not supported bit width " << bit_width_;
    }
  } else if (dtype_ == xir::DataType::FLOAT) {
    if (bit_width_ == 32) {
      COPY_DATA_IN_BY_TYPE(float, float);
    } else if (bit_width_ == 64) {
      COPY_DATA_IN_BY_TYPE(double, double);
    } else {
      UNI_LOG_FATAL(VART_NOT_SUPPORT)
          << "Not supported bit width " << bit_width_;
    }
  } else {
    UNI_LOG_FATAL(VART_NOT_SUPPORT) << "Not supported data type!";
  }
}

std::unique_ptr<vart::mm::HostFlatTensorBuffer>
clone_as_host_flat_tensor_buffer(CPUTensorBuffer* buffer) {
  auto tensor = buffer->get_tensor();
  auto shape = tensor->get_shape();
  auto strides = std::vector<uint32_t>(shape.size(), 0);
  auto step = tensor->get_data_type().bit_width;
  for (int idx = shape.size() - 1; idx >= 0; idx--) {
    strides[idx] = step;
    step *= shape[idx];
  }
  auto ret = std::make_unique<vart::mm::HostFlatTensorBuffer>(tensor);
  buffer->copy_data_out(reinterpret_cast<char*>(ret->data({}).first));
  return ret;
}

void CPUTensorBuffer::copy_data_out(char* ptr) {
  // TODO: check data size

#define COPY_DATA_OUT_BY_TYPE(inner_type, outer_type)                          \
  do {                                                                         \
    auto* ptr_outer = reinterpret_cast<outer_type*>(ptr);                      \
    auto* ptr_inner = reinterpret_cast<inner_type*>(data_ptr_);                \
    for (auto i = 0U; i < data_num_; i++) {                                    \
      ptr_outer[i] = static_cast<outer_type>(ptr_inner[i]);                    \
    }                                                                          \
  } while (0);

  if (dtype_ == xir::DataType::INT) {
    if (bit_width_ == 8) {
      COPY_DATA_OUT_BY_TYPE(int32_t, int8_t);
    } else if (bit_width_ == 16) {
      COPY_DATA_OUT_BY_TYPE(int32_t, int16_t);
    } else if (bit_width_ == 32) {
      COPY_DATA_OUT_BY_TYPE(int32_t, int32_t);
    } else if (bit_width_ == 64) {
      COPY_DATA_OUT_BY_TYPE(int32_t, int64_t);
    } else {
      UNI_LOG_FATAL(VART_NOT_SUPPORT)
          << "Not supported bit width " << bit_width_;
    }
  } else if (dtype_ == xir::DataType::UINT) {
    if (bit_width_ == 8) {
      COPY_DATA_OUT_BY_TYPE(int32_t, uint8_t);
    } else if (bit_width_ == 16) {
      COPY_DATA_OUT_BY_TYPE(int32_t, uint16_t);
    } else if (bit_width_ == 32) {
      COPY_DATA_OUT_BY_TYPE(int32_t, uint32_t);
    } else if (bit_width_ == 64) {
      COPY_DATA_OUT_BY_TYPE(int32_t, uint64_t);
    } else {
      UNI_LOG_FATAL(VART_NOT_SUPPORT)
          << "Not supported bit width " << bit_width_;
    }
  } else if (dtype_ == xir::DataType::XINT) {
    if (bit_width_ == 4) {
      // TODO
      int8_t* dst = (int8_t*)ptr;
      int32_t* src = (int32_t*)data_ptr_;
      dt_2_signed4bit<int32_t>(src, dst, data_num_, tensor_->get_shape(),
                               get_stride(tensor_));
    } else if (bit_width_ == 8) {
      COPY_DATA_OUT_BY_TYPE(int32_t, int8_t);
    } else if (bit_width_ == 16) {
      COPY_DATA_OUT_BY_TYPE(int32_t, int16_t);
    } else if (bit_width_ == 32) {
      COPY_DATA_OUT_BY_TYPE(int32_t, int32_t);
    } else if (bit_width_ == 64) {
      COPY_DATA_OUT_BY_TYPE(int32_t, int64_t);
    } else {
      UNI_LOG_FATAL(VART_NOT_SUPPORT)
          << "Not supported bit width " << bit_width_;
    }
  } else if (dtype_ == xir::DataType::XUINT) {
    if (bit_width_ == 4) {
      // TODO
      uint8_t* dst = (uint8_t*)ptr;
      int32_t* src = (int32_t*)data_ptr_;
      dt_2_unsigned4bit<int32_t>(src, dst, data_num_, tensor_->get_shape(),
                                 get_stride(tensor_));
    } else if (bit_width_ == 8) {
      COPY_DATA_OUT_BY_TYPE(int32_t, uint8_t);
    } else if (bit_width_ == 16) {
      COPY_DATA_OUT_BY_TYPE(int32_t, uint16_t);
    } else if (bit_width_ == 32) {
      COPY_DATA_OUT_BY_TYPE(int32_t, uint32_t);
    } else if (bit_width_ == 64) {
      COPY_DATA_OUT_BY_TYPE(int32_t, uint64_t);
    } else {
      UNI_LOG_FATAL(VART_NOT_SUPPORT)
          << "Not supported bit width " << bit_width_;
    }
  } else if (dtype_ == xir::DataType::FLOAT) {
    if (bit_width_ == 32) {
      COPY_DATA_OUT_BY_TYPE(float, float);
    } else if (bit_width_ == 64) {
      COPY_DATA_OUT_BY_TYPE(double, double);
    } else {
      UNI_LOG_FATAL(VART_NOT_SUPPORT)
          << "Not supported bit width " << bit_width_;
    }
  } else {
    UNI_LOG_FATAL(VART_NOT_SUPPORT) << "Not supported data type!";
  }
}

void CPUTensorBuffer::save_bin() {
  auto save_bin = CPUCfg::Instance().get_save_bin();
  auto save_txt = CPUCfg::Instance().get_save_txt();
  auto debug_path = CPUCfg::Instance().get_debug_path();

  if (save_bin) {
    auto tensor_name = tensor_name_2_save_name(xir_tensor_->get_name());
    auto fname_bin = CPUCfg::Instance().get_debug_path() + tensor_name;

    internal_save_bin(fname_bin);

    if (xir_op_->get_type() == "fix") {
      // for auto diff using following bash script
      auto diff_phrase = "diff ./" + tensor_name + ".fix.bin ${GOLDEN_PATH}/" +
                         tensor_name + ".bin\n";
      SaveBin(debug_path + CPUOPBase::SUBG_DIFF_SCRIPT,
              (char*)diff_phrase.data(), diff_phrase.size(), SM_APPEND);
    }
  }

  if (save_txt) {
    auto tensor_name = tensor_name_2_save_name(xir_tensor_->get_name());
    auto fname_txt = CPUCfg::Instance().get_debug_path() + tensor_name;
    internal_save_txt(fname_txt);
  }
}

void CPUTensorBuffer::save_bin(const string& fname) {
  auto save_bin = CPUCfg::Instance().get_save_bin();

  if (save_bin) {
    internal_save_bin(fname);
  }
}

int CPUTensorBuffer::one_ele_size() {
  if (dtype_ == xir::DataType::FLOAT) {
    if (bit_width_ == 32) {
      return sizeof(float);
    } else if (bit_width_ == 64) {
      return sizeof(double);
    } else {
      abort();
    }
  } else {
    return sizeof(int32_t);
  }
}

void CPUTensorBuffer::internal_save_bin(const string& fname) {
  if (xir_op_->get_type() == "fix") {
    internal_save_fix_bin(fname + ".fix");
  }

  auto normal_fname = fname + ".bin";
  if (dtype_ == xir::DataType::FLOAT) {
    internal_save_float_bin(normal_fname);
  } else {
    if (bit_width_ == 4) {
      internal_save_4bit_bin(normal_fname);
    } else if (bit_width_ == 8) {
      internal_save_8bit_bin(normal_fname);
    } else if (bit_width_ == 16) {
      internal_save_16bit_bin(normal_fname);
    } else if (bit_width_ == 24) {
      // not handle 24bit
    } else if (bit_width_ == 32) {
      internal_save_32bit_bin(normal_fname);
    } else {
      // not handle 64bit
      LOG(WARNING) << "Not handle 64bit dump" << endl;
    }
  }
}

void CPUTensorBuffer::internal_save_txt(const string& fname) {
  if (xir_op_->get_type() == "fix") {
    internal_save_fix_txt(fname + ".fix");
  }

  auto normal_fname = fname + ".txt";
  if (dtype_ == xir::DataType::FLOAT) {
    internal_save_float_txt(normal_fname);
  } else {
    internal_save_int_txt(normal_fname);
  }
}

void CPUTensorBuffer::internal_save_float_bin(const string& fname) {
  if (dtype_ != xir::DataType::FLOAT) return;

  SaveBin(fname, data_ptr_, get_data_size());
}

void CPUTensorBuffer::internal_save_4bit_bin(const string& fname) {
  if (bit_width_ != 4) return;

  int32_t* p = reinterpret_cast<int32_t*>(data_ptr_);
  if (if_signed_) {
    vector<int8_t> save_buf(std::ceil(data_num_ / 2.0));
    dt_2_signed4bit<int32_t>(p, save_buf.data(), data_num_, tensor_->get_shape(),
                             get_stride(tensor_, true));
    SaveBin(fname, (const char*)save_buf.data(), save_buf.size());
  } else {
    vector<uint8_t> save_buf(std::ceil(data_num_ / 2.0));
    dt_2_unsigned4bit<int32_t>(p, save_buf.data(), data_num_,
                               tensor_->get_shape(), get_stride(tensor_, true));
    SaveBin(fname, (const char*)save_buf.data(), save_buf.size());
  }
}

void CPUTensorBuffer::internal_save_8bit_bin(const string& fname) {
  if (bit_width_ != 8) return;

  int32_t* p = reinterpret_cast<int32_t*>(data_ptr_);
  if (if_signed_) {
    vector<int8_t> save_buf(data_num_);
    dta_2_dtb<int32_t, int8_t>(p, save_buf.data(), data_num_);
    SaveBin(fname, (const char*)save_buf.data(), save_buf.size());
  } else {
    vector<uint8_t> save_buf(data_num_);
    dta_2_dtb<int32_t, uint8_t>(p, save_buf.data(), data_num_);
    SaveBin(fname, (const char*)save_buf.data(), save_buf.size());
  }
}

void CPUTensorBuffer::internal_save_16bit_bin(const string& fname) {
  if (bit_width_ != 16) return;

  int32_t* p = reinterpret_cast<int32_t*>(data_ptr_);
  if (if_signed_) {
    vector<int16_t> save_buf(data_num_);
    dta_2_dtb<int32_t, int16_t>(p, save_buf.data(), data_num_);
    SaveBin(fname, (const char*)save_buf.data(), save_buf.size());
  } else {
    vector<uint16_t> save_buf(data_num_);
    dta_2_dtb<int32_t, uint16_t>(p, save_buf.data(), data_num_);
    SaveBin(fname, (const char*)save_buf.data(), save_buf.size());
  }
}

void CPUTensorBuffer::internal_save_32bit_bin(const string& fname) {
  if (bit_width_ != 32) return;

  SaveBin(fname, data_ptr_, get_data_size());
}

void CPUTensorBuffer::internal_save_float_txt(const string& fname) {
  if (dtype_ != xir::DataType::FLOAT) return;

  if (bit_width_ == 32) {
    SaveDec(fname, (float*)data_ptr_, get_data_num(), 1, SM_TRUNC);
  } else if (bit_width_ == 64) {
    SaveDec(fname, (double*)data_ptr_, get_data_num(), 1, SM_TRUNC);
  }
}

void CPUTensorBuffer::internal_save_int_txt(const string& fname) {
  if (dtype_ == xir::DataType::FLOAT) return;

  if (if_signed_) {
    SaveDec(fname, (int32_t*)data_ptr_, get_data_num(), 1, SM_TRUNC);
  } else {
    SaveDec(fname, (uint32_t*)data_ptr_, get_data_num(), 1, SM_TRUNC);
  }
}

void CPUTensorBuffer::internal_save_fix_bin(const string& fname) {
  auto* input_op = xir_op_->get_input_op("input");
  bool const_flag = false;
  if (input_op->get_type() == "const" || input_op->get_type() == "const-fix")
    const_flag = true;
  auto fmap_o = FMap_t(xir_op_->get_output_tensor()->get_shape());

  if (const_flag) {
    auto save_weights_fmt = CPUCfg::Instance().get_save_weights_fmt();
    vector<int8_t> save_buf(fix_data_num_);
    if (save_weights_fmt == WeightsFmt::OC_KH_KW_IC) {
      dta_2_dtb<int32_t, int8_t>(fix_data_ptr_, save_buf.data(), fix_data_num_);
      SaveBin(fname + ".ohwi.bin", (const char*)save_buf.data(),
              save_buf.size());
    } else if (save_weights_fmt == WeightsFmt::KH_KW_IC_OC) {
      vector<int32_t> fix_hwio(fix_data_size_);
      HWC2WCH<int32_t>(fmap_o.n, fmap_o.h * fmap_o.w, fmap_o.c, fix_data_ptr_,
                       fix_hwio.data());
      dta_2_dtb<int32_t, int8_t>(fix_hwio.data(), save_buf.data(),
                                 fix_data_num_);
      SaveBin(fname + ".hwio.bin", (const char*)save_buf.data(),
              save_buf.size());
    } else {
      abort();
    }
  } else {
    if (if_signed_) {
      if (xir_op_->get_attr<int>("bit_width") == 4) {
        vector<int8_t> save_buf(std::ceil(fix_data_num_ / 2.0));
        dt_2_signed4bit<int32_t>(fix_data_ptr_, save_buf.data(), fix_data_num_,
                                 tensor_->get_shape(),
                                 get_stride(tensor_, true));
        SaveBin(fname + ".bin", (const char*)save_buf.data(), save_buf.size());
      } else {
        vector<int8_t> save_buf(fix_data_num_);
        dta_2_dtb<int32_t, int8_t>(fix_data_ptr_, save_buf.data(),
                                   fix_data_num_);
        SaveBin(fname + ".bin", (const char*)save_buf.data(), save_buf.size());
      }
    } else {
      if (xir_op_->get_attr<int>("bit_width") == 4) {
        vector<uint8_t> save_buf(std::ceil(fix_data_num_ / 2.0));
        dt_2_unsigned4bit<int32_t>(fix_data_ptr_, save_buf.data(),
                                   fix_data_num_, tensor_->get_shape(),
                                   get_stride(tensor_, true));
        SaveBin(fname + ".bin", (const char*)save_buf.data(), save_buf.size());
      } else {
        vector<uint8_t> save_buf(fix_data_num_);
        dta_2_dtb<int32_t, uint8_t>(fix_data_ptr_, save_buf.data(),
                                    fix_data_num_);
        SaveBin(fname + ".bin", (const char*)save_buf.data(), save_buf.size());
      }
    }
  }
}

void CPUTensorBuffer::internal_save_fix_txt(const string& fname) {
  auto* input_op = xir_op_->get_input_op("input");
  bool const_flag = false;
  if (input_op->get_type() == "const" || input_op->get_type() == "const-fix")
    const_flag = true;
  auto fmap_o = FMap_t(xir_op_->get_output_tensor()->get_shape());

  if (const_flag) {
    auto save_weights_fmt = CPUCfg::Instance().get_save_weights_fmt();
    if (save_weights_fmt == WeightsFmt::OC_KH_KW_IC) {
      SaveDec(fname + ".ohwi.txt", fix_data_ptr_, fix_data_num_, 1, SM_TRUNC);
    } else if (save_weights_fmt == WeightsFmt::KH_KW_IC_OC) {
      vector<int32_t> fix_hwio(fix_data_num_);
      HWC2WCH<int32_t>(fmap_o.n, fmap_o.h * fmap_o.w, fmap_o.c, fix_data_ptr_,
                       fix_hwio.data());
      SaveDec(fname + ".hwio.txt", fix_hwio.data(), fix_hwio.size(), 1,
              SM_TRUNC);
    } else {
      abort();
    }
  } else {
    SaveDec(fname + ".txt", fix_data_ptr_, fix_data_num_, 1, SM_TRUNC);
  }
}

}  // namespace cpu
}  // namespace vart
