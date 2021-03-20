# /*
# * Copyright 2019 Xilinx Inc.
# *
# * Licensed under the Apache License, Version 2.0 (the "License");
# * you may not use this file except in compliance with the License.
# * You may obtain a copy of the License at
# *
# *    http://www.apache.org/licenses/LICENSE-2.0
# *
# * Unless required by applicable law or agreed to in writing, software
# * distributed under the License is distributed on an "AS IS" BASIS,
# * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# * See the License for the specific language governing permissions and
# * limitations under the License.
# */

# delete the fifo in the axi interconnect
#
set_property -dict [list CONFIG.STRATEGY {0} CONFIG.M00_HAS_REGSLICE {0} CONFIG.M00_HAS_DATA_FIFO {0} CONFIG.S00_HAS_REGSLICE {0} CONFIG.S01_HAS_REGSLICE {0} CONFIG.S00_HAS_DATA_FIFO {0} CONFIG.S01_HAS_DATA_FIFO {0}] [get_bd_cells axi_ic_PS_0_S_AXI_HP1_FPD]
set_property -dict [list CONFIG.STRATEGY {0} CONFIG.M00_HAS_REGSLICE {0} CONFIG.M00_HAS_DATA_FIFO {0} CONFIG.S00_HAS_REGSLICE {0} CONFIG.S01_HAS_REGSLICE {0} CONFIG.S00_HAS_DATA_FIFO {0} CONFIG.S01_HAS_DATA_FIFO {0}] [get_bd_cells axi_ic_PS_0_S_AXI_HP2_FPD]
set_property -dict [list CONFIG.STRATEGY {0} CONFIG.M00_HAS_REGSLICE {0} CONFIG.M00_HAS_DATA_FIFO {0} CONFIG.S00_HAS_REGSLICE {0} CONFIG.S01_HAS_REGSLICE {0} CONFIG.S00_HAS_DATA_FIFO {0} CONFIG.S01_HAS_DATA_FIFO {0}] [get_bd_cells axi_ic_PS_0_S_AXI_LPD]
