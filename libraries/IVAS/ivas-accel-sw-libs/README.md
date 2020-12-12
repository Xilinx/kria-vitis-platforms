# IVAS Accelerator Software libraries

## Native compilation steps:
---
```
meson --libdir=/usr/local/lib/ivas/ build;
cd build;
ninja;
sudo ninja install;
```

## Steps for Cross Compilation:
1. Copy sdk.sh file to <sdk.sh_folder> on build machine
```
/proj/petalinux/2020.1/petalinux-v2020.1_daily_latest/bsp/internal/zynqmp/xilinx-zynqmp-common-v2020.1/sdk.sh 
```
2. Prepare SYSROOT and set environment variables
```
cd <sdk.sh_folder>
./sdk.sh -d `pwd` -y
```
3. Edit ivas-accel-sw-libs/meson.cross to point to SYSROOT path

4. Build & Compile IVAS accelerator libs
```
meson build --cross-file meson.cross
cd build;
ninja;
```
5. For installing user to copy .so to target in respective locations

***Note:***<br />
Following packages need to be added in sysroot :
```
- jansson >= 2.7
- AI Vitis 2020.1 (https://www.xilinx.com/bin/public/openDownload?filename=vitis_ai_2020.1-r1.2.0.tar.gz)
- ivas-utils and ivas-gst-plugins
```

***Note:***<br />

***Enable/disable accelerator sw libraries/features***
By default, all libraries are enabled in build process. User can selectively enable/disable libraries in build using build option “-D”.<br />
Below is example to disable ivas_boundingbox library in build process 
```
meson -Divas_xskboundingbox=disabled --libdir=/usr/local/lib/ivas/ build;  
```
Also, you can include different model class in build process. To enable/disable the model class use the “-D” option with 0/1: 
meson -DMODELCLASS=0 --libdir=/usr/local/lib/ivas/ build;
where MODELCLASS supported by IVAS are 
```
YOLOV3
SSD
CLASSIFICATION
FACEDETECT 
REFINEDET
TFSSD
YOLOV2
```

***Example to enable all models and accel sw libraries in meson build is***

```
meson -DYOLOV3=1 -DSSD=1 -DREID=0 -DCLASSIFICATION=1 -DFACEDETECT=1 -DREFINEDET=1 -DTFSSD=1 -DYOLOV2=1 -Divas_xskboundingbox=enabled --libdir=/usr/local/lib/ivas/ build;
cd build;
ninja;
sudo ninja install;
```


## Test Setup
---
```
cd ../ivas_xdpuinfer/
Copy resnet model to /usr/share/vitis_ai_library/models/
./cmd_resnet50.sh
```
