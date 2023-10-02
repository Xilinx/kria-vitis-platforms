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
        pushd ${work_dir}
        source ${setup} -r ${tool_release} && set -e
        ${lsf} make platform PFM=${pfm_base} JOBS=32
        popd
    '''
}

def deployPlatform() {
    sh label: 'platform deploy',
    script: '''
        if [ "${BRANCH_NAME}" == "${deploy_branch}" ]; then
            pushd ${work_dir}
            DST=${DEPLOYDIR}/platforms
            mkdir -p ${DST}
            cp -rf ${board}/platforms/${pfm} ${DST}
            popd
            cp ${ws}/commitIDs ${DST}/${pfm}
        fi
    '''
}

def buildOverlay() {
    sh label: 'overlay build',
    script: '''
        pushd ${work_dir}
        if [ -d ${board}/platforms/${pfm} ]; then
            echo "Using platform from local build"
        elif [ -d ${DEPLOYDIR}/platforms/${pfm} ]; then
            echo "Using platform from build artifacts"
            ln -s ${DEPLOYDIR}/platforms/${pfm} ${board}/platforms/
        else
            echo "No valid platform found: ${pfm}"
            exit 1
        fi
        source ${setup} -r ${tool_release} && set -e
        ${lsf} make overlay OVERLAY=${overlay}
        popd
    '''
}

def buildFirmware() {
    sh label: 'firmware build',
    script: '''
        pushd ${work_dir}
        source ${setup} -r ${tool_release} && set -e
        ${lsf} make firmware FW=${fw}
        popd
    '''
}

def deployFirmware() {
    sh label: 'firmware deploy',
    script: '''
        if [ "${BRANCH_NAME}" == "${deploy_branch}" ]; then
            DST=${DEPLOYDIR}/firmware
            mkdir -p ${DST}
            cp -rf ${fw_dir} ${DST}
            cp ${ws}/commitIDs ${DST}/${fw}
        fi
    '''
}

pipeline {
    agent {
        label 'Build_Master'
    }
    environment {
        deploy_branch="master"
        tool_release="2023.2"
        tool_build="daily_latest"
        auto_branch="2022.1"
        pfm_ver="202320_1"
        ws="${WORKSPACE}"
        setup="${ws}/paeg-helper/env-setup.sh"
        lsf="${ws}/paeg-helper/scripts/lsf"
        DEPLOYDIR="/wrk/paeg_builds/build-artifacts/kria-vitis-platforms/${tool_release}"
    }
    options {
        // don't let the implicit checkout happen
        skipDefaultCheckout true
        // retain logs for last 30 builds
        buildDiscarder(logRotator(numToKeepStr: '30'))
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
                                    changeset "**/kv260/platforms/kv260_ispMipiRx_vcu_DP/**"
                                    triggeredBy 'TimerTrigger'
                                    triggeredBy 'UserIdCause'
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
                                overlay_dir="${work_dir}/${board}/overlays/${overlay}"
                                fw="kv260-smartcam"
                                fw_dir="${work_dir}/${board}/firmware/${fw}"
                            }
                            when {
                                anyOf {
                                    changeset "**/kv260/overlays/smartcam/**"
                                    triggeredBy 'TimerTrigger'
                                    triggeredBy 'UserIdCause'
                                    environment name: 'BUILD_SMARTCAM', value: '1'
                                }
                            }
                            steps {
                                createWorkDir()
                                buildOverlay()
                                buildFirmware()
                            }
                            post {
                                success {
                                    deployFirmware()
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
                                    changeset "**/kv260/platforms/kv260_vcuDecode_vmixDP/**"
                                    triggeredBy 'TimerTrigger'
                                    triggeredBy 'UserIdCause'
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
                                overlay_dir="${work_dir}/${board}/overlays/${overlay}"
                                fw="kv260-aibox-reid"
                                fw_dir="${work_dir}/${board}/firmware/${fw}"
                            }
                            when {
                                anyOf {
                                    changeset "**/kv260/overlays/aibox-reid/**"
                                    triggeredBy 'TimerTrigger'
                                    triggeredBy 'UserIdCause'
                                    environment name: 'BUILD_AIBOX_REID', value: '1'
                                }
                            }
                            steps {
                                createWorkDir()
                                buildOverlay()
                                buildFirmware()
                            }
                            post {
                                success {
                                    deployFirmware()
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
                                    changeset "**/kv260/platforms/kv260_ispMipiRx_vmixDP/**"
                                    triggeredBy 'TimerTrigger'
                                    triggeredBy 'UserIdCause'
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
                                overlay_dir="${work_dir}/${board}/overlays/${overlay}"
                                fw="kv260-defect-detect"
                                fw_dir="${work_dir}/${board}/firmware/${fw}"
                            }
                            when {
                                anyOf {
                                    changeset "**/kv260/overlays/defect-detect/**"
                                    triggeredBy 'TimerTrigger'
                                    triggeredBy 'UserIdCause'
                                    environment name: 'BUILD_DEFECT_DETECT', value: '1'
                                }
                            }
                            steps {
                                createWorkDir()
                                buildOverlay()
                                buildFirmware()
                            }
                            post {
                                success {
                                    deployFirmware()
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
                                    changeset "**/kv260/platforms/kv260_ispMipiRx_rpiMipiRx_DP/**"
                                    triggeredBy 'TimerTrigger'
                                    triggeredBy 'UserIdCause'
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
                                overlay_dir="${work_dir}/${board}/overlays/${overlay}"
                                fw="kv260-nlp-smartvision"
                                fw_dir="${work_dir}/${board}/firmware/${fw}"
                            }
                            when {
                                anyOf {
                                    changeset "**/kv260/overlays/nlp-smartvision/**"
                                    triggeredBy 'TimerTrigger'
                                    triggeredBy 'UserIdCause'
                                    environment name: 'BUILD_NLP_SMARTVISION', value: '1'
                                }
                            }
                            steps {
                                createWorkDir()
                                buildOverlay()
                                buildFirmware()
                            }
                            post {
                                success {
                                    deployFirmware()
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
                        fw="kr260-tsn-rs485pmod"
                        fw_dir="${work_dir}/${board}/firmware/${fw}"
                    }
                    stages {
                        stage('kr260_tsn_rs485pmod platform build')  {
                            environment {
                                PAEG_LSF_MEM=65536
                                PAEG_LSF_QUEUE="long"
                            }
                            when {
                                anyOf {
                                    changeset "**/kr260/platforms/kr260_tsn_rs485pmod/**"
                                    triggeredBy 'TimerTrigger'
                                    triggeredBy 'UserIdCause'
                                }
                            }
                            steps {
                                createWorkDir()
                                buildPlatform()
                                buildFirmware()
                            }
                            post {
                                success {
                                    deployFirmware()
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
                        fw="kr260-pmod-gps"
                        fw_dir="${work_dir}/${board}/firmware/${fw}"
                    }
                    stages {
                        stage('kr260_pmod_gps platform build')  {
                            environment {
                                PAEG_LSF_MEM=65536
                                PAEG_LSF_QUEUE="long"
                            }
                            when {
                                anyOf {
                                    changeset "**/kr260/platforms/kr260_pmod_gps/**"
                                    triggeredBy 'TimerTrigger'
                                    triggeredBy 'UserIdCause'
                                }
                            }
                            steps {
                                createWorkDir()
                                buildPlatform()
                                buildFirmware()
                            }
                            post {
                                success {
                                    deployFirmware()
                                }
                            }
                        }
                    }
                }
                stage('kd240_motor_ctrl_qei') {
                    environment {
                        pfm_base="kd240_motor_ctrl_qei"
                        pfm="xilinx_${pfm_base}_${pfm_ver}"
                        work_dir="${ws}/build/${pfm_base}"
                        board="kd240"
                        pfm_dir="${work_dir}/${board}/platforms/${pfm}"
                        xpfm="${pfm_dir}/${pfm_base}.xpfm"
                        fw="kd240-motor-ctrl-qei"
                        fw_dir="${work_dir}/${board}/firmware/${fw}"
                    }
                    stages {
                        stage('kd240_motor_ctrl_qei platform build')  {
                            environment {
                                PAEG_LSF_MEM=65536
                                PAEG_LSF_QUEUE="long"
                            }
                            when {
                                anyOf {
                                    changeset "**/kd240/platforms/kd240_motor_ctrl_qei/**"
                                    triggeredBy 'TimerTrigger'
                                    triggeredBy 'UserIdCause'
                                }
                            }
                            steps {
                                createWorkDir()
                                buildPlatform()
                                buildFirmware()
                            }
                            post {
                                success {
                                    deployFirmware()
                                }
                            }
                        }
                    }
                }
                stage('kv260_bist') {
                    environment {
                        pfm_base="kv260_bist"
                        pfm="xilinx_${pfm_base}_${pfm_ver}"
                        work_dir="${ws}/build/${pfm_base}"
                        board="kv260"
                        pfm_dir="${work_dir}/${board}/platforms/${pfm}"
                        xpfm="${pfm_dir}/${pfm_base}.xpfm"
                        fw="kv260-bist"
                        fw_dir="${work_dir}/${board}/firmware/${fw}"
                    }
                    stages {
                        stage('kv260_bist platform build')  {
                            environment {
                                PAEG_LSF_MEM=65536
                                PAEG_LSF_QUEUE="long"
                            }
                            when {
                                anyOf {
                                    changeset "**/kv260/platforms/kv260_bist/**"
                                    triggeredBy 'TimerTrigger'
                                    triggeredBy 'UserIdCause'
                                }
                            }
                            steps {
                                createWorkDir()
                                buildPlatform()
                                buildFirmware()
                            }
                            post {
                                success {
                                    deployFirmware()
                                }
                            }
                        }
                    }
                }
                stage('kd240_bist') {
                    environment {
                        pfm_base="kd240_bist"
                        pfm="xilinx_${pfm_base}_${pfm_ver}"
                        work_dir="${ws}/build/${pfm_base}"
                        board="kd240"
                        pfm_dir="${work_dir}/${board}/platforms/${pfm}"
                        xpfm="${pfm_dir}/${pfm_base}.xpfm"
                        fw="kd240-bist"
                        fw_dir="${work_dir}/${board}/firmware/${fw}"
                    }
                    stages {
                        stage('kd240_bist platform build')  {
                            environment {
                                PAEG_LSF_MEM=65536
                                PAEG_LSF_QUEUE="long"
                            }
                            when {
                                anyOf {
                                    changeset "**/kd240/platforms/kd240_bist/**"
                                    triggeredBy 'TimerTrigger'
                                    triggeredBy 'UserIdCause'
                                }
                            }
                            steps {
                                createWorkDir()
                                buildPlatform()
                                buildFirmware()
                            }
                            post {
                                success {
                                    deployFirmware()
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
