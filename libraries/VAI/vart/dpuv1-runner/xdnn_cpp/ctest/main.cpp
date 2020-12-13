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
#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include "xblas.h"
#include "xdnn.h"

#define PFX "[CXDNN] "

using namespace std;

// Note: this is assuming V1 format (without separate width/height)
int readFromFile(const string &fname, string &layerName, 
  int &kernWidth, int &kernHeight, int &inc, int &outc, 
  std::vector<float> &vals, bool hasHeader=true)
{
  ifstream f(fname.c_str());
  if (!f.good())
    return -1;

  if (hasHeader)
  {
    f >> layerName;
    f >> kernWidth;
    f >> inc; 
    f >> outc;
  }
  vals.clear();
  while (f.good())
  {
    float val;
    f >> val;
    if (!f.good())
      break;

    vals.push_back(val);
  }

  cout << PFX << "reading... " << layerName << " " << vals.size() << endl;

  return 0;
}

XDNNScriptExecutor<float>* loadExecutor(XBLASHandle *handle, const string &dataDir, const string &netCfgFile, const string &quantCfgFile)
{
  std::map<std::string, std::vector<float>*> weights;
  std::map<std::string, std::vector<float>*> bias;
  std::map<std::string, XDNNWeightParam<float> > m;
  std::vector<XBLASHandle*> handles;
  handles.push_back(handle);

  for (int fi=0; ; fi++)
  {
    stringstream ss;
    ss << dataDir << "/fwbqb_" << fi;
    string fname = ss.str();

    string layerName;
    int kern = -1;
    int inc = -1;
    int outc = -1;
    std::vector<float> *w = new vector<float>();

    // read weights
    int ret = readFromFile(fname, layerName, kern, kern, inc, outc, *w);
    if (ret < 0)
    {
      delete w;
      break;
    }
    weights[layerName] = w;

    // read bias
    std::vector<float> *b = new vector<float>();
    ss.str("");
    ss << dataDir << "/fwbqb_bias_" << fi;
    fname = ss.str();
    readFromFile(fname, layerName, kern, kern, inc, outc, *b);
    bias[layerName] = b;

    float *weightPtr = &((*w)[0]);
    float *biasPtr = &((*b)[0]);
    m[layerName] = { weightPtr, biasPtr, w->size(), b->size() };
  }

  XDNNScriptExecutor<float> *executor
    = new XDNNScriptExecutor<float>(handles, m, netCfgFile, quantCfgFile, 
      /*scaleB*/30,/*cuMask*/0);
  return executor;
}

std::vector<float> loadInput(const string &dataDir)
{
  stringstream ss;
  ss << dataDir << "/input";
  string fname = ss.str();

  ifstream f(fname.c_str());
  assert(f.good());
  
  vector<float> vals;
  for (string line; getline(f, line); )
  {
    istringstream iss(line); 
    float val;
    while (iss >> val)
      vals.push_back(val);
  }

  cout << PFX << "input size " << vals.size() << endl;
  return vals;
}

void softmax(std::vector<float> &input) 
{
  float m = std::numeric_limits<float>::min();
  for (size_t i = 0; i < input.size(); i++)
    if (input[i] > m) 
      m = input[i];

  float sum = 0.0;
  for (size_t i = 0; i < input.size(); i++)
    sum += expf(input[i] - m);

  for (size_t i = 0; i < input.size(); i++)
    input[i] = expf(input[i] - m) / sum;
}

std::vector<std::string> getLabels(string fname) 
{
  ifstream f(fname.c_str());
  assert(f.good());
  
  std::vector<std::string> labels;
  for (string line; getline(f, line); )
    labels.push_back(line);
  
  return labels;
}
    
int main(int argc, char* argv[])
{

  const string kernelName = "kernelSxdnn_0";
  const string xclbin = argv[1];
  const string netCfgFile = argv[2];
  const string quantCfgFile = argv[3];
  const string dataDir = argv[4];
  const string labelFile = argv[5];

  const int fpgaOutputSize = 1024;
  const int netOutputSize = 1000; // imagenet

  // load xclbin
  XBLASHandle *handle = NULL;
  int ret = xblasCreate(handle, xclbin.c_str(), kernelName.c_str());
  assert(ret == 0);

  // parse input image data file
  vector<float> input = loadInput(dataDir);
  vector<float> output(fpgaOutputSize);
  float *inputPtr = &(input[0]);
  float *outputPtr = &(output[0]);

  // load XDNN weights & create Executor
  XDNNScriptExecutor<float> *executor = loadExecutor(
    handle, dataDir, netCfgFile, quantCfgFile);

  // load fully-connected layer weights
  int unused;
  std::string unusedStr;
  std::vector<float> fcWeight, fcBias;
  ret = readFromFile(dataDir+"/fc", unusedStr, 
    unused, unused, unused, unused, fcWeight, false);
  assert(ret == 0);
  ret = readFromFile(dataDir+"/fc_bias", unusedStr, 
    unused, unused, unused, unused, fcBias, false);
  assert(ret == 0);

  // run on FPGA
  executor->execute(inputPtr, outputPtr, /*batch_size*/1, /*streamId*/0, /*blocking*/true);

  // run fully-connected layer
  std::vector<float> netOutput(netOutputSize);
  computeFC(&(fcWeight[0]), &(fcBias[0]), outputPtr, 
    1, netOutputSize, fpgaOutputSize, &(netOutput[0]));

  // run softmax
  softmax(netOutput);

  // get labels
  std::vector<std::string> labels = getLabels(labelFile);

  // print top-1
  auto maxIt = std::max_element(netOutput.begin(), netOutput.end());
  int maxIdx = maxIt - netOutput.begin();
  cout << PFX << "Prediction: " << labels[maxIdx] << " " << netOutput[maxIdx] << endl;

  // cleanup
  xblasDestroy(handle);

  return 0;
}
