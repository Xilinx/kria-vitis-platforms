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
#ifndef DPU_H
#define DPU_H

/* Update of this file by the user is not encouraged */
#include <stdio.h>

enum
{
  LOG_LEVEL_ERROR,
  LOG_LEVEL_WARNING,
  LOG_LEVEL_INFO,
  LOG_LEVEL_DEBUG
};

#define NOT_FOUND -1

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define LOG_MESSAGE(level, set_level, ...) {\
  do {\
    char *str; \
    if (level == LOG_LEVEL_ERROR)\
      str = (char*)"ERROR";\
    else if (level == LOG_LEVEL_WARNING)\
      str = (char*)"WARNING";\
    else if (level == LOG_LEVEL_INFO)\
      str = (char*)"INFO";\
    else if (level == LOG_LEVEL_DEBUG)\
      str = (char*)"DEBUG";\
    if (level <= set_level) {\
      printf("[%s %s:%d] %s: ",__FILENAME__, __func__, __LINE__, str);\
      printf(__VA_ARGS__);\
      printf("\n");\
    }\
  } while (0); \
}

#if 0
static void
LOG_DUMP (int level, char *ptr, int num)
{
  if (!(level <= log_level))
    return;

  for (int i = 0; i < num; i++) {
    printf (" %x", *ptr++);
  }
  printf ("\n");
}
#endif

#endif
