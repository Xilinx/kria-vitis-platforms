# Copyright (C) 2022, Advanced Micro Devices, Inc.
# SPDX-License-Identifier: Apache-2.0

proc init { cellpath otherInfo } {
  set ip [get_bd_cells $cellpath]
}

#######
proc post_config_ip {cellpath otherInfo} {
# Any updates to interface properties based on user configuration
#

}
