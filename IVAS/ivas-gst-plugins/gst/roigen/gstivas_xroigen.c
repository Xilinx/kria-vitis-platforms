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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>
#include <gst/video/video.h>
#include <string.h>
#include "gstivas_xroigen.h"
#include <gst/ivas/gstivasmeta.h>
#include <gst/ivas/gstinferencemeta.h>

GST_DEBUG_CATEGORY_STATIC (gst_ivas_xroigen_debug_category);
#define GST_CAT_DEFAULT gst_ivas_xroigen_debug_category

#define gst_ivas_xroigen_parent_class parent_class

static GstFlowReturn gst_ivas_xroigen_transform_ip (GstBaseTransform * trans,
    GstBuffer * buf);
const gchar *ivas_xroigen_get_qp_level_nickname (gint roi_qp_level);
static void gst_ivas_xroigen_finalize (GObject * gobject);

enum
{
  PROP_0,
  PROP_ROI_TYPE,
  PROP_ROI_QP_DELTA,
  PROP_ROI_QP_LEVEL,
  PROP_ROI_MAX_NUM,
  PROP_ROI_RESOLUTION_RANGE,
  PROP_ROI_CLASS_FILTER,
  PROP_INSERT_SEI
};

G_DEFINE_TYPE_WITH_CODE (GstIvas_XROIGen, gst_ivas_xroigen,
    GST_TYPE_BASE_TRANSFORM,
    GST_DEBUG_CATEGORY_INIT (gst_ivas_xroigen_debug_category, "ivas_xroigen", 0,
        "debug category for IVAS roigen element"));

enum
{
  MIN_RELATIVE_QP = -32,
  MAX_RELATIVE_QP = 31,
};

typedef enum
{
  IVAS_XROIGEN_ROI_TYPE_DEFAULT,
  IVAS_XROIGEN_ROI_TYPE_QP_LEVEL,
  IVAS_XROIGEN_ROI_TYPE_QP_DELTA
} GstIvasXRoiGenRoiType;

#define GST_TYPE_IVAS_XROIGEN_ROI_TYPE (gst_ivas_xroigen_roi_type ())
static GType
gst_ivas_xroigen_roi_type (void)
{
  static const GEnumValue values[] = {
    {IVAS_XROIGEN_ROI_TYPE_DEFAULT, "roi without enc QP info", "default"},
    {IVAS_XROIGEN_ROI_TYPE_QP_LEVEL, "roi enc QP level", "qp_level"},
    {IVAS_XROIGEN_ROI_TYPE_QP_DELTA, "roi enc QP delta", "qp_delta"},
    {0, NULL, NULL}
  };
  static volatile GType id = 0;

  if (g_once_init_enter ((gsize *) & id)) {
    GType _id;

    _id = g_enum_register_static ("GstIvasXRoiGenRoiType", values);

    g_once_init_leave ((gsize *) & id, _id);
  }

  return id;
}

typedef enum
{
  IVAS_XROIGEN_ROI_QUALITY_HIGH,
  IVAS_XROIGEN_ROI_QUALITY_MEDIUM,
  IVAS_XROIGEN_ROI_QUALITY_LOW,
  IVAS_XROIGEN_ROI_QUALITY_DONT_CARE,
  IVAS_XROIGEN_ROI_QUALITY_INTRA
} GstIvasXRoiGenRoiQuality;

#define GST_TYPE_IVAS_XROIGEN_ROI_QUALITY (gst_ivas_xroigen_roi_quality_type ())
static GType
gst_ivas_xroigen_roi_quality_type (void)
{
  static const GEnumValue values[] = {
    {IVAS_XROIGEN_ROI_QUALITY_HIGH, "Delta QP of -5", "high"},
    {IVAS_XROIGEN_ROI_QUALITY_MEDIUM, "Delta QP of 0", "medium"},
    {IVAS_XROIGEN_ROI_QUALITY_LOW, "Delta QP of +5", "low"},
    {IVAS_XROIGEN_ROI_QUALITY_DONT_CARE, "Maximum delta QP value", "dont-care"},
    {IVAS_XROIGEN_ROI_QUALITY_INTRA,
        "Region all LCU encoded with intra prediction mode", "intra"},
    {0, NULL, NULL}
  };
  static volatile GType id = 0;

  if (g_once_init_enter ((gsize *) & id)) {
    GType _id;

    _id = g_enum_register_static ("GstIvasXRoiGenRoiQuality", values);

    g_once_init_leave ((gsize *) & id, _id);
  }

  return id;
}

