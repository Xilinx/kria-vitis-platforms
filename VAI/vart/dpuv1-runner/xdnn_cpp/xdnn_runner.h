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
#ifndef XDNN_RUNNER_H
#define XDNN_RUNNER_H

#include "dpu/dpu_runner.hpp"

#include <future>
#include <map>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace vitis {
namespace ai {

class XdnnRunner : public DpuRunner {
public:
  explicit XdnnRunner(const std::string &path);
  // TODO explicit XdnnRunner(const xir::Subgraph);
  ~XdnnRunner();

  virtual std::pair<uint32_t, int>
  execute_async(const std::vector<TensorBuffer*> &input,
                const std::vector<TensorBuffer*> &output) override;

  virtual int wait(int jobid, int timeout=-1) override;

  virtual TensorFormat get_tensor_format() override { 
      return DpuRunner::TensorFormat::NCHW;
  }

  virtual std::vector<Tensor*> get_input_tensors() override;
  virtual std::vector<Tensor*> get_output_tensors() override;

private:
  uint32_t get_job_id();

  const std::string path_;
  std::queue<uint32_t> job_ids_;
  std::vector<XBLASHandle*> handles_;
  std::vector<XDNNScriptExecutor<float>*> executors_;
  std::vector<Tensor*> inputs_;
  std::vector<Tensor*> outputs_;
  std::mutex mtx_;
  std::condition_variable cv_;
};

}
}

extern "C" {
  std::vector<std::unique_ptr<vitis::ai::DpuRunner>> *create_dpu_runner(const vitis::ai::DpuMeta &dpuMeta);
}

#endif
