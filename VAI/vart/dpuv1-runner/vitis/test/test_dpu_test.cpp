// Copyright 2019 Xilinx Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <iostream>

#include "dpu_test/dpu_test_runner.hpp"

using namespace vitis::ai;

static CpuFlatTensorBuffer create_tensor_buffer(Tensor* tensor, float value) {
  auto length = tensor->get_element_num();
  float* data = new float[length];
  for (auto i = 0; i < length; i++) {
    data[i] = value;
  }
  return CpuFlatTensorBuffer{data, tensor};
}

static std::vector<CpuFlatTensorBuffer> create_tb_list(int size,
                                                       Tensor* tensor) {
  auto ret = std::vector<CpuFlatTensorBuffer>{};
  for (auto i = 0; i < size; i++) {
    ret.push_back(create_tensor_buffer(tensor, i));
  }
  return ret;
}

static void destroy_tb_list(std::vector<CpuFlatTensorBuffer>& list) {
  for (auto i = 0U; i < list.size(); i++) {
    delete[]((float*)(list[i].data().first));
  }
}

int main() {
  auto runner = DpuTestRunner{1.f, {10, 1000, 1000}};
  auto batch = 10;
  auto input_tensors = runner.get_input_tensors();
  auto output_tensors = runner.get_output_tensors();
  auto in0_list = create_tb_list(batch, input_tensors[0]);
  auto in1_list = create_tb_list(batch, input_tensors[1]);
  auto out_list = create_tb_list(batch, output_tensors[0]);

  std::cout << "start" << std::endl;

  auto jobs = std::vector<int>{};
  for (auto idx = 0; idx < batch; idx++) {
    auto result = runner.execute_async(
        std::vector<TensorBuffer*>{&in0_list[idx], &in1_list[idx]},
        std::vector<TensorBuffer*>{&out_list[idx]});
    jobs.push_back(result.first);
  }

  for (auto idx = 0; idx < batch; idx++) {
    runner.wait(jobs[idx], -1);
    std::cout << "job " << jobs[idx] << " done, in0 "
              << ((float*)(in0_list[idx].data().first))[0] << ", in1 "
              << ((float*)(in1_list[idx].data().first))[0] << ", out "
              << ((float*)(out_list[idx].data().first))[0] << std::endl;
  }

  destroy_tb_list(in0_list);
  destroy_tb_list(in1_list);
  destroy_tb_list(out_list);
  return 0;
}
