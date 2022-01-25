# Instructions

This repo contains submodules. To clone this repo, run:
```
git clone --recursive https://github.com/Xilinx/kv260-vitis.git
```

# Tools Version

The supported Xilinx tools release is 2021.1.

Install y2k22_patch for Vivado HLS and Vitis HLS tools to avoid 'Y2K22 Overflow Issue'.
Refer to the following Answer Record for obtaining the patch.

https://support.xilinx.com/s/article/76960?language=en_US

# Contents

This reference design contains the following:

1. platforms

   This folder contains the KV260 Vitis platforms. Follow the REAMDE inside this
   folder for build instructions.

2. overlays

   This folder contains the Vitis overlay projects which include the DPU
   inference engine RTL kernel and HLS-based computer vision kernels based on
   the Vitis Vision libraries. Follow the README inside this folder for detailed
   build instructions.

3. Makefile

   Top-level Makefile for building the Vitis platforms and overlay projects.
   Type 'make' or 'make help' to print the help text explaining the different
   make targets.

4. README

   This file.

# Documentation

For additional documentation including architecture information and build
tutorials, visit: https://xilinx.github.io/kria-apps-docs/

# License

(C) Copyright 2020 - 2021 Xilinx, Inc.\
SPDX-License-Identifier: Apache-2.0
