#!/usr/bin/python

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
import filecmp
from os import listdir
from os import walk
from os.path import isfile, join, splitext
from os.path import basename
from difflib import SequenceMatcher

def matchName(file1, gold_dir):
    f1 = basename(file1)
    file2 = ""
    match_size = 0
    for f in gold_dir:
        f2 = basename(f)
        match = SequenceMatcher(None, f1, f2).find_longest_match(0, len(f1), 0, len(f2))
        if match.size > match_size:
            match_size = match.size
            file2 = f

    match_size_added = 0
    for f in gold_dir:
        f2 = basename(f)
        match = SequenceMatcher(None, f1, f2).find_longest_match(0, len(f1), 0, len(f2))
        if match.size == match_size:
            sf1 = f1[match.a+match.size:]
            sf2 = f2[match.b+match.size:]
            added_match = SequenceMatcher(None, sf1, sf2).find_longest_match(0, len(sf1), 0, len(sf2))
            if added_match.size > match_size_added:
                match_size_added = added_match.size
                file2 = f

    return [file1, file2]


def matchLog(filename1, filename2, match_ok):
    print filename1
    print filename2
    if match_ok:
        print("pass")
    else:
        print("fail")
    print("\n")


def compare(dir1, dir2):
    gold_files = [join(dir1, f) for f in listdir(dir1) if isfile(join(dir1, f)) and splitext(f)[1] == ".txt"]
    sim_dir = []
    for(dirpath, dirnames, filenames) in walk(dir2):
        sim_dir.extend(dirnames)
        break
    for layer in sim_dir:
        layer_path = join(dirpath, layer)
        rlt_files = [join(layer_path, f) for f in listdir(layer_path) if isfile(join(layer_path, f)) and splitext(f)[1] == ".dat"]
        for f1 in rlt_files:
            pair = matchName(f1, gold_files)
            matchLog(pair[0], pair[1], filecmp.cmp(pair[0], pair[1]))

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("usage: check.py src_path dest_path")
        sys.exit(1)
    compare(sys.argv[1], sys.argv[2])
