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

class Inst:
    def __init__(self, inst):
        # get Inst's parameters
        self.inst = inst
        self.name = inst.tag
        self.category = inst.attrib["category"]
        self.opcode_str = inst.attrib["opcode"]
        self.opcode_val = int(self.opcode_str, 16)
        self.word_num = int(inst.attrib["word_num"])
        self.node_list = []
        self.word_list = []
        for i in range (0, self.word_num):
            tmp_str = "WORD" + str(i)
            node = self.inst.find(tmp_str)
            print node.tag  + ", attr: " + str(node.attrib)
            self.node_list.append(node)
            self.word_list.append(Word.Word(node))

        # check Inst's parameters
        self.__check()

    def __check(self):
        num = 0
        for child in self.inst:
            num += 1

        if num != self.word_num:
            print "Word number error! Define is " + str(self.word_num) \
                + ", in fact it's " + str(num)
            sys.exit()

    def __reorder_word(self):
        tmp_node_list = []
        tmp_word_list = []
        for i in range (0, self.word_num-1):
            tmp_str = "WORD" + str(i)

    def GetName(self):
        return self.name

    def GetCategory(self):
        return self.category

    def GetOPCodeStr(self):
        return self.opcode_str

    def GetOPCodeVal(self):
        return self.opcode_val

    def GetWordNum(self):
        return self.word_num

    def GetWord(self, idx):
        if idx >= self.GetWordNum():
            print "Word idx error! it's should be less than " \
                + str(self.GetWordNum()) \
                + ", in fact it's " + str(idx)
            sys.exit()

        return self.word_list[idx]


