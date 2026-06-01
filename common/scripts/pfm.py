# (C) Copyright 2020 - 2022 Xilinx, Inc.
# (C) Copyright 2023 - 2026 Advanced Micro Devices, Inc.
# SPDX-License-Identifier: Apache-2.0

import sys
import os
import shutil
import vitis

def help_msg():
    print("Usage: vitis -s pfm.py -xsa <file>")
    print("-xsa  <file>       xsa file location")
    print("-proc <processor>  processor (default: psu_cortexa53)")
    print("-help              this text")

# Set defaults
proc = "psu_cortexa53"
xsafile = None

# Parse arguments
i = 1
while i < len(sys.argv):
    arg = sys.argv[i]
    if arg == "-xsa":
        i += 1
        if i >= len(sys.argv):
            print("Error: -xsa requires a value")
            sys.exit(1)
        xsafile = sys.argv[i]
    elif arg == "-proc":
        i += 1
        if i >= len(sys.argv):
            print("Error: -proc requires a value")
            sys.exit(1)
        proc = sys.argv[i]
    elif arg == "-help":
        help_msg()
        sys.exit(0)
    else:
        print(f"{arg} is an invalid argument")
        sys.exit(1)
    i += 1

if xsafile is None:
    print("Error: -xsa argument is required")
    help_msg()
    sys.exit(1)

# Derive platform name from XSA filename
platform = os.path.splitext(os.path.basename(xsafile))[0]
ws = os.path.join("vitis", platform)

# Create helper directories and files
imagedir = os.path.abspath("image")
os.makedirs(imagedir, exist_ok=True)
bootdir = os.path.abspath("boot")
os.makedirs(bootdir, exist_ok=True)
biffile = os.path.abspath("linux.bif")
if not os.path.exists(biffile):
    open(biffile, "a").close()

# Create Vitis client and set workspace (always start clean)
if os.path.isdir(ws):
    shutil.rmtree(ws)
client = vitis.create_client()
client.set_workspace(path=ws)

# Create platform with Linux domain
plat = client.create_platform_component(
    name=platform,
    hw_design=xsafile,
    os="linux",
    cpu=proc,
    domain_name="smp_linux",
    no_boot_bsp=True,
)

# Configure domain
domain = plat.get_domain(name="smp_linux")
domain.set_sd_dir(path=imagedir)
domain.set_boot_dir(path=bootdir)
domain.set_bif(path=biffile)

# Generate platform
plat.build()

vitis.dispose()
