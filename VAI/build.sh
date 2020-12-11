#!/bin/bash

(cd unilog && bash -x ./cmake.sh --type=release --clean)
(cd xir && bash -x ./cmake.sh --build-python --type=release --clean)
(cd target_factory && bash -x ./cmake.sh --type=release  --clean)
(cd vart && bash -x ./cmake.sh  --build-python --type=release  --cmake-options='-DENABLE_CPU_RUNNER=OFF -DENABLE_SIM_RUNNER=OFF -DENABLE_DPU_RUNNER=ON' --clean )
(cd vitis-ai-library && bash -x ./cmake.sh --clean --type=release  --cmake-options='-DENABLE_OVERVIEW=ON' --clean)
