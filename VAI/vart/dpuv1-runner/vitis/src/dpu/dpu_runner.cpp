// Copyright 2019 Xilinx Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "dpu/dpu_runner.hpp"
#include <dlfcn.h>
#include <glog/logging.h>
#include <json-c/json.h>
#include <fstream>
#include <iostream>

namespace vitis {
namespace ai {
static std::string safe_read_string(json_object* value,
                                    const std::string& key) {
  json_object* field = nullptr;
  CHECK(json_object_object_get_ex(value, key.c_str(), &field))
      << "no such field! key=" << key
      << ", value=" << json_object_to_json_string(value);
  CHECK(json_object_is_type(field, json_type_string))
      << "not a string! key=" << key
      << ", value=" << json_object_to_json_string(value);
  return json_object_get_string(field);
}

static std::vector<std::string> safe_read_string_or_vec_string(
    json_object* value, const std::string& key) {
  json_object* field = nullptr;
  CHECK(json_object_object_get_ex(value, key.c_str(), &field))
      << "no such field! key=" << key
      << ", value=" << json_object_to_json_string(value);
  CHECK(json_object_is_type(field, json_type_string) ||
        json_object_is_type(field, json_type_array))
      << "not a string or array of string ! key=" << key
      << ", value=" << json_object_to_json_string(value);
  if (json_object_is_type(field, json_type_string)) {
    return std::vector<std::string>{std::string(json_object_get_string(field))};
  }
  // must be an array
  auto ret = std::vector<std::string>{};
  auto size = json_object_array_length(field);
  ret.reserve(size);
  for (decltype(size) idx = 0; idx < size; ++idx) {
    auto elt = json_object_array_get_idx(field, idx);
    CHECK(json_object_is_type(elt, json_type_string))
        << "element is not a string or array of string ! key="
        << ", idx=" << idx << ", value=" << json_object_to_json_string(value);
    ret.emplace_back(json_object_get_string(elt));
  }
  return ret;
}
static json_object* read_json_from_directory(
    const std::string& model_directory) {
  auto meta_filename = model_directory + "/" + "meta.json";
  json_object* value = json_object_from_file(meta_filename.c_str());
  CHECK(value != nullptr) << "failed to read meta file! filename="
                          << meta_filename;
  CHECK(json_object_is_type(value, json_type_object))
      << "not a json object. value=" << json_object_to_json_string(value);
  return value;
}
static std::string get_target(json_object* value) {
  return safe_read_string(value, "target");
}

static std::string safe_read_string_as_file_name(json_object* value,
                                                 const std::string& key,
                                                 const std::string& dirname) {
  auto filename = safe_read_string(value, key);
  return (filename[0] != '/') ? filename : dirname + "/" + filename;
}

static DpuMeta read_dpu_meta_from_value(json_object* value,
                                        const std::string& dirname) {
  std::string target = safe_read_string(value, "target");
  std::string lib = safe_read_string(value, "lib");
  DpuMeta ret;
  ret.target = target;
  ret.lib = lib;
  ret.dirname = dirname;
  ret.filename = safe_read_string_as_file_name(value, "filename", dirname);
  ret.kernels = safe_read_string_or_vec_string(value, "kernel");
  ret.config_file =
      safe_read_string_as_file_name(value, "config_file", dirname);
  return ret;
}

static std::vector<std::unique_ptr<vitis::ai::DpuRunner>>*
create_dpu_runner_by_meta(const DpuMeta& dpuMeta) {
  typedef std::vector<std::unique_ptr<vitis::ai::DpuRunner>>* (*INIT_FUN)(
      const DpuMeta& dpuMeta);
  INIT_FUN init_fun = NULL;
  auto handle = dlopen(dpuMeta.lib.c_str(), RTLD_LAZY);
  CHECK(handle != NULL) << "cannot open library!"
                        << " lib=" << dpuMeta.lib << ";error=" << dlerror();
  dlerror();
  init_fun = (INIT_FUN)dlsym(handle, "create_dpu_runner");
  CHECK(init_fun != NULL) << "cannot load symbol 'create_dpu_runner'!"
                          << "! lib=" << dpuMeta.lib << ";error=" << dlerror();
  return init_fun(dpuMeta);
}

std::vector<std::unique_ptr<vitis::ai::DpuRunner>> DpuRunner::create_dpu_runner(
    const std::string& model_directory) {
  auto value = read_json_from_directory(model_directory);
  auto target = get_target(value);
  // CHECK_EQ(target, "DPUv2")
  //    << "target must be DPUv2! value=" << json_object_to_json_string(value);
  auto dpu_meta = read_dpu_meta_from_value(value, model_directory);
  dpu_meta.dirname = model_directory;
  auto ret =
      std::unique_ptr<std::vector<std::unique_ptr<vitis::ai::DpuRunner>>>(
          create_dpu_runner_by_meta(dpu_meta));
  json_object_put(value);
  return std::move(*ret.get());
}

}  // namespace ai
}  // namespace vitis
