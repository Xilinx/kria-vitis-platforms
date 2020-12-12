# IVAS utility for IVAS Gstreamer plugins and IVAS Kernel libs

***We dont't suggest to modify ivas-util code.***

# Native compilation steps:

```
meson --libdir=/usr/local/lib --includedir=/usr/local/include build;
cd build;
ninja;
sudo ninja install;
```

# Steps for Cross Compilation:
1. Copy sdk.sh file to <sdk.sh_folder> on build machine
```
/proj/petalinux/2020.1/petalinux-v2020.1_daily_latest/bsp/internal/zynqmp/xilinx-zynqmp-common-v2020.1/sdk.sh 
```
2. Prepare SYSROOT and set environment variables
```
cd <sdk.sh_folder>
./sdk.sh -d `pwd` -y
```
***Note: Following packages need to be added in sysroot :***
```
- jansson >= 2.7
- AI Vitis 2020.1 (https://www.xilinx.com/bin/public/openDownload?filename=vitis_ai_2020.1-r1.2.0.tar.gz)
```
3. Edit ivas-utils/meson.cross to point to SYSROOT path

4. Build & Compile IVAS utility
```
meson build --cross-file meson.cross
cd build;
ninja;
```
5. For installing user to copy .so to target in respective locations
