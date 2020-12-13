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

#include "ivas_xreid.hpp"


ivas_xreid::ivas_xreid (ivas_xkpriv * kpriv, const std::string & model_name,
    bool need_preprocess)
{
  log_level = kpriv->log_level;
  LOG_MESSAGE (LOG_LEVEL_DEBUG, kpriv->log_level, "enter");
  model = vitis::ai::Reid::create (model_name, need_preprocess);
}

int
ivas_xreid::run (ivas_xkpriv * kpriv, const cv::Mat & image,
    GstInferenceMeta * infer_meta)
{

  LOG_MESSAGE (LOG_LEVEL_DEBUG, kpriv->log_level, "enter");

  Mat imgx = imread ("images/sample_reid_002.jpg");
  auto result = model->run (image);
  auto result2 = model->run (imgx);

  printf ("Imaage heightXweidth = %dX%d\n", result.height, result.width);
  Mat featnew = result.feat;

  printf ("reid img HXw = %dX%d\n", featnew.size ().height,
      featnew.size ().width);

  for (int i = 0; i < featnew.dims; ++i) {
    if (i)
      std::cout << " X ";
    std::cout << featnew.size[i];
  }
  std::cout << "] temp.channels = " << featnew.channels () << std::endl;

  double dismat = (1 - featnew.dot (result2.feat));
  printf ("dismat : %.3lf \n", dismat);


  return true;
}

int
ivas_xreid::requiredwidth (void)
{
  LOG_MESSAGE (LOG_LEVEL_DEBUG, log_level, "enter");
  return model->getInputWidth ();
}

int
ivas_xreid::requiredheight (void)
{
  LOG_MESSAGE (LOG_LEVEL_DEBUG, log_level, "enter");
  return model->getInputHeight ();
}

int
ivas_xreid::close (void)
{
  LOG_MESSAGE (LOG_LEVEL_DEBUG, log_level, "enter");
  return true;
}

ivas_xreid::~ivas_xreid ()
{
  LOG_MESSAGE (LOG_LEVEL_DEBUG, log_level, "enter");
}
