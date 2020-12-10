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

#include "c_xmodel_api.hpp"
#include "xmodel_run_sample.hpp"

#define CHECK_NULL_PTR(ptr)                                                    \
  do {                                                                         \
    if (ptr == nullptr) {                                                      \
      cerr << "nullptr!" << endl;                                              \
      abort();                                                                 \
    }                                                                          \
  } while (0);

#define XMODEL_PTR(xmodel) reinterpret_cast<vart::cpu::XModelRunSample*>(xmodel)

void TestHelloWorld() { std::cout << "Hello world!" << std::endl; }

void* LoadXModel(const char* xmode_name) {
  UNI_LOG_DEBUG_INFO << "enter LoadXModel" << endl;

  auto g = xir::Graph::deserialize(xmode_name);

  auto s = new vart::cpu::XModelRunSample(
      std::move(g), vart::cpu::CPUCfg::Instance().get_xmodel_run_mode());

  auto input_tbs = s->get_input_tbs();
  CHECK(input_tbs.size() == 1) << ", " << input_tbs.size() << " != 1";

  for (auto* tb : input_tbs) {
    vart::cpu::Random(vart::cpu::TBPTR(tb),    //
                      vart::cpu::TBSIZE(tb),   //
                      static_cast<char>(-16),  //
                      static_cast<char>(16),   //
                      0x123);                  //
  }

  return s;
}

void FreeXModel(void* xmodel) {
  UNI_LOG_DEBUG_INFO << "Enter FreeXModel" << endl;
  CHECK_NULL_PTR(xmodel);

  delete XMODEL_PTR(xmodel);
}

void RunXModel(void* xmodel) {
  UNI_LOG_DEBUG_INFO << "Enter RunXModel" << endl;
  CHECK_NULL_PTR(xmodel);

  XMODEL_PTR(xmodel)->run();
}

int GetRunnerNum(void* xmodel) {
  UNI_LOG_DEBUG_INFO << "Enter GetRunnerNum" << endl;
  CHECK_NULL_PTR(xmodel);

  return XMODEL_PTR(xmodel)->get_runner_num();
}

void GetRunners(void* xmodel, void** runner_arr) {
  UNI_LOG_DEBUG_INFO << "Enter GetRunners" << endl;
  auto v = XMODEL_PTR(xmodel)->get_runners();

  for (auto i = 0U; i < v.size(); i++) {
    runner_arr[i] = v[i];
    // for debug
    cout << "runner addr " << v[i] << ", " << runner_arr[i] << endl;
  }
}
