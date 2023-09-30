# valid platforms / overlays
PFM_LIST += kv260_ispMipiRx_vcu_DP kv260_vcuDecode_vmixDP kv260_ispMipiRx_vmixDP kv260_ispMipiRx_rpiMipiRx_DP kv260_bist
OVERLAY_LIST += smartcam aibox-reid defect-detect nlp-smartvision benchmark
FW_LIST += kv260-smartcam kv260-aibox-reid kv260-defect-detect kv260-nlp-smartvision kv260-benchmark kv260-bist

# overrides based on firmware name
ifeq ($(FW),kv260-smartcam)
  override OVERLAY = smartcam
endif
ifeq ($(FW),kv260-aibox-reid)
  override OVERLAY = aibox-reid
endif
ifeq ($(FW),kv260-defect-detect)
  override OVERLAY = defect-detect
endif
ifeq ($(FW),kv260-nlp-smartvision)
  override OVERLAY = nlp-smartvision
endif
ifeq ($(FW),kv260-benchmark)
  override OVERLAY = benchmark
endif
ifeq ($(FW),kv260-bist)
  override PFM = kv260_bist
endif

# overrides bassed on overlay name
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

