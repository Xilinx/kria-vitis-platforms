# valid platforms
PFM_LIST += kr260_tsn_rs485pmod kr260_pmod_gps
FW_LIST += kr260-tsn-rs485pmod kr260-pmod-gps

# overrides based on firmware name
ifeq ($(FW),kr260-tsn-rs485pmod)
  override PFM = kr260_tsn_rs485pmod
endif
ifeq ($(FW),kr260-pmod-gps)
  override PFM = kr260_pmod_gps
endif

