# Copyright (C) 2026, Advanced Micro Devices, Inc.
# SPDX-License-Identifier: Apache-2.0

import vitis
import os
import shutil

# Paths relative to this script's directory
script_dir = os.path.dirname(os.path.abspath(__file__))
vitis_libs = os.path.join(script_dir, '..', '..', '..', '..', 'common', 'Vitis_Libraries', 'motor_control')
svpwm_ip_dir = os.path.join(vitis_libs, 'L1', 'tests', 'IP_SVPWM')

# Parameters - can be overridden by environment variables
clkp = os.environ.get('CLKP', '10')
xpart = os.environ.get('XPART', 'xck24-ubva530-2LV-c')

comp_name = 'hls_svpwm_duty'
workspace = os.path.join(script_dir, 'workspace')

# Clean previous workspace
if os.path.isdir(workspace):
    shutil.rmtree(workspace)

client = vitis.create_client()
client.set_workspace(workspace)

# Create HLS component
comp = client.create_hls_component(name=comp_name, template='empty_hls_component')

# Configure via config file
cfg_path = os.path.join(workspace, comp_name, 'hls_config.cfg')
cfg = client.get_config_file(cfg_path)

# Part and clock
cfg.set_value('', key='part', value=xpart)
cfg.set_value('hls', key='clock', value=clkp)
cfg.set_value('hls', key='clock_uncertainty', value='1.25')

# Sources and top function
src_file = os.path.join(svpwm_ip_dir, 'src', 'ip_svpwm.cpp')
cflags = '-I' + os.path.join(vitis_libs, 'L1', 'include', 'hw') + ' -I' + os.path.join(svpwm_ip_dir, 'src')
cfg.add_lines('hls', [f'syn.file={src_file}'])
cfg.add_lines('hls', [f'syn.file_cflags={src_file}, {cflags}'])
cfg.add_lines('hls', [f'syn.top=hls_svpwm_duty'])

# IP packaging configuration
cfg.set_value('hls', key='package.output.format', value='ip_catalog')
cfg.set_value('hls', key='package.ip.name', value='hls_svpwm_duty')

# Run synthesis and package
comp = client.get_component(name=comp_name)
comp.run(operation='SYNTHESIS')
comp.run(operation='PACKAGE')

# Create sentinel file for Make dependency tracking
with open(os.path.join(script_dir, '.ip_done'), 'w') as f:
    f.write('done\n')

vitis.dispose()
print(f"HLS IP '{comp_name}' generated successfully.")
