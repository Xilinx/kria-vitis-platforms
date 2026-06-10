# Copyright (C) 2026, Advanced Micro Devices, Inc.
# SPDX-License-Identifier: Apache-2.0

# Demote SIM_DEVICE arch mismatch DRC (simulation-only; does not affect hardware)
# Guard: the check may not exist in all Vivado versions
set adef911 [get_drc_checks -quiet ADEF-911]
if {[llength $adef911] > 0} {
    set_property SEVERITY {Warning} $adef911
}