#define GSTIVAS_XROIGEN_DEFAULT_ROI_TYPE IVAS_XROIGEN_ROI_TYPE_DEFAULT
#define GSTIVAS_XROIGEN_DEFAULT_QP_LEVEL IVAS_XROIGEN_ROI_QUALITY_HIGH
#define GSTIVAS_XROIGEN_DEFAULT_QP_DELTA 0
#define GSTIVAS_XROIGEN_DEFAULT_MAX_NUM G_MAXUINT

typedef struct
{
  guint x;
  guint y;
  guint w;
  guint h;
} IvasROIInfo;

/* <timestamp in uint64> + <num rois in uint> */
#define IVAS_ROI_SEI_EXTRA_INFO_SIZE (sizeof(guint64)+sizeof (guint))

const gchar *
ivas_xroigen_get_qp_level_nickname (gint roi_qp_level)
{
  switch (roi_qp_level) {
    case IVAS_XROIGEN_ROI_QUALITY_HIGH:
      return "high";
    case IVAS_XROIGEN_ROI_QUALITY_MEDIUM:
      return "medium";
    case IVAS_XROIGEN_ROI_QUALITY_LOW:
      return "low";
    case IVAS_XROIGEN_ROI_QUALITY_DONT_CARE:
      return "dont-care";
    case IVAS_XROIGEN_ROI_QUALITY_INTRA:
      return "intra";
    default:
      return "invalid";
  }
}

