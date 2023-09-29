# valid platforms / overlays
PFM_LIST += kv260_ispMipiRx_vcu_DP kv260_vcuDecode_vmixDP kv260_ispMipiRx_vmixDP kv260_ispMipiRx_rpiMipiRx_DP kv260_bist
OVERLAY_LIST += smartcam aibox-reid defect-detect nlp-smartvision benchmark

# override platform name based on overlay
ifeq ($(OVERLAY),smartcam)
  override PFM = kv260_ispMipiRx_vcu_DP
endif
ifeq ($(OVERLAY),aibox-reid)
  override PFM = kv260_vcuDecode_vmixDP
endif
ifeq ($(OVERLAY),defect-detect)
  override PFM = kv260_ispMipiRx_vmixDP
endif
ifeq ($(OVERLAY),nlp-smartvision)
  override PFM = kv260_ispMipiRx_rpiMipiRx_DP
endif
ifeq ($(OVERLAY),benchmark)
  override PFM = kv260_ispMipiRx_vcu_DP
endif

