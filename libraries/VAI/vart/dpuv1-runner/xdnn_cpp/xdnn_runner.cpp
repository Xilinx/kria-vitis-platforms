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
#include <chrono>
#include <fstream>
#include <map>
#include <regex>
#include <string>
#include <streambuf>
#include <sys/types.h>
#include <unistd.h>
#include "cJSON.h"
#include "xblas.h"
#include "xdnn.h"
#include "xdnn_env.h"
#include "xdnn_runner.h"

namespace vitis {
namespace ai {

#define MAX_JOB_IDS 1000

static std::string parsePath(const std::string &path) {
  // regex replace any {TEXT} with std::getenv("TEXT")
  // E.g. "{VAI_ALVEO_ROOT}/overlaybins/xdnnv3" > "/opt/ml-suite/overlaybins/xdnnv3"
  std::string retpath = path;
  std::regex re("\\{(.+)\\}");
  std::sregex_iterator next(path.begin(), path.end(), re);
  std::sregex_iterator end;
  while (next != end) {
    std::smatch match = *next;
    std::string word = "\\{"+match.str(1)+"\\}";
    retpath = std::regex_replace(retpath, 
      std::regex(word), 
      std::getenv(match.str(1).c_str()));
    next++;
  } 
  return retpath;
}

XdnnRunner::XdnnRunner(const std::string &path) 
: path_(path)
{
  std::ifstream m(path + "/meta.json");
  std::string mfstr((std::istreambuf_iterator<char>(m)),
    std::istreambuf_iterator<char>());
  cJSON *meta = cJSON_Parse(mfstr.c_str());
  cJSON *xclbin = cJSON_GetObjectItemCaseSensitive(meta, "xclbin");
  std::string xclbinPath = parsePath(xclbin->valuestring);

  int numFPGA = 1;
  cJSON *numf = cJSON_GetObjectItemCaseSensitive(meta, "num_fpga");
  if (numf) numFPGA = numf->valueint;

  cJSON *subId = cJSON_GetObjectItemCaseSensitive(meta, "subscribe_id");
  cJSON *pubId = cJSON_GetObjectItemCaseSensitive(meta, "publish_id");
  std::string subscribeId, publishId;
  if (subId) subscribeId = subId->valuestring;
  if (pubId) publishId = pubId->valuestring;

  const std::string kernelName = "kernelSxdnn_0";
  const int acquireId = 0;
  handles_.resize(numFPGA);
  for (int f=0; f < numFPGA; f++)
    xblasCreate(handles_[f], xclbinPath.c_str(), kernelName.c_str(), f,
      acquireId, subscribeId.c_str(), publishId.c_str());

  std::string weightsPath = path + "/weights.h5";
  std::string netFile = path + "/compiler.json";
  std::string quantFile = path + "/quantizer.json";
  executors_.resize(numFPGA);
  for (int f=0; f < numFPGA; f++)
    executors_[f] = new XDNNScriptExecutor<float>({handles_[f]}, weightsPath, netFile, quantFile);

  std::ifstream f(netFile);
  std::string fstr((std::istreambuf_iterator<char>(f)),
    std::istreambuf_iterator<char>());
  cJSON *json = cJSON_Parse(fstr.c_str());
  assert(json != NULL);

  std::unordered_map<std::string, std::string> inputNames, outputNames;

  cJSON *input;
  cJSON_ArrayForEach(input, cJSON_GetObjectItemCaseSensitive(json, "inputs"))
  {
    cJSON *name = cJSON_GetObjectItemCaseSensitive(input, "input_name");
    inputNames[name->valuestring] = name->valuestring;
  }

  cJSON *output;
  cJSON_ArrayForEach(output, cJSON_GetObjectItemCaseSensitive(json, "outputs"))
  {
    cJSON *name = cJSON_GetObjectItemCaseSensitive(output, "output_name");
    cJSON *tensorName = cJSON_GetArrayItem(
      cJSON_GetObjectItemCaseSensitive(output, "previous_tensors"), 0);
    outputNames[name->valuestring] = tensorName->valuestring;
  }

  cJSON *op;
  cJSON_ArrayForEach(op, cJSON_GetObjectItemCaseSensitive(json, "network"))
  {
    cJSON *name = cJSON_GetObjectItemCaseSensitive(op, "name");
    if(name && name->valuestring) {
      if (inputNames.find(name->valuestring) != inputNames.end()
        || outputNames.find(name->valuestring) != outputNames.end())
      {
        std::vector<std::int32_t> dims;

        cJSON *dim;
        cJSON_ArrayForEach(dim, cJSON_GetObjectItemCaseSensitive(op, "outputshapes"))
          dims.push_back(dim->valueint);

        if (inputNames.find(name->valuestring) != inputNames.end())
        {
          std::string tname = inputNames[name->valuestring];
          Tensor *t = new Tensor(tname, dims, Tensor::DataType::FLOAT);
          inputs_.push_back(t);
        }
        else
        {
          std::string tname = outputNames[name->valuestring];
          Tensor *t = new Tensor(tname, dims, Tensor::DataType::FLOAT);
          outputs_.push_back(t);
        }
      }
    }
  }

  // init finite pool of job IDs that will be reused
  auto pid = getpid();
  auto numJobIds = XDNN_USE_JOB_THREADS_TOTAL_THREADS;
  assert(numJobIds < MAX_JOB_IDS);
  for (int i=0; i < numJobIds; i++)
  {
    auto iStr = std::to_string(i);
    auto id = std::stoi(std::to_string(pid) // prefix with pid
      + std::string(3 - iStr.length(), '0') // pad with zeros
      + iStr);                              // id
    job_ids_.push(id);
  }
}

XdnnRunner::~XdnnRunner() {
  for (int i=0; i < inputs_.size(); i++)
    delete inputs_[i];
  for (int i=0; i < outputs_.size(); i++)
    delete outputs_[i];

  for (int i=0; i < executors_.size(); i++)
    delete executors_[i];

  for (int i=0; i < handles_.size(); i++)
    xblasDestroy(handles_[i]);
}

std::vector<Tensor *> XdnnRunner::get_input_tensors() {
  return inputs_;
}
std::vector<Tensor *> XdnnRunner::get_output_tensors() {
  return outputs_;
}

std::pair<uint32_t, int>
XdnnRunner::execute_async(const std::vector<TensorBuffer *> &inputs,
                          const std::vector<TensorBuffer *> &outputs) {
  try {
    std::unordered_map<std::string, std::vector<const float*> > inputPtrs;
    std::unordered_map<std::string, std::vector<float*> > outPtrs;

    uint32_t job_id = get_job_id();

    for (unsigned j = 0; j < inputs.size(); j++){
      TensorBuffer *tb = inputs[j]; 
      inputPtrs[tb->get_tensor()->get_name()].size();
    }
    for (unsigned j = 0; j < outputs.size(); j++){
      TensorBuffer *tb = outputs[j]; 
      outPtrs[tb->get_tensor()->get_name()].size();
    }

    for (unsigned j = 0; j < inputs.size(); j++){
      TensorBuffer *tb = inputs[j]; 
      float *dptr = (float*)(tb->data().first);
      int bsz = tb->get_tensor()->get_shape().at(0);
      int elsize = tb->get_tensor()->get_element_num() / bsz;

      for (int b=0; b < bsz; b++)
        inputPtrs[tb->get_tensor()->get_name()].push_back(dptr + (b*elsize));
    }

    for (unsigned j = 0; j < outputs.size(); j++) {
      TensorBuffer *tb = outputs[j]; 
      float *dptr = (float*)(tb->data().first);
      int bsz = tb->get_tensor()->get_shape().at(0);
      int elsize = tb->get_tensor()->get_element_num() / bsz;

      for (int b=0; b < bsz; b++)
        outPtrs[tb->get_tensor()->get_name()].push_back(dptr + (b*elsize));
    }

    auto eid = (job_id % MAX_JOB_IDS) % executors_.size();
    
    executors_[eid]->execute_async(inputPtrs, outPtrs, job_id);

    return {job_id, 0};
  }
  catch (...) {
    return {0, -1};
  }
}

uint32_t XdnnRunner::get_job_id() {
  std::unique_lock<std::mutex> lock(mtx_);
  while (job_ids_.empty()) 
    if (cv_.wait_for(lock, std::chrono::seconds(10)) == std::cv_status::timeout)
      throw;
    
  uint32_t id = job_ids_.front();
  job_ids_.pop();
  return id;
}

int XdnnRunner::wait(int jobId, int timeout) {
  auto eid = (jobId % MAX_JOB_IDS) % executors_.size();
  executors_[eid]->waitForResults(jobId);

  std::unique_lock<std::mutex> lock(mtx_);
  job_ids_.push(jobId);
  cv_.notify_all();

  return 0;
}

}
}


