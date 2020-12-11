#!/bin/bash

BUILDDIR=$(realpath ${1:-"./build"})
mkdir -p $BUILDDIR
(cd unilog && bash -x ./cmake.sh --type=release --clean --build-dir=$BUILDDIR/unilog )
(cd xir && bash -x ./cmake.sh --build-python --type=release --clean --build-dir=$BUILDDIR/xir )
(cd target_factory && bash -x ./cmake.sh --type=release  --clean --build-dir=$BUILDDIR/target_factory )
(cd vart && bash -x ./cmake.sh  --build-python --type=release  --cmake-options='-DENABLE_CPU_RUNNER=OFF -DENABLE_SIM_RUNNER=OFF -DENABLE_DPU_RUNNER=ON' --clean --build-dir=$BUILDDIR/vart)
(cd vitis-ai-library && bash -x ./cmake.sh --clean --type=release  --cmake-options='-DENABLE_OVERVIEW=ON' --clean --build-dir=$BUILDDIR/vitis-ai-library)
