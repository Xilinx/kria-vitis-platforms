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

#ifndef DPU2_H
#define DPU2_H

#include <vector>
#include <stdio.h>
#include <string>
#include <opencv2/opencv.hpp>

#include <ivas/ivas_kernel.h>
#include <ivas/ivaslogs.h>
#include <gst/ivas/gstinferencemeta.h>
#include <gst/ivas/gstivasinpinfer.h>

using namespace cv;
using namespace std;

/*  #define INT_MAX 2147483647 */

struct ivas_xkpriv;

class ivas_xdpumodel
{
public:
  virtual int run (ivas_xkpriv * kpriv, const cv::Mat & image,
      GstInferenceMeta * ivas_meta) = 0;
  virtual int requiredwidth (void) = 0;
  virtual int requiredheight (void) = 0;
  virtual int close (void) = 0;
    virtual ~ ivas_xdpumodel () = 0;
};

struct performance_test
{
  int test_started = 0;
  unsigned long frames = 0;
  unsigned long last_displayed_frame = 0;
  long long timer_start;
  long long last_displayed_time;
};
typedef struct performance_test ivas_perf;

struct labels
{
  std::string name;
  int label;
    std::string display_name;
};
typedef struct lables lables;

enum
{
  IVAS_XLABEL_NOT_REQUIRED = 0,
  IVAS_XLABEL_REQUIRED = 1,
  IVAS_XLABEL_NOT_FOUND = 2,
  IVAS_XLABEL_FOUND = 4
};

struct model_list
{
  ivas_xdpumodel *model;
    std::string modelname;
  labels *labelptr;
  int modelclass;
};
typedef struct model_list model_list;



/**
 * struct ivas_xkpriv - Keep private data of ivas_xdpuinfer
 */
struct ivas_xkpriv
{
  vector < model_list > mlist;
  ivas_xdpumodel *model;        /* current dpu handler */
  IVASKernel *handle;           /* ivas kernel handler */
  int modelclass;               /* Class of model, from Json file */
  int modelnum;                 /* map class to number ivas_xmodelclass[] */
  int log_level;                /* LOG_LEVEL_ERROR=0, LOG_LEVEL_WARNING=1,
                                   LOG_LEVEL_INFO=2, LOG_LEVEL_DEBUG=3 */
  bool need_preprocess;         /* enable/disable pre-processing of DPU */
  bool performance_test;        /* enable/disable performance */
  bool run_time_model;          /* enable model load on every frame */
  labels *labelptr;             /* contain label array */
  int labelflags;               /* IVAS_XLABEL_NOT_REQUIRED, IVAS_XLABEL_REQUIRED,
                                   IVAS_XLABEL_NOT_FOUND, IVAS_XLABEL_FOUND */
  int max_labels;               /* number of labels in label.json */
  ivas_perf pf;                 /* required for performance */
    std::string modelpath;      /* contain model files path from json */
    std::string modelname;      /* contain name of model from json */
    std::string elfname;        /* contail model elf name */
};
typedef struct ivas_xkpriv ivas_xkpriv;

#endif
