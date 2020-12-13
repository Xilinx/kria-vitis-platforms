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

#pragma once
#ifndef DPUMODELS_H
#define DPUMODELS_H

#if 0
enum
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
};
#endif

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

#endif
