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

#include "ivas_xrefinedet.hpp"

ivas_xrefinedet::ivas_xrefinedet (ivas_xkpriv * kpriv,
    const std::string & model_name, bool need_preprocess)
{
  log_level = kpriv->log_level;
  LOG_MESSAGE (LOG_LEVEL_DEBUG, kpriv->log_level, "enter");

  model = vitis::ai::RefineDet::create (model_name, need_preprocess);
}

int
ivas_xrefinedet::run (ivas_xkpriv * kpriv, const cv::Mat & image,
    GstInferenceMeta * infer_meta)
{

  LOG_MESSAGE (LOG_LEVEL_DEBUG, kpriv->log_level, "enter");
  auto result = model->run (image);

  int cols = image.cols;
  int rows = image.rows;
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

for (auto & box:result.bboxes) {
    float xmin = box.x * cols + 1;
    float ymin = box.y * rows + 1;
    float xmax = xmin + box.width * cols;
    float ymax = ymin + box.height * rows;
    if (xmin < 0.)
      xmin = 1.;
    if (ymin < 0.)
      ymin = 1.;
    if (xmax > cols)
      xmax = cols;
    if (ymax > rows)
      ymax = rows;
    float confidence = box.score;

    BoundingBox bbox;
    GstInferencePrediction *predict;
    GstInferenceClassification *c = NULL;

    bbox.x = xmin;
    bbox.y = ymin;
    bbox.width = xmax - xmin;
    bbox.height = ymax - ymin;

    predict = gst_inference_prediction_new_full (&bbox);

    c = gst_inference_classification_new_full (-1, confidence, NULL, 0, NULL,
        NULL, NULL);
    gst_inference_prediction_append_classification (predict, c);

    gst_inference_prediction_append (infer_meta->prediction, predict);

    LOG_MESSAGE (LOG_LEVEL_INFO, kpriv->log_level,
        "RESULT: %f %f %f %f (%f)", xmin, ymin, xmax, ymax, confidence);
  }
  pstr = gst_inference_prediction_to_string (infer_meta->prediction);
  LOG_MESSAGE (LOG_LEVEL_DEBUG, kpriv->log_level, "prediction tree : \n%s",
      pstr);
  free(pstr);
  LOG_MESSAGE (LOG_LEVEL_INFO, kpriv->log_level, " ");

  return true;

}


int
ivas_xrefinedet::requiredwidth (void)
{
  LOG_MESSAGE (LOG_LEVEL_DEBUG, log_level, "enter");
  return model->getInputWidth ();
}

int
ivas_xrefinedet::requiredheight (void)
{
  LOG_MESSAGE (LOG_LEVEL_DEBUG, log_level, "enter");
  return model->getInputHeight ();
}

int
ivas_xrefinedet::close (void)
{
  LOG_MESSAGE (LOG_LEVEL_DEBUG, log_level, "enter");
  return true;
}

ivas_xrefinedet::~ivas_xrefinedet ()
{
  LOG_MESSAGE (LOG_LEVEL_DEBUG, log_level, "enter");
}