/**
 * singleton class to manage runners and cache its pytensors
 */
class DpuPyRunnerMgr {
public:
  static DpuPyRunnerMgr &instance() {
    static DpuPyRunnerMgr inst;
    return inst;
  }

  void *createRunner(char *path)
  {
    std::vector<std::unique_ptr<vitis::ai::DpuRunner>> objs
      = vitis::ai::DpuRunner::create_dpu_runner(path);
    vitis::ai::DpuRunner *runner = objs[0].get();
    runners.emplace(runner, std::move(objs[0]));
    
    pyInputTensors.erase(runner);
    pyOutputTensors.erase(runner);

    std::vector<DpuPyTensor> inTensors, outTensors;
    std::vector<vitis::ai::Tensor*> in = runner->get_input_tensors();
    for (int i=0; i < in.size(); i++)
      inTensors.push_back(DpuPyTensor(*in[i]));
    std::vector<vitis::ai::Tensor*> out = runner->get_output_tensors();
    for (int i=0; i < out.size(); i++)
      outTensors.push_back(DpuPyTensor(*out[i]));

    pyInputTensors[runner] = inTensors;
    pyOutputTensors[runner] = outTensors;

    return runner;
  }

  void destroyRunner(void *runner)
  {
    pyInputTensors.erase(runner);
    pyOutputTensors.erase(runner);
    runners.erase(runner);
  } 
  
