/*
 * Copyright 2019 Xilinx Inc.
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

#ifndef _XDNN_COMMON_H_
#define _XDNN_COMMON_H_

/* 64 bit detected */
#if defined(__x86_64__)
#include <glibc_ver_header/x64/force_link_glibc_2.17.h>
/* 32 bit x86 detected */
#elif defined(__i386__)
#include <glibc_ver_header/x86/force_link_glibc_2.17.h>
#endif

#endif
