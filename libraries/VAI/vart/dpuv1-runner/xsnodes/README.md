<!--
  Copyright 2019 Xilinx Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 -->
# XSnodes

A node is the building block for services (e.g. [services in Neptune](../../../../neptune/recipes/README.md)), which are a chain of nodes connected through a graph.
Nodes use a pub/sub model to communicate.
Each node may be run as a separate process.
It gets data, performs some operations, and then publishes to the next node, which may vary depending on the service.

Each node can take a dictionary of arguments so that the same node can execute differently in different contexts depending on the arguments.
Arguments are provided when the [GraphNode](grapher.py#L16) object is instantiated.
This dictionary can specify both instantiation time and run-time arguments for the node to use.
In addition, you can pass in run-time arguments when the node is [started](grapher.py#L34).
To do this, use the following structure for the args dictionary:

```python
args = {
    'node_1': {
        'add': {
            'key1': value,
            ...
        },
        'remove': {
            'key2': None,
            ...
        }
    },
    ...
}
```

It should be a dict with the node names as the top key. 
Each node dict can contain the _add_ and _remove_ keys, indicating which key/value pairs to add/update or which keys to remove, respectively, from the node args used during initialization.
If this dictionary is not present or contains unknown values, it is ignored.

## Writing New Nodes

The structure and methods of nodes are defined in [base.py](base.py).
Also refer exisitng nodes as needed for reference. 
Look at [ping.py](ping.py) as a simple example.
For such cases, the only required method a node must define is `loop()`.
A more complex example is [pre.py](pre.py), which opts to go for a more custom structure.
It does so by overriding the default `run()` method from the base class.

Rules:
* Node definitions must be placed in this folder in separate files.
The name of the file is used to name the node and is therefore unique.
* All new node classes that extend the base XstreamNode must be called Node
