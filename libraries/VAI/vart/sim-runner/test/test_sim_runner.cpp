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
#include <fstream>
#include <iostream>
#include <xir/util/tool_function.hpp>

#include "sim_runner.hpp"
#include "sim_tensor_buffer.hpp"
#include "vart/runner.hpp"
#include "xir/graph/graph.hpp"
#include "xir/graph/subgraph.hpp"

using namespace std;

static std::string layer_name(const std::string& name) {
  (void)layer_name;
  auto name_remove_xfix = xir::remove_xfix(name);
  std::string ret;
  ret.reserve(name_remove_xfix.size());
  std::transform(name_remove_xfix.begin(), name_remove_xfix.end(),
                 std::back_inserter(ret), [](char c) {
                   bool ok = c >= '0' && c <= '9';
                   ok = ok || (c >= 'a' && c <= 'z');
                   ok = ok || (c >= 'A' && c <= 'Z');
                   // ok = ok || (c == '/');
                   ok = ok || (c == '_');
                   return ok ? c : '_';
                 });
  return ret;
}

int main(int argc, char* argv[]) {
  UniLog::Initial(argv[0], UNI_LOG_STD, UNI_LOG_LEVEL_INFO,
                  UNI_LOG_STD_LEVEL_INFO);

  if (argc != 4) {
    std::cout
        << "usage: test_sim_runner  <xmodel> <subgraph_name> <input_data>!"
        << std::endl;
    exit(-1);
  }
  std::string xmodel_file(argv[1]);
  auto kernel = argv[2];
  auto input_file = std::string(argv[3]);
  std::unique_ptr<xir::Graph> graph = xir::Graph::deserialize(xmodel_file);

  xir::Subgraph* root_sg = graph->get_root_subgraph();
  std::vector<xir::Subgraph*> children = root_sg->children_topological_sort();
  int dpu_graph_num = 0;
  xir::Subgraph* subgraph = nullptr;
  for (auto child : children) {
    auto device = child->get_attr<std::string>("device");
    LOG(INFO) << "checking subgraph " << child->get_name()
              << " device = " << device;
    if (device == "DPU") {
      dpu_graph_num++;
    }
    if (child->get_name() == kernel) {
      CHECK_EQ(device, "DPU") << "kernel is not DPU kernel. " << kernel;
      subgraph = child;
    }
  }
  CHECK(subgraph != nullptr) << "cannot find kernel. kernel = " << kernel;
  auto runner = std::make_unique<vart::sim::SimRunner>(subgraph);

  // create TensorBuffer
  auto in_tensor = runner->get_input_tensors();
  auto out_tensor = runner->get_output_tensors();

  std::vector<vart::TensorBuffer*> input;
  std::vector<vart::TensorBuffer*> output;
  for (auto in : in_tensor) {
    input.push_back(new vart::sim::SimTensorBuffer<int8_t>(in));
  }
  for (auto out : out_tensor) {
    output.push_back(new vart::sim::SimTensorBuffer<int8_t>(out));
  }

  auto dat_info = input[0]->data(std::vector<int>{0, 0, 0, 0});
  auto batch_size = input[0]->get_tensor()->get_shape()[0];
  char* data = reinterpret_cast<char*>(dat_info.first);
  size_t input_size = dat_info.second;
  CHECK(std::ifstream(input_file)
            .read((char*)data, input_size / batch_size)
            .good())
      << "fail to read! filename=" << input_file;
  // start runner
  runner->execute_async(input, output);

  // dump outputs
  for (auto i = 0; i < batch_size && i == 0; i++) {
    for (auto tb : output) {
      auto dat_info = tb->data(std::vector<int>{i, 0, 0, 0});
      char* data = reinterpret_cast<char*>(dat_info.first);
      size_t chunk_size = dat_info.second;
      std::string output_file = layer_name(tb->get_tensor()->get_name());
      LOG(INFO) << "dump " << chunk_size << " bytes to  " << output_file;
      CHECK(std::ofstream(output_file, std::ios::binary | std::ios::trunc)
                .write(data, chunk_size)
                .good())
          << "write failed";
    }
  }
  return 0;
}
