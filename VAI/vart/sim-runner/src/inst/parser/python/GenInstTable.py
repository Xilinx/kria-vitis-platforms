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
import sys

class GenInstTable:
    TAB = "    "
    def __init__(self, root, strVer):
        # get Inst's parameters
        self.root = root
        self.strVer = strVer


    # hpp file related
    # ================================================================================================
    def get_hpp(self):
        contex = ''
        contex += self.__write_hpp_inst_version()
        #contex += self.__write_hpp_inst_category()
        contex += self.__write_hpp_inst_type()
        contex += self.__write_hpp_inst_field()
        contex += self.__write_hpp_inst_field_data_type()
        contex += self.__write_hpp_var()
        return contex

    def __write_hpp_inst_version(self):
        line = ''
        line += (self.TAB + "// instruction version\n")
        line += (self.TAB + "const static string INST_VERSION;")
        line += ("\n\n\n")
        return line

    def __write_hpp_inst_category(self):
        line = ''
        line += (self.TAB + "// instruction category enum\n")
        line += (self.TAB + "enum InstCategoryEnum {\n")
        # write INST_TYPE_MIN
        line += (self.TAB + self.TAB + "INST_CATEGORY_MIN = 0,\n")
        line += (self.TAB + self.TAB + "INST_CATEGORY_LOAD = INST_CATEGORY_MIN,\n")
        line += (self.TAB + self.TAB + "INST_CATEGORY_SAVE,\n")
        line += (self.TAB + self.TAB + "INST_CATEGORY_CONV,\n")
        line += (self.TAB + self.TAB + "INST_CATEGORY_MISC,\n")
        # write INST_TYPE_MAX
        line += (self.TAB + self.TAB + "INST_CATEGORY_MAX,\n")
        line += (self.TAB + "};\n\n\n")
        return line

    def __write_hpp_inst_type(self):
        line = ''
        line += (self.TAB + "// instruction type enum\n")
        line += (self.TAB + "enum InstTypeEnum {\n")

        # write INST_TYPE_MIN
        line += (self.TAB + self.TAB + "INST_TYPE_MIN = 0,\n")

        # write every inst macro
        for idx in range(0, self.root.GetInstNum()):
            inst = self.root.GetInst(idx)
            inst_name = inst.GetName()
            if idx == 0:
                line += (self.TAB + self.TAB + "INST_TYPE_" + inst_name.upper() + " = INST_TYPE_MIN,")
            else:
                line += (self.TAB + self.TAB + "INST_TYPE_" + inst_name.upper() + ",")
            line += (" // " + str(idx) + "\n")

        # write INST_TYPE_MAX
        line += (self.TAB + self.TAB + "INST_TYPE_MAX,")
        line += (" // " + str(self.root.GetInstNum()) + "\n")
        line += (self.TAB + "};\n\n\n")
        return line

    def __write_hpp_inst_field(self):
        line = ''
        for i in range(0, self.root.GetInstNum()):
            inst = self.root.GetInst(i)
            inst_name = inst.GetName()
            prefix = inst_name.upper() + "_FIELD_";

            line += (self.TAB + "// " + inst_name.upper() + " field enum\n")
            line += (self.TAB + "enum " + inst_name + "FieldEnum {\n")

            real_field_num = 0
            for j in range(0, inst.GetWordNum()):
                word = inst.GetWord(j)
                for k in range(0, word.GetFieldNum()):
                    field = word.GetField(k)
                    field_name = field.GetName()
                    if field_name == "reserved":
                        continue
                    line += (self.TAB + self.TAB + prefix + field_name.upper() + ",")
                    line += (" // " + str(real_field_num) + "\n")
                    real_field_num += 1

            # write XXX_MAX
            line += (self.TAB + self.TAB + prefix + "MAX,")
            line += (" // " + str(real_field_num) + "\n")
            line += (self.TAB + "};\n")

            # define var like this: const static vector<string> xxxFieldName;
            line += (self.TAB + "const static vector<string> " + inst_name + "FieldName;\n")
            line += (self.TAB + "const static vector<int> " + inst_name + "FieldDataType;\n")
            line += (self.TAB + "const static vector<int> " + inst_name + "FieldMinus;\n")
            line += (self.TAB + "const static vector<map<uint32_t, uint32_t>> " + inst_name + "FieldPos;\n")
            line += (self.TAB + "const static vector<map<uint32_t, uint32_t>> " + inst_name + "FieldLen;\n\n\n")
        return line

    def __write_hpp_inst_field_data_type(self):
        line = ''
        line += (self.TAB + "// instruction field data type enum\n")
        line += (self.TAB + "enum InstFieldDataTypeEnum {\n")

        # write INST_TYPE_MIN
        line += (self.TAB + self.TAB + "INST_FIELD_DATA_TYPE_MIN = 0,\n")
        line += (self.TAB + self.TAB + "INST_FIELD_DATA_TYPE_STRING = INST_FIELD_DATA_TYPE_MIN,\n")
        line += (self.TAB + self.TAB + "INST_FIELD_DATA_TYPE_UINT,\n")

        # write INST_TYPE_MAX
        line += (self.TAB + self.TAB + "INST_FIELD_DATA_TYPE_MAX,\n")
        line += (self.TAB + "};\n\n\n")
        return line

    def __write_hpp_var(self):
        line = ''
        line += (self.TAB + "// global vector var\n")
        line += (self.TAB + "const static vector<string> CategoryName;\n")
        line += (self.TAB + "const static vector<string> InstName;\n")
        line += (self.TAB + "const static vector<string> InstNameLower;\n")
        line += (self.TAB + "const static vector<Category> InstCategory;\n")
        line += (self.TAB + "const static vector<uint32_t> WordNum;\n")
        line += (self.TAB + "const static vector<uint32_t> FieldNum;\n")
        line += (self.TAB + "const static vector<uint32_t> OPCode;\n")
        line += ("\n\n")
        line += (self.TAB + "// global map var\n")
        line += (self.TAB + "const static unordered_map<uint32_t, uint32_t> OPCode2InstType;\n")
        line += (self.TAB + "const static unordered_map<string, uint32_t> InstName2InstType;\n")
        line += (self.TAB + "const static unordered_map<uint32_t, const vector<string> &> InstFieldName;\n")
        line += (self.TAB + "const static unordered_map<uint32_t, const vector<int> &> InstFieldDataType;\n")
        line += (self.TAB + "const static unordered_map<uint32_t, const vector<int> &> InstFieldMinus;\n")
        line += (self.TAB + "const static unordered_map<uint32_t, const vector<map<uint32_t, uint32_t>> &> InstFieldPos;\n")
        line += (self.TAB + "const static unordered_map<uint32_t, const vector<map<uint32_t, uint32_t>> &> InstFieldLen;\n")
        return line

    # cpp file related
    # ================================================================================================
    def get_cpp(self):
        contex = ''
        contex += self.__write_cpp_inst_version()
        contex += self.__write_cpp_category_name()
        contex += self.__write_cpp_inst_name()
        contex += self.__write_cpp_inst_name_lower()
        contex += self.__write_cpp_inst_category()
        contex += self.__write_cpp_word_num()
        contex += self.__write_cpp_field_num()
        contex += self.__write_cpp_opcode()
        contex += self.__write_cpp_field_name()
        contex += self.__write_cpp_field_data_type()
        contex += self.__write_cpp_field_minus()
        contex += self.__write_cpp_field_pos()
        contex += self.__write_cpp_field_len()
        contex += self.__write_cpp_opcode_2_inst_type()
        contex += self.__write_cpp_inst_name_2_inst_type()
        contex += self.__write_cpp_inst_field_name()
        contex += self.__write_cpp_inst_field_data_type()
        contex += self.__write_cpp_inst_field_minus()
        contex += self.__write_cpp_inst_field_pos()
        contex += self.__write_cpp_inst_field_len()
        return contex


    def __write_cpp_inst_version(self):
        line = ''
        line += ("// instruction version\n")
        line += ("const string InstTable::INST_VERSION = ")
        line += ("\"" + self.root.GetVersion() + "\";")
        line += ("\n\n\n")
        return line

    def __write_cpp_category_name(self):
        line = ''
        line += ("// category name vector\n")
        line += ("const vector<string> InstTable::CategoryName = {\n")

        line += (self.TAB + "\"LOAD\",\n")
        line += (self.TAB + "\"SAVE\",\n")
        line += (self.TAB + "\"CONV\",\n")
        line += (self.TAB + "\"MISC\",\n")

        line += ("};\n\n\n")
        return line

    def __write_cpp_inst_name(self):
        line = ''
        line += ("// inst name vector\n")
        line += ("const vector<string> InstTable::InstName = {\n")

        # write every inst name
        for idx in range(0, self.root.GetInstNum()):
            inst = self.root.GetInst(idx)
            inst_name = inst.GetName()
            line += (self.TAB + "\"" + inst_name.upper() + "\",")
            line += (" // " + str(idx) + "\n")

        line += ("};\n\n\n")
        return line

    def __write_cpp_inst_name_lower(self):
        line = ''
        line += ("// lower inst name vector\n")
        line += ("const vector<string> InstTable::InstNameLower = {\n")

        # write every inst name
        for idx in range(0, self.root.GetInstNum()):
            inst = self.root.GetInst(idx)
            inst_name = inst.GetName()
            line += (self.TAB + "\"" + inst_name.lower() + "\",")
            line += (" // " + str(idx) + "\n")

        line += ("};\n\n\n")
        return line


    def __write_cpp_inst_category(self):
        line = ''
        line += ("// inst category\n")
        line += ("const vector<Category> InstTable::InstCategory = {\n")

        # write every inst's word number
        for idx in range(0, self.root.GetInstNum()):
            inst = self.root.GetInst(idx)
            category = inst.GetCategory()
            line += (self.TAB + "Category::INST_CATEGORY_" + category.upper() + ",")
            line += (" // " + inst.GetName().upper() + "\n")

        line += ("};\n\n\n")
        return line

    def __write_cpp_word_num(self):
        line = ''
        line += ("// inst word number\n")
        line += ("const vector<uint32_t> InstTable::WordNum = {\n")

        # write every inst's word number
        for idx in range(0, self.root.GetInstNum()):
            inst = self.root.GetInst(idx)
            line += (self.TAB + str(inst.GetWordNum()) + ",")
            line += (" // " + inst.GetName().upper() + "\n")

        line += ("};\n\n\n")
        return line

    def __write_cpp_field_num(self):
        line = ''
        line += ("// inst field number\n")
        line += ("const vector<uint32_t> InstTable::FieldNum = {\n")

        # write every inst's field number
        for idx in range(0, self.root.GetInstNum()):
            inst = self.root.GetInst(idx)
            line += (self.TAB + inst.GetName().upper() + "_FIELD_MAX,")
            line += (" // " + str(idx) + "\n")

        line += ("};\n\n\n")
        return line

    def __write_cpp_opcode(self):
        line = ''
        line += ("// inst opcode\n")
        line += ("const vector<uint32_t> InstTable::OPCode = {\n")

        # write every inst's opcode
        for idx in range(0, self.root.GetInstNum()):
            inst = self.root.GetInst(idx)
            line += (self.TAB + inst.GetOPCodeStr() + ",")
            line += (" // " + inst.GetName().upper() + "\n")

        line += ("};\n\n\n")
        return line

    def __write_cpp_field_name(self):
        line = ''
        for i in range(0, self.root.GetInstNum()):
            inst = self.root.GetInst(i)
            inst_name = inst.GetName()

            # write every inst's field name
            line += ("// " + inst_name + " field name\n")
            line += ("const vector<string> InstTable::" + inst_name + "FieldName = {\n")

            real_field_num = 0
            for j in range(0, inst.GetWordNum()):
                word = inst.GetWord(j)
                for k in range(0, word.GetFieldNum()):
                    field = word.GetField(k)
                    field_name = field.GetName()
                    if field_name == "reserved":
                        continue
                    line += (self.TAB + "\"" + field_name + "\",")
                    line += (" // " + str(real_field_num) + "\n")
                    real_field_num += 1
            line += ("};\n\n\n")
        return line

    def __write_cpp_field_data_type(self):
        line = ''
        for i in range(0, self.root.GetInstNum()):
            inst = self.root.GetInst(i)
            inst_name = inst.GetName()

            # write every inst's field data type
            line += ("// " + inst_name + " field data type\n")
            line += ("const vector<int> InstTable::" + inst_name + "FieldDataType = {\n")

            real_field_num = 0
            for j in range(0, inst.GetWordNum()):
                word = inst.GetWord(j)
                for k in range(0, word.GetFieldNum()):
                    field = word.GetField(k)
                    field_name = field.GetName()
                    field_type = field.GetType()
                    if field_name == "reserved":
                        continue
                    line += (self.TAB + "INST_FIELD_DATA_TYPE_" + field_type.upper() + ",")
                    line += (" // " + field_name + "(" + str(real_field_num) + ")\n")
                    real_field_num += 1
            line += ("};\n\n\n")
        return line

    def __write_cpp_field_minus(self):
        line = ''
        for i in range(0, self.root.GetInstNum()):
            inst = self.root.GetInst(i)
            inst_name = inst.GetName()

            # write every inst's field data type
            line += ("// " + inst_name + " field minus value\n")
            line += ("const vector<int> InstTable::" + inst_name + "FieldMinus = {\n")

            real_field_num = 0
            for j in range(0, inst.GetWordNum()):
                word = inst.GetWord(j)
                for k in range(0, word.GetFieldNum()):
                    field = word.GetField(k)
                    field_name = field.GetName()
                    field_minus = field.GetMinus()
                    if field_name == "reserved":
                        continue
                    line += (self.TAB + str(field_minus) + ",")
                    line += (" // " + field_name + "(" + str(real_field_num) + ")\n")
                    real_field_num += 1
            line += ("};\n\n\n")
        return line

    def __write_cpp_field_pos(self):
        line = ''
        for i in range(0, self.root.GetInstNum()):
            inst = self.root.GetInst(i)
            inst_name = inst.GetName()

            # write every inst's field postion
            line += ("// " + inst_name + " field position\n")
            line += ("const vector<map<uint32_t, uint32_t>> InstTable::" + inst_name + "FieldPos = {\n")

            real_field_num = 0
            for j in range(0, inst.GetWordNum()):
                word = inst.GetWord(j)
                line += (self.TAB + "map<uint32_t, uint32_t> {\n")
                for k in range(0, word.GetFieldNum()):
                    field = word.GetField(k)
                    field_name = field.GetName()
                    field_pos = field.GetPos()
                    if field_name == "reserved":
                        continue
                    line += (self.TAB + self.TAB +"{ " + inst_name.upper() \
                            + "_FIELD_" + field_name.upper() + ", " + str(field_pos) + " },")
                    line += (" // " + str(real_field_num) + "\n")
                    real_field_num += 1
                line += (self.TAB + "},\n")
            line += ("};\n\n\n")
        return line

    def __write_cpp_field_len(self):
        line = ''
        for i in range(0, self.root.GetInstNum()):
            inst = self.root.GetInst(i)
            inst_name = inst.GetName()

            # write every inst's field length
            line += ("// " + inst_name + " field length\n")
            line += ("const vector<map<uint32_t, uint32_t>> InstTable::" + inst_name + "FieldLen = {\n")

            real_field_num = 0
            for j in range(0, inst.GetWordNum()):
                word = inst.GetWord(j)
                line += (self.TAB + "map<uint32_t, uint32_t> {\n")
                for k in range(0, word.GetFieldNum()):
                    field = word.GetField(k)
                    field_name = field.GetName()
                    field_len = field.GetLen()
                    if field_name == "reserved":
                        continue
                    line += (self.TAB + self.TAB +"{ " + inst_name.upper() \
                            + "_FIELD_" + field_name.upper() + ", " + str(field_len) + " },")
                    line += (" // " + str(real_field_num) + "\n")
                    real_field_num += 1
                line += (self.TAB + "},\n")
            line += ("};\n\n\n")
        return line

    def __write_cpp_opcode_2_inst_type(self):
        line = ''
        line += ("// inst opcode to inst type map\n")
        line += ("const unordered_map<uint32_t, uint32_t> InstTable::OPCode2InstType = {\n")

        # write every inst's opcode
        for idx in range(0, self.root.GetInstNum()):
            inst = self.root.GetInst(idx)
            line += (self.TAB + "{ ")
            line += (inst.GetOPCodeStr() + ", ")
            line += ("INST_TYPE_" + inst.GetName().upper() + " },")
            line += (" // " + str(idx) + "\n")

        line += ("};\n\n\n")
        return line

    def __write_cpp_inst_name_2_inst_type(self):
        line = ''
        line += ("// inst name to inst type map\n")
        line += ("const unordered_map<string, uint32_t> InstTable::InstName2InstType = {\n")

        # write every inst's opcode
        for idx in range(0, self.root.GetInstNum()):
            inst = self.root.GetInst(idx)
            line += (self.TAB + "{ ")
            line += ("\"" + inst.GetName().upper() + "\", ")
            line += ("INST_TYPE_" + inst.GetName().upper() + " },")
            line += (" // " + str(idx) + "\n")

        line += ("};\n\n\n")
        return line

    def __write_cpp_inst_field_name(self):
        line = ''
        line += ("// inst type to field name map\n")
        line += ("const unordered_map<uint32_t, const vector<string> &> InstTable::InstFieldName = {\n")

        # write every inst's type map to field name vector
        for idx in range(0, self.root.GetInstNum()):
            inst = self.root.GetInst(idx)
            line += (self.TAB + "{ ")
            line += ("INST_TYPE_" + inst.GetName().upper() + ", ")
            line += (inst.GetName() + "FieldName },")
            line += (" // " + str(idx) + "\n")

        line += ("};\n\n\n")
        return line

    def __write_cpp_inst_field_data_type(self):
        line = ''
        line += ("// inst type to field data type map\n")
        line += ("const unordered_map<uint32_t, const vector<int> &> InstTable::InstFieldDataType = {\n")

        # write every inst's type map to field data type vector
        for idx in range(0, self.root.GetInstNum()):
            inst = self.root.GetInst(idx)
            line += (self.TAB + "{ ")
            line += ("INST_TYPE_" + inst.GetName().upper() + ", ")
            line += (inst.GetName() + "FieldDataType },")
            line += (" // " + str(idx) + "\n")

        line += ("};\n\n\n")
        return line

    def __write_cpp_inst_field_minus(self):
        line = ''
        line += ("// inst type to field minus map\n")
        line += ("const unordered_map<uint32_t, const vector<int> &> InstTable::InstFieldMinus = {\n")

        # write every inst's type map to field data type vector
        for idx in range(0, self.root.GetInstNum()):
            inst = self.root.GetInst(idx)
            line += (self.TAB + "{ ")
            line += ("INST_TYPE_" + inst.GetName().upper() + ", ")
            line += (inst.GetName() + "FieldMinus },")
            line += (" // " + str(idx) + "\n")

        line += ("};\n\n\n")
        return line


    def __write_cpp_inst_field_pos(self):
        line = ''
        line += ("// inst type to field position map\n")
        line += ("const unordered_map<uint32_t, const vector<map<uint32_t, uint32_t>> &> InstTable::InstFieldPos = {\n")

        # write every inst's type map to field position vector
        for idx in range(0, self.root.GetInstNum()):
            inst = self.root.GetInst(idx)
            line += (self.TAB + "{ ")
            line += ("INST_TYPE_" + inst.GetName().upper() + ", ")
            line += (inst.GetName() + "FieldPos },")
            line += (" // " + str(idx) + "\n")

        line += ("};\n\n\n")
        return line

    def __write_cpp_inst_field_len(self):
        line = ''
        line += ("// inst type to field length map\n")
        line += ("const unordered_map<uint32_t, const vector<map<uint32_t, uint32_t>> &> InstTable::InstFieldLen = {\n")

        # write every inst's type map to field length vector
        for idx in range(0, self.root.GetInstNum()):
            inst = self.root.GetInst(idx)
            line += (self.TAB + "{ ")
            line += ("INST_TYPE_" + inst.GetName().upper() + ", ")
            line += (inst.GetName() + "FieldLen },")
            line += (" // " + str(idx) + "\n")

        line += ("};\n\n\n")
        return line

    # other
    def __write_tab(self, ofs, num):
        for i in range(0, num):
            ofs.write("    ")
