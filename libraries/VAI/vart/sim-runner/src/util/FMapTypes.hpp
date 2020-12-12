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
#ifndef __FMAP_TYPES_HPP__
#define __FMAP_TYPES_HPP__

#include "Util.hpp"

const int FILE_DEFAULT_TYPE = 0;
const string FILE_NULL_NAME = "null";

struct AddrAttr {
    int location;
    int ff_mode;
    int ff_shift;
    int reg;
    uint64_t off;

    void PrintCfg(const string& head = "addr") {
        UNI_LOG_INFO << head << ": ["
            << "location: " << location
            << ", ff_mode: " << ff_mode
            << ", ff_shift: " << ff_shift
            << ", reg: " << reg
            << ", off: 0x" << std::hex << off
            << "(" << std::dec << off
            << ")]" << endl;
    }

    void Check() {
        UNI_LOG_CHECK(reg>=0 && reg<10, SIM_OUT_OF_RANGE) << ": error reg value " << reg << endl;
    }
};

struct FMapAttr {
    int h; // height
    int hs; // height stride
    int w; // width
    int ws; // width stride
    int c; // channel
    int cs; // channel stride

    void PrintCfg(const string& head = "fmap") {
        UNI_LOG_INFO << head << ": ["
            << "h: " << h
            << ", hs: " << hs
            << ", w: " << w
            << ", ws: " << ws
            << ", c: " << c
            << ", cs: " << cs
            << "]" << endl;
    }
    void Check() {
        UNI_LOG_CHECK(h>0 && hs>0 && hs>=h
                && w>0 && ws>0 && ws>=w
                && c>0 && cs>0 && cs>=c, SIM_PARAMETER_FAILED)
            << ": error fmap param "
            << "h = " << h
            << ", hs = " << hs
            << ", w = " << w
            << ", ws = " << ws
            << ", c = " << c
            << ", cs = " << cs
            << endl;
    }
};

struct KernelAttr {
    int ori_h;
    int ori_w;
    int dilation;
    int h;
    int w;

    void PrintCfg(const string& head = "kernel") {
        UNI_LOG_INFO << head << ": ["
            << "ori_h: " << ori_h
            << ", ori_w: " << ori_w
            << ", dilation: " << dilation
            << ", h: " << h
            << ", w: " << w
            << "]" << endl;
    }
    void Check() {
        UNI_LOG_CHECK(ori_h>0 && ori_h<16
                && ori_w>0 && ori_w<16
                && dilation>=1
                && h>0 && h<16
                && w>0 && w<16, SIM_PARAMETER_FAILED)
            << ": error kernel param "
            << "ori_h = " << ori_h
            << ", ori_w = " << ori_w
            << ", dilation = " << dilation
            << ", h = " << h
            << ", w = " << w
            << endl;
    }
};

struct StrideAttr {
    int h;
    int w;

    void PrintCfg(const string& head = "stride") {
        UNI_LOG_INFO << head << ": ["
            << "h: " << h
            << ", w: " << w
            << "]" << endl;
    }
    void Check() {
        UNI_LOG_CHECK(h>0 && w>0, SIM_PARAMETER_FAILED)
            << ": error stride param "
            << "h = " << h
            << ", w = " << w
            << endl;
    }
};

struct ScaleAttr {
    int h;
    int w;

    void PrintCfg(const string& head = "scale") {
        UNI_LOG_INFO << head << ": ["
            << "h: " << h
            << ", w: " << w
            << "]" << endl;
    }
    void Check() {
        UNI_LOG_CHECK(h>0 && w>0,SIM_PARAMETER_FAILED)
            << ": error scale param "
            << "h = " << h
            << ", w = " << w
            << endl;
    }
};

struct PadAttr {
    int l; // left
    int t; // top
    int r; // right
    int b; // bottom

    void PrintCfg(const string& head = "pad") {
        UNI_LOG_INFO << head << ": ["
            << "left: " << l
            << ", top: " << t
            << ", right: " << r
            << ", bottom: " << b
            << "]" << endl;
    }

    void Check() {
        UNI_LOG_CHECK(l>=0 && t>=0 && r>=0 && b>=0, SIM_PARAMETER_FAILED)
            << ": error stride param "
            << "l = " << l
            << ", t = " << t
            << ", r = " << r
            << ", b = " << b
            << endl;
    }
};

struct FileAttr {
    int type;
    string name;

    void PrintCfg(const string& head = "file") {
        UNI_LOG_INFO << head << ": ["
            << "type: " << type
            << ", name: " << name
            << "]" << endl;
    }

    void Check() { }
};

#endif /* __FMAP_TYPES_HPP__ */
