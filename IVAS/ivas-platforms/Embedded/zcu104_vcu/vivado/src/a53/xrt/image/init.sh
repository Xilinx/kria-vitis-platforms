echo "sourcing Init script..."
SCRIPTPATH4=$(dirname $BASH_SOURCE)
echo ${SCRIPTPATH4}
cp ${SCRIPTPATH4}/platform_desc.txt /etc/xocl.txt
export XILINX_XRT=/usr

if [ ! -e /usr/bin/irps5401 ]
then
    echo "IRPS5401 regulation adjustment app not present. Please install or run manually"
else
    irps5401
fi

cd /media/sd-mmcblk0p1/

cp dpu.xclbin /usr/lib/dpu.xclbin

#HP0
/sbin/devmem 0xFD380008 w 0x7
/sbin/devmem 0xFD38001C w 0x7
/sbin/devmem 0xFD380004 w 0xF
/sbin/devmem 0xFD380018 w 0xF

#HP1
/sbin/devmem 0xFD390008 w 0x3
/sbin/devmem 0xFD39001C w 0x3
/sbin/devmem 0xFD390004 w 0xF
/sbin/devmem 0xFD390018 w 0xF

#HP2
/sbin/devmem 0xFD3A0000 w 0x3b0
/sbin/devmem 0xFD3A0014 w 0x3b0
/sbin/devmem 0xFD3A0008 w 0x3
/sbin/devmem 0xFD3A001C w 0x3
/sbin/devmem 0xFD3A0004 w 0xF
/sbin/devmem 0xFD3A0018 w 0xF

#HP3
/sbin/devmem 0xFD3B0000 w 0x3b0
/sbin/devmem 0xFD3B0014 w 0x3b0
/sbin/devmem 0xFD3B0008 w 0x3
/sbin/devmem 0xFD3B001C w 0x3
/sbin/devmem 0xFD3B0004 w 0xF
/sbin/devmem 0xFD3B0018 w 0xF

#HPC0
/sbin/devmem 0xFD360000 w 0x3b0
/sbin/devmem 0xFD360014 w 0x3b0
/sbin/devmem 0xFD360008 w 0x3
/sbin/devmem 0xFD36001C w 0x3
/sbin/devmem 0xFD360004 w 0xF
/sbin/devmem 0xFD360018 w 0xF

#HPC1
/sbin/devmem 0xFD370000 w 0x3b0
/sbin/devmem 0xFD370014 w 0x3b0
/sbin/devmem 0xFD370008 w 0x3
/sbin/devmem 0xFD37001C w 0x3
/sbin/devmem 0xFD370004 w 0xF
/sbin/devmem 0xFD370018 w 0xF

#LPD
/sbin/devmem 0xFF9B0008 w 0x3
/sbin/devmem 0xFF9B001C w 0x3
/sbin/devmem 0xFF9B0004 w 0xF
/sbin/devmem 0xFF9B0018 w 0xF

echo "Init script done"

cd

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
