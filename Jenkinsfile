pipeline {
    agent {
        label 'Build_Master'
    }
    environment {
        PROOT="petalinux/xilinx-kv260-smartcamera-2020.2-final"
        tool_release="2020.2.2"
        auto_branch="2020.2"
        rel_name="kv260_apps_${tool_release}"
    }
    triggers {
        cron(BRANCH_NAME == tool_release ? 'H 24 * * *' : '')
    }
    stages {
        stage('Clone Helper Repo') {
            steps {
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
                            steps {
                                sh label: 'smartcamera-build',
                                script: '''
                                    source ./paeg-helper/env-setup.sh -r ${tool_release}
                                    ./paeg-helper/scripts/lsf make smartcamera
                                '''
                            }
                        }
                        stage('AA1 Build') {
                            steps {
                                sh label: 'aa1-build',
                                script: '''
                                    source ./paeg-helper/env-setup.sh -r ${tool_release}
                                    ./paeg-helper/scripts/lsf make aa1
                                '''
                            }
                        }
                        stage('AA1 Import') {
                            steps {
                                sh label: 'aa1-import',
                                script: '''
                                    source ./paeg-helper/env-setup.sh -r ${tool_release}
                                    ./paeg-helper/scripts/lsf make aa1-import
                                '''
                            }
                        }
                    }
                }
                stage('KV260 AI Box') {
                    stages {
                        stage('KV260 AI Box Platform Build')  {
                            steps {
                                sh label: 'aibox-build',
                                script: '''
                                    source ./paeg-helper/env-setup.sh -r ${tool_release}
                                    ./paeg-helper/scripts/lsf make aibox
                                '''
                            }
                        }
                        stage('AA2 Build') {
                            steps {
                                sh label: 'aa2-build',
                                script: '''
                                    source ./paeg-helper/env-setup.sh -r ${tool_release}
                                    ./paeg-helper/scripts/lsf make aa2
                                '''
                            }
                        }
                        stage('AA2 Import') {
                            steps {
                                sh label: 'aa2-import',
                                script: '''
                                    source ./paeg-helper/env-setup.sh -r ${tool_release}
                                    ./paeg-helper/scripts/lsf make aa2-import
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
            steps {
                sh label: 'build PetaLinux project',
                script: '''
                    source ./paeg-helper/env-setup.sh -r ${tool_release} -p
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
                branch tool_release
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
                    rm -rf paeg-helper
                    mkdir -p ${rel_name}
                    cp -rf ./* ${rel_name} || true
                    rm ${rel_name}/Jenkinsfile
                '''

                sh label: 'create release zip file',
                script: 'zip -r ${rel_name}.zip ${rel_name}'

                sh label: 'deploy release zip file',
                script: '''
                    DST=/wrk/paeg_builds/build-artifacts/release-packages/${tool_release}/${rel_name}
                    mkdir -p ${DST}
                    cp ${rel_name}.zip ${DST}
                '''
            }
        }
    }
    post {
        cleanup {
            cleanWs()
        }
    }
}
