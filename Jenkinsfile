pipeline {
    agent {
        label 'Build_Master'
    }
    environment {
        DEPLOYDIR="/wrk/paeg_builds/build-artifacts"
        PNAME="xilinx-kv260-apps-2020.2.2"
        PROOT="petalinux/${PNAME}"
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
                                    changeset "**/accelerators"
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
                        stage('AA1 Build & Import') {
                            environment {
                                PAEG_LSF_MEM=65536
                                PAEG_LSF_QUEUE="long"
                            }
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
                                    pushd src
                                    source ../paeg-helper/env-setup.sh -r ${tool_release}
                                    ../paeg-helper/scripts/lsf make fw-import AA=aa1
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
                                    changeset "**/accelerators"
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
                        stage('AA2 Build & Import') {
                            environment {
                                PAEG_LSF_MEM=65536
                                PAEG_LSF_QUEUE="long"
                            }
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
                                    pushd src
                                    source ../paeg-helper/env-setup.sh -r ${tool_release}
                                    ../paeg-helper/scripts/lsf make fw-import AA=aa2
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
                                    changeset "**/accelerators"
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
                        stage('AA4 Build & Import') {
                            environment {
                                PAEG_LSF_MEM=65536
                                PAEG_LSF_QUEUE="long"
                            }
                            when {
                                anyOf {
                                    changeset "**/accelerators"
                                    triggeredBy 'TimerTrigger'
                                    environment name: 'BUILD_AA4', value: '1'
                                }
                            }
                            steps {
                                script {
                                    env.BUILD_PLNX = '1'
                                }
                                sh label: 'aa4-build',
                                script: '''
                                    pushd src
                                    source ../paeg-helper/env-setup.sh -r ${tool_release}
                                    ../paeg-helper/scripts/lsf make fw-import AA=aa4
                                    popd
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
                    changeset "**/${PROOT}"
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
                    pushd src
                    source ../paeg-helper/env-setup.sh -p -r ${tool_release} -b ${tool_build}
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
                    popd
                '''

                sh label: 'copy wic image',
                script:'''
                    pushd src
                    cp ${PROOT}/images/linux/petalinux-sdimage.wic.gz sdcard
                    popd
                '''
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
                allOf {
                    branch tool_release
                    anyOf {
                        triggeredBy 'TimerTrigger'
                        environment name: 'DEPLOY', value: '1'
                    }
                }
            }
            steps {
                sh label: 'clean project',
                script: '''
                    pushd src
                    make clean
                    cd ${PROOT}
                    git clean -dfx
                    cd -
                    cd accelerators
                    git clean -dfx
                    cd -
                    find . -name "*.git*" | xargs rm -rf {}
                    cd petalinux
                    tar cfvz ${PNAME}.bsp ${PNAME}
                    rm -rf ${PNAME}
                    cd -
                    rm Jenkinsfile
                    popd
                '''

                sh label: 'create release zip file',
                script: '''
                    mv src ${rel_name}
                    zip -r ${rel_name}.zip ${rel_name}
                '''
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
