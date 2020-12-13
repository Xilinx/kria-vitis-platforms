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

#include <dirent.h>
#include "vart/util_4bit.hpp"

namespace vart {
namespace cpu {

// static public function
void DelFileOrDir(const string& name) {
  struct stat buf;
  const char* fname = name.c_str();

  if (lstat(fname, &buf) == -1) {
    UNI_LOG_ERROR(VART_FILE_ERROR)
        << fname << " stat error: " << strerror(errno) << endl;
    abort();
  }

  // handle symbol link
  if (S_ISLNK(buf.st_mode)) {
    unlink(fname);
    return;
  }

  // handle common file
  if (S_ISREG(buf.st_mode)) {
    remove(fname);
    return;
  }

  // handle not directory
  if (!S_ISDIR(buf.st_mode)) {
    remove(fname);
    return;
  }

  // handle directory
  DIR* dir = NULL;
  struct dirent* ptr;

  // open directory
  if ((dir = opendir(fname)) == NULL) {
    UNI_LOG_ERROR(VART_FILE_ERROR)
        << fname << " opendir error: " << strerror(errno) << endl;
    abort();
  }

  while ((ptr = readdir(dir))) {
    if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0))
      continue;
    string next_fname = string{fname} + "/" + ptr->d_name;
    DelFileOrDir(string(next_fname));
  }

  if (closedir(dir) == -1) {
    UNI_LOG_ERROR(VART_FILE_ERROR)
        << fname << " closedir error: " << strerror(errno) << endl;
    abort();
  }

  if (rmdir(fname) == -1) {
    UNI_LOG_ERROR(VART_FILE_ERROR)
        << fname << " rmdir error: " << strerror(errno) << endl;
    abort();
  }
}

bool ChkFile(const string& name, bool del_exists) {
  bool found = (access(name.c_str(), R_OK) == 0);

  if (!del_exists || !found) return found;

  // file exists, return
  if (access(name.c_str(), R_OK | W_OK) == 0) {
    DelFileOrDir(name);
  } else {
    UNI_LOG_ERROR(VART_FILE_ERROR)
        << "No priviledge to delete " << name << endl;
    abort();
  }

  return found;
}

