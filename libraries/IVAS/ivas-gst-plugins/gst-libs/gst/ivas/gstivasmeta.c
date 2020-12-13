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

#include <gstivasmeta.h>
#include <stdio.h>

GType
gst_ivas_meta_api_get_type (void)
{
  static volatile GType type = 0;
  static const gchar *tags[] =
      { GST_META_TAG_VIDEO_STR, GST_META_TAG_VIDEO_SIZE_STR,
    GST_META_TAG_VIDEO_ORIENTATION_STR, NULL
  };

  if (g_once_init_enter (&type)) {
    GType _type = gst_meta_api_type_register ("GstIvasMetaAPI", tags);
    g_once_init_leave (&type, _type);
  }

  return type;
}

static gboolean
gst_ivas_meta_init (GstMeta * meta, gpointer params, GstBuffer * buffer)
{
  GstIvasMeta *ivasmeta = (GstIvasMeta *) meta;

  ivasmeta->xmeta.objects = NULL;
  ivasmeta->xmeta.display_meta = NULL;
  ivasmeta->xmeta.pts = -1;

  return TRUE;
}

static void
ivas_free_object (gpointer data)
{
  IvasObjectMetadata *obj = data;

  if (obj->obj_list) {
    g_list_free_full (g_steal_pointer (&obj->obj_list),
        (GDestroyNotify) gst_buffer_unref);
  }

  g_free (data);
}

static gboolean
gst_ivas_meta_free (GstMeta * meta, GstBuffer * buffer)
{
  GstIvasMeta *ivasmeta = (GstIvasMeta *) meta;

  GST_LOG ("freeing ivas meta %p with buffer %p", ivasmeta, buffer);

  if (ivasmeta->xmeta.objects) {
    g_list_free_full (g_steal_pointer (&ivasmeta->xmeta.objects),
        ivas_free_object);
    ivasmeta->xmeta.objects = NULL;
  }

  if (ivasmeta->xmeta.display_meta) {
    g_list_free_full (ivasmeta->xmeta.display_meta, g_free);
    ivasmeta->xmeta.display_meta = NULL;
  }

  return TRUE;
}

static gboolean
gst_ivas_meta_transform (GstBuffer * dest, GstMeta * meta,
    GstBuffer * buffer, GQuark type, gpointer data)
{
  GstIvasMeta *dmeta, *smeta;
  GList *node;

  if (GST_META_TRANSFORM_IS_COPY (type)) {
    IvasObjectMetadata *sobj = NULL;

    smeta = (GstIvasMeta *) meta;
    dmeta = gst_buffer_add_ivas_meta (dest);

    if (!dmeta)
      return FALSE;

    GST_LOG ("copy metadata from %p -> %p buffer %p -> %p", smeta, dmeta,
        buffer, dest);
    node = smeta->xmeta.objects;

    while (node) {
      GList *src_lbuf = NULL;

      IvasObjectMetadata *dobj =
          (IvasObjectMetadata *) calloc (1, sizeof (IvasObjectMetadata));

      if (dobj == NULL) {
        GST_ERROR ("Failed to allocate meta obj data buffer");
        return FALSE;
      }

      sobj = (IvasObjectMetadata *) node->data;
      GST_LOG ("copying metaobject %p->%p", sobj, dobj);

      memcpy (dobj, sobj, sizeof (IvasObjectMetadata));
      dobj->obj_list = NULL;

      src_lbuf = sobj->obj_list;
      while (src_lbuf) {
        GstBuffer *obj_buf;

        obj_buf = gst_buffer_ref (GST_BUFFER (src_lbuf->data));
        dobj->obj_list = g_list_append (dobj->obj_list, obj_buf);
        src_lbuf = g_list_next (src_lbuf);
      }
      dmeta->xmeta.objects = g_list_append (dmeta->xmeta.objects, dobj);
      node = g_list_next (node);
    }
  } else if (GST_VIDEO_META_TRANSFORM_IS_SCALE (type)) {
    GstVideoMetaTransform *trans = data;
    IvasObjectMetadata *sobj = NULL;
    float x_scale_f;
    float y_scale_f;

    smeta = (GstIvasMeta *) meta;
    dmeta = gst_buffer_add_ivas_meta (dest);

    if (!dmeta)
      return FALSE;

    node = smeta->xmeta.objects;

    x_scale_f =
        (float) GST_VIDEO_INFO_WIDTH (trans->out_info) /
        GST_VIDEO_INFO_WIDTH (trans->in_info);
    y_scale_f =
        (float) GST_VIDEO_INFO_HEIGHT (trans->out_info) /
        GST_VIDEO_INFO_HEIGHT (trans->in_info);

    while (node) {
      IvasObjectMetadata *dobj =
          (IvasObjectMetadata *) calloc (1, sizeof (IvasObjectMetadata));

      if (dobj == NULL) {
        GST_ERROR ("Failed to allocate meta obj data buffer");
        return FALSE;
      }

      sobj = (IvasObjectMetadata *) node->data;

      dobj->bbox_meta.xmin = sobj->bbox_meta.xmin * x_scale_f;
      dobj->bbox_meta.ymin = sobj->bbox_meta.ymin * y_scale_f;
      dobj->bbox_meta.xmax = sobj->bbox_meta.xmax * x_scale_f;
      dobj->bbox_meta.ymax = sobj->bbox_meta.ymax * y_scale_f;

      GST_LOG ("source metadata : (%f, %f) -> (%f, %f)", sobj->bbox_meta.xmin,
          sobj->bbox_meta.ymin, sobj->bbox_meta.xmax, sobj->bbox_meta.ymax);
      GST_LOG ("destination metadata : (%f, %f) -> (%f, %f)",
          dobj->bbox_meta.xmin, dobj->bbox_meta.ymin, dobj->bbox_meta.xmax,
          dobj->bbox_meta.ymax);

      dmeta->xmeta.objects = g_list_append (dmeta->xmeta.objects, dobj);
      node = g_list_next (node);
    }
  } else {
    return FALSE;
  }

  return TRUE;
}

const GstMetaInfo *
gst_ivas_meta_get_info (void)
{
  static const GstMetaInfo *ivas_meta_info = NULL;

  if (g_once_init_enter ((GstMetaInfo **) & ivas_meta_info)) {
    const GstMetaInfo *meta =
        gst_meta_register (GST_IVAS_META_API_TYPE, "GstIvasMeta",
        sizeof (GstIvasMeta), (GstMetaInitFunction) gst_ivas_meta_init,
        (GstMetaFreeFunction) gst_ivas_meta_free, gst_ivas_meta_transform);
    g_once_init_leave ((GstMetaInfo **) & ivas_meta_info, (GstMetaInfo *) meta);
  }
  return ivas_meta_info;
}
