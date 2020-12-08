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

#ifndef __IVAS_META_H__
#define __IVAS_META_H__

/* Update of this file by the user is not encouraged */
#include <stdint.h>

#define MAX_NAME_LENGTH 256

typedef struct _IvasColorMetadata {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint8_t alpha;
} IvasColorMetadata;

typedef struct _IvasTextMetadata {

  int8_t disp_text[MAX_NAME_LENGTH];

  IvasColorMetadata text_color;
} IvasTextMetadata;


typedef struct _XVABBoxMeta {
  float xmin;
  float ymin;
  float xmax;
  float ymax;

  IvasColorMetadata box_color;
} IvasBBoxMetadata;

typedef struct _IvasObjectMetadata {
  int32_t obj_id;
  int8_t obj_class[MAX_NAME_LENGTH];
  double obj_prob;
  IvasBBoxMetadata bbox_meta;

  IvasTextMetadata text_meta;

  GList *obj_list;
} IvasObjectMetadata;

#endif
