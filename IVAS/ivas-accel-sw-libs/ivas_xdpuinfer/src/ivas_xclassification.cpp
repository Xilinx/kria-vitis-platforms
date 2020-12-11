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

#include "ivas_xclassification.hpp"


ivas_xclassification::ivas_xclassification (ivas_xkpriv * kpriv,
    const std::string & model_name, bool need_preprocess)
{
  log_level = kpriv->log_level;
  LOG_MESSAGE (LOG_LEVEL_DEBUG, kpriv->log_level, "enter");
  model = vitis::ai::Classification::create (model_name, need_preprocess);
}

int
ivas_xclassification::run (ivas_xkpriv * kpriv, const cv::Mat & image,
    GstInferenceMeta * infer_meta)
{

  LOG_MESSAGE (LOG_LEVEL_DEBUG, kpriv->log_level, "enter");
  auto result = model->run (image);

  int cols = image.cols;
  int rows = image.rows;
  int i;
  char *pstr;                   /* prediction string */


  if (NULL == infer_meta->prediction) {
    infer_meta->prediction = gst_inference_prediction_new ();
    infer_meta->prediction->bbox.width = cols;
    infer_meta->prediction->bbox.height = rows;
  } else {
    infer_meta->prediction->bbox.width = cols;
    infer_meta->prediction->bbox.height = rows;
  }
  LOG_MESSAGE (LOG_LEVEL_DEBUG, kpriv->log_level,
      " IN width %d, height %d infer ptr %p", cols, rows, infer_meta);
  LOG_MESSAGE (LOG_LEVEL_DEBUG, kpriv->log_level, "root prediction ptr %p",
      infer_meta->prediction);

for (auto & r:result.scores) {
    i++;

    BoundingBox bbox;
    GstInferencePrediction *predict;
    GstInferenceClassification *c = NULL;

    bbox.x = 0;
    bbox.y = 0;
    bbox.width = 0;
    bbox.height = 0;

    predict = gst_inference_prediction_new_full (&bbox);

    c = gst_inference_classification_new_full (-1, r.score,
        result.lookup (r.index), 0, NULL, NULL, NULL);
    gst_inference_prediction_append_classification (predict, c);

    gst_inference_prediction_append (infer_meta->prediction, predict);

    LOG_MESSAGE (LOG_LEVEL_INFO, kpriv->log_level,
        " r.index %d %s, r.score, %f", r.index,
        result.lookup (r.index), r.score);
  }
  pstr = gst_inference_prediction_to_string (infer_meta->prediction);
  LOG_MESSAGE (LOG_LEVEL_DEBUG, kpriv->log_level, "prediction tree : \n%s",
      pstr);
  free(pstr);
  LOG_MESSAGE (LOG_LEVEL_INFO, kpriv->log_level, " ");
  return true;
}

int
ivas_xclassification::requiredwidth (void)
{
  LOG_MESSAGE (LOG_LEVEL_DEBUG, log_level, "enter");
  return model->getInputWidth ();
}

int
ivas_xclassification::requiredheight (void)
{
  LOG_MESSAGE (LOG_LEVEL_DEBUG, log_level, "enter");
  return model->getInputHeight ();
}

int
ivas_xclassification::close (void)
{
  LOG_MESSAGE (LOG_LEVEL_DEBUG, log_level, "enter");
  return true;
}

ivas_xclassification::~ivas_xclassification ()
{
  LOG_MESSAGE (LOG_LEVEL_DEBUG, log_level, "enter");
}
