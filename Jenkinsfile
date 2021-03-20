pipeline {
    agent {
        label 'Build_Master'
    }
    environment {
        tool_release="2020.2.2"
        auto_branch="2020.2"
    }
    options {
        // don't let the implicit checkout happen
        skipDefaultCheckout true
    }
    triggers {
        cron(env.BRANCH_NAME == '2020.2.2' ? 'H 21 * * *' : '')
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
	stage('AA Builds') {
            parallel {
                stage('KV260 Smartcamera') {
                    stages {
                        stage('KV260 Smartcamera Platform Build')  {
                            when {
                                anyOf {
                                    changeset "**/platforms/vivado/kv260_smartcamera/**"
                                    changeset "**/kernels/examples/som_aa1/**"
                                    triggeredBy 'TimerTrigger'
                                }
                            }
                            steps {
                                script {
                                    env.BUILD_AA1 = '1'
                                }
                                sh label: 'smartcamera-build',
                                script: '''
                                    pushd src
                                    source ../paeg-helper/env-setup.sh -r ${tool_release}
                                    ../paeg-helper/scripts/lsf make platform PFM=kv260_smartcamera
                                    popd
                                '''
                            }
                        }
                        stage('AA1 Build') {
                            environment {
                                PAEG_LSF_MEM=65536
                                PAEG_LSF_QUEUE="long"
                            }
                            when {
                                anyOf {
                                    changeset "**/kernels/examples/som_aa1/**"
                                    triggeredBy 'TimerTrigger'
                                    environment name: 'BUILD_AA1', value: '1'
                                }
                            }
                            steps {
                                sh label: 'aa1-build',
                                script: '''
                                    pushd src
                                    source ../paeg-helper/env-setup.sh -r ${tool_release}
                                    ../paeg-helper/scripts/lsf make kernel AA=aa1
                                    popd
                                '''
                            }
                        }
                    }
                }
                stage('KV260 AI Box') {
                    stages {
                        stage('KV260 AI Box Platform Build')  {
                            when {
                                anyOf {
                                    changeset "**/platforms/vivado/kv260_aibox/**"
                                    changeset "**/kernels/examples/som_aa2/**"
                                    triggeredBy 'TimerTrigger'
                                }
                            }
                            steps {
                                script {
                                    env.BUILD_AA2 = '1'
                                }
                                sh label: 'aibox-build',
                                script: '''
                                    pushd src
                                    source ../paeg-helper/env-setup.sh -r ${tool_release}
                                    ../paeg-helper/scripts/lsf make platform PFM=kv260_aibox
                                    popd
                                '''
                            }
                        }
                        stage('AA2 Build') {
                            environment {
                                PAEG_LSF_MEM=65536
                                PAEG_LSF_QUEUE="long"
                            }
                            when {
                                anyOf {
                                    changeset "**/kernels/examples/som_aa2/**"
                                    triggeredBy 'TimerTrigger'
                                    environment name: 'BUILD_AA2', value: '1'
                                }
                            }
                            steps {
                                sh label: 'aa2-build',
                                script: '''
                                    pushd src
                                    source ../paeg-helper/env-setup.sh -r ${tool_release}
                                    ../paeg-helper/scripts/lsf make kernel AA=aa2
                                    popd
                                '''
                            }
                        }
                    }
                }
                stage('KV260 Defect Detection') {
                    stages {
                        stage('KV260 Defect Detection Platform Build')  {
                            when {
                                anyOf {
                                    changeset "**/platforms/vivado/kv260_defectdetect/**"
                                    changeset "**/kernels/examples/som_aa4/**"
                                    triggeredBy 'TimerTrigger'
                                }
                            }
                            steps {
                                script {
                                    env.BUILD_AA4 = '1'
                                }
                                sh label: 'defectdetect-build',
                                script: '''
                                    pushd src
                                    source ../paeg-helper/env-setup.sh -r ${tool_release}
                                    ../paeg-helper/scripts/lsf make platform PFM=kv260_defectdetect
                                    popd
                                '''
                            }
                        }
                        stage('AA4 Build') {
                            environment {
                                PAEG_LSF_MEM=65536
                                PAEG_LSF_QUEUE="long"
                            }
                            when {
                                anyOf {
                                    changeset "**/kernels/examples/som_aa4/**"
                                    triggeredBy 'TimerTrigger'
                                    environment name: 'BUILD_AA4', value: '1'
                                }
                            }
                            steps {
                                sh label: 'aa4-build',
                                script: '''
                                    pushd src
                                    source ../paeg-helper/env-setup.sh -r ${tool_release}
                                    ../paeg-helper/scripts/lsf make kernel AA=aa4
                                    popd
                                '''
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
