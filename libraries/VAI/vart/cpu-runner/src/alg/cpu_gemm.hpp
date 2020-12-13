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

#pragma once

// #include <immintrin.h>
#include "cpu_std_inc.hpp"
#include "cpu_types.hpp"

namespace vart {
namespace cpu {

template<typename T>
inline void inner_product(const T* A, const T* B, T* C, int K) {
  *C = 0;
  for(auto k=0; k<K; k++) {
    *C += A[k] * B[k];
  }
}

template<typename T>
inline void eltwise_product(const T* A, const T* B, T* C, int K) {
  for(auto k=0; k<K; k++) {
    C[k] = A[k] * B[k];
  }
}

template<typename T>
inline void group_product(const T* A, const T* B, T* C, int K, int group) {
  int group_size = K / group;

  int k = 0;
  for(auto i=0; i<group; i++) {
    C[i] = 0;
    for(auto j=0; j<group_size; j++) {
      C[i] += A[k] * B[k];
      k++;
    }
  }
}

template<typename T>
inline void inner_product_with_kernel_stride(
    const T* A, const T* B, T* C, int fmap_slice, int kernel_slice, Kernel_t kernel) {

  *C = 0;
  for(auto i=0; i<kernel.h; i++) {
    auto *addrA = A + i*fmap_slice;
    auto *addrB = B + i*kernel_slice;
    for(auto j=0; j<kernel_slice; j++) {
      *C += addrA[j] * addrB[j];
    }
  }
}

/*
template<>
inline void inner_product<int32_t>(const int32_t* A, const int32_t* B, int32_t* C, int K) {
  auto ALIGNMENT = 64;
  auto parallel = 16;
  auto ngroups = K / parallel;
  auto left = K % parallel;
  auto pad = 0;
  if(left != 0) {
    ngroups += 1;
    pad = parallel - left;
  }
  auto DATA_SIZE = ngroups * parallel * sizeof(int32_t);

  // A's tmp buffer
  auto sizeA = DATA_SIZE + ALIGNMENT;
  char* bufA = new char[sizeA];
  void* ptrA = bufA;
  size_t spaceA = sizeA;
  std::align(ALIGNMENT, DATA_SIZE, ptrA, spaceA);
  std::copy_n(A, K, (int32_t*)ptrA);
  if(pad != 0) {
    std::fill_n((int32_t*)ptrA + K, pad, 0);
  }

  // B's tmp buffer
  auto sizeB = DATA_SIZE + ALIGNMENT;
  char* bufB = new char[sizeB];
  void* ptrB = bufB;
  size_t spaceB = sizeB;
  std::align(ALIGNMENT, DATA_SIZE, ptrB, spaceB);
  std::copy_n(B, K, (int32_t*)ptrB);
  if(pad != 0) {
    std::fill_n((int32_t*)ptrB + K, pad, 0);
  }

  // C's tmp buffer
  alignas(64) int32_t bufC[16];

  __m512i acc = _mm512_setzero_epi32();

  for(auto i=0; i<ngroups; i++) {
    const auto* tmpA = (int32_t*)ptrA + i*parallel;
    const auto* tmpB = (int32_t*)ptrB + i*parallel;

    __m512i a = _mm512_load_epi32(tmpA);
    __m512i b = _mm512_load_epi32(tmpB);

    __m512i c = _mm512_mullo_epi32(a, b);

    acc = _mm512_add_epi32(c, acc);
  }

  _mm512_store_epi32(bufC, acc);

  *C = 0;
  for(auto i=0; i<parallel; i++) {
    *C += bufC[i];
  }

  delete []bufA;
  delete []bufB;
}
//*/


// K is the axis of matrix multiply
// A dimention: [X, K]
// B dimention: [Y, K]
// C dimention: [X, Y]
template<typename T>
void matmul(const T* A, const T* B, T* C, int X, int Y, int K) {
  for(auto x=0; x<X; x++) {
    for(auto y=0; y<Y; y++) {
      const auto* addrA = A + x*K;
      const auto* addrB = B + y*K;

      auto* addrC = C + x*Y + y;

      inner_product(addrA, addrB, addrC, K);
    }
  }
}

template<typename T>
void matmul_thread(const T* A, const T* B, T* C, int X, int Y, int K) {
  int THREAD_NUM = CPU_NUM;
  int SIZE = X * Y;
  int THREAD_WORKLOAD = ceil((float)SIZE / THREAD_NUM);

  vector<std::future<int>> fut(THREAD_NUM);

  for(auto i=0; i<THREAD_NUM; i++) {
    fut[i] = std::async(std::launch::async,
        [A, B, C, X, Y, K, SIZE, THREAD_WORKLOAD](decltype(i) i) {
      auto BASE_POS = i * THREAD_WORKLOAD;
      for(auto j=0; j<THREAD_WORKLOAD; j++) {
        int pos = BASE_POS + j;
        if(pos >= SIZE)
          break;

        int x = pos / Y;
        int y = pos % Y;
        auto *addrA = A + x * K;
        auto *addrB = B + y * K;
        auto *addrC = C + pos;
        inner_product(addrA, addrB, addrC, K);
      }

      return 0;
    }, i);
  }

  for(auto i=0; i<THREAD_NUM; i++) {
    fut[i].wait();
  }
}

}  // namespace cpu
}  // namespace vart
