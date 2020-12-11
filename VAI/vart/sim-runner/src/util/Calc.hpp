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
#ifndef __CALC_HPP__
#define __CALC_HPP__

#include "common.hpp"
#include "FMapTypes.hpp"

class Calc {
public:
    Calc() = delete;
    ~Calc() = delete;

public:
    enum ReluType {
        RELU_TYPE_MIN = 0,
        RELU_TYPE_NONE = RELU_TYPE_MIN, // 0
        RELU_TYPE_RELU, // 1
        RELU_TYPE_PRELU, // 2
        RELU_TYPE_LEAKY_RELU, // 3
        RELU_TYPE_RELUSIX, // 4
        RELU_TYPE_BN, // 5
        RELU_TYPE_BN_RELU, // 6
        RELU_TYPE_MAX, // 7
    };
    const static array<string, RELU_TYPE_MAX> ReluTypeStr;

public:
    template<typename T>
    static void Softmax(const T *src, int num, T *rlt);

// change data access order
public:
    template <typename T>
    static T DPURound(double data);
    template <typename T>
    static void HWC2CHW(int h, int w, int c, const T *src, T *dst);
    template <typename T>
    static void CHW2HWC(int c, int h, int w, const T *src, T *dst);

public:
    enum TransDimMethod{
        TDM_MIN = 0,
        NSTRIDE_2_NSTRIDE = TDM_MIN,
        NSTRIDE_2_YSTRIDE,
        YSTRIDE_2_NSTRIDE,
        YSTRIDE_2_YSTRIDE,
        TDM_MAX,
    };
    static string GetTDMName(int method);
    template <typename T>
    static void TransDim(T *dst, size_t dst_size, const T *src, size_t src_size, const FMapAttr& fmap, int method);

};

template<typename T>
void Calc::Softmax(const T *src, int num, T *rlt) {
    T sum = 0;

    for(auto i=0; i<num; i++) {
        rlt[i] = exp(src[i]);
        sum += rlt[i];
    }

    for(auto i=0; i<num; i++) {
        rlt[i] = rlt[i] / sum;
    }
}

template <typename T>
T Calc::DPURound(double data)
{
    T rlt = 0;
    T data_max = std::numeric_limits<T>::max();
    T data_min = std::numeric_limits<T>::min();

    if(data > data_max) {
        rlt = data_max;
    } else if(data < data_min) {
        rlt = data_min;
    } else if(data<0 && (data-floor(data))==0.5) {
        rlt = static_cast<T>(ceil(data));
    } else {
        rlt = static_cast<T>(round(data));
    }

    return rlt;
}

template <typename T>
void Calc::HWC2CHW(int h, int w, int c, const T *src, T *dst)
{
    for(int idx_c=0; idx_c<c; idx_c++) {
        for(int idx_h=0; idx_h<h; idx_h++) {
            for(int idx_w=0; idx_w<w; idx_w++) {
                dst[idx_c*h*w+idx_h*w+idx_w] = src[idx_h*w*c+idx_w*c+idx_c];
            }
        }
    }
}

template <typename T>
void Calc::CHW2HWC(int c, int h, int w, const T *src, T *dst)
{
    for(int idx_h=0; idx_h<h; idx_h++) {
        for(int idx_w=0; idx_w<w; idx_w++) {
            for(int idx_c=0; idx_c<c; idx_c++) {
                dst[idx_h*w*c+idx_w*c+idx_c] = src[idx_c*h*w+idx_h*w+idx_w];
            }
        }
    }
}

template <typename T>
void Calc::TransDim(T *dst, size_t dst_size, const T *src, size_t src_size, const FMapAttr& fmap, int method) {
    UNI_LOG_CHECK(method>=TDM_MIN && method<TDM_MAX, SIM_OUT_OF_RANGE)
        << ": TransDimMethod value " << method << " error!" << endl;
    UNI_LOG_CHECK(dst != nullptr, SIM_NULL_PTR);
    UNI_LOG_CHECK(src != nullptr, SIM_NULL_PTR);

    if(method == NSTRIDE_2_NSTRIDE) {
        UNI_LOG_CHECK(dst_size >= static_cast<size_t>(fmap.h*fmap.w*fmap.c), SIM_PARAMETER_FAILED)
            << ": dst_size " << dst_size << " error!" << endl;
        UNI_LOG_CHECK(src_size >= static_cast<size_t>(fmap.h*fmap.w*fmap.c), SIM_PARAMETER_FAILED)
            << ": src_size " << src_size << " error!" << endl;
    } else if(method == NSTRIDE_2_YSTRIDE) {
        UNI_LOG_CHECK(dst_size >= static_cast<size_t>(fmap.hs*fmap.ws*fmap.cs), SIM_PARAMETER_FAILED)
            << ": dst_size " << dst_size << " error!" << endl;
        UNI_LOG_CHECK(src_size >= static_cast<size_t>(fmap.h*fmap.w*fmap.c), SIM_PARAMETER_FAILED)
            << ": src_size " << src_size << " error!" << endl;
    } else if(method == YSTRIDE_2_NSTRIDE) {
        UNI_LOG_CHECK(dst_size >= static_cast<size_t>(fmap.h*fmap.w*fmap.c), SIM_PARAMETER_FAILED)
            << ": dst_size " << dst_size << " error!" << endl;
        UNI_LOG_CHECK(src_size >= static_cast<size_t>(fmap.hs*fmap.ws*fmap.cs), SIM_PARAMETER_FAILED)
            << ": src_size " << src_size << " error!" << endl;
    } else if(method == YSTRIDE_2_YSTRIDE) {
        UNI_LOG_CHECK(dst_size >= static_cast<size_t>(fmap.hs*fmap.ws*fmap.cs), SIM_PARAMETER_FAILED)
            << ": dst_size " << dst_size << " error!" << endl;
        UNI_LOG_CHECK(src_size >= static_cast<size_t>(fmap.hs*fmap.ws*fmap.cs), SIM_PARAMETER_FAILED)
            << ": src_size " << src_size << " error!" << endl;
    }

    // normal general assign algorithm
    for(auto i=0; i<fmap.h; i++) {
        for(auto j=0; j<fmap.w; j++) {
            for(auto k=0; k<fmap.c; k++) {
                auto src_pos = 0U;
                auto dst_pos = 0U;
                if(method == NSTRIDE_2_NSTRIDE) {
                    src_pos = i*fmap.w*fmap.c + j*fmap.c + k;
                    dst_pos = i*fmap.w*fmap.c + j*fmap.c + k;
                } else if(method == NSTRIDE_2_YSTRIDE) {
                    src_pos = i*fmap.w*fmap.c + j*fmap.c + k;
                    dst_pos = i*fmap.ws*fmap.cs + j*fmap.cs + k;
                } else if(method == YSTRIDE_2_NSTRIDE) {
                    src_pos = i*fmap.ws*fmap.cs + j*fmap.cs + k;
                    dst_pos = i*fmap.w*fmap.c + j*fmap.c + k;
                } else if(method == YSTRIDE_2_YSTRIDE) {
                    src_pos = i*fmap.ws*fmap.cs + j*fmap.cs + k;
                    dst_pos = i*fmap.ws*fmap.cs + j*fmap.cs + k;
                }

                dst[dst_pos] = src[src_pos];
            }
        }
    }
}

#endif /* __CALC_HPP__ */
