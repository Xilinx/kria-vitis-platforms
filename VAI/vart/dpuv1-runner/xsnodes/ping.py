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
"""
This simple node demonstrates the structure of a generic xstreamNode.
"""
import time

import numpy as np

from vai.dpuv1.rt.xsnodes.base import XstreamNode

class Node(XstreamNode):
    def loop(self, payload, pub=None):
        (meta, buf) = payload

        # add 'pong' response
        meta['pong'] = str(time.time())

        # delete objects to decrement obj store refcount
        # -- this is so the object can be auto-deleted,
        #    so we can reuse the id in the put() below
        del buf
        del payload

        self.put(meta, pub=pub)
