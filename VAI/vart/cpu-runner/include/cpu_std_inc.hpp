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

#include <fcntl.h>
#include <getopt.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <cassert>
#include <cmath>
#include <cfenv>

#include <atomic>
#include <array>
#include <bitset>
#include <deque>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <algorithm>
#include <functional>
#include <future>
#include <iomanip>
#include <limits>
#include <memory>
#include <mutex>
#include <random>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <utility>

#include <glog/logging.h>
#include <UniLog/UniLog.hpp>

namespace vart {
namespace cpu {

using std::array;
using std::async;
using std::bitset;
using std::cout;
using std::deque;
using std::endl;
using std::fstream;
using std::function;
using std::ifstream;
using std::list;
using std::make_shared;
using std::make_unique;
using std::map;
using std::mutex;
using std::ofstream;
using std::pair;
using std::shared_ptr;
using std::string;
using std::stringstream;
using std::thread;
using std::unique_ptr;
using std::unordered_map;
using std::vector;

using std::stod;
using std::stof;
using std::stoi;
using std::stol;
using std::stoul;
using std::to_string;

using std::dec;
using std::hex;
using std::left;
using std::oct;
using std::setfill;
using std::setw;

}  // namespace cpu
}  // namespace vart
