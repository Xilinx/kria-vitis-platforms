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

#ifndef __GST_IVAS_BUFFER_POOL_H__
#define __GST_IVAS_BUFFER_POOL_H__

#include <gst/gst.h>
#include <gst/video/video.h>

G_BEGIN_DECLS

typedef struct _GstIvasBufferPool GstIvasBufferPool;
typedef struct _GstIvasBufferPoolClass GstIvasBufferPoolClass;
typedef struct _GstIvasBufferPoolPrivate GstIvasBufferPoolPrivate;

#define GST_TYPE_IVAS_BUFFER_POOL \
  (gst_ivas_buffer_pool_get_type())
#define GST_IS_IVAS_BUFFER_POOL(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_IVAS_BUFFER_POOL))
#define GST_IVAS_BUFFER_POOL(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_IVAS_BUFFER_POOL, GstIvasBufferPool))
#define GST_IVAS_BUFFER_POOL_CAST(obj) \
  ((GstIvasBufferPool*)(obj))

typedef void (*ReleaseBufferCallback)(GstBuffer *buf, gpointer user_data);

struct _GstIvasBufferPool
{
  GstVideoBufferPool parent;
  GstIvasBufferPoolPrivate *priv;
  ReleaseBufferCallback release_buf_cb;
  gpointer cb_user_data; /* release callback user data */
};

struct _GstIvasBufferPoolClass
{
  GstVideoBufferPoolClass parent_class;
};

GST_EXPORT
GType gst_ivas_buffer_pool_get_type (void) G_GNUC_CONST;

GST_EXPORT
GstBufferPool *gst_ivas_buffer_pool_new (guint stride_align, guint elevation_align);

GST_EXPORT
void gst_ivas_buffer_pool_set_release_buffer_cb (GstIvasBufferPool *xpool, ReleaseBufferCallback release_buf_cb, gpointer user_data);

G_END_DECLS

#endif /* __GST_IVAS_BUFFER_POOL_H__ */
