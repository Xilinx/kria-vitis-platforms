/*
 * (C) Copyright 2020 - 2021 Xilinx, Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

pipeline {
    agent {
        label 'Build_Master'
    }
    environment {
        DEPLOYDIR="/wrk/paeg_builds/build-artifacts"
        deploy_branch="2021.1"
        tool_release="2021.1"
        auto_branch="2020.2"
    }
    options {
        // don't let the implicit checkout happen
        skipDefaultCheckout true
    }
    triggers {
        cron(env.BRANCH_NAME == '2021.1' ? 'H 21 * * *' : '')
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
                stage('KV260 Smartcamera') {
                    stages {
                        stage('kv260_ispMipiRx_vcu_DP platform build')  {
                            when {
                                anyOf {
                                    changeset "**/platforms/vivado/kv260_ispMipiRx_vcu_DP/**"
                                    changeset "**/overlays/examples/smartcam/**"
                                    triggeredBy 'TimerTrigger'
                                }
                            }
                            steps {
                                script {
                                    env.BUILD_SMARTCAM = '1'
                                }
                                sh label: 'kv260_ispMipiRx_vcu_DP build',
                                script: '''
                                    pushd src
                                    source ../paeg-helper/env-setup.sh -r ${tool_release}
                                    ../paeg-helper/scripts/lsf make platform PFM=kv260_ispMipiRx_vcu_DP JOBS=32
                                    popd
                                '''
                            }
                            post {
                                success {
                                    sh label: 'kv260_ispMipiRx_vcu_DP deploy',
                                    script: '''
                                        if [ "${BRANCH_NAME}" == "${deploy_branch}" ]; then
                                            pushd src
                                            DST=${DEPLOYDIR}/kv260-vitis/${tool_release}
                                            mkdir -p ${DST}
                                            cp -rf platforms/xilinx_kv260_ispMipiRx_vcu_DP* ${DST}
                                            popd
                                        fi
                                    '''
                                }
                            }
                        }
                        stage('smartcam overlay build') {
                            environment {
                                PAEG_LSF_MEM=65536
                                PAEG_LSF_QUEUE="long"
                            }
                            when {
                                anyOf {
                                    changeset "**/overlays/examples/smartcam/**"
                                    triggeredBy 'TimerTrigger'
                                    environment name: 'BUILD_SMARTCAM', value: '1'
                                }
                            }
                            steps {
                                sh label: 'smartcam build',
                                script: '''
                                    pushd src
                                    source ../paeg-helper/env-setup.sh -r ${tool_release}
                                    ../paeg-helper/scripts/lsf make overlay OVERLAY=smartcam

                                    pushd overlays/examples/smartcam/binary_container_1/link/int
                                    echo 'all: { system.bit }' > bootgen.bif
                                    bootgen -arch zynqmp -process_bitstream bin -image bootgen.bif
                                    popd

                                    popd
                                '''
                            }
                            post {
                                success {
                                    sh label: 'smartcam deploy',
                                    script: '''
                                        if [ "${BRANCH_NAME}" == "${deploy_branch}" ]; then
                                            pushd src
                                            DST=${DEPLOYDIR}/kv260-vitis/${tool_release}/smartcam
                                            mkdir -p ${DST}

                                            cp -f overlays/examples/smartcam/*.xsa \
                                                  overlays/examples/smartcam/binary_container_1/*.xclbin \
                                                  overlays/examples/smartcam/binary_container_1/link/int/system.bit* \
                                                  ${DST}
                                            popd
                                        fi
                                    '''
                                }
                            }
                        }
                    }
                }
                stage('kv260_vcuDecode_vmixDP') {
                    stages {
                        stage('kv260_vcuDecode_vmixDP platform build')  {
                            when {
                                anyOf {
                                    changeset "**/platforms/vivado/kv260_vcuDecode_vmixDP/**"
                                    changeset "**/overlays/examples/aibox-reid/**"
                                    triggeredBy 'TimerTrigger'
                                }
                            }
                            steps {
                                script {
                                    env.BUILD_AIBOX_REID = '1'
                                }
                                sh label: 'kv260_vcuDecode_vmixDP build',
                                script: '''
                                    pushd src
                                    source ../paeg-helper/env-setup.sh -r ${tool_release}
                                    ../paeg-helper/scripts/lsf make platform PFM=kv260_vcuDecode_vmixDP JOBS=32
                                    popd
                                '''
                            }
                            post {
                                success {
                                    sh label: 'kv260_vcuDecode_vmixDP deploy',
                                    script: '''
                                        if [ "${BRANCH_NAME}" == "${deploy_branch}" ]; then
                                            pushd src
                                            DST=${DEPLOYDIR}/kv260-vitis/${tool_release}
                                            mkdir -p ${DST}
                                            cp -rf platforms/xilinx_kv260_vcuDecode_vmixDP* ${DST}
                                            popd
                                        fi
                                    '''
                                }
                            }
                        }
                        stage('aibox-reid build') {
                            environment {
                                PAEG_LSF_MEM=65536
                                PAEG_LSF_QUEUE="long"
                            }
                            when {
                                anyOf {
                                    changeset "**/overlays/examples/aibox-reid/**"
                                    triggeredBy 'TimerTrigger'
                                    environment name: 'BUILD_AIBOX_REID', value: '1'
                                }
                            }
                            steps {
                                sh label: 'aibox-reid build',
                                script: '''
                                    pushd src
                                    source ../paeg-helper/env-setup.sh -r ${tool_release}
                                    ../paeg-helper/scripts/lsf make overlay OVERLAY=aibox-reid

                                    pushd overlays/examples/aibox-reid/binary_container_1/link/int
                                    echo 'all: { system.bit }' > bootgen.bif
                                    bootgen -arch zynqmp -process_bitstream bin -image bootgen.bif
                                    popd

                                    popd
                                '''
                            }
                            post {
                                success {
                                    sh label: 'aibox-reid deploy',
                                    script: '''
                                        if [ "${BRANCH_NAME}" == "${deploy_branch}" ]; then
                                            pushd src
                                            DST=${DEPLOYDIR}/kv260-vitis/${tool_release}/aibox-reid
                                            mkdir -p ${DST}

                                            cp -f overlays/examples/aibox-reid/*.xsa \
                                                  overlays/examples/aibox-reid/binary_container_1/*.xclbin \
                                                  overlays/examples/aibox-reid/binary_container_1/link/int/system.bit* \
                                                  ${DST}
                                            popd
                                        fi
                                    '''
                                }
                            }
                        }
                    }
                }
                stage('kv260_ispMipiRx_vmixDP') {
                    stages {
                        stage('kv260_ispMipiRx_vmixDP platform build')  {
                            when {
                                anyOf {
                                    changeset "**/platforms/vivado/kv260_ispMipiRx_vmixDP/**"
                                    changeset "**/overlays/examples/defect-detect/**"
                                    triggeredBy 'TimerTrigger'
                                }
                            }
                            steps {
                                script {
                                    env.BUILD_DEFECT_DETECT = '1'
                                }
                                sh label: 'kv260_ispMipiRx_vmixDP build',
                                script: '''
                                    pushd src
                                    source ../paeg-helper/env-setup.sh -r ${tool_release}
                                    ../paeg-helper/scripts/lsf make platform PFM=kv260_ispMipiRx_vmixDP JOBS=32
                                    popd
                                '''
                            }
                            post {
                                success {
                                    sh label: 'kv260_ispMipiRx_vmixDP deploy',
                                    script: '''
                                        if [ "${BRANCH_NAME}" == "${deploy_branch}" ]; then
                                            pushd src
                                            DST=${DEPLOYDIR}/kv260-vitis/${tool_release}
                                            mkdir -p ${DST}
                                            cp -rf platforms/xilinx_kv260_ispMipiRx_vmixDP* ${DST}
                                            popd
                                        fi
                                    '''
                                }
                            }
                        }
                        stage('defect-detect overlay build') {
                            environment {
                                PAEG_LSF_MEM=65536
                                PAEG_LSF_QUEUE="long"
                            }
                            when {
                                anyOf {
                                    changeset "**/overlays/examples/defect-detect/**"
                                    triggeredBy 'TimerTrigger'
                                    environment name: 'BUILD_DEFECT_DETECT', value: '1'
                                }
                            }
                            steps {
                                sh label: 'defect-detect build',
                                script: '''
                                    pushd src
                                    source ../paeg-helper/env-setup.sh -r ${tool_release}
                                    ../paeg-helper/scripts/lsf make overlay OVERLAY=defect-detect

                                    pushd overlays/examples/defect-detect/binary_container_1/link/int
                                    echo 'all: { system.bit }' > bootgen.bif
                                    bootgen -arch zynqmp -process_bitstream bin -image bootgen.bif
                                    popd

                                    popd
                                '''
                            }
                            post {
                                success {
                                    sh label: 'defect-detect deploy',
                                    script: '''
                                        if [ "${BRANCH_NAME}" == "${deploy_branch}" ]; then
                                            pushd src
                                            DST=${DEPLOYDIR}/kv260-vitis/${tool_release}/defect-detect
                                            mkdir -p ${DST}

                                            cp -f overlays/examples/defect-detect/*.xsa \
                                                  overlays/examples/defect-detect/binary_container_1/*.xclbin \
                                                  overlays/examples/defect-detect/binary_container_1/link/int/system.bit* \
                                                  ${DST}
                                            popd
                                        fi
                                    '''
                                }
                            }
                        }
                    }
                }
                stage('kv260_ispMipiRx_DP') {
                    stages {
                        stage('kv260_ispMipiRx_DP platform build')  {
                            when {
                                anyOf {
                                    changeset "**/platforms/vivado/kv260_ispMipiRx_DP/**"
                                    changeset "**/overlays/examples/nlp-smartvision/**"
                                    triggeredBy 'TimerTrigger'
                                }
                            }
                            steps {
                                script {
                                    env.BUILD_NLP_SMARTVISION = '1'
                                }
                                sh label: 'kv260_ispMipiRx_DP build',
                                script: '''
                                    pushd src
                                    source ../paeg-helper/env-setup.sh -r ${tool_release}
                                    ../paeg-helper/scripts/lsf make platform PFM=kv260_ispMipiRx_DP JOBS=32
                                    popd
                                '''
                            }
                            post {
                                success {
                                    sh label: 'kv260_ispMipiRx_DP deploy',
                                    script: '''
                                        if [ "${BRANCH_NAME}" == "${deploy_branch}" ]; then
                                            pushd src
                                            DST=${DEPLOYDIR}/kv260-vitis/${tool_release}
                                            mkdir -p ${DST}
                                            cp -rf platforms/xilinx_kv260_ispMipiRx_DP* ${DST}
                                            popd
                                        fi
                                    '''
                                }
                            }
                        }
                        stage('nlp-smartvision overlay build') {
                            environment {
                                PAEG_LSF_MEM=65536
                                PAEG_LSF_QUEUE="long"
                            }
                            when {
                                anyOf {
                                    changeset "**/overlays/examples/nlp-smartvision/**"
                                    triggeredBy 'TimerTrigger'
                                    environment name: 'BUILD_NLP_SMARTVISION', value: '1'
                                }
                            }
                            steps {
                                sh label: 'nlp-smartvision build',
                                script: '''
                                    pushd src
                                    source ../paeg-helper/env-setup.sh -r ${tool_release}
                                    ../paeg-helper/scripts/lsf make overlay OVERLAY=nlp-smartvision

                                    pushd overlays/examples/nlp-smartvision/binary_container_1/link/int
                                    echo 'all: { system.bit }' > bootgen.bif
                                    bootgen -arch zynqmp -process_bitstream bin -image bootgen.bif
                                    popd

                                    popd
                                '''
                            }
                            post {
                                success {
                                    sh label: 'nlp-smartvision deploy',
                                    script: '''
                                        if [ "${BRANCH_NAME}" == "${deploy_branch}" ]; then
                                            pushd src
                                            DST=${DEPLOYDIR}/kv260-vitis/${tool_release}/nlp-smartvision
                                            mkdir -p ${DST}

                                            cp -f overlays/examples/nlp-smartvision/*.xsa \
                                                  overlays/examples/nlp-smartvision/binary_container_1/*.xclbin \
                                                  overlays/examples/nlp-smartvision/binary_container_1/link/int/system.bit* \
                                                  ${DST}
                                            popd
                                        fi
                                    '''
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
