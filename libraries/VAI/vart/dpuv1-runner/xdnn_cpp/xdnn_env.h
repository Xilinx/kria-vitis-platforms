// SPDX-License-Identifier: BSD-3-CLAUSE
//
// (C) Copyright 2018, Xilinx, Inc.
//
#ifndef XDNN_ENV_H
#define XDNN_ENV_H

#include <sstream>
#include "xdnn_common.h"

const bool XDNN_VERBOSE =
    std::getenv("XDNN_VERBOSE") ?
        atoi(std::getenv("XDNN_VERBOSE")) == 1 : false;
const bool XLNX_DUMP_XDNN_STANDALONE_DATA =
    std::getenv("XDNN_DUMP_STANDALONE_DATA") ?
        atoi(std::getenv("XDNN_DUMP_STANDALONE_DATA")) == 1 : false;

const bool XDNN_EMIT_PROFILING_INFO =
  std::getenv("XDNN_EMIT_PROFILING_INFO") ?
    atoi(std::getenv("XDNN_EMIT_PROFILING_INFO")) == 1 : false;
const bool XBLAS_EMIT_PROFILING_INFO = 
  (XDNN_EMIT_PROFILING_INFO)? XDNN_EMIT_PROFILING_INFO 
  : (std::getenv("XBLAS_EMIT_PROFILING_INFO") ?
       atoi(std::getenv("XBLAS_EMIT_PROFILING_INFO")) == 1 : false);

const bool XDNN_READ_HARDWARE_GENERAL_COUNTER = 
    std::getenv("XDNN_READ_HARDWARE_GENERAL_COUNTER") ?
      atoi(std::getenv("XDNN_READ_HARDWARE_GENERAL_COUNTER")) == 1 : false;
const bool XDNN_HARDWARE_GENERAL_COUNTER_TIME_DL =
    std::getenv("XDNN_HARDWARE_GENERAL_COUNTER_TIME_DL") ?
    atoi(std::getenv("XDNN_HARDWARE_GENERAL_COUNTER_TIME_DL")) == 1 : false;
const bool XDNN_HARDWARE_GENERAL_COUNTER_TIME_UL =
    std::getenv("XDNN_HARDWARE_GENERAL_COUNTER_TIME_UL") ?
    atoi(std::getenv("XDNN_HARDWARE_GENERAL_COUNTER_TIME_UL")) == 1 : false;
const bool XDNN_HARDWARE_GENERAL_COUNTER_TIME_FL =
    std::getenv("XDNN_HARDWARE_GENERAL_COUNTER_TIME_FL") ?
    atoi(std::getenv("XDNN_HARDWARE_GENERAL_COUNTER_TIME_FL")) == 1 : false;
const bool XDNN_HARDWARE_GENERAL_COUNTER_TIME_MISC =
    std::getenv("XDNN_HARDWARE_GENERAL_COUNTER_TIME_MISC") ?
    atoi(std::getenv("XDNN_HARDWARE_GENERAL_COUNTER_TIME_MISC")) == 1 : false;
const bool XDNN_HARDWARE_GENERAL_COUNTER_TIME_SYSARR =
    std::getenv("XDNN_HARDWARE_GENERAL_COUNTER_TIME_SYSARR") ?
    atoi(std::getenv("XDNN_HARDWARE_GENERAL_COUNTER_TIME_SYSARR")) == 1 : false;

const int XDNN_USE_JOB_THREADS_TOTAL_THREADS =
    std::getenv("XDNN_USE_JOB_THREADS_TOTAL_THREADS") ?
        atoi(std::getenv("XDNN_USE_JOB_THREADS_TOTAL_THREADS")) : 32;

const int XSTREAM_PORT = std::getenv("XSTREAM_PORT") ?
    atoi(std::getenv("XSTREAM_PORT")) : 6661;

#endif
