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
usage() {
    echo -e "\nUsage: ./run-layerwise.sh -cn <compiler json> -cq <quantizer json> -cw <.h5 file>\n"
}
# Parse Options
OPTS=`getopt -o p:t:m:k:b:d:s:a:n:ns:i:c:y:gvzfxh --long platform:,test:,model:,kcfg:,bitwidth:,directory:,numdevices:,numstreams:,deviceid:,batchsize:,compilerOpt:,numprepproc,checkaccuracy,verbose,zelda,frodo,perpetual,help -n "$0" -- "$@"`

if [ $? != 0 ] ; then echo "Failed parsing options." >&2 ; usage; exit 1 ; fi

while true
do
    case "$1" in
        -cn|--customnet     ) CUSTOM_NETCFG="$2"    ; shift 2 ;;
        -cq|--customquant   ) CUSTOM_QUANTCFG="$2"  ; shift 2 ;;
        -cw|--customwts     ) CUSTOM_WEIGHTS="$2"   ; shift 2 ;;
        -h |--help          ) usage                 ; exit  1 ;;
        *) break ;;
    esac
done
# Set Platform Environment Variables
if [ -z $VAI_ALVEO_ROOT ]; then
    VAI_ALVEO_ROOT=../..
fi
. ${VAI_ALVEO_ROOT}/overlaybins/setup.sh ${MLSUITE_PLATFORM}

TEST=layerwise.py
BATCHSIZE=1

if [ ! -z $CUSTOM_NETCFG ]; then
    NETCFG=$CUSTOM_NETCFG
else
    echo "[ERROR] Expected Compiler JSON : No such file" ; usage
    exit 1
fi
if [ ! -z $CUSTOM_WEIGHTS ]; then
    WEIGHTS=$CUSTOM_WEIGHTS
else
    echo "[ERROR] Expected Weights : No such file" ; usage 
    exit 1
fi
if [ ! -z $CUSTOM_QUANTCFG ]; then
    QUANTCFG=$CUSTOM_QUANTCFG
else 
    echo "[ERROR] Expected Quantizer JSON : No such file" ;  usage
    exit 1
fi

XCLBIN=${VAI_ALVEO_ROOT}/overlaybins/xdnnv3
index=0
previousLatency=0.0
totalLatency=0.0
BASEOPT="--xclbin ${XCLBIN}
         --netcfg $NETCFG
         --weights $WEIGHTS
         --quantizecfg $QUANTCFG
         --batch_sz $BATCHSIZE
         --images ${VAI_ALVEO_ROOT}/examples/deployment_modes/dog.jpg"
#echo $BASEOPT
printf '%s\n' "--------------------------------------------------"
printf '%-30s   %.8s\n' "Layer Name" "Time"
printf '%s\n' "--------------------------------------------------"
# Run each layer, one by one
OIFS=$IFS
while :
do
    layertime=`python ${TEST} --layerindex ${index} ${BASEOPT} | grep " = "`
    index=$((index+1))
    IFS=" = "
    read -ra Arr <<< "$layertime"
    layname=${Arr[0]}
    latency=${Arr[1]}
    #echo -e "$layname ---------------------- $latency"
    IFS=$OIFS
    if test "$latency" = "Done"; then
        break 
    elif test "$latency" = "0"; then
        printf '%-30s = %s\n' "$layname" "$latency"
        continue
    fi
    timetaken=`awk '{print $1-$2}' <<< "$latency $previousLatency"`
    totalLatency=`awk '{print $1+$2}' <<< "$totalLatency $timetaken"`
    previousLatency=$latency
    #printf '%-30s = -- %s\n' "${layname}" "${previousLatency}"
    printf '%-30s = %s\n' "${layname}" "${timetaken}"
done

printf '%s\n' "--------------------------------------------------"
printf '%-30s = %.8s\n' "Total Time" "${totalLatency}"
printf '%s\n' "--------------------------------------------------"
`rm *.json`

IFS=$OIFS
