# valid platforms
PFM_LIST += kv260_ispMipiRx_vcu_DP kv260_vcuDecode_vmixDP kv260_ispMipiRx_vmixDP kv260_ispMipiRx_rpiMipiRx_DP kv260_bist
FW_LIST += kv260-bist

# overrides based on firmware name
ifeq ($(FW),kv260-bist)
  override PFM = kv260_bist
endif
