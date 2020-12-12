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

#include "cpu_util.hpp"
#include "load_cfg.hpp"

namespace vart {
namespace cpu {

const string LoadCfg::HEX_CHARS = "xXabcdefABCDEF";

// constructors and deconstructors
LoadCfg::LoadCfg(const string& fname)
    : fname_(fname) {
  read_file();
}

LoadCfg::~LoadCfg() {}

// public funcs
string LoadCfg::operator[] (const string &key) {
  UNI_LOG_CHECK(cfg_.find(key) != cfg_.end(), VART_NOT_FOUND)
    << ", Not found config key: " << key << endl;

  return cfg_.at(key);
}

bool LoadCfg::HasKey(const string& key) {
  if(cfg_.find(key) == cfg_.end())
    return false;
  return true;
}


// private funcs
void LoadCfg::read_file() {
  ifstream ifs(fname_);

  if(ifs.fail()) {
    UNI_LOG_ERROR(VART_FILE_ERROR) << "Open error: " << fname_ << endl;
    abort();
  } else {
    // UNI_LOG_DEBUG_INFO << "Open file " << fname_ << " successfully!" << endl;
  }

  while(!ifs.eof()) {
    string line;

    // get one line from file
    getline(ifs, line);
    line = Trim(line);
    if(line.size() == 0) { // empty line
      continue;
    } else if(line[0] == '#') { // annotation line
      continue;
    } else if(line[0] == '/' && line[1] == '/') { // annotation line
      continue;
    }

    // split current line's every field,
    // store all key-value of this line into a branch
    // UNI_LOG_DEBUG_INFO << "Handling line: " << line << endl;

    auto vec = Split(line, ":");
    if(vec.size() == 1) { // only a graph tip, not handle
      continue;
    } else if(vec.size() < 1 || vec.size() > 2) { // key: value number invalid
      UNI_LOG_ERROR(VART_INVALIDE_PARAM) << "Invalid line: " << line << endl;
      abort();
    } else { // check key: value pair
      check_key(vec[0]);
      check_value(vec[1]);

      cfg_[vec[0]] = vec[1];
    }

  }
}

void LoadCfg::check_key(const string& key) {
  if(cfg_.find(key) != cfg_.end()) {
    UNI_LOG_ERROR(VART_FOUND) << "config key existed: " << key << endl;
    abort();
  }
}

void LoadCfg::check_value(const string& value) {
  UNI_LOG_CHECK(value.size() > 0, VART_INVALID_VALUE) << " : invalid value "
    << value << endl;

  auto val = Trim(value);

  if(val[0] == '[') {
    UNI_LOG_CHECK(val.back() == ']', VART_INVALID_VALUE) << " : invalid value "
      << value << endl;
    UNI_LOG_CHECK(val.size() > 2, VART_INVALID_VALUE) << " : invalid value "
      << value << endl;
  } else if(val.back() == ']') {
    UNI_LOG_CHECK(val[0] == '[', VART_INVALID_VALUE) << " : invalid value "
      << value << endl;
    UNI_LOG_CHECK(val.size() > 2, VART_INVALID_VALUE) << " : invalid value "
      << value << endl;
  } else {
    auto v = Split(value, " ,\t");
    UNI_LOG_CHECK(v.size() == 1, VART_INVALID_VALUE) << " : invalid value "
      << value << endl;
  }
}

template<>
string LoadCfg::GetValue<string>(const string& key, const string& dft_val, bool use_dft_val) {
  const string HEX_CHARS = "xXabcdefABCDEF";

  if(!HasKey(key)) {
    if(use_dft_val) {
      return dft_val;
    } else {
      UNI_LOG_ERROR(VART_NOT_FOUND) << "Not found key " << key << endl;
      abort();
    }
  } else {
    return cfg_.at(key);
  }
}

template<>
bool LoadCfg::GetValue<bool>(const string& key, const bool& dft_val, bool use_dft_val) {
  if(!HasKey(key)) {
    if(use_dft_val) {
      return dft_val;
    } else {
      UNI_LOG_ERROR(VART_NOT_FOUND) << "Not found key " << key << endl;
      abort();
    }
  } else {
    string val_str = cfg_.at(key);

    return Str2Bool(val_str);
  }
}

template<>
int LoadCfg::GetValue<int>(const string& key, const int& dft_val, bool use_dft_val) {
  if(!HasKey(key)) {
    if(use_dft_val) {
      return dft_val;
    } else {
      UNI_LOG_ERROR(VART_NOT_FOUND) << "Not found key " << key << endl;
      abort();
    }
  } else {
    string val_str = cfg_.at(key);

    if(val_str.find_first_of(HEX_CHARS) == string::npos)
      return stoi(val_str, nullptr, 10);
    else
      return stoi(val_str, nullptr, 16);
  }
}

template<>
long LoadCfg::GetValue<long>(const string& key, const long& dft_val, bool use_dft_val) {
  if(!HasKey(key)) {
    if(use_dft_val) {
      return dft_val;
    } else {
      UNI_LOG_ERROR(VART_NOT_FOUND) << "Not found key " << key << endl;
      abort();
    }
  } else {
    string val_str = cfg_.at(key);

    if(val_str.find_first_of(HEX_CHARS) == string::npos)
      return stol(val_str, nullptr, 10);
    else
      return stol(val_str, nullptr, 16);
  }
}

template<>
uint64_t LoadCfg::GetValue<uint64_t>(const string& key, const uint64_t& dft_val, bool use_dft_val) {
  if(!HasKey(key)) {
    if(use_dft_val) {
      return dft_val;
    } else {
      UNI_LOG_ERROR(VART_NOT_FOUND) << "Not found key " << key << endl;
      abort();
    }
  } else {
    string val_str = cfg_.at(key);

    if(val_str.find_first_of(HEX_CHARS) == string::npos)
      return stoull(val_str, nullptr, 10);
    else
      return stoull(val_str, nullptr, 16);
  }
}

template<>
float LoadCfg::GetValue<float>(const string& key, const float& dft_val, bool use_dft_val) {
  if(!HasKey(key)) {
    if(use_dft_val) {
      return dft_val;
    } else {
      UNI_LOG_ERROR(VART_NOT_FOUND) << "Not found key " << key << endl;
      abort();
    }
  } else {
    string val_str = cfg_.at(key);
    return stof(val_str);
  }
}

vector<string> LoadCfg::GetVector(const string& key) {
  if(!HasKey(key)) {
    UNI_LOG_ERROR(VART_NOT_FOUND) << "Not found key " << key << endl;
    abort();
  }

  string val = cfg_.at(key);
  UNI_LOG_CHECK(val.size() > 0, VART_INVALID_VALUE);

  if(val[0] == '[') {
    val.erase(val.begin());
    val.erase(val.end()-1);
  }

  auto vec = Split(val);
  UNI_LOG_CHECK(val.size() > 0, VART_INVALID_VALUE);

  return vec;
}

} // namespace cpu
} // namespace vart

