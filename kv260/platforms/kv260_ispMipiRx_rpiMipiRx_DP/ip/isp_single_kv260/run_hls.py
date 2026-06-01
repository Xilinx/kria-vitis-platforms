# Copyright (C) 2026, Advanced Micro Devices, Inc.
# SPDX-License-Identifier: Apache-2.0

import vitis
import os
import shutil

script_dir = os.path.dirname(os.path.abspath(__file__))
vitis_libs = os.path.join(script_dir, '..', '..', '..', '..', '..', 'common', 'Vitis_Libraries', 'vision')

clkp = os.environ.get('CLKP', '3.3')
xpart = os.environ.get('XPART', 'xck26-sfvc784-2LV-c')

comp_name = 'isppipeline'
workspace = os.path.join(script_dir, 'isppipeline.prj')

if os.path.isdir(workspace):
    shutil.rmtree(workspace)

client = vitis.create_client()
client.set_workspace(workspace)

comp = client.create_hls_component(name=comp_name, template='empty_hls_component')

cfg_path = os.path.join(workspace, comp_name, 'hls_config.cfg')
cfg = client.get_config_file(cfg_path)

cfg.set_value('', key='part', value=xpart)
cfg.set_value('hls', key='clock', value=clkp)
cfg.set_value('hls', key='flow_target', value='vivado')

src_file = os.path.join(vitis_libs, 'L1', 'examples', 'isppipeline', 'xf_isp_accel.cpp')
build_dir = os.path.join(script_dir, 'build')
cflags = (
    '-I' + os.path.join(vitis_libs, 'L1', 'include') +
    ' -I' + build_dir + ' -I' + script_dir + ' -D__SDSVHLS__ -std=c++0x'
)
cfg.add_lines('hls', [f'syn.file={src_file}'])
cfg.add_lines('hls', [f'syn.file_cflags={src_file}, {cflags}'])
cfg.add_lines('hls', ['syn.top=ISPPipeline_accel'])

cfg.set_value('hls', key='package.output.format', value='ip_catalog')
cfg.set_value('hls', key='package.ip.name', value='ISPPipeline_accel')

# Vitis normalizes absolute include paths to relative paths based on the
# component dir (isppipeline.prj/isppipeline/). Copy xf_config_params.h into
# a build/ subdir there so the stored -I./build cflags entry resolves correctly.
comp_dir = os.path.join(workspace, comp_name)
os.makedirs(comp_dir, exist_ok=True)
build_copy_dir = os.path.join(comp_dir, 'build')
os.makedirs(build_copy_dir, exist_ok=True)
shutil.copy(
    os.path.join(build_dir, 'xf_config_params.h'),
    os.path.join(build_copy_dir, 'xf_config_params.h'))

comp = client.get_component(name=comp_name)
comp.run('SYNTHESIS')
comp.run('PACKAGE')

vitis.dispose()