static void
gst_ivas_xroigen_set_property (GObject * object,
    guint prop_id, const GValue * value, GParamSpec * pspec)
{
  GstIvas_XROIGen *roigen = GST_IVAS_XROIGEN (object);

  switch (prop_id) {
    case PROP_ROI_TYPE:
      roigen->roi_type = g_value_get_enum (value);
      break;
    case PROP_ROI_QP_DELTA:
      roigen->qp_delta = g_value_get_int (value);
      break;
    case PROP_ROI_QP_LEVEL:
      roigen->qp_level = g_value_get_enum (value);
      break;
    case PROP_ROI_MAX_NUM:
      roigen->max_num = g_value_get_uint (value);
      break;
    case PROP_INSERT_SEI:
      roigen->insert_roi_sei = g_value_get_boolean (value);
      break;
    case PROP_ROI_RESOLUTION_RANGE:{
      const GValue *v;

      if (gst_value_array_get_size (value) != 4) {
        g_warning ("resolution-range property not set correctly");
        break;
      }

      v = gst_value_array_get_value (value, 0);
      if (!G_VALUE_HOLDS_INT (v)) {
        g_warning ("wrong min-width value");
        break;
      }
      roigen->min_width = g_value_get_int (v);

      v = gst_value_array_get_value (value, 1);
      if (!G_VALUE_HOLDS_INT (v)) {
        g_warning ("wrong min-height value");
        break;
      }
      roigen->min_width = g_value_get_int (v);

      v = gst_value_array_get_value (value, 2);
      if (!G_VALUE_HOLDS_INT (v)) {
        g_warning ("wrong max-width value");
        break;
      }
      roigen->max_width = g_value_get_int (v);

      v = gst_value_array_get_value (value, 3);
      if (!G_VALUE_HOLDS_INT (v)) {
        g_warning ("wrong max-height value");
        break;
      }
      roigen->max_height = g_value_get_int (v);
      break;
    }
    case PROP_ROI_CLASS_FILTER:{
      int i;
      const GValue *v;

      if (roigen->class_list) {
        g_slist_free_full (roigen->class_list, g_free);
        roigen->class_list = NULL;
      }

      for (i = 0; i < gst_value_array_get_size (value); i++) {

        v = gst_value_array_get_value (value, i);
        if (!G_VALUE_HOLDS_STRING (v)) {
          g_warning ("value is not of string type");
          break;
        }

        roigen->class_list =
            g_slist_append (roigen->class_list, g_value_dup_string (v));
      }
      break;
    }
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_ivas_xroigen_get_property (GObject * object,
    guint prop_id, GValue * value, GParamSpec * pspec)
{
  GstIvas_XROIGen *roigen = GST_IVAS_XROIGEN (object);

  switch (prop_id) {
    case PROP_ROI_TYPE:
      g_value_set_enum (value, roigen->roi_type);
      break;
    case PROP_ROI_QP_DELTA:
      g_value_set_int (value, roigen->qp_delta);
      break;
    case PROP_ROI_QP_LEVEL:
      g_value_set_enum (value, roigen->qp_level);
      break;
    case PROP_ROI_MAX_NUM:
      g_value_set_uint (value, roigen->max_num);
      break;
    case PROP_INSERT_SEI:
      g_value_set_boolean (value, roigen->insert_roi_sei);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_ivas_xroigen_class_init (GstIvas_XROIGenClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GstBaseTransformClass *transform_class = GST_BASE_TRANSFORM_CLASS (klass);

  gobject_class->set_property = gst_ivas_xroigen_set_property;
  gobject_class->get_property = gst_ivas_xroigen_get_property;
  gobject_class->finalize = gst_ivas_xroigen_finalize;

  gst_element_class_add_pad_template (GST_ELEMENT_CLASS (klass),
      gst_pad_template_new ("src", GST_PAD_SRC, GST_PAD_ALWAYS,
          gst_caps_from_string (GST_VIDEO_CAPS_MAKE (GST_VIDEO_FORMATS_ALL))));
  gst_element_class_add_pad_template (GST_ELEMENT_CLASS (klass),
      gst_pad_template_new ("sink", GST_PAD_SINK, GST_PAD_ALWAYS,
          gst_caps_from_string (GST_VIDEO_CAPS_MAKE (GST_VIDEO_FORMATS_ALL))));

  g_object_class_install_property (gobject_class, PROP_ROI_TYPE,
      g_param_spec_enum ("roi-type", "ROI Type",
          "type to be used to generate ROI metadata",
          GST_TYPE_IVAS_XROIGEN_ROI_TYPE,
          GSTIVAS_XROIGEN_DEFAULT_ROI_TYPE,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_ROI_QP_DELTA,
      g_param_spec_int ("roi-qp-delta", "ROI QP Delta",
          "QP delta to be used for ROI in encoder",
          MIN_RELATIVE_QP, MAX_RELATIVE_QP, GSTIVAS_XROIGEN_DEFAULT_QP_DELTA,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property (gobject_class, PROP_ROI_QP_LEVEL,
      g_param_spec_enum ("roi-qp-level", "ROI Qualtiy Level",
          "QP level to be used for ROI in encoder",
          GST_TYPE_IVAS_XROIGEN_ROI_QUALITY,
          GSTIVAS_XROIGEN_DEFAULT_QP_LEVEL,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_ROI_MAX_NUM,
      g_param_spec_uint ("roi-max-num", "Maximum number of ROIs",
          "Max Number of ROIs to be attached to metadata",
          0, G_MAXUINT, GSTIVAS_XROIGEN_DEFAULT_MAX_NUM,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property (G_OBJECT_CLASS (klass),
      PROP_ROI_CLASS_FILTER, gst_param_spec_array ("class-filters",
          "Array of inference classes",
          "Array of desired inference classes ('<\"person\", \"car\">')",
          g_param_spec_string ("class", "inference class",
              "Inference class to allow", NULL,
              G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS),
          G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property (G_OBJECT_CLASS (klass),
      PROP_ROI_RESOLUTION_RANGE, gst_param_spec_array ("resolution-range",
          "Resolution range",
          "The resolution range ('<min-width, min-height, max-width, max-height>')",
          g_param_spec_int ("rect-value", "Rectangle Value",
              "One of min width/height or max width/height.", G_MININT,
              G_MAXINT, -1, G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS),
          G_PARAM_WRITABLE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property (gobject_class, PROP_INSERT_SEI,
      g_param_spec_boolean ("insert-roi-sei", "Insert ROI as SEI",
          "when true, generates custom event to OMX encoder to insert ROI information as SEI packet",
          FALSE, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  gst_element_class_set_static_metadata (GST_ELEMENT_CLASS (klass),
      "ROI Metadata Generator from IVAS Metadata",
      "Video/Filter", "ROI Metadata Generator from IVAS Metadata",
      "Xilinx Inc");

  transform_class->transform_ip =
      GST_DEBUG_FUNCPTR (gst_ivas_xroigen_transform_ip);
}

static void
gst_ivas_xroigen_init (GstIvas_XROIGen * roigen)
{
  roigen->need_dummy_roi = TRUE;
  roigen->roi_type = GSTIVAS_XROIGEN_DEFAULT_ROI_TYPE;
  roigen->qp_delta = GSTIVAS_XROIGEN_DEFAULT_QP_DELTA;
  roigen->qp_level = GSTIVAS_XROIGEN_DEFAULT_QP_LEVEL;
  roigen->max_num = GSTIVAS_XROIGEN_DEFAULT_MAX_NUM;
  roigen->min_width = roigen->min_height = 0;
  roigen->max_width = roigen->max_height = G_MAXINT;
  roigen->class_list = NULL;
  gst_base_transform_set_in_place (GST_BASE_TRANSFORM (roigen), TRUE);
}

static void
gst_ivas_xroigen_finalize (GObject * gobject)
{
  GstIvas_XROIGen *roigen = GST_IVAS_XROIGEN (gobject);

  if (roigen->class_list) {
    g_slist_free_full (roigen->class_list, g_free);
    roigen->class_list = NULL;
  }

  G_OBJECT_CLASS (parent_class)->finalize (gobject);
}

static void
ivas_xroigen_attach_dummy_roi (GstBuffer * buf)
{
  GstVideoRegionOfInterestMeta *roi_meta;
  int8_t obj_class[MAX_NAME_LENGTH];

  strcpy ((char *) obj_class, "None");
  roi_meta = gst_buffer_add_video_region_of_interest_meta (buf,
      (const gchar *) obj_class, 0, 0, 0, 0);

  gst_video_region_of_interest_meta_add_param (roi_meta,
      gst_structure_new ("roi-by-value/omx-alg", "delta-qp",
          G_TYPE_INT, 0, NULL));
}

static gboolean
ivas_xroigen_is_class_allowed (GstIvas_XROIGen * roigen, gchar * class)
{
  if (!roigen->class_list)
    return TRUE;

  if (g_slist_find_custom (roigen->class_list, class,
          (int (*)(const void *, const void *)) g_strcmp0)) {
    return TRUE;
  } else {
    return FALSE;
  }
}

static void
ivas_xroigen_free_roi_info (gpointer data)
{
  g_slice_free (IvasROIInfo, data);
}

static GstFlowReturn
gst_ivas_xroigen_transform_ip (GstBaseTransform * trans, GstBuffer * buf)
{
  GstIvas_XROIGen *roigen = GST_IVAS_XROIGEN (trans);
  GstVideoRegionOfInterestMeta *roi_meta;
  GstIvasMeta *ivas_meta;
  GstInferenceMeta *infer_meta = NULL;
  GstInferencePrediction *root, *child;
  GstInferenceClassification *classification;
  GSList *child_predictions;
  GList *classes;
  GList *node = NULL;
  guint roi_count = 0;
  GSList *roi_info_list = NULL;

  ivas_meta = gst_buffer_get_ivas_meta (buf);
  infer_meta = ((GstInferenceMeta *) gst_buffer_get_meta ((GstBuffer *)
          buf, gst_inference_meta_api_get_type ()));

  GST_LOG_OBJECT (roigen, "ivasmeta %p and infer_meta %p", ivas_meta,
      infer_meta);

  if (ivas_meta) {
    node = ivas_meta->xmeta.objects;

    while (node) {
      guint x, y, w, h;
      IvasObjectMetadata *obj = (IvasObjectMetadata *) node->data;

      x = obj->bbox_meta.xmin;
      y = obj->bbox_meta.ymin;
      w = obj->bbox_meta.xmax - obj->bbox_meta.xmin;
      h = obj->bbox_meta.ymax - obj->bbox_meta.ymin;

      if (w >= roigen->min_width && w <= roigen->max_width &&
          h >= roigen->min_height && h <= roigen->max_height &&
          ivas_xroigen_is_class_allowed (roigen, (gchar *) obj->obj_class)) {

        if (roi_count == roigen->max_num) {
          GST_DEBUG_OBJECT (roigen, "reached max number of ROIs");
          break;
        }

        roi_count++;

        GST_LOG_OBJECT (roigen,
            "attaching class %s with ROI position (%u x %u) and "
            "wxh = (%u x %u) to buffer %p", obj->obj_class, x, y, w, h, buf);

        if (roigen->insert_roi_sei) {
          IvasROIInfo *roi_info;

          roi_info = g_slice_alloc0 (sizeof (IvasROIInfo));
          roi_info->x = x;
          roi_info->y = y;
          roi_info->w = w;
          roi_info->h = h;

          roi_info_list = g_slist_append (roi_info_list, roi_info);
        }

        roi_meta = gst_buffer_add_video_region_of_interest_meta (buf,
            (const gchar *) obj->obj_class, x, y, w, h);

        if (roigen->roi_type == IVAS_XROIGEN_ROI_TYPE_QP_LEVEL) {
          gst_video_region_of_interest_meta_add_param (roi_meta,
              gst_structure_new ("roi/omx-alg", "quality", G_TYPE_STRING,
                  ivas_xroigen_get_qp_level_nickname (roigen->qp_level), NULL));
        } else if (roigen->roi_type == IVAS_XROIGEN_ROI_TYPE_QP_DELTA) {
          gst_video_region_of_interest_meta_add_param (roi_meta,
              gst_structure_new ("roi-by-value/omx-alg", "delta-qp",
                  G_TYPE_INT, roigen->qp_delta, NULL));
        }
      } else {
        GST_LOG_OBJECT (roigen,
            "skipping meta object <%u, %u, %u, %u> with label %s", x, y, w, h,
            obj->obj_class);
      }

      node = g_list_next (node);
    }
  } else if (infer_meta) {

    root = infer_meta->prediction;

    /* Iterate through the immediate child predictions */
    for (child_predictions = gst_inference_prediction_get_children (root);
        child_predictions;
        child_predictions = g_slist_next (child_predictions)) {
      child = (GstInferencePrediction *) child_predictions->data;

      /* On each children, iterate through the different associated classes */
      for (classes = child->classifications;
          classes; classes = g_list_next (classes)) {
        guint x, y, w, h;
        classification = (GstInferenceClassification *) classes->data;

        x = child->bbox.x;
        y = child->bbox.y;
        w = child->bbox.width;
        h = child->bbox.height;

        if (w >= roigen->min_width && w <= roigen->max_width &&
            h >= roigen->min_height && h <= roigen->max_height &&
            ivas_xroigen_is_class_allowed (roigen,
                (gchar *) classification->class_label)) {

          if (roi_count == roigen->max_num) {
            GST_DEBUG_OBJECT (roigen, "reached max number of ROIs");
            break;
          }

          roi_count++;

          if (roigen->insert_roi_sei) {
            IvasROIInfo *roi_info;

            roi_info = g_slice_alloc0 (sizeof (IvasROIInfo));
            roi_info->x = x;
            roi_info->y = y;
            roi_info->w = w;
            roi_info->h = h;

            roi_info_list = g_slist_append (roi_info_list, roi_info);
          }

          GST_LOG_OBJECT (roigen,
              "attaching class %s with ROI position (%u x %u) and "
              "wxh = (%u x %u) to buffer %p", classification->class_label, x, y,
              w, h, buf);

          roi_meta = gst_buffer_add_video_region_of_interest_meta (buf,
              (const gchar *) classification->class_label, x, y, w, h);

          if (roigen->roi_type == IVAS_XROIGEN_ROI_TYPE_QP_LEVEL) {
            gst_video_region_of_interest_meta_add_param (roi_meta,
                gst_structure_new ("roi/omx-alg", "quality", G_TYPE_STRING,
                    ivas_xroigen_get_qp_level_nickname (roigen->qp_level),
                    NULL));
          } else if (roigen->roi_type == IVAS_XROIGEN_ROI_TYPE_QP_DELTA) {
            gst_video_region_of_interest_meta_add_param (roi_meta,
                gst_structure_new ("roi-by-value/omx-alg", "delta-qp",
                    G_TYPE_INT, roigen->qp_delta, NULL));
          }
        } else {
          GST_LOG_OBJECT (roigen,
              "skipping meta object <%u, %u, %u, %u> with label %s", x, y, w, h,
              classification->class_label);
        }
      }

      if (roi_count >= roigen->max_num)
        break;

    }
  }

  /* send custom event, so that encoder will insert SEI packets in byte stream */
  if (roigen->insert_roi_sei && roi_count) {
    GstBuffer *sei_buf = NULL;
    gchar *sei_data = NULL;
    guint offset = 0;
    gsize sei_size =
        IVAS_ROI_SEI_EXTRA_INFO_SIZE + (roi_count * sizeof (IvasROIInfo));
    GstStructure *s = NULL;
    GstEvent *event = NULL;

    sei_data = (gchar *) g_malloc0 (sei_size);
    if (!sei_data) {
      GST_ERROR_OBJECT (roigen, "failed to allocate memory");
      return GST_FLOW_ERROR;
    }

    GST_DEBUG_OBJECT (roigen, "preparing SEI event for ROI count %u",
        roi_count);

    memcpy (sei_data + offset, &(GST_BUFFER_PTS (buf)), sizeof (GstClockTime));
    offset += sizeof (GstClockTime);
    memcpy (sei_data + offset, &roi_count, sizeof (guint));
    offset += sizeof (guint);

    while (roi_info_list) {
      memcpy (sei_data + offset, roi_info_list->data, sizeof (IvasROIInfo));
      offset += sizeof (IvasROIInfo);

      roi_info_list = g_slist_next (roi_info_list);
    }

    sei_buf = gst_buffer_new_wrapped_full (0, sei_data, sei_size, 0, sei_size,
        sei_data, g_free);

    s = gst_structure_new ("omx-alg/insert-suffix-sei",
        "payload-type", G_TYPE_UINT, 77,
        "payload", GST_TYPE_BUFFER, sei_buf, NULL);

    event = gst_event_new_custom (GST_EVENT_CUSTOM_DOWNSTREAM, s);

    if (!gst_pad_push_event (GST_BASE_TRANSFORM_SRC_PAD (trans), event)) {
      GST_ERROR_OBJECT (roigen, "failed to send custom SEI event");
      gst_buffer_unref (sei_buf);
      return GST_FLOW_ERROR;
    }

    GST_DEBUG_OBJECT (roigen, "sent SEI event with SEI payload size %lu",
        sei_size);

    g_slist_free_full (roi_info_list, ivas_xroigen_free_roi_info);
    gst_buffer_unref (sei_buf);
  }

  if (G_UNLIKELY (roigen->need_dummy_roi) && !roi_count) {
    /* Add dummy ROI metadata to first frame if there is no metadata already present in the buffer.
     * This is requirement of omx encoder to have roi-meta atleast in the first frame to initialize
     * certain things at omx encoder creation.
     */
    ivas_xroigen_attach_dummy_roi (buf);
    GST_INFO_OBJECT (roigen, "send dummy ROI metadata for the first frame");
    roigen->need_dummy_roi = FALSE;
  }

  return GST_FLOW_OK;
}

static gboolean
ivas_xroigen_init (GstPlugin * ivas_xroigen)
{
  return gst_element_register (ivas_xroigen, "ivas_xroigen",
      GST_RANK_NONE, GST_TYPE_IVAS_XROIGEN);
}

/* PACKAGE: this is usually set by autotools depending on some _INIT macro
 * in configure.ac and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use autotools to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "ivas_xroigen"
#endif

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    ivas_xroigen,
    "Xilinx IVAS SDK plugin to generate ROI metadata",
    ivas_xroigen_init, "1.0", "MIT/X11",
    "Xilinx IVAS SDK plugin", "http://xilinx.com/")
