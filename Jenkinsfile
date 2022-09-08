/*
 * (C) Copyright 2020 - 2022 Xilinx, Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

def buildPlatform() {
    sh label: 'platform build',
    script: '''
        pushd ${root_dir}
        source ${setup} -r ${tool_release} && set -e
        ${lsf} make platform PFM=${pfm_base} JOBS=32
        popd
    '''
}

def deployPlatform() {
    sh label: 'platform deploy',
    script: '''
        if [ "${BRANCH_NAME}" == "${deploy_branch}" ]; then
            pushd ${root_dir}
            mkdir -p ${DEPLOYDIR}
            cp -rf platforms/${pfm} ${DEPLOYDIR}
            popd
        fi
    '''
}

def buildOverlay() {
    sh label: 'overlay build',
    script: '''
        pushd ${root_dir}

        if [ -d platforms/${pfm} ]; then
            echo "Using platform from local build"
        elif [ -d ${DEPLOYDIR}/${pfm} ]; then
            echo "Using platform from build artifacts"
            ln -s ${DEPLOYDIR}/${pfm} platforms/
        else
            echo "No valid platform found: ${pfm}"
            exit 1
        fi

        source ${setup} -r ${tool_release} && set -e
        ${lsf} make overlay OVERLAY=${overlay}

        pushd ${example_dir}/binary_container_1/link/int
        echo 'all: { system.bit }' > bootgen.bif
        bootgen -arch zynqmp -process_bitstream bin -image bootgen.bif
        popd

        popd
    '''
}

def deployOverlay() {
    sh label: 'overlay deploy',
    script: '''
        if [ "${BRANCH_NAME}" == "${deploy_branch}" ]; then
            pushd ${root_dir}
            DST=${DEPLOYDIR}/${overlay}
            mkdir -p ${DST}

            cp -f ${example_dir}/binary_container_1/*.xsa \
                    ${example_dir}/binary_container_1/*.xclbin \
                    ${example_dir}/binary_container_1/link/int/system.bit* \
                    ${DST}
            popd
        fi
    '''
}

pipeline {
    agent {
        label 'Build_Master'
    }
    environment {
        deploy_branch="release-2022.1"
        tool_release="2022.1"
        tool_build="daily_latest"
        auto_branch="2022.1"
        pfm_ver="202210_1"
        setup="${WORKSPACE}/paeg-helper/env-setup.sh"
        lsf="${WORKSPACE}/paeg-helper/scripts/lsf"
        DEPLOYDIR="/wrk/paeg_builds/build-artifacts/kria-vitis-platforms/${tool_release}"
    }
    options {
        // don't let the implicit checkout happen
        skipDefaultCheckout true
    }
    triggers {
        cron(env.BRANCH_NAME == 'release-2022.1' ? 'H 21 * * *' : '')
    }
    stages {
        stage ('Fix Changelog') {
            // only do this if there is no prior build
            when {
                expression {
                    return !currentBuild.previousBuild
                }
            }
            steps {
                checkout([
                    $class: 'GitSCM',
                    branches: scm.branches,
                    userRemoteConfigs: scm.userRemoteConfigs,
                    // this extension builds the changesets from the compareTarget branch
                    extensions:
                    [
                        [$class: 'ChangelogToBranch', options:
                              [compareRemote: 'origin', compareTarget: env.tool_release]
                        ]
                    ]
                ])
            }
        }
        stage('Clone Repos') {
            steps {
                // checkout main source repo
                checkout([
                    $class: 'GitSCM',
                    branches: scm.branches,
                    extensions: [[$class: 'RelativeTargetDirectory', relativeTargetDir: 'src'],
                                 [$class: 'SubmoduleOption',recursiveSubmodules: false,trackingSubmodules: false]],
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
            }
        }
        stage('Vitis Builds') {
            parallel {
                stage('kv260_ispMipiRx_vcu_DP') {
                    environment {
                        root_dir="${WORKSPACE}/src/kv260"
                        pfm_base="kv260_ispMipiRx_vcu_DP"
                        pfm="xilinx_${pfm_base}_${pfm_ver}"
                        pfm_dir="${root_dir}/platforms/${pfm}"
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
                                example_dir="${root_dir}/overlays/examples/${overlay}"
                            }
                            when {
                                anyOf {
                                    changeset "**/kv260/overlays/examples/smartcam/**"
                                    triggeredBy 'TimerTrigger'
                                    environment name: 'BUILD_SMARTCAM', value: '1'
                                }
                            }
                            steps {
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
                                example_dir="${root_dir}/overlays/examples/${overlay}"
                            }
                            when {
                                anyOf {
                                    changeset "**/kv260/overlays/examples/benchmark/**"
                                    triggeredBy 'TimerTrigger'
                                    environment name: 'BUILD_SMARTCAM', value: '1'
                                }
                            }
                            steps {
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
                        root_dir="${WORKSPACE}/src/kv260"
                        pfm_base="kv260_vcuDecode_vmixDP"
                        pfm="xilinx_${pfm_base}_${pfm_ver}"
                        pfm_dir="${root_dir}/platforms/${pfm}"
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
                                example_dir="${root_dir}/overlays/examples/${overlay}"
                            }
                            when {
                                anyOf {
                                    changeset "**/kv260/overlays/examples/aibox-reid/**"
                                    triggeredBy 'TimerTrigger'
                                    environment name: 'BUILD_AIBOX_REID', value: '1'
                                }
                            }
                            steps {
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
                        root_dir="${WORKSPACE}/src/kv260"
                        pfm_base="kv260_ispMipiRx_vmixDP"
                        pfm="xilinx_${pfm_base}_${pfm_ver}"
                        pfm_dir="${root_dir}/platforms/${pfm}"
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
                                example_dir="${root_dir}/overlays/examples/${overlay}"
                            }
                            when {
                                anyOf {
                                    changeset "**/kv260/overlays/examples/defect-detect/**"
                                    triggeredBy 'TimerTrigger'
                                    environment name: 'BUILD_DEFECT_DETECT', value: '1'
                                }
                            }
                            steps {
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
                        root_dir="${WORKSPACE}/src/kv260"
                        pfm_base="kv260_ispMipiRx_rpiMipiRx_DP"
                        pfm="xilinx_${pfm_base}_${pfm_ver}"
                        pfm_dir="${root_dir}/platforms/${pfm}"
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
                                example_dir="${root_dir}/overlays/examples/${overlay}"
                            }
                            when {
                                anyOf {
                                    changeset "**/kv260/overlays/examples/nlp-smartvision/**"
                                    triggeredBy 'TimerTrigger'
                                    environment name: 'BUILD_NLP_SMARTVISION', value: '1'
                                }
                            }
                            steps {
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
                        root_dir="${WORKSPACE}/src/kr260"
                        pfm_base="kr260_tsn_rs485pmod"
                        pfm="xilinx_${pfm_base}_${pfm_ver}"
                        pfm_dir="${root_dir}/platforms/${pfm}"
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
                                buildPlatform()
                            }
                            post {
                                success {
                                    deployPlatform()
                                }
                            }
                        }
                    }
                }
	        stage('k26_base_starter_kit') {
                    environment {
                        root_dir="${WORKSPACE}/src/k26"
                        pfm_base="k26_base_starter_kit"
                        pfm="xilinx_${pfm_base}_${pfm_ver}"
                        pfm_dir="${root_dir}/platforms/${pfm}"
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
                                buildPlatform()
                            }
                            post {
                                success {
                                    deployPlatform()
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
