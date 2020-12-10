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

class Word:
    def __init__(self, word):
        self.word = word
        self.name = word.tag
        self.node_list = []
        self.field_list = []
        print "\tFields before reorder:"
        for child in word:
            print "\tField: " + child.tag  \
                + ", attr: " + str(child.attrib)
            self.node_list.append(child)
            self.field_list.append(Field.Field(child))
        self.field_num = len(self.node_list)

        # check Inst's parameters
        self.__check()

        # re-order field's order, by the field pos form high to low
        self.__reorder_field()

    def __check(self):
        # check all fields, it's pos must be INST_WORD_LEN finally
        cur_pos = 0
        traversed_field_num = 0
        while traversed_field_num < self.field_num:
            for field in self.field_list:
                if field.GetPos() == cur_pos:
                    cur_pos += field.GetLen()
                    break
            traversed_field_num += 1

        if cur_pos != Field.GetWordLen():
            print "Field pos or len error! word check's last pos is "\
                 + str(cur_pos) + ", it should be " + str(Field.GetWordLen())
            sys.exit()

    def __reorder_field(self):
        tmp_node_list = []
        tmp_field_list = []

        cur_pos = 0
        traversed_field_num = 0
        while traversed_field_num < self.field_num:
            for idx in range(len(self.field_list)):
                if self.field_list[idx].GetPos() == cur_pos:
                    tmp_node_list.append(self.node_list[idx])
                    tmp_field_list.append(self.field_list[idx])
                    cur_pos += self.field_list[idx].GetLen()
                    break
            traversed_field_num += 1

        tmp_node_list.reverse()
        self.node_list = tmp_node_list;
        tmp_field_list.reverse()
        self.field_list = tmp_field_list;

        # just for debug, print fields after reorder
        print "\n\tFields after reorder:"
        for field in self.field_list:
            field.Print()

    def GetFieldNum(self):
        return self.field_num

    def GetField(self, idx):
        if idx >= self.GetFieldNum():
            print "Field idx error! it's should be less than " \
                + str(self.GetFieldNum()) \
                + ", in fact it's " + str(idx)
            sys.exit()

        return self.field_list[idx]

