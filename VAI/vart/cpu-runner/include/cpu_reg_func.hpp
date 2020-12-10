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
#include "cpu_tensor_buffer.hpp"

#define VART_STR_CONCAT_(__x, __y) __x##__y
#define VART_STR_CONCAT(__x, __y) VART_STR_CONCAT_(__x, __y)

#define REG_OP_INSTANCE_FUNC(xir_op_type, class_name)                          \
  static vart::cpu::CPURegFunc& VART_STR_CONCAT(class_name, __COUNTER__) =     \
      vart::cpu::CPURegFunc::instance()->register_func(                        \
          std::string{xir_op_type},                                            \
          [](const xir::Subgraph* subg, const xir::Op* xir_op,                 \
             IMapTBs_t inputs, CPUTBPtr_t output) {                            \
            auto* tensor = xir_op->get_output_tensor();                        \
            auto xir_dtype = tensor->get_data_type().type;                     \
            auto bit_width = tensor->get_data_type().bit_width;                \
            string dbg_str = get_data_type_str(tensor);                        \
            vart::cpu::CPUOPBase* op = nullptr;                                \
            if (xir_dtype == xir::DataType::FLOAT) {                           \
              if (bit_width == 32) {                                           \
                op = CPUOPFactory::Instance()                                  \
                         .create<vart::cpu::class_name<float>>(                \
                             subg, xir_op, inputs, output);                    \
              } else if (bit_width == 64) {                                    \
                op = CPUOPFactory::Instance()                                  \
                         .create<vart::cpu::class_name<double>>(               \
                             subg, xir_op, inputs, output);                    \
              } else {                                                         \
                UNI_LOG_FATAL(VART_NOT_SUPPORT)                                \
                    << "Not support bit_width " << bit_width << endl;          \
              }                                                                \
            } else {                                                           \
              op = CPUOPFactory::Instance()                                    \
                       .create<vart::cpu::class_name<int32_t>>(                \
                           subg, xir_op, inputs, output);                      \
            }                                                                  \
            if (VART_DEBUG) {                                                  \
              UNI_LOG_DEBUG_INFO << "Construct vart::cpu::" << #class_name     \
                                 << "<" << dbg_str << "> obj" << endl;         \
            }                                                                  \
            return op;                                                         \
          });

#define REG_NONTP_OP_INSTANCE_FUNC(xir_op_type, class_name)                    \
  static vart::cpu::CPURegFunc& VART_STR_CONCAT(class_name, __COUNTER__) =     \
      vart::cpu::CPURegFunc::instance()->register_func(                        \
          std::string{xir_op_type},                                            \
          [](const xir::Subgraph* subg, const xir::Op* xir_op,                 \
             IMapTBs_t inputs, CPUTBPtr_t output) {                            \
            vart::cpu::CPUOPBase* op =                                         \
                CPUOPFactory::Instance().create<vart::cpu::class_name>(        \
                    subg, xir_op, inputs, output);                             \
            if (VART_DEBUG) {                                                  \
              UNI_LOG_DEBUG_INFO << "Construct vart::cpu::" << #class_name     \
                                 << "obj" << endl;                             \
            }                                                                  \
            return op;                                                         \
          });

namespace vart {
namespace cpu {

class CPUOPBase;

class CPUOPFactory {
 private:
  CPUOPFactory() = default;
  ~CPUOPFactory() = default;

 public:
  static CPUOPFactory& Instance() {
    static CPUOPFactory f;
    return f;
  }

  template <typename C>
  CPUOPBase* create(const xir::Subgraph* subg, const xir::Op* xir_op,
                    IMapTBs_t inputs, CPUTBPtr_t output) {
    auto op_name = xir_op->get_name();

    // NOTE: must create every time, can not reuse,
    // because inputs and output will change
    // if(op_name_map_.find(op_name) != op_name_map_.end())
    //   return op_name_map_.at(op_name).get();

    auto op = make_unique<C>(subg, xir_op, inputs, output);

    op_name_map_[op_name] = std::move(op);
    return op_name_map_.at(op_name).get();
  }

 private:
  map<string, unique_ptr<CPUOPBase>> op_name_map_;
};

class CPURegFunc {
 public:
  using RegFuncType = function<CPUOPBase*(const xir::Subgraph*, const xir::Op*,
                                          IMapTBs_t, CPUTBPtr_t)>;

 private:
  explicit CPURegFunc();
  ~CPURegFunc() = default;

 public:
  static CPURegFunc* instance() {
    static CPURegFunc rf;

    return &rf;
  }

  CPURegFunc& register_func(const string& op_type, RegFuncType regfunc);
  RegFuncType get_register_func(const string& op_type);

  void show_regmap();

 private:
  map<string, RegFuncType> regfunc_;
};

}  // namespace cpu
}  // namespace vart
