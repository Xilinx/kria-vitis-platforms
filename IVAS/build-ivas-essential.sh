#!/bin/bash

usage()  
{  
	echo "Usage: $0 <PCIe/Edge>"  
	exit 1  
} 

if [ "$#" -ne 1 ]; then
	usage
	exit 1
fi

if [[ ("$1" = "Edge") || ("$1" = "EDGE") || ("$1" = "edge") ]]; then
	echo "Building for Edge"
else
	usage
fi

set -e

if [ -z "$CC" ]
then
    echo "Cross compilation not set - source environment setup first"
    exit
fi

if [ ! -e "$SDKTARGETSYSROOT/usr/lib/libvart-util.so" ]
then
    echo "Vitis AI is not installed in the target sysroot"
    echo "Please see instructions here: "
    echo "    https://github.com/Xilinx/Vitis-AI/tree/v1.2/Vitis-AI-Library#quick-start-for-edge"
    while true; do
    read -p "Do you wish to install this program? (y/n) " yn
    case $yn in
        [Yy]* ) wget -O vitis_ai_2020.1.tar.gz https://www.xilinx.com/bin/public/openDownload?filename=vitis_ai_2020.1-r1.2.0.tar.gz; tar -xzvf vitis_ai_2020.1.tar.gz -C $SDKTARGETSYSROOT; break;;
        [Nn]* ) exit;;
        * ) echo "Please answer yes or no.";;
    esac
done
fi

# This script automates the build and deployment of images to the target. If your
# environment is different you may need to make minor changes.
rm -rf install

cd ivas-utils
sed -E 's@<SYSROOT>@'"$SDKTARGETSYSROOT"'@g; s@<NATIVESYSROOT>@'"$OECORE_NATIVE_SYSROOT"'@g' meson.cross.template > meson.cross
cp meson.cross $OECORE_NATIVE_SYSROOT/usr/share/meson/aarch64-xilinx-linux-meson.cross
meson build --prefix /usr --cross-file $PWD/meson.cross
cd build
ninja
DESTDIR=$SDKTARGETSYSROOT ninja install
DESTDIR=../../install ninja install

cd ../../ivas-gst-plugins
sed -E 's@<SYSROOT>@'"$SDKTARGETSYSROOT"'@g; s@<NATIVESYSROOT>@'"$OECORE_NATIVE_SYSROOT"'@g' meson.cross.template > meson.cross
cp meson.cross $OECORE_NATIVE_SYSROOT/usr/share/meson/aarch64-xilinx-linux-meson.cross
meson --prefix /usr build --cross-file meson.cross
cd build
ninja
DESTDIR=$SDKTARGETSYSROOT ninja install
DESTDIR=../../install ninja install

cd ../../ivas-accel-sw-libs
sed -E 's@<SYSROOT>@'"$SDKTARGETSYSROOT"'@g; s@<NATIVESYSROOT>@'"$OECORE_NATIVE_SYSROOT"'@g' meson.cross.template > meson.cross
cp meson.cross $OECORE_NATIVE_SYSROOT/usr/share/meson/aarch64-xilinx-linux-meson.cross
meson build --cross-file meson.cross --prefix /usr 
cd build
ninja
DESTDIR=$SDKTARGETSYSROOT ninja install
DESTDIR=../../install ninja install

cd ../../install
tar -pczvf ivas_installer.tar.gz usr
cd ..

########################################################################
 # Copyright 2020 Xilinx, Inc.
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
##########################################################################