  std::vector<DpuPyTensor> &getInputTensors(void *runner) 
  {
    return pyInputTensors[runner];
  }
  std::vector<DpuPyTensor> &getOutputTensors(void *runner) 
  {
    return pyOutputTensors[runner];
  }
  
private:
  DpuPyRunnerMgr() {};
  DpuPyRunnerMgr(DpuPyRunnerMgr const&) {};
  DpuPyRunnerMgr& operator=(DpuPyRunnerMgr const&) {};

  std::unordered_map<void*, std::unique_ptr<vitis::ai::DpuRunner>> runners;
  std::unordered_map<void*, std::vector<DpuPyTensor>> pyInputTensors;
  std::unordered_map<void*, std::vector<DpuPyTensor>> pyOutputTensors;
};

/**
 * C API implementation
 */

void *DpuPyRunnerCreate(char *path)
{
  return DpuPyRunnerMgr::instance().createRunner(path);
}

void DpuPyRunnerGetInputTensors(void *runner, DpuPyTensor **tensors, int *tensor_count) {
  std::vector<DpuPyTensor> &vals 
    = DpuPyRunnerMgr::instance().getInputTensors(runner);
  *tensors = vals.data();
  *tensor_count = vals.size();
}

void DpuPyRunnerGetOutputTensors(void *runner, DpuPyTensor **tensors, int *tensor_count) {
  std::vector<DpuPyTensor> &vals 
    = DpuPyRunnerMgr::instance().getOutputTensors(runner);
  *tensors = vals.data();
  *tensor_count = vals.size();
}

int DpuPyRunnerGetTensorFormat(void *runner_) {
  vitis::ai::DpuRunner *runner = (vitis::ai::DpuRunner*) runner_;
  return int(runner->get_tensor_format());
}

int DpuPyRunnerExecuteAsync(void *runner_, void **pyin, void **pyout, int batch_sz, int *status) 
{
  vitis::ai::DpuRunner *runner = (vitis::ai::DpuRunner*) runner_;
  std::vector<vitis::ai::Tensor *> inputs = runner->get_input_tensors();
  std::vector<vitis::ai::Tensor *> outputs = runner->get_output_tensors();
  std::vector<std::shared_ptr<vitis::ai::Tensor> > batchmodTensors;

  std::vector<vitis::ai::CpuFlatTensorBuffer> inputBuffers, outputBuffers;
  for (int i=0; i < inputs.size(); i++)
  {
    auto dims = inputs[i]->get_shape();
    dims[0] = batch_sz; // override batch size
    batchmodTensors.push_back(std::shared_ptr<vitis::ai::Tensor>(
      new vitis::ai::Tensor(
        inputs[i]->get_name(), dims, inputs[i]->get_data_type())));
    inputBuffers.push_back(vitis::ai::CpuFlatTensorBuffer(pyin[i], 
      batchmodTensors.back().get()));
  }
  for (int i=0; i < outputs.size(); i++)
  {
    /* 
     * FIXME?
     * If input batch size is N, 
     * will output batch size be N as well, or merged into 1?
     */
    auto dims = outputs[i]->get_shape();
    dims[0] = batch_sz; // override batch size
    batchmodTensors.push_back(std::shared_ptr<vitis::ai::Tensor>(
      new vitis::ai::Tensor(
        outputs[i]->get_name(), dims, outputs[i]->get_data_type())));
    outputBuffers.push_back(vitis::ai::CpuFlatTensorBuffer(pyout[i],
      batchmodTensors.back().get()));
  }

  std::vector<vitis::ai::TensorBuffer*> inputBufferPtrs, outputBufferPtrs;
  for (int i=0; i < inputBuffers.size(); i++)
    inputBufferPtrs.push_back(&inputBuffers[i]);
  for (int i=0; i < outputBuffers.size(); i++)
    outputBufferPtrs.push_back(&outputBuffers[i]);

  auto response = runner->execute_async(inputBufferPtrs, outputBufferPtrs);
  *status = response.second;
  return response.first;
}

int DpuPyRunnerWait(void *runner_, int jobId)
{
  vitis::ai::DpuRunner *runner = (vitis::ai::DpuRunner*) runner_;
  return runner->wait(jobId, -1);
}

void DpuPyRunnerDestroy(void *runner_)
{
  DpuPyRunnerMgr::instance().destroyRunner(runner_);
}

/** @brief create dpu runner
 */
std::vector<std::unique_ptr<vitis::ai::DpuRunner>> *create_dpu_runner(
  const vitis::ai::DpuMeta& dpuMeta) {
  auto runners = new std::vector<std::unique_ptr<vitis::ai::DpuRunner>>();
  runners->emplace_back(new vitis::ai::XdnnRunner(dpuMeta.dirname));
  return runners;
}
