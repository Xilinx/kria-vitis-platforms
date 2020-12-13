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

#include "cpu_std_inc.hpp"

#ifndef VART_CPU_DLL
#define VART_CPU_DLL __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Interface structure for python
struct APIOpOverview {
  char* name; // op name
  char* type; // op type
  int input_num; // op input number
  char* input_list; // op input op name list
  char* output_tensor_name; // op output tensor name
  char* data_type; // op output data type
  uint64_t workload; // op workload
};

/*!
 * \brief get given op's overview,
 *  details are in APIOpOverview structure
 *  op: the op pointer given as input
 *  info: op's overview which will be returned
 *
 *  The function is thread-safe,
 *  different thread can call this func.
 */
VART_CPU_DLL void GetCPUOpOverview(void* op, APIOpOverview* info);

/*!
 * \brief run given op
 *  op: the op raw pointer
 *
 *  The function is thread-safe,
 *  different thread can call this func.
 */
VART_CPU_DLL void CPUOpRun(void* op);

#ifdef __cplusplus
}
#endif