bool ChkFolder(const string& name, bool del_exists) {
  bool found = (access(name.c_str(), R_OK) == 0);

  if (found) {
    if (del_exists) {
      if (access(name.c_str(), R_OK | W_OK | X_OK) == 0) {
        DelFileOrDir(name);
      } else {
        UNI_LOG_ERROR(VART_FILE_ERROR)
            << "No priviledge to delete " << name << endl;
        abort();
      }
    } else {
      return found;
    }
  }

  // folder doesn't exist, create it
  if (mkdir(name.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
    UNI_LOG_ERROR(VART_FILE_ERROR) << "mkdir error: " << name << endl;
    abort();
  }
  UNI_LOG_DEBUG_INFO << "Create folder: " << name << endl;

  return found;
}

uint64_t GetFileSize(const string& fname) {
  /* NOTE: if using c++ code, seekg will return -1 sometimes, don't know why
  std::fstream f(fname);

  ChkOpen(f, fname);

  f.seekg(0, f.end);
  auto size = f.tellg();
  UNI_LOG_CHECK(size != -1, VART_SIZE_ERROR) << "Get file " << fname << " size
  error!" << endl; f.seekg(0, f.beg); f.close();

  return size;
  //*/

  struct stat buf;
  if (stat(fname.c_str(), &buf) == -1) {
    UNI_LOG_ERROR(VART_FILE_ERROR)
        << fname << " stat error: " << strerror(errno) << endl;
    abort();
  }

  return buf.st_size;
}

uint64_t GetFileLine(const string& fname) {
  std::fstream f(fname);

  ChkOpen(f, fname);

  uint64_t cnt = 0;
  while (!f.eof()) {
    string line;
    getline(f, line);
    if (line.empty()) continue;
    cnt++;
  }

  return cnt;
}

uint64_t LoadBin(const string& load_name, char* data, uint64_t size) {
  // todo, should use CHECK instead of LOG(WARNING)
  // check memory size's validation
  auto fsize = GetFileSize(load_name);
  // UNI_LOG_CHECK(size >= fsize, VART_SIZE_ERROR) << size << " < " << fsize;
  if (size < fsize) {
    UNI_LOG_ERROR(VART_BUF_SIZE_ERROR)
        << "Buffer size (" << size << ") is less than file size(" << fsize
        << ")!" << endl;
    abort();
  }

  // get file length, allocate data buf
  std::fstream f(load_name, std::ios_base::in | std::ios_base::binary);
  ChkOpen(f, load_name);

  // read file contents
  f.read(data, size);
  f.close();

  return fsize;
}

void SaveBin(const string& save_name, const char* data, uint64_t size,
             int mode) {
  // write into file
  std::ofstream f;

  if (mode == SM_TRUNC) {
    f.open(save_name, std::ios::trunc | std::ios::binary | std::ios::out);
  } else {
    f.open(save_name, std::ios::app | std::ios::binary | std::ios::out);
  }

  ChkOpen(f, save_name);

  f.write(data, size);
  f.close();
}

bool Str2Bool(const string& str) {
  string tmp;

  std::transform(str.begin(), str.end(), std::back_inserter(tmp), ::tolower);
  if (tmp == "true") return true;
  return false;
}

string Bool2Str(bool flag) {
  if (flag) return "true";
  return "false";
}

template <>
string Vec2Str<string>(const vector<string>& v, const string& split) {
  string str;

  for (auto i = 0U; i < v.size(); i++) {
    str += v[i];
    if (i != v.size() - 1) str += split;
  }

  return str;
}

template <>
string Vec2Str<bool>(const vector<bool>& v, const string& split) {
  string str;

  for (auto i = 0U; i < v.size(); i++) {
    str += Bool2Str(v[i]);
    if (i != v.size() - 1) str += split;
  }

  return str;
}

int64_t Str2S64(const string& str) {
  const string HEX_CHARS = "xXabcdefABCDEF";

  int64_t val = 0;

  if (str.find_first_of(HEX_CHARS) == string::npos)
    val = stoll(str, nullptr, 10);
  else
    val = stoll(str, nullptr, 16);

  return val;
}

uint64_t Str2U64(const string& str) {
  const string HEX_CHARS = "xXabcdefABCDEF";

  int64_t val = 0;

  if (str.find_first_of(HEX_CHARS) == string::npos)
    val = stoull(str, nullptr, 10);
  else
    val = stoull(str, nullptr, 16);

  return val;
}

uint32_t FillMaskU32(int start, int num) {
  UNI_LOG_CHECK(start >= 0 && start < 32, VART_SIZE_ERROR);
  UNI_LOG_CHECK(num >= 0 && num <= 32, VART_SIZE_ERROR);

  uint32_t mask = 0x0U;

  for (auto pos = start; pos < start + num; pos++) {
    auto x = pos % 32;
    mask |= (0x1U << x);
  }

  return mask;
}

uint64_t FillMaskU64(int start, int num) {
  UNI_LOG_CHECK(start >= 0 && start < 64, VART_SIZE_ERROR);
  UNI_LOG_CHECK(num >= 0 && num <= 64, VART_SIZE_ERROR);

  uint64_t mask = 0x0U;

  for (auto pos = start; pos < start + num; pos++) {
    auto x = pos % 64;
    mask |= (0x1U << x);
  }

  return mask;
}

uint64_t AlignByN(uint64_t data, uint64_t align) {
  UNI_LOG_CHECK(align > 0, VART_SIZE_ERROR);
  uint64_t rlt = ((data + align - 1) / align) * align;
  return rlt;
}

string GetFileNameSuffix(int fmt) {
  string suffix;
  if (fmt == DATA_FMT_BIN)
    suffix = ".bin";
  else if (fmt == DATA_FMT_DEC)
    suffix = ".dec";
  else if (fmt == DATA_FMT_HEX_CONT_SMALLEND)
    suffix = ".hex.cont.smallend";
  else if (fmt == DATA_FMT_HEX_CONT_BIGEND)
    suffix = ".hex.cont.bigend";
  else if (fmt == DATA_FMT_HEX_CONT_SMALLEND_BANKADDR)
    suffix = ".hex.cont.smallend.bankaddr";
  else if (fmt == DATA_FMT_HEX_CONT_BIGEND_BANKADDR)
    suffix = ".hex.cont.bigend.bankaddr";
  else if (fmt == DATA_FMT_HEX_CONT_SMALLEND_DDRADDR)
    suffix = ".hex.cont.smallend.ddraddr";
  else if (fmt == DATA_FMT_HEX_CONT_BIGEND_DDRADDR)
    suffix = ".hex.cont.bigend.ddraddr";
  else {
    UNI_LOG_ERROR(VART_NOT_SUPPORT) << "Not support fmt " << fmt << endl;
    abort();
  }

  return suffix;
}

string Trim(const string& str) {
  auto tmp = str;

  // erase whitespace before the string
  string::iterator it1;
  for (it1 = tmp.begin(); it1 < tmp.end(); it1++) {
    if (!isspace(*it1)) break;
  }
  tmp.erase(0, it1 - tmp.begin());

  // erase whitespace after the string
  string::reverse_iterator it2;
  for (it2 = tmp.rbegin(); it2 < tmp.rend(); it2++) {
    if (!isspace(*it2)) break;
  }
  tmp.erase(tmp.rend() - it2, it2 - tmp.rbegin());

  return tmp;
}

// split all words in the string
vector<string> Split(const string& str, const string& delim, bool trim_flag) {
  vector<string> result;

  string::size_type start = 0;
  string::size_type pos = 0;
  do {
    pos = str.find_first_of(delim, start);

    int len = (pos == string::npos) ? (str.size() - start) : (pos - start);
    if (len) {
      string substr =
          trim_flag ? Trim(str.substr(start, len)) : str.substr(start, len);
      if (!substr.empty()) {
        result.emplace_back(substr);
      }
    }
    start = pos + 1;
  } while (pos != string::npos);

  return result;
}

// split first word in the string
string SplitFirst(const string& str, const string& delim, bool trim_flag) {
  vector<string> v = Split(str, delim, trim_flag);

  if (v.empty()) return string("");
  return v[0];
}

void time_start(struct timeval& start) { gettimeofday(&start, 0); }

unsigned long time_finish(const struct timeval& start, int type) {
  struct timeval finish;
  gettimeofday(&finish, 0);
  if (type == TIME_UNIT_S) {
    return (1000000 * (finish.tv_sec - start.tv_sec) +
            (finish.tv_usec - start.tv_usec)) /
           1000000;
  } else if (type == TIME_UNIT_MS) {
    return (1000000 * (finish.tv_sec - start.tv_sec) +
            (finish.tv_usec - start.tv_usec)) /
           1000;
  } else {
    return (1000000 * (finish.tv_sec - start.tv_sec) +
            (finish.tv_usec - start.tv_usec)) /
           1;
  }
}

float Py3Round(float input) {
  float ret;
  auto default_round_mode = std::fegetround();
  if (!(FE_TONEAREST == default_round_mode)) {
    std::fesetround(FE_TONEAREST);
    ret = std::nearbyint(input);
    std::fesetround(default_round_mode);
  } else {
    ret = std::nearbyint(input);
  }

  return ret;
}

string tensor_name_2_save_name(string tensor_name) {
  for (auto i = 0U; i < tensor_name.size(); i++) {
    if (tensor_name[i] == '/') tensor_name[i] = '_';
  }
  return SplitFirst(tensor_name, "(", true);
}

}  // namespace cpu
}  // namespace vart
