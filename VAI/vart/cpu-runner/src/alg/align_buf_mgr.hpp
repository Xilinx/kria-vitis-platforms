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

#include <cstddef>
#include "cpu_std_inc.hpp"
#include "pair_hash.hpp"
#include "spin_lock.hpp"

namespace vart {
namespace cpu {

class AlignBuf {
public:
  AlignBuf(uint32_t size, uint32_t align)
    : size_(size), align_(align), space_(align + size) {

    raw_ptr_ = new char [align_ + size_];
    align_ptr_ = raw_ptr_;
    std::align(align_, size_, align_ptr_, space_);
  }

  ~AlignBuf() {
    if(raw_ptr_)
      delete []raw_ptr_;
  }

  static unique_ptr<AlignBuf> make(uint32_t size, uint32_t align) {
    return make_unique<AlignBuf>(size, align);
  }
  void *get_aligned_ptr() { return align_ptr_; }

private:
  uint32_t size_;
  uint32_t align_;
  size_t space_;
  char* raw_ptr_;
  void* align_ptr_;

  class AlignBufMgr;
  friend class AlignBufMgr;
};

class AlignBufMgr {
private:
  using InternalKey = uint64_t;

  struct InternalStatistics {
    uint32_t allocate_cnt_{0};
    uint32_t release_cnt_{0};
    uint32_t new_cnt_{0};
    uint32_t hit_cnt_{0};
    uint32_t err_cnt_{0};
  };

private:
  AlignBufMgr() = default;
  ~AlignBufMgr() = default;

public:
  static AlignBufMgr* Instance() {
    static AlignBufMgr m;
    return &m;
  }

  void* allocate(uint32_t size, uint32_t align = alignof(std::max_align_t)) {
    std::lock_guard<mutex> lock(mtx_);

    statistics_.allocate_cnt_++;
    statistics_.new_cnt_++;

    auto buf = AlignBuf::make(size, align);
    void *align_ptr = buf->get_aligned_ptr();
    buf_[align_ptr] = std::move(buf);

    return align_ptr;
  }

  void release(void *ptr) {
    std::lock_guard<mutex> lock(mtx_);

    statistics_.release_cnt_++;

    if(buf_.find(ptr) == buf_.end()) {
      statistics_.err_cnt_++;
      return;
    }

    buf_.erase(ptr);
  }

  void print_statistics() {
    cout << "allocate_cnt_: " << statistics_.allocate_cnt_ << endl;
    cout << "release_cnt_: " << statistics_.release_cnt_ << endl;
    cout << "new_cnt_: " << statistics_.new_cnt_ << endl;
    cout << "hit_cnt_: " << statistics_.hit_cnt_ << endl;
    cout << "err_cnt_: " << statistics_.err_cnt_ << endl;
  }

private:
  map<void*, unique_ptr<AlignBuf>> buf_;

  InternalStatistics statistics_;

  // because of mutex, this manager is not fit for high concurrency situation,
  // even use a spin lock, the situation still not change
  std::mutex mtx_;
  // SpinLock l;
};

}  // namespace cpu
}  // namespace vart
