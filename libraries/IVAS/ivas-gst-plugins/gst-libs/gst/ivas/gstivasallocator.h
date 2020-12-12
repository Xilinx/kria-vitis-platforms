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

#ifndef __GST_IVAS_ALLOCATOR_H__
#define __GST_IVAS_ALLOCATOR_H__

#include <gst/gst.h>
#include <gst/video/video.h>
#ifdef XLNX_PCIe_PLATFORM
#include <xrt.h>
#else
#include <xrt/xrt.h>
#endif

G_BEGIN_DECLS

#define GST_TYPE_IVAS_ALLOCATOR  \
   (gst_ivas_allocator_get_type())
#define GST_IS_IVAS_ALLOCATOR(obj)       \
   (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_IVAS_ALLOCATOR))
#define GST_IS_IVAS_ALLOCATOR_CLASS(klass)     \
   (G_TYPE_CHECK_CLASS_TYPE ((klass), GST_TYPE_IVAS_ALLOCATOR))
#define GST_IVAS_ALLOCATOR_GET_CLASS(obj)      \
   (G_TYPE_INSTANCE_GET_CLASS ((obj), GST_TYPE_IVAS_ALLOCATOR, GstIvasAllocatorClass))
#define GST_IVAS_ALLOCATOR(obj)        \
   (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_IVAS_ALLOCATOR, GstIvasAllocator))
#define GST_IVAS_ALLOCATOR_CLASS(klass)      \
   (G_TYPE_CHECK_CLASS_CAST ((klass), GST_TYPE_IVAS_ALLOCATOR, GstIvasAllocatorClass))

typedef struct _GstIvasAllocator GstIvasAllocator;
typedef struct _GstIvasAllocatorClass GstIvasAllocatorClass;
typedef struct _GstIvasAllocatorPrivate GstIvasAllocatorPrivate;

struct _GstIvasAllocator
{
  GstAllocator parent;
  GstIvasAllocatorPrivate *priv;
};

struct _GstIvasAllocatorClass {
  GstAllocatorClass parent_class;
};

GST_EXPORT
GType gst_ivas_allocator_get_type (void) G_GNUC_CONST;
GST_EXPORT
GstAllocator* gst_ivas_allocator_new (guint dev_idx, gboolean need_dma);
GST_EXPORT
gboolean gst_is_ivas_memory (GstMemory *mem);
GST_EXPORT
guint64  gst_ivas_allocator_get_paddr (GstMemory *mem);
GST_EXPORT
guint  gst_ivas_allocator_get_bo (GstMemory *mem);
GST_EXPORT
gboolean gst_ivas_memory_can_avoid_copy (GstMemory *mem, guint cur_devid);
GST_EXPORT
guint gst_ivas_allocator_get_device_idx (GstAllocator * allocator);

#ifdef XLNX_PCIe_PLATFORM
typedef enum {
  IVAS_SYNC_NONE = 0,
  IVAS_SYNC_TO_DEVICE = 1 << 0, /* sync data to device using DMA transfer */
  IVAS_SYNC_FROM_DEVICE = 1 << 1, /* sync data to device using DMA transfer */
} IvasSyncFlags;

GST_EXPORT
void gst_ivas_memory_set_sync_flag (GstMemory *mem, IvasSyncFlags flag);
GST_EXPORT
gboolean gst_ivas_memory_sync_bo (GstMemory *mem);
GST_EXPORT
gboolean gst_ivas_memory_sync_with_flags (GstMemory *mem, GstMapFlags flags);
#endif

G_END_DECLS

#endif /* __GST_IVAS_ALLOCATOR_H__ */
