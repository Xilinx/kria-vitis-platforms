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
#include "cpu_helper.hpp"
#include "cpu_cfg.hpp"
#include "cpu_tensor_buffer.hpp"

namespace vart {
namespace cpu {

char* TBPTR(TensorBuffer* tb) {
  UNI_LOG_CHECK(tb != nullptr, VART_NULL_PTR);
  auto* tensor = tb->get_tensor();
  UNI_LOG_CHECK(tensor != nullptr, VART_NULL_PTR);
  auto buf_pair = tb->data(std::vector<int>(tensor->get_shape().size(), 0));
  UNI_LOG_CHECK(buf_pair.second != 0, VART_INVALID_VALUE);

  return reinterpret_cast<char*>(buf_pair.first);
}

size_t TBSIZE(TensorBuffer* tb) {
  UNI_LOG_CHECK(tb != nullptr, VART_NULL_PTR);
  auto* tensor = tb->get_tensor();
  UNI_LOG_CHECK(tensor != nullptr, VART_NULL_PTR);
  auto buf_pair = tb->data(std::vector<int>(tensor->get_shape().size(), 0));
  UNI_LOG_CHECK(buf_pair.second != 0, VART_INVALID_VALUE);

  return buf_pair.second;
}

vector<xir::Op*> get_input_ops(const xir::Subgraph* subg) {
  auto* g = subg->get_graph();
  UNI_LOG_CHECK(g != nullptr, VART_NULL_PTR);
  vector<xir::Op*> input_ops;

  auto input_tensors = subg->get_input_tensors();
  for (auto* tensor : input_tensors) {
    UNI_LOG_CHECK(tensor != nullptr, VART_NULL_PTR);
    auto* op = g->get_tensor_producer(tensor);
    input_ops.push_back(const_cast<xir::Op*>(op));
  }

  return input_ops;
}

vector<xir::Op*> get_output_ops(const xir::Subgraph* subg) {
  auto* g = subg->get_graph();
  UNI_LOG_CHECK(g != nullptr, VART_NULL_PTR);
  vector<xir::Op*> output_ops;

  auto output_tensors = subg->get_output_tensors();
  for (auto* tensor : output_tensors) {
    UNI_LOG_CHECK(tensor != nullptr, VART_NULL_PTR);
    auto* op = g->get_tensor_producer(tensor);
    output_ops.push_back(const_cast<xir::Op*>(op));
  }

  return output_ops;
}

map<string, vector<const xir::Op*>> get_input_map(const xir::Op* op) {
  const auto* opdef = op->get_opdef();
  const auto input_args = opdef->input_args();

  map<string, vector<const xir::Op*>> m;
  for (auto i = 0U; i < input_args.size(); i++) {
    const auto& e = input_args[i];
    auto ops = op->get_input_ops(e.name);
    if (ops.size() == 0) continue;

    m[e.name] = ops;
  }

  return m;
}

bool get_if_signed(const xir::DataType::Type& dt) {
  if (dt == xir::DataType::FLOAT || dt == xir::DataType::INT ||
      dt == xir::DataType::XINT) {
    return true;
  }

  return false;
}

std::string get_data_type_str(const xir::Tensor* xir_tensor) {
  UNI_LOG_CHECK(xir_tensor != nullptr, VART_NULL_PTR);

  auto bit_width = xir_tensor->get_data_type().bit_width;
  auto xir_dtype = xir_tensor->get_data_type().type;

  string s;
  if (xir_dtype == xir::DataType::INT) {
    s = "int_" + to_string(bit_width) + "bit";
  } else if (xir_dtype == xir::DataType::UINT) {
    s = "uint_" + to_string(bit_width) + "bit";
  } else if (xir_dtype == xir::DataType::XINT) {
    s = "xint_" + to_string(bit_width) + "bit";
  } else if (xir_dtype == xir::DataType::XUINT) {
    s = "xuint_" + to_string(bit_width) + "bit";
  } else if (xir_dtype == xir::DataType::FLOAT) {
    s = "float_" + to_string(bit_width) + "bit";
  } else {
    LOG(ERROR) << "Not support data type!" << endl;
    abort();
  }

  return s;
}

void print_xir_op(const xir::Op* xir_op) {
  VART_DEBUG_GO_ON();

  const auto* tensor = xir_op->get_output_tensor();
  UNI_LOG_DEBUG_INFO << "op_type: " << xir_op->get_type() << endl;
  UNI_LOG_DEBUG_INFO << "op_name: " << xir_op->get_name() << endl;
  UNI_LOG_DEBUG_INFO << "tensor_name: " << tensor->get_name() << endl;
  UNI_LOG_DEBUG_INFO << "tensor_dims: " << Vec2Str(tensor->get_shape(), ",")
                     << endl;
  UNI_LOG_DEBUG_INFO << "tensor_dtype: " << get_data_type_str(tensor) << endl;

  UNI_LOG_DEBUG_INFO << "inputs: " << endl;
  auto m = get_input_map(xir_op);
  for (const auto& e : m) {
    string pair = "\t" + e.first + "(" + to_string(e.second.size()) + "): ";

    for (auto k = 0U; k < e.second.size(); k++) {
      pair += e.second[k]->get_name();
      if (k != e.second.size() - 1) pair += ", ";
    }
    UNI_LOG_DEBUG_INFO << pair << endl;
  }
}

void print_cpu_tb(const CPUTensorBuffer* cputb) {
  VART_DEBUG_GO_ON();

  const auto* tensor = cputb->get_xir_tensor();
  auto dims = tensor->get_shape();

  UNI_LOG_DEBUG_INFO << "tensor_name: " << tensor->get_name() << endl;
  UNI_LOG_DEBUG_INFO << "tensor_dtype: " << get_data_type_str(tensor) << endl;
  UNI_LOG_DEBUG_INFO << "dims: [" << Vec2Str<int>(dims, ",") << "]"
                     << ", data_num: " << get_vec_mul(dims)
                     << ", data_size: " << cputb->get_data_size() << endl;
}

void print_xir_subg(const xir::Subgraph* xir_subg) {
  VART_DEBUG_GO_ON();

  auto device = !xir_subg->has_attr("device")
                    ? "CPU"
                    : xir_subg->get_attr<string>("device");

  UNI_LOG_DEBUG_INFO << "subg_name: " << xir_subg->get_name() << endl;
  UNI_LOG_DEBUG_INFO << "subg_device: " << device << endl;

  // input ops summary
  auto input_ops = get_input_ops(xir_subg);
  UNI_LOG_DEBUG_INFO << "subg_inputs(" << input_ops.size() << "): " << endl;
  for (const auto* op : input_ops) {
    UNI_LOG_DEBUG_INFO << "\ttype: " << op->get_type()
                       << ", name: " << op->get_name() << endl;
  }

  // output ops summary
  auto output_ops = get_output_ops(xir_subg);
  UNI_LOG_DEBUG_INFO << "subg_outputs(" << output_ops.size() << "): " << endl;
  for (const auto* op : output_ops) {
    UNI_LOG_DEBUG_INFO << "\ttype: " << op->get_type()
                       << ", name: " << op->get_name() << endl;
  }

  auto subg_ops = const_cast<xir::Subgraph*>(xir_subg)->topological_sort();
  UNI_LOG_DEBUG_INFO << "topological_sort op lists(" << subg_ops.size()
                     << "): " << endl;
  for (const auto* op : subg_ops) {
    UNI_LOG_DEBUG_INFO << "\ttype: " << op->get_type()
                       << ", name: " << op->get_name() << endl;
  }
}

Pad_t calc_pad(FMap_t fmap_i, FMap_t fmap_o, Kernel_t kernel, Stride_t stride,
               Pad_t pad) {
  if (pad.mode == PadAttr::PAD_MODE_VALID) {
    UNI_LOG_CHECK(pad.l == 0, VART_INVALID_VALUE) << ", " << pad.l << " != 0";
    UNI_LOG_CHECK(pad.t == 0, VART_INVALID_VALUE) << ", " << pad.t << " != 0";
    UNI_LOG_CHECK(pad.r == 0, VART_INVALID_VALUE) << ", " << pad.r << " != 0";
    UNI_LOG_CHECK(pad.b == 0, VART_INVALID_VALUE) << ", " << pad.b << " != 0";
  }

  if (pad.mode != PadAttr::PAD_MODE_SAME) return pad;

  Pad_t ret;
  // calculate new pad's left and right
  auto pad_extra_w = (fmap_o.w - 1) * stride.w + std::max(kernel.w, stride.w) -
                     (fmap_i.w + pad.l + pad.r);
  if (pad_extra_w >= 0) {
    auto half_extra = floor(pad_extra_w / 2.0);
    ret.l = pad.l + half_extra;
    ret.r = pad.r + pad_extra_w - half_extra;
  } else {
    ret.l = pad.l;
    ret.r = pad.r + pad_extra_w;
  }

  // calculate new pad's top and bottom
  auto pad_extra_h = (fmap_o.h - 1) * stride.h + std::max(kernel.h, stride.h) -
                     (fmap_i.h + pad.t + pad.b);
  if (pad_extra_h >= 0) {
    auto half_extra = floor(pad_extra_h / 2.0);
    ret.t = pad.t + half_extra;
    ret.b = pad.b + pad_extra_h - half_extra;
  } else {
    ret.t = pad.t;
    ret.b = pad.b + pad_extra_h;
  }

  ret.mode = pad.mode;
  return ret;
}

}  // namespace cpu
}  // namespace vart
