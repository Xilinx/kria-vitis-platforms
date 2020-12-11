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

/*!
 * \brief just a test function
 *  print hello world
 */
VART_CPU_DLL void TestHelloWorld();

/*!
 * \brief used to load xmodel using giving name
 *  xmode_name: xmodel's name
 *
 *  The function is thread-safe,
 *  different thread can call this func.
 */
VART_CPU_DLL void* LoadXModel(const char* xmode_name);

/*!
 * \brief used to free xmodel using giving xmodel pointer
 *  xmode: xmodel's raw pointer
 *
 *  The function is thread-safe,
 *  different thread can call this func.
 */
VART_CPU_DLL void FreeXModel(void* xmodel);

/*!
 * \brief used to run loaded xmodel
 *  xmode: xmodel's raw pointer
 *
 *  The function is thread-safe,
 *  different thread can call this func.
 */
VART_CPU_DLL void RunXModel(void* xmodel);

/*!
 * \brief used to get xmodel's runner number
 *  xmode: xmodel's raw pointer
 *
 *  The function is thread-safe,
 *  different thread can call this func.
 */
VART_CPU_DLL int GetRunnerNum(void* xmodel);

/*!
 * \brief used to get all xmodel's runners
 *  xmode: xmodel's raw pointer
 *  runner_arr: array used to store returned runners
 *
 *  The function is thread-safe,
 *  different thread can call this func.
 */
VART_CPU_DLL void GetRunners(void* xmodel, void** runner_arr);

#ifdef __cplusplus
}
#endif

