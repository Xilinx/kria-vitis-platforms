#!/usr/bin/env python

# Copyright 2019 Xilinx Inc.
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os
import re
import sys
import GenInstTable

class GenInstTableList:
    TAB = "    "
    def __init__(self, root_list):
        # get Inst's parameters
        self.root_list = root_list
        self.fname_hpp = "../InstTable.hpp"
        self.fname_cpp = "../InstTable.cpp"
        self.hpp = open(self.fname_hpp, "w")
        self.cpp = open(self.fname_cpp, "w")

        self.strVer=[]
        for idx in range(0, len(self.root_list)):
            ver = self.root_list[idx].GetVersion()
            ver = ver.replace('.', '')
            ver = ver.upper()
            self.strVer.append(ver)

        self.namespace_ver=[]
        for idx in range(0, len(self.root_list)):
            ver = self.strVer[idx]
            ver = ver.replace("V", "V_")
            self.namespace_ver.append(ver)

        self.__check_version()
        self.__write_pp()

    def __check_version(self):
        tmplist = list(set(self.strVer))
        for idx in range(0, len(tmplist)):
            print tmplist[idx]
            itCount = self.strVer.count(tmplist[idx])
            if itCount>1:
                print ("ERROR: %s repeat %d times" %(tmplist[idx], itCount))
                sys.exit()

    def __write_pp(self):
      self.__write_hpp_head()
      self.__write_cpp_head()
      #iter version
      for idx in range(0, len(self.root_list)):
        verContex = GenInstTable.GenInstTable(self.root_list[idx], self.strVer[idx])
        # write hpp
        # initial struct
        self.hpp.write("namespace " + self.namespace_ver[idx] + " {\n")
        self.hpp.write("struct InstTable {\n")
        self.hpp.write("public:\n")
        self.hpp.write(verContex.get_hpp())
        self.hpp.write("};\n")
        # setting contex
        self.hpp.write("}\n\n")
        #write cpp
        self.cpp.write("namespace " + self.namespace_ver[idx] + " {\n")
        self.cpp.write(verContex.get_cpp())
        self.cpp.write("}\n\n")
      # define TableInterface
      self.hpp.write("template<DPUVersion T> class TableInterface;\n\n")
      # specialize TableInterface
      for idx in range(0, len(self.root_list)):
        self.hpp.write("template<> class TableInterface<DPUVersion::")
        self.hpp.write(self.strVer[idx]+"> {\n")
        self.hpp.write("public:\n")
        self.hpp.write("using inst_table = "+self.namespace_ver[idx]+"::InstTable;\n")
        self.hpp.write("};\n\n")
      self.hpp.write("#endif /*__INST_TABLE_HPP__*/\n")

    # hpp file related
    # ================================================================================================
    def __write_hpp_head(self):
        self.hpp.write("#ifndef __INST_TABLE_HPP__\n")
        self.hpp.write("#define __INST_TABLE_HPP__\n")
        self.hpp.write("\n\n")
        self.hpp.write("#include <vector>\n")
        self.hpp.write("#include <string>\n")
        self.hpp.write("#include <map>\n")
        self.hpp.write("#include <unordered_map>\n\n")
        self.hpp.write("using std::vector;\n")
        self.hpp.write("using std::string;\n")
        self.hpp.write("using std::map;\n")
        self.hpp.write("using std::unordered_map;\n\n")
        # define DPUVersion
        self.hpp.write("enum class DPUVersion{\n")
        for idx in range(0, len(self.root_list)):
            self.hpp.write("\t"+self.strVer[idx]+",\n")
        self.hpp.write("};\n\n")
        # define Category
        self.hpp.write("// instruction category enum\n")
        self.hpp.write("enum class Category{\n")
        self.hpp.write("    INST_CATEGORY_MIN = 0,\n")
        self.hpp.write("    INST_CATEGORY_LOAD = INST_CATEGORY_MIN,\n")
        self.hpp.write("    INST_CATEGORY_SAVE,\n")
        self.hpp.write("    INST_CATEGORY_CONV,\n")
        self.hpp.write("    INST_CATEGORY_MISC,\n")
        self.hpp.write("    INST_CATEGORY_MAX,\n")
        self.hpp.write("};\n\n")

    def __write_cpp_head(self):
        self.cpp.write("#include <InstTable.hpp>\n")
        self.cpp.write("\n\n")
