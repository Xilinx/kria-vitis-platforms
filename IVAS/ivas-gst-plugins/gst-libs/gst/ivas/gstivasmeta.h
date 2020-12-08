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

#ifndef __GST_IVAS_META_H__
#define __GST_IVAS_META_H__

#include <gst/gst.h>
#include <gst/video/video.h>
#include <string.h>
#include <ivas/ivasmeta.h>

G_BEGIN_DECLS

#define GST_IVAS_META_API_TYPE  (gst_ivas_meta_api_get_type())
#define GST_IVAS_META_INFO  (gst_ivas_meta_get_info())

typedef struct _ivas_meta {
  /* List of IvasObjectMetadata objects */
  GList *objects;
  GList *display_meta;
  uint64_t pts;
} IvasMeta;

typedef struct _GstIvasMeta GstIvasMeta;

struct _GstIvasMeta {
  GstMeta meta;
  IvasMeta xmeta;
};

GST_EXPORT
GType gst_ivas_meta_api_get_type (void);

GST_EXPORT
const GstMetaInfo * gst_ivas_meta_get_info (void);

#define gst_buffer_get_ivas_meta(b) ((GstIvasMeta*)gst_buffer_get_meta((b),GST_IVAS_META_API_TYPE))
#define gst_buffer_add_ivas_meta(b) ((GstIvasMeta*)gst_buffer_add_meta((b),GST_IVAS_META_INFO, NULL))

G_END_DECLS

#endif /* __GST_IVAS_META_H__ */
