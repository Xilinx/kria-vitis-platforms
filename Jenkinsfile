pipeline {
    agent {
        label 'Build_Master'
    }
    environment {
        DEPLOYDIR="/wrk/paeg_builds/build-artifacts"
        PROOT="petalinux/xilinx-kv260-smartcamera-2020.2-final"
        tool_release="2020.2.2"
        tool_build="regression_latest"
        auto_branch="2020.2"
        rel_name="kv260_apps_${tool_release}"
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
                // checkout main repo
                checkout scm
                // checkout paeg-automation helper
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
                                    triggeredBy 'TimerTrigger'
                                }
                            }
                            steps {
                                script {
                                    env.BUILD_AA1 = '1'
                                }
                                sh label: 'smartcamera-build',
                                script: '''
                                    source ./paeg-helper/env-setup.sh -r ${tool_release}
                                    ./paeg-helper/scripts/lsf make platform PFM=kv260_smartcamera
                                '''
                            }
                        }
                        stage('AA1 Build & Import') {
                            when {
                                anyOf {
                                    changeset "**/accelerators"
                                    triggeredBy 'TimerTrigger'
                                    environment name: 'BUILD_AA1', value: '1'
                                }
                            }
                            steps {
                                script {
                                    env.BUILD_PLNX = '1'
                                }
                                sh label: 'aa1-build',
                                script: '''
                                    source ./paeg-helper/env-setup.sh -r ${tool_release}
                                    export PAEG_LSF_MEM=65536
                                    export PAEG_LSF_QUEUE="long"
                                    ./paeg-helper/scripts/lsf make fw-import AA=aa1
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
                                    triggeredBy 'TimerTrigger'
                                }
                            }
                            steps {
                                script {
                                    env.BUILD_AA2 = '1'
                                }
                                sh label: 'aibox-build',
                                script: '''
                                    source ./paeg-helper/env-setup.sh -r ${tool_release}
                                    ./paeg-helper/scripts/lsf make platform PFM=kv260_aibox
                                '''
                            }
                        }
                        stage('AA2 Build & Import') {
                            when {
                                anyOf {
                                    changeset "**/accelerators"
                                    triggeredBy 'TimerTrigger'
                                    environment name: 'BUILD_AA2', value: '1'
                                }
                            }
                            steps {
                                script {
                                    env.BUILD_PLNX = '1'
                                }
                                sh label: 'aa2-build',
                                script: '''
                                    source ./paeg-helper/env-setup.sh -r ${tool_release}
                                    export PAEG_LSF_MEM=65536
                                    export PAEG_LSF_QUEUE="long"
                                    ./paeg-helper/scripts/lsf make fw-import AA=aa2
                                '''
                            }
                        }
                    }
                }
            }
        }
        stage('PetaLinux Build') {
            agent {
                node {
                    label 'Slave'
                    customWorkspace "${WORKSPACE}"
                }
            }
            environment {
                NEWTMPDIR = sh(script: 'mktemp -d /tmp/${rel_name}.XXXXXXXXXX', returnStdout: true).trim()
            }
            options {
                skipDefaultCheckout true
            }
            when {
                anyOf {
                    changeset "**/petalinux/**"
                    triggeredBy 'TimerTrigger'
                    environment name: 'BUILD_PLNX', value: '1'
                }
            }
            steps {
                script {
                    env.DEPLOY = '1'
                }
                sh label: 'build PetaLinux project',
                script: '''
                    source ./paeg-helper/env-setup.sh -p -r ${tool_release} -b ${tool_build}
                    cd ${rel_name}
                    # set TMPDIR
                    sed -i -e "s#CONFIG_TMP_DIR_LOCATION=.*#CONFIG_TMP_DIR_LOCATION=\\"${NEWTMPDIR}\\"#" \
                        ${PROOT}/project-spec/configs/config
                    # answer yes to SDK update prompt
                    cd ${PROOT}
                    yes | petalinux-config --silentconfig
                    cd -
                    # main build
                    make sdcard
                    # restore TMPDIR to default
                    sed -i -e "s#CONFIG_TMP_DIR_LOCATION=.*#CONFIG_TMP_DIR_LOCATION=\\"\\${PROOT}/build/tmp\\"#" \
                        ${PROOT}/project-spec/configs/config
                '''

                sh label: 'copy wic image',
                script:'cp ${PROOT}/images/linux/petalinux-sdimage.wic.gz sdcard'
            }
            post {
                cleanup {
                    sh label: 'delete TMPDIR',
                    script: 'rm -rf ${NEWTMPDIR}'
                }
            }
        }
        stage('Package and Deploy') {
            when {
                anyOf {
                    branch tool_release
                    triggeredBy 'TimerTrigger'
                    environment name: 'DEPLOY', value: '1'
                }
            }
            steps {
                sh label: 'clean project',
                script: '''
                    make clean
                    cd ${PROOT}
                    git clean -dfx
                    cd -
                    cd accelerators
                    git clean -dfx
                    cd -
                    find . -name "*.git*" | xargs rm -rf {}
                    rm -rf paeg-helper*
                    mkdir -p ${rel_name}
                    cp -rf ./* ${rel_name} || true
                    rm ${rel_name}/Jenkinsfile
                    rmdir ${rel_name}/${rel_name}
                '''

                sh label: 'create release zip file',
                script: 'zip -r ${rel_name}.zip ${rel_name}'
            }
            post {
                success {
                    sh label: 'deploy release zip file',
                    script: '''
                        if [ "${BRANCH_NAME}" == "${tool_release}" ]; then
                            DST=${DEPLOYDIR}/release-packages/${tool_release}/${rel_name}
                            mkdir -p ${DST}
                            cp ${rel_name}.zip ${DST}
                        fi
                    '''
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
