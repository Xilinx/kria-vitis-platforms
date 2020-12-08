/*
 * Copyright 2020 Xilinx, Inc.
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
#include "ivas_xdpupriv.hpp"

#include <vitis/ai/refinedet.hpp>
#include <vitis/ai/nnpp/refinedet.hpp>

using namespace std;
using namespace cv;

class ivas_xrefinedet:public ivas_xdpumodel
{

  int log_level = 0;
    std::unique_ptr < vitis::ai::RefineDet > model;
  char (*label_str)[MAX_NAME_LENGTH];

public:

    ivas_xrefinedet (ivas_xkpriv * kpriv, const std::string & model_name,
      bool need_preprocess);

  virtual int run (ivas_xkpriv * kpriv, const cv::Mat & image,
      GstInferenceMeta * ivas_meta);

  virtual int requiredwidth (void);
  virtual int requiredheight (void);
  virtual int close (void);

    virtual ~ ivas_xrefinedet ();
};
