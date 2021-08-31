/*
 * (C) Copyright 2020 - 2021 Xilinx, Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

def buildPlatform() {
    sh label: 'platform build',
    script: '''
        pushd src
        source ${setup} -r ${tool_release} && set -e
        ${lsf} make platform PFM=${pfm_base} JOBS=32
        popd
    '''
}

def deployPlatform() {
    sh label: 'platform deploy',
    script: '''
        if [ "${BRANCH_NAME}" == "${deploy_branch}" ]; then
            pushd src
            mkdir -p ${DEPLOYDIR}
            cp -rf platforms/${pfm} ${DEPLOYDIR}
            popd
        fi
    '''
}

def buildOverlay() {
    sh label: 'overlay build',
    script: '''
        pushd src

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
            pushd src
            DST=${DEPLOYDIR}/${overlay}
            mkdir -p ${DST}

            cp -f ${example_dir}/*.xsa \
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
        deploy_branch="release-2021.2"
        tool_release="2021.2"
        tool_build="daily_latest"
        auto_branch="2021.2"
        pfm_ver="202120_1"
        setup="${WORKSPACE}/paeg-helper/env-setup.sh"
        lsf="${WORKSPACE}/paeg-helper/scripts/lsf"
        DEPLOYDIR="/wrk/paeg_builds/build-artifacts/kv260-vitis/${tool_release}"
    }
    options {
        // don't let the implicit checkout happen
        skipDefaultCheckout true
    }
    triggers {
        cron(env.BRANCH_NAME == 'release-2021.2' ? 'H 21 * * *' : '')
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
                    doGenerateSubmoduleConfigurations: scm.doGenerateSubmoduleConfigurations,
                    extensions: scm.extensions + [[$class: 'RelativeTargetDirectory', relativeTargetDir: 'src']],
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
	stage('Vitis builds') {
            parallel {
                stage('kv260_ispMipiRx_vcu_DP') {
                    environment {
                        pfm_base="kv260_ispMipiRx_vcu_DP"
                        pfm="xilinx_${pfm_base}_${pfm_ver}"
                        pfm_dir="${WORKSPACE}/src/platforms/${pfm}"
                        xpfm="${pfm_dir}/${pfm_base}.xpfm"
                    }
                    stages {
                        stage('kv260_ispMipiRx_vcu_DP platform build')  {
                            when {
                                anyOf {
                                    changeset "**/platforms/vivado/kv260_ispMipiRx_vcu_DP/**"
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
                                example_dir="overlays/examples/${overlay}"
                            }
                            when {
                                anyOf {
                                    changeset "**/overlays/examples/smartcam/**"
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
                                example_dir="overlays/examples/${overlay}"
                            }
                            when {
                                anyOf {
                                    changeset "**/overlays/examples/benchmark/**"
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
                        pfm_base="kv260_vcuDecode_vmixDP"
                        pfm="xilinx_${pfm_base}_${pfm_ver}"
                        pfm_dir="${WORKSPACE}/src/platforms/${pfm}"
                        xpfm="${pfm_dir}/${pfm_base}.xpfm"
                    }
                    stages {
                        stage('kv260_vcuDecode_vmixDP platform build')  {
                            when {
                                anyOf {
                                    changeset "**/platforms/vivado/kv260_vcuDecode_vmixDP/**"
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
                                example_dir="overlays/examples/${overlay}"
                            }
                            when {
                                anyOf {
                                    changeset "**/overlays/examples/aibox-reid/**"
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
                        pfm_base="kv260_ispMipiRx_vmixDP"
                        pfm="xilinx_${pfm_base}_${pfm_ver}"
                        pfm_dir="${WORKSPACE}/src/platforms/${pfm}"
                        xpfm="${pfm_dir}/${pfm_base}.xpfm"
                    }
                    stages {
                        stage('kv260_ispMipiRx_vmixDP platform build')  {
                            when {
                                anyOf {
                                    changeset "**/platforms/vivado/kv260_ispMipiRx_vmixDP/**"
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
                                example_dir="overlays/examples/${overlay}"
                            }
                            when {
                                anyOf {
                                    changeset "**/overlays/examples/defect-detect/**"
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
                stage('kv260_ispMipiRx_DP') {
                    environment {
                        pfm_base="kv260_ispMipiRx_DP"
                        pfm="xilinx_${pfm_base}_${pfm_ver}"
                        pfm_dir="${WORKSPACE}/src/platforms/${pfm}"
                        xpfm="${pfm_dir}/${pfm_base}.xpfm"
                    }
                    stages {
                        stage('kv260_ispMipiRx_DP platform build')  {
                            when {
                                anyOf {
                                    changeset "**/platforms/vivado/kv260_ispMipiRx_DP/**"
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
                                example_dir="overlays/examples/${overlay}"
                            }
                            when {
                                anyOf {
                                    changeset "**/overlays/examples/nlp-smartvision/**"
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
            }
        }
    }
    post {
        cleanup {
            cleanWs()
        }
    }
}
