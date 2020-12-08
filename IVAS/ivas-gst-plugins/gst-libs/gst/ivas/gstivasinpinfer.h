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

#ifndef __GST_IVAS_INP_INFER_META_H__
#define __GST_IVAS_INP_INFER_META_H__

#include <gst/gst.h>
#include <gst/video/video.h>
#include <string.h>

G_BEGIN_DECLS

typedef enum
{
  IVAS_XCLASS_YOLOV3,
  IVAS_XCLASS_FACEDETECT,
  IVAS_XCLASS_CLASSIFICATION,
  IVAS_XCLASS_SSD,
  IVAS_XCLASS_REID,
  IVAS_XCLASS_REFINEDET,
  IVAS_XCLASS_TFSSD,
  IVAS_XCLASS_YOLOV2,

  IVAS_XCLASS_NOTFOUND
}IvasClass;

typedef struct _GstIvasInpInferMeta GstIvasInpInferMeta;

struct _GstIvasInpInferMeta {
  GstMeta meta;
  IvasClass ml_class;
  gchar *model_name;
};

GST_EXPORT
GstIvasInpInferMeta *
gst_buffer_add_ivas_inp_infer_meta (GstBuffer *buffer, IvasClass ml_class, gchar *model_name);

GType gst_ivas_inp_infer_meta_api_get_type (void);
#define GST_IVAS_INP_INFER_META_API_TYPE (gst_ivas_inp_infer_meta_api_get_type())

GST_EXPORT
const GstMetaInfo *gst_ivas_inp_infer_meta_get_info (void);
#define GST_IVAS_INP_INFER_EXAMPLE_META_INFO ((gst_ivas_inp_infer_meta_get_info()))

#define gst_buffer_get_ivas_inp_infer_meta(b) ((GstIvasInpInferMeta*)gst_buffer_get_meta((b),GST_IVAS_INP_INFER_META_API_TYPE))

G_END_DECLS
#endif /* __GST_IVAS_INP_INFER_META_H__  */
