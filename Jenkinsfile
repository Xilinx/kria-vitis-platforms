/*
 * Copyright (C) 2020 - 2022 Xilinx, Inc.
 * Copyright (C) 2023, Advanced Micro Devices, Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

def logCommitIDs() {
    sh label: 'log commit IDs',
    script: '''
        idfile=${ws}/commitIDs
        pushd ${ws}/src
        echo -n "src : " >> ${idfile}
        git rev-parse HEAD >> ${idfile}
        subm=($(cat .gitmodules | grep path | cut -d "=" -f2))
        for sm in "${subm[@]}"; do
            pushd ${sm}
            echo -n "${sm} : " >> ${idfile}
            git rev-parse HEAD >> ${idfile}
            popd
        done
        popd
        pushd ${ws}/paeg-helper
        echo -n "paeg-helper : " >> ${idfile}
        git rev-parse HEAD >> ${idfile}
        popd
        cat ${idfile}
    '''
}

def createWorkDir() {
    sh label: 'create work dir',
    script: '''
        if [ ! -d ${work_dir} ]; then
            mkdir -p ${work_dir}
            cp -rf ${ws}/src/* ${work_dir}
        fi
    '''
}

def buildPlatform() {
    sh label: 'platform build',
    script: '''
        pushd ${work_dir}/${board}
        source ${setup} -r ${tool_release} && set -e
        ${lsf} make platform PFM=${pfm_base} JOBS=32
        popd
    '''
}

def deployPlatform() {
    sh label: 'platform deploy',
    script: '''
        if [ "${BRANCH_NAME}" == "${deploy_branch}" ]; then
            pushd ${work_dir}/${board}
            DST=${DEPLOYDIR}/platforms
            mkdir -p ${DST}
            cp -rf platforms/${pfm} ${DST}
            popd
            cp ${ws}/commitIDs ${DST}/${pfm}
        fi
    '''
}

def deployPlatformFirmware() {
    sh label: 'platform firmware deploy',
    script: '''
        if [ "${BRANCH_NAME}" == "${deploy_branch}" ]; then
            pushd ${work_dir}/${board}
            mkdir -p tmp
            unzip platforms/${pfm}/hw/${pfm_base}.xsa -d tmp
            pushd tmp
            source ${setup} -r ${tool_release} && set -e
            echo "all: { ${pfm_base}.bit }" > bootgen.bif
            bootgen -arch zynqmp -process_bitstream bin -image bootgen.bif
            popd
            fw=$(echo ${pfm_base} | tr _ -)
            DST=${DEPLOYDIR}/firmware/${fw}
            mkdir -p ${DST}
            cp -f tmp/${pfm_base}.bit ${DST}/${fw}.bit
            cp -f tmp/${pfm_base}.bit.bin ${DST}/${fw}.bin
            popd
            cp ${ws}/commitIDs ${DST}
        fi
    '''
}

def buildOverlay() {
    sh label: 'overlay build',
    script: '''
        pushd ${work_dir}/${board}
        if [ -d platforms/${pfm} ]; then
            echo "Using platform from local build"
        elif [ -d ${DEPLOYDIR}/platforms/${pfm} ]; then
            echo "Using platform from build artifacts"
            ln -s ${DEPLOYDIR}/platforms/${pfm} platforms/
        else
            echo "No valid platform found: ${pfm}"
            exit 1
        fi
        source ${setup} -r ${tool_release} && set -e
        ${lsf} make overlay OVERLAY=${overlay}
        popd

        pushd ${example_dir}/binary_container_1/link/int
        echo 'all: { system.bit }' > bootgen.bif
        bootgen -arch zynqmp -process_bitstream bin -image bootgen.bif
        popd
    '''
}

def deployOverlay() {
    sh label: 'overlay deploy',
    script: '''
        if [ "${BRANCH_NAME}" == "${deploy_branch}" ]; then
            DST=${DEPLOYDIR}/firmware/${board}-${overlay}
            mkdir -p ${DST}
            cp -f ${example_dir}/binary_container_1/*.xclbin ${DST}/${board}-${overlay}.xclbin
            cp -f ${example_dir}/binary_container_1/link/int/system.bit ${DST}/${board}-${overlay}.bit
            cp -f ${example_dir}/binary_container_1/link/int/system.bit.bin ${DST}/${board}-${overlay}.bin
            cp ${ws}/commitIDs ${DST}
        fi
    '''
}

pipeline {
    agent {
        label 'Build_Master'
    }
    environment {
        deploy_branch="master"
        tool_release="2022.2"
        tool_build="daily_latest"
        auto_branch="2022.1"
        pfm_ver="202220_1"
        ws="${WORKSPACE}"
        setup="${ws}/paeg-helper/env-setup.sh"
        lsf="${ws}/paeg-helper/scripts/lsf"
        DEPLOYDIR="/wrk/paeg_builds/build-artifacts/kria-vitis-platforms/${tool_release}"
    }
    options {
        // don't let the implicit checkout happen
        skipDefaultCheckout true
    }
    triggers {
        cron(env.BRANCH_NAME == 'master' ? 'H 21 * * *' : '')
    }
    stages {
        stage('Clone Repos') {
            steps {
                // checkout main source repo
                checkout([
                    $class: 'GitSCM',
                    branches: scm.branches,
                    doGenerateSubmoduleConfigurations: scm.doGenerateSubmoduleConfigurations,
                    extensions: scm.extensions +
                    [
                        [$class: 'RelativeTargetDirectory', relativeTargetDir: 'src'],
                        [$class: 'ChangelogToBranch', options: [compareRemote: 'origin', compareTarget: env.deploy_branch]]
                    ],
                    userRemoteConfigs: scm.userRemoteConfigs
                ])
                // checkout paeg-automation helper repo
                checkout([
                    $class: 'GitSCM',
                    branches: [[name: auto_branch]],
                    doGenerateSubmoduleConfigurations: false,
                    extensions:
                    [
                        [$class: 'CleanCheckout'],
                        [$class: 'RelativeTargetDirectory', relativeTargetDir: 'paeg-helper']
                    ],
                    submoduleCfg: [],
                    userRemoteConfigs: [[
                        credentialsId: '01d4faf7-fb5a-4bd9-b300-57ac0bfc7991',
                        url: 'https://gitenterprise.xilinx.com/PAEG/paeg-automation.git'
                    ]]
                ])
                logCommitIDs()
            }
        }
        stage('Vitis Builds') {
            parallel {
                stage('kv260_ispMipiRx_vcu_DP') {
                    environment {
                        pfm_base="kv260_ispMipiRx_vcu_DP"
                        pfm="xilinx_${pfm_base}_${pfm_ver}"
                        work_dir="${ws}/build/${pfm_base}"
                        board="kv260"
                        pfm_dir="${work_dir}/${board}/platforms/${pfm}"
                        xpfm="${pfm_dir}/${pfm_base}.xpfm"
                    }
                    stages {
                        stage('kv260_ispMipiRx_vcu_DP platform build')  {
                            environment {
                                PAEG_LSF_MEM=65536
                                PAEG_LSF_QUEUE="long"
                            }
                            when {
                                anyOf {
                                    changeset "**/kv260/platforms/vivado/kv260_ispMipiRx_vcu_DP/**"
                                    triggeredBy 'TimerTrigger'
                                }
                            }
                            steps {
                                script {
                                    env.BUILD_SMARTCAM = '1'
                                }
                                createWorkDir()
                                buildPlatform()
                            }
                            post {
                                success {
                                    deployPlatform()
                                }
                            }
                        }
                        stage('smartcam overlay build') {
                            environment {
                                PAEG_LSF_MEM=65536
                                PAEG_LSF_QUEUE="long"
                                overlay="smartcam"
                                example_dir="${work_dir}/${board}/overlays/examples/${overlay}"
                            }
                            when {
                                anyOf {
                                    changeset "**/kv260/overlays/examples/smartcam/**"
                                    triggeredBy 'TimerTrigger'
                                    environment name: 'BUILD_SMARTCAM', value: '1'
                                }
                            }
                            steps {
                                createWorkDir()
                                buildOverlay()
                            }
                            post {
                                success {
                                    deployOverlay()
                                }
                            }
                        }
                        stage('benchmark overlay build') {
                            environment {
                                PAEG_LSF_MEM=65536
                                PAEG_LSF_QUEUE="long"
                                overlay="benchmark"
                                example_dir="${work_dir}/${board}/overlays/examples/${overlay}"
                            }
                            when {
                                anyOf {
                                    changeset "**/kv260/overlays/examples/benchmark/**"
                                    triggeredBy 'TimerTrigger'
                                    environment name: 'BUILD_SMARTCAM', value: '1'
                                }
                            }
                            steps {
                                createWorkDir()
                                buildOverlay()
                            }
                            post {
                                success {
                                    deployOverlay()
                                }
                            }
                        }
                    }
                }
                stage('kv260_vcuDecode_vmixDP') {
                    environment {
                        pfm_base="kv260_vcuDecode_vmixDP"
                        pfm="xilinx_${pfm_base}_${pfm_ver}"
                        work_dir="${ws}/build/${pfm_base}"
                        board="kv260"
                        pfm_dir="${work_dir}/${board}/platforms/${pfm}"
                        xpfm="${pfm_dir}/${pfm_base}.xpfm"
                    }
                    stages {
                        stage('kv260_vcuDecode_vmixDP platform build')  {
                            environment {
                                PAEG_LSF_MEM=65536
                                PAEG_LSF_QUEUE="long"
                            }
                            when {
                                anyOf {
                                    changeset "**/kv260/platforms/vivado/kv260_vcuDecode_vmixDP/**"
                                    triggeredBy 'TimerTrigger'
                                }
                            }
                            steps {
                                script {
                                    env.BUILD_AIBOX_REID = '1'
                                }
                                createWorkDir()
                                buildPlatform()
                            }
                            post {
                                success {
                                    deployPlatform()
                                }
                            }
                        }
                        stage('aibox-reid overlay build') {
                            environment {
                                PAEG_LSF_MEM=65536
                                PAEG_LSF_QUEUE="long"
                                overlay="aibox-reid"
                                example_dir="${work_dir}/${board}/overlays/examples/${overlay}"
                            }
                            when {
                                anyOf {
                                    changeset "**/kv260/overlays/examples/aibox-reid/**"
                                    triggeredBy 'TimerTrigger'
                                    environment name: 'BUILD_AIBOX_REID', value: '1'
                                }
                            }
                            steps {
                                createWorkDir()
                                buildOverlay()
                            }
                            post {
                                success {
                                    deployOverlay()
                                }
                            }
                        }
                    }
                }
                stage('kv260_ispMipiRx_vmixDP') {
                    environment {
                        pfm_base="kv260_ispMipiRx_vmixDP"
                        pfm="xilinx_${pfm_base}_${pfm_ver}"
                        work_dir="${ws}/build/${pfm_base}"
                        board="kv260"
                        pfm_dir="${work_dir}/${board}/platforms/${pfm}"
                        xpfm="${pfm_dir}/${pfm_base}.xpfm"
                    }
                    stages {
                        stage('kv260_ispMipiRx_vmixDP platform build')  {
                            environment {
                                PAEG_LSF_MEM=65536
                                PAEG_LSF_QUEUE="long"
                            }
                            when {
                                anyOf {
                                    changeset "**/kv260/platforms/vivado/kv260_ispMipiRx_vmixDP/**"
                                    triggeredBy 'TimerTrigger'
                                }
                            }
                            steps {
                                script {
                                    env.BUILD_DEFECT_DETECT = '1'
                                }
                                createWorkDir()
                                buildPlatform()
                            }
                            post {
                                success {
                                    deployPlatform()
                                }
                            }
                        }
                        stage('defect-detect overlay build') {
                            environment {
                                PAEG_LSF_MEM=65536
                                PAEG_LSF_QUEUE="long"
                                overlay="defect-detect"
                                example_dir="${work_dir}/${board}/overlays/examples/${overlay}"
                            }
                            when {
                                anyOf {
                                    changeset "**/kv260/overlays/examples/defect-detect/**"
                                    triggeredBy 'TimerTrigger'
                                    environment name: 'BUILD_DEFECT_DETECT', value: '1'
                                }
                            }
                            steps {
                                createWorkDir()
                                buildOverlay()
                            }
                            post {
                                success {
                                    deployOverlay()
                                }
                            }
                        }
                    }
                }
                stage('kv260_ispMipiRx_rpiMipiRx_DP') {
                    environment {
                        pfm_base="kv260_ispMipiRx_rpiMipiRx_DP"
                        pfm="xilinx_${pfm_base}_${pfm_ver}"
                        work_dir="${ws}/build/${pfm_base}"
                        board="kv260"
                        pfm_dir="${work_dir}/${board}/platforms/${pfm}"
                        xpfm="${pfm_dir}/${pfm_base}.xpfm"
                    }
                    stages {
                        stage('kv260_ispMipiRx_rpiMipiRx_DP platform build')  {
                            environment {
                                PAEG_LSF_MEM=65536
                                PAEG_LSF_QUEUE="long"
                            }
                            when {
                                anyOf {
                                    changeset "**/kv260/platforms/vivado/kv260_ispMipiRx_rpiMipiRx_DP/**"
                                    triggeredBy 'TimerTrigger'
                                }
                            }
                            steps {
                                script {
                                    env.BUILD_NLP_SMARTVISION = '1'
                                }
                                createWorkDir()
                                buildPlatform()
                            }
                            post {
                                success {
                                    deployPlatform()
                                }
                            }
                        }
                        stage('nlp-smartvision overlay build') {
                            environment {
                                PAEG_LSF_MEM=65536
                                PAEG_LSF_QUEUE="long"
                                overlay="nlp-smartvision"
                                example_dir="${work_dir}/${board}/overlays/examples/${overlay}"
                            }
                            when {
                                anyOf {
                                    changeset "**/kv260/overlays/examples/nlp-smartvision/**"
                                    triggeredBy 'TimerTrigger'
                                    environment name: 'BUILD_NLP_SMARTVISION', value: '1'
                                }
                            }
                            steps {
                                createWorkDir()
                                buildOverlay()
                            }
                            post {
                                success {
                                    deployOverlay()
                                }
                            }
                        }
                    }
                }
                stage('kr260_tsn_rs485pmod') {
                    environment {
                        pfm_base="kr260_tsn_rs485pmod"
                        pfm="xilinx_${pfm_base}_${pfm_ver}"
                        work_dir="${ws}/build/${pfm_base}"
                        board="kr260"
                        pfm_dir="${work_dir}/${board}/platforms/${pfm}"
                        xpfm="${pfm_dir}/${pfm_base}.xpfm"
                    }
                    stages {
                        stage('kr260_tsn_rs485pmod platform build')  {
                            environment {
                                PAEG_LSF_MEM=65536
                                PAEG_LSF_QUEUE="long"
                            }
                            when {
                                anyOf {
                                    changeset "**/kr260/platforms/vivado/kr260_tsn_rs485pmod/**"
                                    triggeredBy 'TimerTrigger'
                                }
                            }
                            steps {
                                createWorkDir()
                                buildPlatform()
                            }
                            post {
                                success {
                                    deployPlatformFirmware()
                                }
                            }
                        }
                    }
                }
                stage('kr260_pmod_gps') {
                    environment {
                        pfm_base="kr260_pmod_gps"
                        pfm="xilinx_${pfm_base}_${pfm_ver}"
                        work_dir="${ws}/build/${pfm_base}"
                        board="kr260"
                        pfm_dir="${work_dir}/${board}/platforms/${pfm}"
                        xpfm="${pfm_dir}/${pfm_base}.xpfm"
                    }
                    stages {
                        stage('kr260_pmod_gps platform build')  {
                            environment {
                                PAEG_LSF_MEM=65536
                                PAEG_LSF_QUEUE="long"
                            }
                            when {
                                anyOf {
                                    changeset "**/kr260/platforms/vivado/kr260_pmod_gps/**"
                                    triggeredBy 'TimerTrigger'
                                }
                            }
                            steps {
                                createWorkDir()
                                buildPlatform()
                            }
                            post {
                                success {
                                    deployPlatformFirmware()
                                }
                            }
                        }
                    }
                }
                stage('k26_base_starter_kit') {
                    environment {
                        pfm_base="k26_base_starter_kit"
                        pfm="xilinx_${pfm_base}_${pfm_ver}"
                        work_dir="${ws}/build/${pfm_base}"
                        board="k26"
                        pfm_dir="${work_dir}/${board}/platforms/${pfm}"
                        xpfm="${pfm_dir}/${pfm_base}.xpfm"
                    }
                    stages {
                        stage('k26_base_starter_kit platform build')  {
                            environment {
                                PAEG_LSF_MEM=65536
                                PAEG_LSF_QUEUE="long"
                            }
                            when {
                                anyOf {
                                    changeset "**/k26/platforms/vivado/k26_base_starter_kit/**"
                                    triggeredBy 'TimerTrigger'
                                }
                            }
                            steps {
                                createWorkDir()
                                buildPlatform()
                            }
                            post {
                                success {
                                    deployPlatformFirmware()
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    post {
        cleanup {
            cleanWs()
        }
    }
}
