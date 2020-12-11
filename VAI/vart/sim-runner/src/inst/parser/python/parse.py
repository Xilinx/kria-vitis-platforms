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
import re
import os.path
import xml.etree.ElementTree as ET

import Field
import Word
import Inst
import Root
import GenInstTable
import GenInstTableList
class XMLParse:
    def __init__(self, xml_file):
        self.tree = None
        self.root = None
        self.xml_file = xml_file

    def ReadXML(self):
        try:
            self.tree = ET.parse(self.xml_file)
            self.root = self.tree.getroot()
        except Exception as e:
            print("Parse " + self.xml_file + " failed!")
            sys.exit()
        else:
            print("Parse " + self.xml_file + " successful!")
        finally:
            return self.tree

def getFileName(file_dir):
    rlt = []
    for roots, dirs, files in os.walk(file_dir):
        rlt.append(files)
    return rlt[0]

def getRootList(xml_list):
    root_list = []
    for i in range(0, len(xml_list)):
        print xml_list[i]
        xml_file = os.path.abspath("./xml/"+xml_list[i])
        parse = XMLParse(xml_file)
        tree = parse.ReadXML()
        root = Root.Root(tree.getroot())
        root_list.append(root)
    return root_list


def main():
    xml_list = getFileName("./xml")
    root_list = getRootList(xml_list)
    GenInstTableList.GenInstTableList(root_list)

if __name__ == "__main__":
    main()

