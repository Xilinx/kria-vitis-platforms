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

#include <gstivasinpinfer.h>


GType
gst_ivas_inp_infer_meta_api_get_type (void)
{
  static volatile GType type = 0;
  static const gchar *tags[] =
      { GST_META_TAG_VIDEO_STR, GST_META_TAG_VIDEO_SIZE_STR,
    GST_META_TAG_VIDEO_ORIENTATION_STR, NULL
  };

  if (g_once_init_enter (&type)) {
    GType _type = gst_meta_api_type_register ("GstIvasInpInferMetaAPI", tags);
    g_once_init_leave (&type, _type);
  }

  return type;
}

static gboolean
gst_ivas_inp_infer_meta_init (GstMeta * meta, gpointer params, GstBuffer * buffer)
{
  GstIvasInpInferMeta *ivasmeta = (GstIvasInpInferMeta *) meta;

  ivasmeta->ml_class = IVAS_XCLASS_NOTFOUND;
  ivasmeta->model_name = NULL;

  return TRUE;
}


static gboolean
gst_ivas_inp_infer_meta_free (GstMeta * meta, GstBuffer * buffer)
{
  GstIvasInpInferMeta *ivasmeta = (GstIvasInpInferMeta *) meta;

  if(ivasmeta->model_name) {
    g_free (ivasmeta->model_name);
    ivasmeta->model_name = NULL;
  }
  
  return TRUE;
}


static gboolean
gst_ivas_inp_infer_meta_transform (GstBuffer * dest, GstMeta * meta,
    GstBuffer * buffer, GQuark type, gpointer data)
{
  GstIvasInpInferMeta *smeta;

  smeta = (GstIvasInpInferMeta *) meta;
  gst_buffer_add_ivas_inp_infer_meta (dest, smeta->ml_class, smeta->model_name);
  
  return TRUE;
}

const GstMetaInfo *
gst_ivas_inp_infer_meta_get_info (void)
{
  static const GstMetaInfo *ivas_inp_infer_meta_info = NULL;

  if (g_once_init_enter ((GstMetaInfo **) & ivas_inp_infer_meta_info)) {
    const GstMetaInfo *meta =
        gst_meta_register (GST_IVAS_INP_INFER_META_API_TYPE, "GstIvasInpInferMeta",
        sizeof (GstIvasInpInferMeta), 
        (GstMetaInitFunction) gst_ivas_inp_infer_meta_init,
        (GstMetaFreeFunction) gst_ivas_inp_infer_meta_free,
        gst_ivas_inp_infer_meta_transform);
    g_once_init_leave ((GstMetaInfo **) & ivas_inp_infer_meta_info, (GstMetaInfo *) meta);
  }
  return ivas_inp_infer_meta_info;
}

GstIvasInpInferMeta *
gst_buffer_add_ivas_inp_infer_meta (GstBuffer *buffer, IvasClass ml_class, gchar *model_name)
{
  GstIvasInpInferMeta *meta;
  g_return_val_if_fail (GST_IS_BUFFER (buffer), NULL);

  meta = (GstIvasInpInferMeta *) gst_buffer_add_meta (buffer,
  GST_IVAS_INP_INFER_EXAMPLE_META_INFO, NULL);

  meta->ml_class = ml_class;
  meta->model_name = g_strdup (model_name);

  return meta;
}
