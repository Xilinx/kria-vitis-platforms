#/usr/bin/env python

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

INST_WORD_LEN = 32

def SetWordLen(word_len):
    global INST_WORD_LEN
    INST_WORD_LEN = word_len
def GetWordLen():
    global INST_WORD_LEN
    return INST_WORD_LEN


class Field:
    def __init__(self, field):
        # get Field's parameters
        self.field = field
        self.name = field.tag
        self.attrib = field.attrib
        self.type = self.attrib["type"]
        self.pos = int(self.attrib["pos"])
        self.len = int(self.attrib["len"])
        self.minus = 0
        if self.attrib.has_key("minus"):
            self.minus = int(self.attrib["minus"])

        # check Field's parameters
        self.__check()

    def __check(self):
        if self.pos < 0 or self.pos >= GetWordLen():
            print "Field pos error! should be 0~" + str(GetWordLen()-1) \
                + ", in fact it's " + str(self.pos)
            sys.exit()
        if self.len < 1 or self.len > GetWordLen():
            print "Field len error! should be 1~" + str(GetWordLen()) \
                + ", in fact it's " + str(self.len)
            sys.exit()
        if (self.pos + self.len) > GetWordLen():
            print "Field pos+len error! should be 1~" + str(GetWordLen()) \
                + ", in fact it's " + str(self.pos + self.len)
            sys.exit()

    def GetName(self):
        return self.name

    def GetType(self):
        return self.type

    def GetPos(self):
        return self.pos

    def GetLen(self):
        return self.len

    def GetMinus(self):
        return self.minus

    def Print(self):
        print "\tField: " + self.name  \
            + ", attr: " + str(self.attrib)

