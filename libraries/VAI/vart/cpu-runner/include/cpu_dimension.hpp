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

#include "cpu_std_inc.hpp"

namespace vart {
namespace cpu {

// NOTE: FMap_t in cpu_types.hpp is a special shape which has 4 dimentions,
// It's much faster than this general Dimension structure when dim == 4,
// The rest circumstance should use Dimension instead of vector<int>
using dim_t = int;
using coord_t = vector<dim_t>;
class Dimension {
public:
  Dimension() = default;
  Dimension(const Dimension&) = default;
  Dimension& operator = (const Dimension&) = default;
  Dimension(Dimension&&) = default;
  Dimension& operator = (Dimension&&) = default;
  ~Dimension() = default;

  Dimension(const vector<dim_t>& v) {
    update(v);
  }
  Dimension(initializer_list<dim_t> list) {
    vector<dim_t> v{list};
    update(v);
  }

public:
  inline dim_t ndims() const { return dims_.size(); }
  inline dim_t num() const { return num_; }
  inline dim_t dim(int idx) const { return operator[](idx); }
  inline dim_t operator[](int idx) { return dims_[idx]; }
  inline dim_t operator[](int idx) const { return dims_[idx]; }
  inline dim_t front() { return operator[](0); }
  inline dim_t front() const { return operator[](0); }
  inline dim_t back() { return operator[](ndims()-1); }
  inline dim_t back() const { return operator[](ndims()-1); }
  inline dim_t cod(int idx) const { return cod_[idx]; }
  inline vector<dim_t> vdims() const { return dims_; }

  inline void set_dim(int idx, dim_t dim) {
    dims_[idx] = dim;
    update();
  }
  inline void insert_dim(int idx, dim_t dim) {
    dims_.insert(dims_.begin() + idx, dim);
    update();
  }
  inline void erase(int idx) {
    dims_.erase(dims_.begin() + idx);
    update();
  }

  inline dim_t coord2pos(const coord_t& coord) const {
    dim_t pos = 0;
    for(auto i=0U; i<coord.size(); i++) {
      pos += coord[i] * cod_[i];
    }

    return pos;
  }
  inline coord_t pos2coord(dim_t pos) const {
    auto remainder = pos;
    coord_t coord;
    for(auto i=0; i<ndims(); i++) {
      coord.push_back(remainder / cod_[i]);
      remainder = remainder % cod_[i];
    }

    return coord;
  }

  bool operator==(const Dimension& other) {
    if(ndims() != other.ndims())
      return false;

    for(auto i=0; i<ndims(); i++) {
      if(dim(i) != other.dim(i))
        return false;
    }

    return true;
  }

  bool operator!=(const Dimension& other) {
    return !operator==(other);
  }

  void print_param(const string& head = "") const {
    string shape_str;
    string cod_str;

    for(auto i=0; i<ndims(); i++) {
      shape_str += to_string(dim(i));
      cod_str += to_string(cod(i));
      if(i != ndims()-1) {
        shape_str += ", ";
        cod_str += ", ";
      }
    }
    UNI_LOG_DEBUG_INFO << head << " shape: [" << shape_str
      << "], cod: [" << cod_str << "]" << endl;
  }

protected:
  void update() {
    // check validation of dims_ firstly
    UNI_LOG_CHECK(dims_.size() > 0, VART_INVALID_VALUE);
    for(auto i=0U; i<dims_.size(); i++) {
      UNI_LOG_CHECK(dims_[i] > 0, VART_INVALID_VALUE);
    }

    // calculate coefficient of dimension of dims_
    cod_.clear();
    for(auto i=0U; i<dims_.size(); i++) {
      if(i == dims_.size()-1) {
        cod_.push_back(1);
      } else {
        auto t = vec_mul(i + 1);
        cod_.push_back(t);
      }
    }

    // update element number
    num_ = vec_mul(0);
  }

  void update(const vector<dim_t>& v) {
    // check validation of v firstly
    UNI_LOG_CHECK(v.size() > 0, VART_INVALID_VALUE);
    for(auto i=0U; i<v.size(); i++) {
      UNI_LOG_CHECK(v[i] > 0, VART_INVALID_VALUE);
    }

    dims_ = v;

    update();
  }

  dim_t vec_mul(dim_t start_idx) const {
    UNI_LOG_CHECK(start_idx >= 0, VART_INVALID_VALUE);
    UNI_LOG_CHECK(start_idx < ndims(), VART_INVALID_VALUE);

    dim_t r = static_cast<dim_t>(1);
    for(auto i=start_idx; i<ndims(); i++) {
      r *= dim(i);
    }

    return r;
  }

private:
  dim_t num_;
  vector<dim_t> dims_;

  // coefficient of dimension
  vector<dim_t> cod_;

};

}  // namespace cpu
}  // namespace vart
