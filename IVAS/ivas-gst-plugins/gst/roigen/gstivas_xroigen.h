/*
 * Copyright (C) 2020 - 2021 Xilinx, Inc.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software
 * is furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
 * KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO
 * EVENT SHALL XILINX BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
 * OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. Except as contained in this notice, the name of the Xilinx shall
 * not be used in advertising or otherwise to promote the sale, use or other
 * dealings in this Software without prior written authorization from Xilinx.
 */

#ifndef _GST_IVAS_XROIGEN_H_
#define _GST_IVAS_XROIGEN_H_

#include <gst/base/gstbasetransform.h>

G_BEGIN_DECLS

#define GST_TYPE_IVAS_XROIGEN   (gst_ivas_xroigen_get_type())
#define GST_IVAS_XROIGEN(obj)   (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_IVAS_XROIGEN,GstIvas_XROIGen))
#define GST_IVAS_XROIGEN_CLASS(klass)   (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_IVAS_XROIGEN,GstIvas_XROIGenClass))
#define GST_IS_IVAS_XROIGEN(obj)   (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_IVAS_XROIGEN))
#define GST_IS_IVAS_XROIGEN_CLASS(obj)   (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_IVAS_XROIGEN))

typedef struct _GstIvas_XROIGen GstIvas_XROIGen;
typedef struct _GstIvas_XROIGenClass GstIvas_XROIGenClass;

struct _GstIvas_XROIGen
{
  GstBaseTransform parent;
  gint qp_level;
  gint roi_type;
  gint qp_delta;
  guint max_num;
  gboolean insert_roi_sei;
  gboolean need_dummy_roi;
  gint min_width;
  gint min_height;
  gint max_width;
  gint max_height;
  GSList *class_list;
};

struct _GstIvas_XROIGenClass
{
  GstBaseTransformClass parentclass;
};

GType gst_ivas_xroigen_get_type (void);

G_END_DECLS

#endif
