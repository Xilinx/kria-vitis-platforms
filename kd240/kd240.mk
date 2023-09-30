# valid platforms
PFM_LIST += kd240_motor_ctrl_qei kd240_bist
FW_LIST += kd240-motor-ctrl-qei kd240-bist

# overrides based on firmware name
ifeq ($(FW),kd240-motor-ctrl-qei)
  override PFM = kd240_motor_ctrl_qei
endif
ifeq ($(FW),kd240-bist)
  override PFM = kd240_bist
endif

