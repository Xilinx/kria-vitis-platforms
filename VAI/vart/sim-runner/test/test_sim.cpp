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
#include <random>
#include "SimCfg.hpp"
#include "UniLog/UniLog.hpp"
#include "buffer/DDR.hpp"
#include "sim_runner.hpp"
#include "sim_tensor_buffer.hpp"
#include "vart/runner.hpp"
#include "xir/graph/graph.hpp"
#include "xir/graph/subgraph.hpp"

using namespace std;

void random_input(char* data, size_t sz) {
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist(-128, 127);

  for (auto i = 0U; i < sz; i++) {
    *(data + i) = dist(rng);
  }
}

int main(int argc, char* argv[]) {
  UniLog::Initial(argv[0], UNI_LOG_STD, UNI_LOG_LEVEL_INFO,
                  UNI_LOG_STD_LEVEL_INFO);

  if (argc != 2) {
    std::cout << "fatal: a xmodel file must be provided!" << std::endl;
    exit(-1);
  }
  std::string xmodel_file(argv[1]);
  std::unique_ptr<xir::Graph> graph = xir::Graph::deserialize(xmodel_file);

  xir::Subgraph* root_sg = graph->get_root_subgraph();
  std::vector<xir::Subgraph*> children = root_sg->children_topological_sort();
  int dpu_graph_num = 0;
  for (auto child : children) {
    if (child->get_attr<std::string>("device") == "DPU") {
      dpu_graph_num++;
    }
  }
  if (dpu_graph_num == 1) {
    std::cout << "warning: more than one DPU subgraph exist! only the first "
                 "one is simulated...";
    std::cout << std::endl;
  }
  // UNI_LOG_CHECK(dpu_graph_num == 1, SIM_OUT_OF_RANGE) << "more than one DPU
  // subgraph exist!";

  int32_t batch = 0;
  for (auto child : children) {
    if (child->get_attr<std::string>("device") == "DPU") {
      vart::Runner* runner = new vart::sim::SimRunner(child);

      // std::ifstream infile(
      //        "/proj/rdi/staff/charlyl/xmodel/resnet_50_0920/dump/input_aquant_fix.bin",
      //        std::ios::binary
      //        );

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

      batch = input[0]->get_tensor()->get_shape()[0];
      // generate random input data
      for (int i = 0; i < batch; i++) {
        for (auto tb : input) {
          auto dat_info = tb->data(std::vector<int>{i, 0, 0, 0});
          char* data = reinterpret_cast<char*>(dat_info.first);
          size_t chunk_size = dat_info.second;
          random_input(data, chunk_size);
        }
      }

      runner->execute_async(input, output);

      // dump outputs
      for (int i = 0; i < batch; i++) {
        std::string dpu_out_file = SimCfg::Instance().get_debug_path() + "/" +
                                   "batch_" + std::to_string(i) +
                                   "_dpu_output_fmap.bin";
        ofstream outfile(dpu_out_file, std::ios::binary | std::ios::trunc);

        for (auto tb : output) {
          auto dat_info = tb->data(std::vector<int>{i, 0, 0, 0});
          char* data = reinterpret_cast<char*>(dat_info.first);
          size_t chunk_size = dat_info.second;
          outfile.write(data, chunk_size);
        }
      }
      break;
    }
  }

  std::string arch_file =
      SimCfg::Instance().get_debug_path() + "/" + "case.cfg";
  std::ofstream fd(arch_file, std::ios::app);
  fd << "batch : " << batch << std::endl;
  fd.close();
  DDR::Instance().SaveReg(arch_file);

  return 0;
}
