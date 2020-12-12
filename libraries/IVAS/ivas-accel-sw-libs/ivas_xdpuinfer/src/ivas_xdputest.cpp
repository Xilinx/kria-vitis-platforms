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

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <fstream>

#include <ivas/ivas_kernel.h>
#include <ivas/ivaslogs.h>
#include <gst/ivas/gstivasinpinfer.h>

using namespace cv;
using namespace std;

struct testkpriv
{
  int modelclass;               /* Class of model, from Json file */
  int modelnum;                 /* map class to number ivas_xmodelclass[] */
  int log_level;                /* LOG_LEVEL_ERROR=0, LOG_LEVEL_WARNING=1,
                                   LOG_LEVEL_INFO=2, LOG_LEVEL_DEBUG=3 */
    std::string modelname;      /* contain name of model from json */
};
typedef struct testkpriv testkpriv;

static const char *ivas_xmodelclass[IVAS_XCLASS_NOTFOUND + 1] = {
  [IVAS_XCLASS_YOLOV3] = "YOLOV3",
  [IVAS_XCLASS_FACEDETECT] = "FACEDETECT",
  [IVAS_XCLASS_CLASSIFICATION] = "CLASSIFICATION",
  [IVAS_XCLASS_SSD] = "SSD",
  [IVAS_XCLASS_REID] = "REID",
  [IVAS_XCLASS_REFINEDET] = "REFINEDET",
  [IVAS_XCLASS_TFSSD] = "TFSSD",
  [IVAS_XCLASS_YOLOV2] = "YOLOV2",

  /* Add model above this */
  [IVAS_XCLASS_NOTFOUND] = ""
};

int
ivas_xclass_to_num (char *name)
{
  int nameslen = 0;
  while (ivas_xmodelclass[nameslen] != NULL) {
    if (!strcmp (ivas_xmodelclass[nameslen], name))
      return nameslen;
    nameslen++;
  }
  return IVAS_XCLASS_NOTFOUND;
}

extern "C"
{

  int32_t xlnx_kernel_init (IVASKernel * handle)
  {
    testkpriv *kpriv = (testkpriv *) calloc (1, sizeof (testkpriv));

    json_t *jconfig = handle->kernel_config;
    json_t *val;                /* kernel config from app */

    /* parse config */

      val = json_object_get (jconfig, "debug_level");
    if (!val || !json_is_integer (val))
        kpriv->log_level = LOG_LEVEL_WARNING;
    else
        kpriv->log_level = json_integer_value (val);
      LOG_MESSAGE (LOG_LEVEL_DEBUG, kpriv->log_level, "enter");

      val = json_object_get (jconfig, "model-class");
    if (!json_is_string (val))
    {
      LOG_MESSAGE (LOG_LEVEL_ERROR, kpriv->log_level,
          "model-class is not proper\n");
      goto err;
    }
    kpriv->modelclass =
        (int) ivas_xclass_to_num ((char *) json_string_value (val));
    if (kpriv->modelclass == IVAS_XCLASS_NOTFOUND) {
      LOG_MESSAGE (LOG_LEVEL_ERROR, kpriv->log_level,
          "SORRY NOT SUPPORTED MODEL CLASS %s",
          (char *) json_string_value (val));
      goto err;
    }

    val = json_object_get (jconfig, "model-name");
    if (!json_is_string (val)) {
      LOG_MESSAGE (LOG_LEVEL_ERROR, kpriv->log_level,
          "model-name is not proper\n");
      goto err;
    }
    kpriv->modelname = (char *) json_string_value (val);

    handle->kernel_priv = (void *) kpriv;
    return true;

  err:
    free (kpriv);
    return -1;
  }

  uint32_t xlnx_kernel_deinit (IVASKernel * handle)
  {
    testkpriv *kpriv = (testkpriv *) handle->kernel_priv;
    if (!kpriv)
      return true;
    LOG_MESSAGE (LOG_LEVEL_DEBUG, kpriv->log_level, "enter");

    free (kpriv);

    return true;
  }

  uint32_t xlnx_kernel_start (IVASKernel * handle, int start,
      IVASFrame * input[MAX_NUM_OBJECT], IVASFrame * output[MAX_NUM_OBJECT])
  {
    static int frame = 0;
    testkpriv *kpriv = (testkpriv *) handle->kernel_priv;
    GstIvasInpInferMeta *ivas_inputmeta = NULL;
    IVASFrame *inframe = input[0];

    LOG_MESSAGE (LOG_LEVEL_DEBUG, kpriv->log_level, "enter");

    if (frame++ % 2) {
      ivas_inputmeta =
          gst_buffer_add_ivas_inp_infer_meta ((GstBuffer *) inframe->app_priv,
          (IvasClass) kpriv->modelclass, (gchar *) kpriv->modelname.c_str ());
    } else {
      ivas_inputmeta =
          gst_buffer_add_ivas_inp_infer_meta ((GstBuffer *) inframe->app_priv,
          (IvasClass) 2, (gchar *) "resnet50");
    }
    if (ivas_inputmeta == NULL) {
      LOG_MESSAGE (LOG_LEVEL_ERROR, kpriv->log_level,
          "ivas meta data is not available for dpu");
      return -1;
    } else {
      LOG_MESSAGE (LOG_LEVEL_DEBUG, kpriv->log_level, "ivas_mata ptr %p",
          ivas_inputmeta);
    }

    return true;
  }

  int32_t xlnx_kernel_done (IVASKernel * handle)
  {

    testkpriv *kpriv = (testkpriv *) handle->kernel_priv;
    LOG_MESSAGE (LOG_LEVEL_DEBUG, kpriv->log_level, "enter");
    return true;
  }

}
