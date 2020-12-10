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

#pragma once

#include "cpu_base_inc.hpp"

namespace vart {
namespace mm {
class HostFlatTensorBuffer;
}
}  // namespace vart

namespace vart {
namespace cpu {

// NOTE: we use int32_t as fix data type, it will
// simplify program's architecture and speedup execution
using FixData_t = int32_t;

#define ALIGN_SIZE 4 * 1024

#define DOWNCAST_2_CPUTB(tb) std::dynamic_pointer_cast<CPUTensorBuffer>(tb)

#define GET_CPUTB_DType_PTR(DType, cputb) (DType*)(cputb->get_data_ptr())

#define GET_CPUTB_FLOAT_PTR(cputb) (float*)(cputb->get_data_ptr())

#define GET_CPUTB_INT32_PTR(cputb) (int32_t*)(cputb->get_data_ptr())

class CPUTensorBuffer;
using CPUTBPtr_t = CPUTensorBuffer*;
using CPUTBPtrVec_t = vector<CPUTBPtr_t>;
using IMapTBs_t = map<string, CPUTBPtrVec_t>;

class CPUTensorBuffer : public TensorBuffer {
 public:
  explicit CPUTensorBuffer(const xir::Op* xir_op,
                           const xir::Tensor* xir_tensor);
  virtual ~CPUTensorBuffer() = default;
  VART_DISABLE_COPY(CPUTensorBuffer);

  static unique_ptr<CPUTensorBuffer> make(const xir::Op* xir_op,
                                          const xir::Tensor* xir_tensor) {
    return make_unique<CPUTensorBuffer>(xir_op, xir_tensor);
  }

 public:
  const xir::Op* get_xir_op() const { return xir_op_; }
  const xir::Tensor* get_xir_tensor() const { return xir_tensor_; }

  virtual std::pair<uint64_t, size_t> data(
      const std::vector<std::int32_t> index = {}) override final;

  void set_data_ptr(void* ptr);
  void copy_data_in(char* in);
  void copy_data_out(char* out);

  uint32_t get_data_num() const { return data_num_; }
  uint32_t get_data_size() const { return data_size_; }
  char* get_data_ptr() { return data_ptr_; }

  // NOTE: Special handle for fix op, if we want to dump
  // fix op's middle result, save fix data here is the best choice.
  // and also for simplify op template design, all fix data will be int32_t,
  // and it works very well.
  uint32_t get_fix_data_num() const { return fix_data_num_; }
  uint32_t get_fix_data_size() const { return fix_data_size_; }
  FixData_t* get_fix_data_ptr() { return fix_data_ptr_; }

  void save_bin();
  void save_bin(const string& fname);

 private:
  int one_ele_size();

  void internal_save_bin(const string& fname);
  void internal_save_txt(const string& fname);

  void internal_save_float_bin(const string& fname);
  void internal_save_4bit_bin(const string& fname);
  void internal_save_8bit_bin(const string& fname);
  void internal_save_16bit_bin(const string& fname);
  void internal_save_32bit_bin(const string& fname);

  void internal_save_float_txt(const string& fname);
  void internal_save_int_txt(const string& fname);

  void internal_save_fix_bin(const string& fname);
  void internal_save_fix_txt(const string& fname);

 private:
  int unique_id_;

  // NOTE: do not reorder following var's order
  const xir::Op* xir_op_{nullptr};
  const xir::Tensor* xir_tensor_{nullptr};
  vector<int> dims_;
  xir::DataType::Type dtype_;
  int bit_width_;
  bool if_signed_;

  // NOTE: Pls have more intention to this pointer,
  // if want to use, maybe you should cast it to right type
  bool use_internal_buf_{true};
  uint32_t data_num_;
  uint32_t data_size_;
  vector<char> internal_data_buf_;
  char* data_ptr_{nullptr};

  // NOTE: Special handle for fix op, if we want to dump
  // fix op's middle result, save fix data here is the best choice.
  uint32_t fix_data_num_;
  uint32_t fix_data_size_;
  vector<char> internal_fix_data_buf_;
  FixData_t* fix_data_ptr_{nullptr};
};

std::unique_ptr<vart::mm::HostFlatTensorBuffer>
clone_as_host_flat_tensor_buffer(CPUTensorBuffer* buffer);

}  // namespace cpu
}  // namespace vart
