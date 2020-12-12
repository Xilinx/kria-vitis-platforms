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

import sys

import Field
import Word
import Inst

class Root:
    def __init__(self, root):
        # get Root's parameters
        self.root = root
        self.version = root.attrib["ver"]
        self.inst_num = int(root.attrib["inst_num"])
        self.word_len = int(root.attrib["word_len"])
        Field.SetWordLen(self.word_len)
        self.node_list = []
        self.inst_list = []
        i = 0
        for child in root:
            print "---------------------------------------------------------------"
            print "Inst " + str(i) + ": " + child.tag + ", attr: " + str(child.attrib)
            print "---------------------------------------------------------------"
            self.node_list.append(child)
            self.inst_list.append(Inst.Inst(child))
            i += 1

        # check Root's parameters
        self.__check()

    def __check(self):
        if len(self.node_list) != self.inst_num:
            print "Inst number error! Define is " + str(self.inst_num) \
                + ", in fact it's " + str(len(self.node_list))
            sys.exit()

    def GetVersion(self):
        return self.version

    def GetInstNum(self):
        return len(self.inst_list)

    def GetWordLen(self):
        return self.word_len

    def GetInst(self, idx):
        if idx >= self.GetInstNum():
            print "Inst idx error! it's should be less than " \
                + str(self.GetInstNum()) \
                + ", in fact it's " + str(idx)
            sys.exit()

        return self.inst_list[idx]

