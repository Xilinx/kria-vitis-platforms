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
#ifndef __INST_FACTORY_HPP__
#define __INST_FACTORY_HPP__

#include "pub/DumpBank.hpp"
#include "pub/DumpDDR.hpp"
#include "pub/DumpDDRSlice.hpp"
#include "pub/InstBase.hpp"

#include "CBLoad.hpp"
#include "Conv.hpp"
#include "ConvInit.hpp"
#include "DWInit.hpp"
#include "DptWise.hpp"
#include "Elew.hpp"
#include "ElewInit.hpp"
#include "End.hpp"
#include "Load.hpp"
#include "Pool.hpp"
#include "PoolInit.hpp"
#include "Save.hpp"
#include "Thd.hpp"

class InstFactory {
 public:
  InstFactory() = delete;
  ~InstFactory() = delete;

 public:
  static shared_ptr<InstBase> CreateInst(int inst_id, vector<string>& inst_str,
                                         vector<uint32_t>& inst_val);
};

#endif /* __INST_FACTORY_HPP__ */
