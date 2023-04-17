# Copyright (C) 2022 - 2023, Advanced Micro Devices, Inc.
# SPDX-License-Identifier: Apache-2.0

# Definitional proc to organize widgets for parameters.
proc init_gui { IPINST } {

  #Adding Page
  set Page_0 [ipgui::add_page $IPINST -name "Page 0" -display_name {Page 0}]
  set_property tooltip {Page 0} ${Page_0}

  set Component_Name  [  ipgui::add_param $IPINST -name "Component_Name" -parent ${Page_0} -display_name {Component Name}]
  set_property tooltip {Component Name} ${Component_Name}

  set NUM_CHANNELS [ipgui::add_param $IPINST -parent ${Page_0} -name "NUM_CHANNELS" -display_name {Number of Lanes} -widget comboBox]
  set_property tooltip {Number of Analog to Digital Converters to be Controlled} ${NUM_CHANNELS}

  set C_S_AXI_DATA_WIDTH [ipgui::add_param $IPINST -parent ${Page_0} -name "C_S_AXI_DATA_WIDTH" -display_name {AXI-Lite Data Width} -widget comboBox]

  set C_S_AXI_ADDR_WIDTH [ipgui::add_param $IPINST -parent ${Page_0} -name "C_S_AXI_ADDR_WIDTH" -display_name {AXI-Lite Address Width} -widget comboBox]

  # Hard-code for now
  set PARAM_VALUE.QSCALE_MAX_BITS 24

  variable table
  set table [ipgui::add_table $IPINST  -name "Lane Table" -rows "17" -columns "5" -parent ${Page_0}]

  ipgui::add_static_text  $IPINST -name Lane -parent $table -text "Lane" -center_align true
  ipgui::add_static_text  $IPINST -name ADC_Bits -parent $table -text "ADC Bits" -center_align true
  ipgui::add_static_text  $IPINST -name Type -parent $table -text "Type" -center_align true
  ipgui::add_static_text  $IPINST -name Encoding -parent $table -text "Encoding" -center_align true
  ipgui::add_static_text  $IPINST -name Use_DSP -parent $table -text "Use\nDSP" -center_align true

  set_property cell_location "0,0" [ipgui::get_textspec Lane -of $IPINST]
  set_property cell_location "0,1" [ipgui::get_textspec ADC_Bits -of $IPINST]
  set_property cell_location "0,2" [ipgui::get_textspec Type -of $IPINST]
  set_property cell_location "0,3" [ipgui::get_textspec Encoding -of $IPINST]
  set_property cell_location "0,4" [ipgui::get_textspec Use_DSP -of $IPINST]

  # Lane 0
  ipgui::add_static_text  $IPINST -name Lane0 -parent  $table -text "0" -center_align true
  set_property cell_location "1,0" [ipgui::get_textspec Lane0 -of $IPINST]

  set L0_ADC_BITS [ipgui::add_param $IPINST -parent $table -name L0_ADC_BITS]
  set_property cell_location "1,1" [ipgui::get_guiparamspec L0_ADC_BITS -of $IPINST]

#  set L0_TYPE [ipgui::add_param $IPINST -parent $table -name L0_TYPE -show_range false]
  set L0_TYPE [ipgui::add_param $IPINST -parent $table -name L0_TYPE]
  set_property cell_location "1,2" [ipgui::get_guiparamspec L0_TYPE -of $IPINST]

#  set L0_CODE [ipgui::add_param $IPINST -parent $table -name L0_CODE -show_range false]
  set L0_CODE [ipgui::add_param $IPINST -parent $table -name L0_CODE]
  set_property cell_location "1,3" [ipgui::get_guiparamspec L0_CODE -of $IPINST]

  set L0_USE_DSP [ipgui::add_param $IPINST -parent $table -name L0_USE_DSP -show_range false]
  set_property cell_location "1,4" [ipgui::get_guiparamspec L0_USE_DSP -of $IPINST]

  set_property tooltip "Hub uses either Unipolar Straight Binary (USB) or Binary Two's Complement (BTC)" $L0_CODE
#  set_property tooltip  "Set the line buffer width of layer 1. Valid range is ([get_property range $LAYER1_MAX_WIDTH])." $LAYER1_MAX_WIDTH
#  set_property tooltip  "Enable alpha blending for layer 1." $LAYER1_ALPHA
#  set_property tooltip  "Enable scaling for layer 1." $LAYER1_UPSAMPLE

  # Lane 1
  ipgui::add_static_text  $IPINST -name Lane1 -parent  $table -text "1" -center_align true
  set_property cell_location "2,0" [ipgui::get_textspec Lane1 -of $IPINST]

  set L1_ADC_BITS [ipgui::add_param $IPINST -parent $table -name L1_ADC_BITS]
  set_property cell_location "2,1" [ipgui::get_guiparamspec L1_ADC_BITS -of $IPINST]

  set L1_TYPE [ipgui::add_param $IPINST -parent $table -name L1_TYPE]
  set_property cell_location "2,2" [ipgui::get_guiparamspec L1_TYPE -of $IPINST]

  set L1_CODE [ipgui::add_param $IPINST -parent $table -name L1_CODE -show_range false]
  set_property cell_location "2,3" [ipgui::get_guiparamspec L1_CODE -of $IPINST]

  set L1_USE_DSP [ipgui::add_param $IPINST -parent $table -name L1_USE_DSP -show_range false]
  set_property cell_location "2,4" [ipgui::get_guiparamspec L1_USE_DSP -of $IPINST]

  set_property tooltip "Hub uses either Unipolar Straight Binary (USB) or Binary Two's Complement (BTC)" $L1_CODE
#  set_property tooltip  "Set the line buffer width of layer 1. Valid range is ([get_property range $LAYER1_MAX_WIDTH])." $LAYER1_MAX_WIDTH
#  set_property tooltip  "Enable alpha blending for layer 1." $LAYER1_ALPHA
#  set_property tooltip  "Enable scaling for layer 1." $LAYER1_UPSAMPLE

  # Lane 2
  ipgui::add_static_text  $IPINST -name Lane2 -parent  $table -text "2" -center_align true
  set_property cell_location "3,0" [ipgui::get_textspec Lane2 -of $IPINST]

  set L2_ADC_BITS [ipgui::add_param $IPINST -parent $table -name L2_ADC_BITS]
  set_property cell_location "3,1" [ipgui::get_guiparamspec L2_ADC_BITS -of $IPINST]

  set L2_TYPE [ipgui::add_param $IPINST -parent $table -name L2_TYPE]
  set_property cell_location "3,2" [ipgui::get_guiparamspec L2_TYPE -of $IPINST]

  set L2_CODE [ipgui::add_param $IPINST -parent $table -name L2_CODE -show_range false]
  set_property cell_location "3,3" [ipgui::get_guiparamspec L2_CODE -of $IPINST]

  set L2_USE_DSP [ipgui::add_param $IPINST -parent $table -name L2_USE_DSP -show_range false]
  set_property cell_location "3,4" [ipgui::get_guiparamspec L2_USE_DSP -of $IPINST]

  set_property tooltip "Hub uses either Unipolar Straight Binary (USB) or Binary Two's Complement (BTC)" $L2_CODE
#  set_property tooltip  "Set the line buffer width of layer 1. Valid range is ([get_property range $LAYER1_MAX_WIDTH])." $LAYER1_MAX_WIDTH
#  set_property tooltip  "Enable alpha blending for layer 1." $LAYER1_ALPHA
#  set_property tooltip  "Enable scaling for layer 1." $LAYER1_UPSAMPLE

  # Lane 3
  ipgui::add_static_text  $IPINST -name Lane3 -parent  $table -text "3" -center_align true
  set_property cell_location "4,0" [ipgui::get_textspec Lane3 -of $IPINST]

  set L3_ADC_BITS [ipgui::add_param $IPINST -parent $table -name L3_ADC_BITS]
  set_property cell_location "4,1" [ipgui::get_guiparamspec L3_ADC_BITS -of $IPINST]

  set L3_TYPE [ipgui::add_param $IPINST -parent $table -name L3_TYPE]
  set_property cell_location "4,2" [ipgui::get_guiparamspec L3_TYPE -of $IPINST]

  set L3_CODE [ipgui::add_param $IPINST -parent $table -name L3_CODE -show_range false]
  set_property cell_location "4,3" [ipgui::get_guiparamspec L3_CODE -of $IPINST]

  set L3_USE_DSP [ipgui::add_param $IPINST -parent $table -name L3_USE_DSP -show_range false]
  set_property cell_location "4,4" [ipgui::get_guiparamspec L3_USE_DSP -of $IPINST]

  set_property tooltip "Hub uses either Unipolar Straight Binary (USB) or Binary Two's Complement (BTC)" $L3_CODE
#  set_property tooltip  "Set the line buffer width of layer 1. Valid range is ([get_property range $LAYER1_MAX_WIDTH])." $LAYER1_MAX_WIDTH
#  set_property tooltip  "Enable alpha blending for layer 1." $LAYER1_ALPHA
#  set_property tooltip  "Enable scaling for layer 1." $LAYER1_UPSAMPLE

  # Lane 4
  ipgui::add_static_text  $IPINST -name Lane4 -parent  $table -text "4" -center_align true
  set_property cell_location "5,0" [ipgui::get_textspec Lane4 -of $IPINST]

  set L4_ADC_BITS [ipgui::add_param $IPINST -parent $table -name L4_ADC_BITS]
  set_property cell_location "5,1" [ipgui::get_guiparamspec L4_ADC_BITS -of $IPINST]

  set L4_TYPE [ipgui::add_param $IPINST -parent $table -name L4_TYPE]
  set_property cell_location "5,2" [ipgui::get_guiparamspec L4_TYPE -of $IPINST]

  set L4_CODE [ipgui::add_param $IPINST -parent $table -name L4_CODE -show_range false]
  set_property cell_location "5,3" [ipgui::get_guiparamspec L4_CODE -of $IPINST]

  set L4_USE_DSP [ipgui::add_param $IPINST -parent $table -name L4_USE_DSP -show_range false]
  set_property cell_location "5,4" [ipgui::get_guiparamspec L4_USE_DSP -of $IPINST]

  set_property tooltip "Hub uses either Unipolar Straight Binary (USB) or Binary Two's Complement (BTC)" $L4_CODE
#  set_property tooltip  "Set the line buffer width of layer 1. Valid range is ([get_property range $LAYER1_MAX_WIDTH])." $LAYER1_MAX_WIDTH
#  set_property tooltip  "Enable alpha blending for layer 1." $LAYER1_ALPHA
#  set_property tooltip  "Enable scaling for layer 1." $LAYER1_UPSAMPLE

  # Lane 5
  ipgui::add_static_text  $IPINST -name Lane5 -parent  $table -text "5" -center_align true
  set_property cell_location "6,0" [ipgui::get_textspec Lane5 -of $IPINST]

  set L5_ADC_BITS [ipgui::add_param $IPINST -parent $table -name L5_ADC_BITS]
  set_property cell_location "6,1" [ipgui::get_guiparamspec L5_ADC_BITS -of $IPINST]

  set L5_TYPE [ipgui::add_param $IPINST -parent $table -name L5_TYPE]
  set_property cell_location "6,2" [ipgui::get_guiparamspec L5_TYPE -of $IPINST]

  set L5_CODE [ipgui::add_param $IPINST -parent $table -name L5_CODE -show_range false]
  set_property cell_location "6,3" [ipgui::get_guiparamspec L5_CODE -of $IPINST]

  set L5_USE_DSP [ipgui::add_param $IPINST -parent $table -name L5_USE_DSP -show_range false]
  set_property cell_location "6,4" [ipgui::get_guiparamspec L5_USE_DSP -of $IPINST]

  set_property tooltip "Hub uses either Unipolar Straight Binary (USB) or Binary Two's Complement (BTC)" $L5_CODE
#  set_property tooltip  "Set the line buffer width of layer 1. Valid range is ([get_property range $LAYER1_MAX_WIDTH])." $LAYER1_MAX_WIDTH
#  set_property tooltip  "Enable alpha blending for layer 1." $LAYER1_ALPHA
#  set_property tooltip  "Enable scaling for layer 1." $LAYER1_UPSAMPLE

  # Lane 6
  ipgui::add_static_text  $IPINST -name Lane6 -parent  $table -text "6" -center_align true
  set_property cell_location "7,0" [ipgui::get_textspec Lane6 -of $IPINST]

  set L6_ADC_BITS [ipgui::add_param $IPINST -parent $table -name L6_ADC_BITS]
  set_property cell_location "7,1" [ipgui::get_guiparamspec L6_ADC_BITS -of $IPINST]

  set L6_TYPE [ipgui::add_param $IPINST -parent $table -name L6_TYPE]
  set_property cell_location "7,2" [ipgui::get_guiparamspec L6_TYPE -of $IPINST]

  set L6_CODE [ipgui::add_param $IPINST -parent $table -name L6_CODE -show_range false]
  set_property cell_location "7,3" [ipgui::get_guiparamspec L6_CODE -of $IPINST]

  set L6_USE_DSP [ipgui::add_param $IPINST -parent $table -name L6_USE_DSP -show_range false]
  set_property cell_location "7,4" [ipgui::get_guiparamspec L6_USE_DSP -of $IPINST]

  set_property tooltip "Hub uses either Unipolar Straight Binary (USB) or Binary Two's Complement (BTC)" $L6_CODE
#  set_property tooltip  "Set the line buffer width of layer 1. Valid range is ([get_property range $LAYER1_MAX_WIDTH])." $LAYER1_MAX_WIDTH
#  set_property tooltip  "Enable alpha blending for layer 1." $LAYER1_ALPHA
#  set_property tooltip  "Enable scaling for layer 1." $LAYER1_UPSAMPLE

  # Lane 7
  ipgui::add_static_text  $IPINST -name Lane7 -parent  $table -text "7" -center_align true
  set_property cell_location "8,0" [ipgui::get_textspec Lane7 -of $IPINST]

  set L7_ADC_BITS [ipgui::add_param $IPINST -parent $table -name L7_ADC_BITS]
  set_property cell_location "8,1" [ipgui::get_guiparamspec L7_ADC_BITS -of $IPINST]

  set L7_TYPE [ipgui::add_param $IPINST -parent $table -name L7_TYPE]
  set_property cell_location "8,2" [ipgui::get_guiparamspec L7_TYPE -of $IPINST]

  set L7_CODE [ipgui::add_param $IPINST -parent $table -name L7_CODE -show_range false]
  set_property cell_location "8,3" [ipgui::get_guiparamspec L7_CODE -of $IPINST]

  set L7_USE_DSP [ipgui::add_param $IPINST -parent $table -name L7_USE_DSP -show_range false]
  set_property cell_location "8,4" [ipgui::get_guiparamspec L7_USE_DSP -of $IPINST]

  set_property tooltip "Hub uses either Unipolar Straight Binary (USB) or Binary Two's Complement (BTC)" $L7_CODE
#  set_property tooltip  "Set the line buffer width of layer 1. Valid range is ([get_property range $LAYER1_MAX_WIDTH])." $LAYER1_MAX_WIDTH
#  set_property tooltip  "Enable alpha blending for layer 1." $LAYER1_ALPHA
#  set_property tooltip  "Enable scaling for layer 1." $LAYER1_UPSAMPLE

  # Lane 8
  ipgui::add_static_text  $IPINST -name Lane8 -parent  $table -text "8" -center_align true
  set_property cell_location "9,0" [ipgui::get_textspec Lane8 -of $IPINST]

  set L8_ADC_BITS [ipgui::add_param $IPINST -parent $table -name L8_ADC_BITS]
  set_property cell_location "9,1" [ipgui::get_guiparamspec L8_ADC_BITS -of $IPINST]

  set L8_TYPE [ipgui::add_param $IPINST -parent $table -name L8_TYPE]
  set_property cell_location "9,2" [ipgui::get_guiparamspec L8_TYPE -of $IPINST]

  set L8_CODE [ipgui::add_param $IPINST -parent $table -name L8_CODE -show_range false]
  set_property cell_location "9,3" [ipgui::get_guiparamspec L8_CODE -of $IPINST]

  set L8_USE_DSP [ipgui::add_param $IPINST -parent $table -name L8_USE_DSP -show_range false]
  set_property cell_location "9,4" [ipgui::get_guiparamspec L8_USE_DSP -of $IPINST]

  set_property tooltip "Hub uses either Unipolar Straight Binary (USB) or Binary Two's Complement (BTC)" $L8_CODE
#  set_property tooltip  "Set the line buffer width of layer 1. Valid range is ([get_property range $LAYER1_MAX_WIDTH])." $LAYER1_MAX_WIDTH
#  set_property tooltip  "Enable alpha blending for layer 1." $LAYER1_ALPHA
#  set_property tooltip  "Enable scaling for layer 1." $LAYER1_UPSAMPLE

  # Lane 9
  ipgui::add_static_text  $IPINST -name Lane9 -parent  $table -text "9" -center_align true
  set_property cell_location "10,0" [ipgui::get_textspec Lane9 -of $IPINST]

  set L9_ADC_BITS [ipgui::add_param $IPINST -parent $table -name L9_ADC_BITS]
  set_property cell_location "10,1" [ipgui::get_guiparamspec L9_ADC_BITS -of $IPINST]

  set L9_TYPE [ipgui::add_param $IPINST -parent $table -name L9_TYPE]
  set_property cell_location "10,2" [ipgui::get_guiparamspec L9_TYPE -of $IPINST]

  set L9_CODE [ipgui::add_param $IPINST -parent $table -name L9_CODE -show_range false]
  set_property cell_location "10,3" [ipgui::get_guiparamspec L9_CODE -of $IPINST]

  set L9_USE_DSP [ipgui::add_param $IPINST -parent $table -name L9_USE_DSP -show_range false]
  set_property cell_location "10,4" [ipgui::get_guiparamspec L9_USE_DSP -of $IPINST]

  set_property tooltip "Hub uses either Unipolar Straight Binary (USB) or Binary Two's Complement (BTC)" $L9_CODE
#  set_property tooltip  "Set the line buffer width of layer 1. Valid range is ([get_property range $LAYER1_MAX_WIDTH])." $LAYER1_MAX_WIDTH
#  set_property tooltip  "Enable alpha blending for layer 1." $LAYER1_ALPHA
#  set_property tooltip  "Enable scaling for layer 1." $LAYER1_UPSAMPLE

  # Lane 10
  ipgui::add_static_text  $IPINST -name Lane10 -parent  $table -text "10" -center_align true
  set_property cell_location "11,0" [ipgui::get_textspec Lane10 -of $IPINST]

  set L10_ADC_BITS [ipgui::add_param $IPINST -parent $table -name L10_ADC_BITS]
  set_property cell_location "11,1" [ipgui::get_guiparamspec L10_ADC_BITS -of $IPINST]

  set L10_TYPE [ipgui::add_param $IPINST -parent $table -name L10_TYPE]
  set_property cell_location "11,2" [ipgui::get_guiparamspec L10_TYPE -of $IPINST]

  set L10_CODE [ipgui::add_param $IPINST -parent $table -name L10_CODE -show_range false]
  set_property cell_location "11,3" [ipgui::get_guiparamspec L10_CODE -of $IPINST]

  set L10_USE_DSP [ipgui::add_param $IPINST -parent $table -name L10_USE_DSP -show_range false]
  set_property cell_location "11,4" [ipgui::get_guiparamspec L10_USE_DSP -of $IPINST]

  set_property tooltip "Hub uses either Unipolar Straight Binary (USB) or Binary Two's Complement (BTC)" $L10_CODE
#  set_property tooltip  "Set the line buffer width of layer 1. Valid range is ([get_property range $LAYER1_MAX_WIDTH])." $LAYER1_MAX_WIDTH
#  set_property tooltip  "Enable alpha blending for layer 1." $LAYER1_ALPHA
#  set_property tooltip  "Enable scaling for layer 1." $LAYER1_UPSAMPLE

  # Lane 11
  ipgui::add_static_text  $IPINST -name Lane11 -parent  $table -text "11" -center_align true
  set_property cell_location "12,0" [ipgui::get_textspec Lane11 -of $IPINST]

  set L11_ADC_BITS [ipgui::add_param $IPINST -parent $table -name L11_ADC_BITS]
  set_property cell_location "12,1" [ipgui::get_guiparamspec L11_ADC_BITS -of $IPINST]

  set L11_TYPE [ipgui::add_param $IPINST -parent $table -name L11_TYPE]
  set_property cell_location "12,2" [ipgui::get_guiparamspec L11_TYPE -of $IPINST]

  set L11_CODE [ipgui::add_param $IPINST -parent $table -name L11_CODE -show_range false]
  set_property cell_location "12,3" [ipgui::get_guiparamspec L11_CODE -of $IPINST]

  set L11_USE_DSP [ipgui::add_param $IPINST -parent $table -name L11_USE_DSP -show_range false]
  set_property cell_location "12,4" [ipgui::get_guiparamspec L11_USE_DSP -of $IPINST]

  set_property tooltip "Hub uses either Unipolar Straight Binary (USB) or Binary Two's Complement (BTC)" $L11_CODE
#  set_property tooltip  "Set the line buffer width of layer 1. Valid range is ([get_property range $LAYER1_MAX_WIDTH])." $LAYER1_MAX_WIDTH
#  set_property tooltip  "Enable alpha blending for layer 1." $LAYER1_ALPHA
#  set_property tooltip  "Enable scaling for layer 1." $LAYER1_UPSAMPLE

  # Lane 12
  ipgui::add_static_text  $IPINST -name Lane12 -parent  $table -text "12" -center_align true
  set_property cell_location "13,0" [ipgui::get_textspec Lane12 -of $IPINST]

  set L12_ADC_BITS [ipgui::add_param $IPINST -parent $table -name L12_ADC_BITS]
  set_property cell_location "13,1" [ipgui::get_guiparamspec L12_ADC_BITS -of $IPINST]

  set L12_TYPE [ipgui::add_param $IPINST -parent $table -name L12_TYPE]
  set_property cell_location "13,2" [ipgui::get_guiparamspec L12_TYPE -of $IPINST]

  set L12_CODE [ipgui::add_param $IPINST -parent $table -name L12_CODE -show_range false]
  set_property cell_location "13,3" [ipgui::get_guiparamspec L12_CODE -of $IPINST]

  set L12_USE_DSP [ipgui::add_param $IPINST -parent $table -name L12_USE_DSP -show_range false]
  set_property cell_location "13,4" [ipgui::get_guiparamspec L12_USE_DSP -of $IPINST]

  set_property tooltip "Hub uses either Unipolar Straight Binary (USB) or Binary Two's Complement (BTC)" $L12_CODE
#  set_property tooltip  "Set the line buffer width of layer 1. Valid range is ([get_property range $LAYER1_MAX_WIDTH])." $LAYER1_MAX_WIDTH
#  set_property tooltip  "Enable alpha blending for layer 1." $LAYER1_ALPHA
#  set_property tooltip  "Enable scaling for layer 1." $LAYER1_UPSAMPLE

  # Lane 13
  ipgui::add_static_text  $IPINST -name Lane13 -parent  $table -text "13" -center_align true
  set_property cell_location "14,0" [ipgui::get_textspec Lane13 -of $IPINST]

  set L13_ADC_BITS [ipgui::add_param $IPINST -parent $table -name L13_ADC_BITS]
  set_property cell_location "14,1" [ipgui::get_guiparamspec L13_ADC_BITS -of $IPINST]

  set L13_TYPE [ipgui::add_param $IPINST -parent $table -name L13_TYPE]
  set_property cell_location "14,2" [ipgui::get_guiparamspec L13_TYPE -of $IPINST]

  set L13_CODE [ipgui::add_param $IPINST -parent $table -name L13_CODE -show_range false]
  set_property cell_location "14,3" [ipgui::get_guiparamspec L13_CODE -of $IPINST]

  set L13_USE_DSP [ipgui::add_param $IPINST -parent $table -name L13_USE_DSP -show_range false]
  set_property cell_location "14,4" [ipgui::get_guiparamspec L13_USE_DSP -of $IPINST]

  set_property tooltip "Hub uses either Unipolar Straight Binary (USB) or Binary Two's Complement (BTC)" $L13_CODE
#  set_property tooltip  "Set the line buffer width of layer 1. Valid range is ([get_property range $LAYER1_MAX_WIDTH])." $LAYER1_MAX_WIDTH
#  set_property tooltip  "Enable alpha blending for layer 1." $LAYER1_ALPHA
#  set_property tooltip  "Enable scaling for layer 1." $LAYER1_UPSAMPLE

  # Lane 14
  ipgui::add_static_text  $IPINST -name Lane14 -parent  $table -text "14" -center_align true
  set_property cell_location "15,0" [ipgui::get_textspec Lane14 -of $IPINST]

  set L14_ADC_BITS [ipgui::add_param $IPINST -parent $table -name L14_ADC_BITS]
  set_property cell_location "15,1" [ipgui::get_guiparamspec L14_ADC_BITS -of $IPINST]

  set L14_TYPE [ipgui::add_param $IPINST -parent $table -name L14_TYPE]
  set_property cell_location "15,2" [ipgui::get_guiparamspec L14_TYPE -of $IPINST]

  set L14_CODE [ipgui::add_param $IPINST -parent $table -name L14_CODE -show_range false]
  set_property cell_location "15,3" [ipgui::get_guiparamspec L14_CODE -of $IPINST]

  set L14_USE_DSP [ipgui::add_param $IPINST -parent $table -name L14_USE_DSP -show_range false]
  set_property cell_location "15,4" [ipgui::get_guiparamspec L14_USE_DSP -of $IPINST]

  set_property tooltip "Hub uses either Unipolar Straight Binary (USB) or Binary Two's Complement (BTC)" $L14_CODE
#  set_property tooltip  "Set the line buffer width of layer 1. Valid range is ([get_property range $LAYER1_MAX_WIDTH])." $LAYER1_MAX_WIDTH
#  set_property tooltip  "Enable alpha blending for layer 1." $LAYER1_ALPHA
#  set_property tooltip  "Enable scaling for layer 1." $LAYER1_UPSAMPLE

  # Lane 15
  ipgui::add_static_text  $IPINST -name Lane15 -parent  $table -text "15" -center_align true
  set_property cell_location "16,0" [ipgui::get_textspec Lane15 -of $IPINST]

  set L15_ADC_BITS [ipgui::add_param $IPINST -parent $table -name L15_ADC_BITS]
  set_property cell_location "16,1" [ipgui::get_guiparamspec L15_ADC_BITS -of $IPINST]

  set L15_TYPE [ipgui::add_param $IPINST -parent $table -name L15_TYPE]
  set_property cell_location "16,2" [ipgui::get_guiparamspec L15_TYPE -of $IPINST]

  set L15_CODE [ipgui::add_param $IPINST -parent $table -name L15_CODE -show_range false]
  set_property cell_location "16,3" [ipgui::get_guiparamspec L15_CODE -of $IPINST]

  set L15_USE_DSP [ipgui::add_param $IPINST -parent $table -name L15_USE_DSP -show_range false]
  set_property cell_location "16,4" [ipgui::get_guiparamspec L15_USE_DSP -of $IPINST]

  set_property tooltip "Hub uses either Unipolar Straight Binary (USB) or Binary Two's Complement (BTC)" $L15_CODE
#  set_property tooltip  "Set the line buffer width of layer 1. Valid range is ([get_property range $LAYER1_MAX_WIDTH])." $LAYER1_MAX_WIDTH
#  set_property tooltip  "Enable alpha blending for layer 1." $LAYER1_ALPHA
#  set_property tooltip  "Enable scaling for layer 1." $LAYER1_UPSAMPLE

}


proc update_PARAM_VALUE.C_S_AXI_ADDR_WIDTH { PARAM_VALUE.C_S_AXI_ADDR_WIDTH } {
	# Procedure called to update C_S_AXI_ADDR_WIDTH when any of the dependent parameters in the arguments change
  set_property range "12,13,14,15,16" ${PARAM_VALUE.C_S_AXI_ADDR_WIDTH}
}

proc validate_PARAM_VALUE.C_S_AXI_ADDR_WIDTH { PARAM_VALUE.C_S_AXI_ADDR_WIDTH } {
	# Procedure called to validate C_S_AXI_ADDR_WIDTH
	return true
}

proc update_PARAM_VALUE.C_S_AXI_DATA_WIDTH { PARAM_VALUE.C_S_AXI_DATA_WIDTH } {
	# Procedure called to update C_S_AXI_DATA_WIDTH when any of the dependent parameters in the arguments change
  set_property range "32,64" ${PARAM_VALUE.C_S_AXI_DATA_WIDTH}
}

proc validate_PARAM_VALUE.C_S_AXI_DATA_WIDTH { PARAM_VALUE.C_S_AXI_DATA_WIDTH } {
	# Procedure called to validate C_S_AXI_DATA_WIDTH
	return true
}

proc update_PARAM_VALUE.NUM_CHANNELS { PARAM_VALUE.NUM_CHANNELS } {
	# Procedure called to update NUM_CHANNELS when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.NUM_CHANNELS { PARAM_VALUE.NUM_CHANNELS } {
	# Procedure called to validate NUM_CHANNELS
	return true
}

# Lane 1

proc update_PARAM_VALUE.L0_USE_DSP { PARAM_VALUE.L0_USE_DSP } {
	# Procedure called to update USE_DSP when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.L0_USE_DSP { PARAM_VALUE.L0_USE_DSP } {
	# Procedure called to validate USE_DSP
	return true
}

proc update_PARAM_VALUE.L0_ADC_BITS { PARAM_VALUE.L0_ADC_BITS } {
  # Procedure called to update ADC_BITS when any of the dependent parameters in the arguments change
  set_property range "2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24" ${PARAM_VALUE.L0_ADC_BITS}
  set_property enabled true ${PARAM_VALUE.L0_ADC_BITS}

}

proc validate_PARAM_VALUE.L0_ADC_BITS { PARAM_VALUE.L0_ADC_BITS } {
	# Procedure called to validate ADC_BITS
	return true
}

proc update_PARAM_VALUE.L0_TYPE { PARAM_VALUE.L0_TYPE } {
	# Procedure called to update TYPE when any of the dependent parameters in the arguments change
  set_property range "VOLTAGE,CURRENT" ${PARAM_VALUE.L0_TYPE}
}

proc validate_PARAM_VALUE.L0_TYPE { PARAM_VALUE.L0_TYPE } {
	# Procedure called to validate TYPE
	return true
}

proc update_PARAM_VALUE.L0_CODE { PARAM_VALUE.L0_CODE } {
	# Procedure called to update CODE when any of the dependent parameters in the arguments change
  set_property range "USB,BTC" ${PARAM_VALUE.L0_CODE}

}

proc validate_PARAM_VALUE.L0_CODE { PARAM_VALUE.L0_CODE } {
	# Procedure called to validate CODE
	return true
}

# Lane 2

proc update_PARAM_VALUE.L1_USE_DSP { PARAM_VALUE.L1_USE_DSP } {
	# Procedure called to update USE_DSP when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.L1_USE_DSP { PARAM_VALUE.L1_USE_DSP } {
	# Procedure called to validate USE_DSP
	return true
}

proc update_PARAM_VALUE.L1_ADC_BITS { PARAM_VALUE.L1_ADC_BITS } {
  # Procedure called to update ADC_BITS when any of the dependent parameters in the arguments change
  set_property range "2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24" ${PARAM_VALUE.L1_ADC_BITS}
  set_property enabled true ${PARAM_VALUE.L1_ADC_BITS}
}

proc validate_PARAM_VALUE.L1_ADC_BITS { PARAM_VALUE.L1_ADC_BITS } {
	# Procedure called to validate ADC_BITS
	return true
}

proc update_PARAM_VALUE.L1_TYPE { PARAM_VALUE.L1_TYPE } {
	# Procedure called to update TYPE when any of the dependent parameters in the arguments change
  set_property range "VOLTAGE,CURRENT" ${PARAM_VALUE.L1_TYPE}
}

proc validate_PARAM_VALUE.L1_TYPE { PARAM_VALUE.L1_TYPE } {
	# Procedure called to validate TYPE
	return true
}

proc update_PARAM_VALUE.L1_CODE { PARAM_VALUE.L1_CODE } {
	# Procedure called to update CODE when any of the dependent parameters in the arguments change
  set_property range "USB,BTC" ${PARAM_VALUE.L1_CODE}
}

proc validate_PARAM_VALUE.L1_CODE { PARAM_VALUE.L1_CODE } {
	# Procedure called to validate CODE
	return true
}

# Lane 3

proc update_PARAM_VALUE.L2_USE_DSP { PARAM_VALUE.L2_USE_DSP } {
	# Procedure called to update USE_DSP when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.L2_USE_DSP { PARAM_VALUE.L2_USE_DSP } {
	# Procedure called to validate USE_DSP
	return true
}

proc update_PARAM_VALUE.L2_ADC_BITS { PARAM_VALUE.L2_ADC_BITS } {
  # Procedure called to update ADC_BITS when any of the dependent parameters in the arguments change
  set_property range "2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24" ${PARAM_VALUE.L2_ADC_BITS}
}

proc validate_PARAM_VALUE.L2_ADC_BITS { PARAM_VALUE.L2_ADC_BITS } {
	# Procedure called to validate ADC_BITS
	return true
}

proc update_PARAM_VALUE.L2_TYPE { PARAM_VALUE.L2_TYPE } {
	# Procedure called to update TYPE when any of the dependent parameters in the arguments change
  set_property range "VOLTAGE,CURRENT" ${PARAM_VALUE.L2_TYPE}
}

proc validate_PARAM_VALUE.L2_TYPE { PARAM_VALUE.L2_TYPE } {
	# Procedure called to validate TYPE
	return true
}

proc update_PARAM_VALUE.L2_CODE { PARAM_VALUE.L2_CODE } {
	# Procedure called to update CODE when any of the dependent parameters in the arguments change
  set_property range "USB,BTC" ${PARAM_VALUE.L2_CODE}
}

proc validate_PARAM_VALUE.L2_CODE { PARAM_VALUE.L2_CODE } {
	# Procedure called to validate CODE
	return true
}

# Lane 4

proc update_PARAM_VALUE.L3_USE_DSP { PARAM_VALUE.L3_USE_DSP } {
	# Procedure called to update USE_DSP when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.L3_USE_DSP { PARAM_VALUE.L3_USE_DSP } {
	# Procedure called to validate USE_DSP
	return true
}

proc update_PARAM_VALUE.L3_ADC_BITS { PARAM_VALUE.L3_ADC_BITS } {
  # Procedure called to update ADC_BITS when any of the dependent parameters in the arguments change
  set_property range "2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24" ${PARAM_VALUE.L3_ADC_BITS}
}

proc validate_PARAM_VALUE.L3_ADC_BITS { PARAM_VALUE.L3_ADC_BITS } {
	# Procedure called to validate ADC_BITS
	return true
}

proc update_PARAM_VALUE.L3_TYPE { PARAM_VALUE.L3_TYPE } {
	# Procedure called to update TYPE when any of the dependent parameters in the arguments change
  set_property range "VOLTAGE,CURRENT" ${PARAM_VALUE.L3_TYPE}
}

proc validate_PARAM_VALUE.L3_TYPE { PARAM_VALUE.L3_TYPE } {
	# Procedure called to validate TYPE
	return true
}

proc update_PARAM_VALUE.L3_CODE { PARAM_VALUE.L3_CODE } {
	# Procedure called to update CODE when any of the dependent parameters in the arguments change
  set_property range "USB,BTC" ${PARAM_VALUE.L3_CODE}
}

proc validate_PARAM_VALUE.L3_CODE { PARAM_VALUE.L3_CODE } {
	# Procedure called to validate CODE
	return true
}

# Lane 5

proc update_PARAM_VALUE.L4_USE_DSP { PARAM_VALUE.L4_USE_DSP } {
	# Procedure called to update USE_DSP when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.L4_USE_DSP { PARAM_VALUE.L4_USE_DSP } {
	# Procedure called to validate USE_DSP
	return true
}

proc update_PARAM_VALUE.L4_ADC_BITS { PARAM_VALUE.L4_ADC_BITS } {
  # Procedure called to update ADC_BITS when any of the dependent parameters in the arguments change
  set_property range "2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24" ${PARAM_VALUE.L4_ADC_BITS}
}

proc validate_PARAM_VALUE.L4_ADC_BITS { PARAM_VALUE.L4_ADC_BITS } {
	# Procedure called to validate ADC_BITS
	return true
}

proc update_PARAM_VALUE.L4_TYPE { PARAM_VALUE.L4_TYPE } {
	# Procedure called to update TYPE when any of the dependent parameters in the arguments change
  set_property range "VOLTAGE,CURRENT" ${PARAM_VALUE.L4_TYPE}
}

proc validate_PARAM_VALUE.L4_TYPE { PARAM_VALUE.L4_TYPE } {
	# Procedure called to validate TYPE
	return true
}

proc update_PARAM_VALUE.L4_CODE { PARAM_VALUE.L4_CODE } {
	# Procedure called to update CODE when any of the dependent parameters in the arguments change
  set_property range "USB,BTC" ${PARAM_VALUE.L4_CODE}
}

proc validate_PARAM_VALUE.L4_CODE { PARAM_VALUE.L4_CODE } {
	# Procedure called to validate CODE
	return true
}

# Lane 6

proc update_PARAM_VALUE.L5_USE_DSP { PARAM_VALUE.L5_USE_DSP } {
	# Procedure called to update USE_DSP when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.L5_USE_DSP { PARAM_VALUE.L5_USE_DSP } {
	# Procedure called to validate USE_DSP
	return true
}

proc update_PARAM_VALUE.L5_ADC_BITS { PARAM_VALUE.L5_ADC_BITS } {
  # Procedure called to update ADC_BITS when any of the dependent parameters in the arguments change
  set_property range "2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24" ${PARAM_VALUE.L5_ADC_BITS}
}

proc validate_PARAM_VALUE.L5_ADC_BITS { PARAM_VALUE.L5_ADC_BITS } {
	# Procedure called to validate ADC_BITS
	return true
}

proc update_PARAM_VALUE.L5_TYPE { PARAM_VALUE.L5_TYPE } {
	# Procedure called to update TYPE when any of the dependent parameters in the arguments change
  set_property range "VOLTAGE,CURRENT" ${PARAM_VALUE.L5_TYPE}
}

proc validate_PARAM_VALUE.L5_TYPE { PARAM_VALUE.L5_TYPE } {
	# Procedure called to validate TYPE
	return true
}

proc update_PARAM_VALUE.L5_CODE { PARAM_VALUE.L5_CODE } {
	# Procedure called to update CODE when any of the dependent parameters in the arguments change
  set_property range "USB,BTC" ${PARAM_VALUE.L5_CODE}
}

proc validate_PARAM_VALUE.L5_CODE { PARAM_VALUE.L5_CODE } {
	# Procedure called to validate CODE
	return true
}

# Lane 7

proc update_PARAM_VALUE.L6_USE_DSP { PARAM_VALUE.L6_USE_DSP } {
	# Procedure called to update USE_DSP when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.L6_USE_DSP { PARAM_VALUE.L6_USE_DSP } {
	# Procedure called to validate USE_DSP
	return true
}

proc update_PARAM_VALUE.L6_ADC_BITS { PARAM_VALUE.L6_ADC_BITS } {
  # Procedure called to update ADC_BITS when any of the dependent parameters in the arguments change
  set_property range "2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24" ${PARAM_VALUE.L6_ADC_BITS}
}

proc validate_PARAM_VALUE.L6_ADC_BITS { PARAM_VALUE.L6_ADC_BITS } {
	# Procedure called to validate ADC_BITS
	return true
}

proc update_PARAM_VALUE.L6_TYPE { PARAM_VALUE.L6_TYPE } {
	# Procedure called to update TYPE when any of the dependent parameters in the arguments change
  set_property range "VOLTAGE,CURRENT" ${PARAM_VALUE.L6_TYPE}
}

proc validate_PARAM_VALUE.L6_TYPE { PARAM_VALUE.L6_TYPE } {
	# Procedure called to validate TYPE
	return true
}

proc update_PARAM_VALUE.L6_CODE { PARAM_VALUE.L6_CODE } {
	# Procedure called to update CODE when any of the dependent parameters in the arguments change
  set_property range "USB,BTC" ${PARAM_VALUE.L6_CODE}
}

proc validate_PARAM_VALUE.L6_CODE { PARAM_VALUE.L6_CODE } {
	# Procedure called to validate CODE
	return true
}

# Lane 8

proc update_PARAM_VALUE.L7_USE_DSP { PARAM_VALUE.L7_USE_DSP } {
	# Procedure called to update USE_DSP when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.L7_USE_DSP { PARAM_VALUE.L7_USE_DSP } {
	# Procedure called to validate USE_DSP
	return true
}

proc update_PARAM_VALUE.L7_ADC_BITS { PARAM_VALUE.L7_ADC_BITS } {
  # Procedure called to update ADC_BITS when any of the dependent parameters in the arguments change
  set_property range "2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24" ${PARAM_VALUE.L7_ADC_BITS}
}

proc validate_PARAM_VALUE.L7_ADC_BITS { PARAM_VALUE.L7_ADC_BITS } {
	# Procedure called to validate ADC_BITS
	return true
}

proc update_PARAM_VALUE.L7_TYPE { PARAM_VALUE.L7_TYPE } {
	# Procedure called to update TYPE when any of the dependent parameters in the arguments change
  set_property range "VOLTAGE,CURRENT" ${PARAM_VALUE.L7_TYPE}
}

proc validate_PARAM_VALUE.L7_TYPE { PARAM_VALUE.L7_TYPE } {
	# Procedure called to validate TYPE
	return true
}

proc update_PARAM_VALUE.L7_CODE { PARAM_VALUE.L7_CODE } {
	# Procedure called to update CODE when any of the dependent parameters in the arguments change
  set_property range "USB,BTC" ${PARAM_VALUE.L7_CODE}
}

proc validate_PARAM_VALUE.L7_CODE { PARAM_VALUE.L7_CODE } {
	# Procedure called to validate CODE
	return true
}

# Lane 9

proc update_PARAM_VALUE.L8_USE_DSP { PARAM_VALUE.L8_USE_DSP } {
	# Procedure called to update USE_DSP when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.L8_USE_DSP { PARAM_VALUE.L8_USE_DSP } {
	# Procedure called to validate USE_DSP
	return true
}

proc update_PARAM_VALUE.L8_ADC_BITS { PARAM_VALUE.L8_ADC_BITS } {
  # Procedure called to update ADC_BITS when any of the dependent parameters in the arguments change
  set_property range "2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24" ${PARAM_VALUE.L8_ADC_BITS}
}

proc validate_PARAM_VALUE.L8_ADC_BITS { PARAM_VALUE.L8_ADC_BITS } {
	# Procedure called to validate ADC_BITS
	return true
}

proc update_PARAM_VALUE.L8_TYPE { PARAM_VALUE.L8_TYPE } {
	# Procedure called to update TYPE when any of the dependent parameters in the arguments change
  set_property range "VOLTAGE,CURRENT" ${PARAM_VALUE.L8_TYPE}
}

proc validate_PARAM_VALUE.L8_TYPE { PARAM_VALUE.L8_TYPE } {
	# Procedure called to validate TYPE
	return true
}

proc update_PARAM_VALUE.L8_CODE { PARAM_VALUE.L8_CODE } {
	# Procedure called to update CODE when any of the dependent parameters in the arguments change
  set_property range "USB,BTC" ${PARAM_VALUE.L8_CODE}
}

proc validate_PARAM_VALUE.L8_CODE { PARAM_VALUE.L8_CODE } {
	# Procedure called to validate CODE
	return true
}

# Lane 10

proc update_PARAM_VALUE.L9_USE_DSP { PARAM_VALUE.L9_USE_DSP } {
	# Procedure called to update USE_DSP when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.L9_USE_DSP { PARAM_VALUE.L9_USE_DSP } {
	# Procedure called to validate USE_DSP
	return true
}

proc update_PARAM_VALUE.L9_ADC_BITS { PARAM_VALUE.L9_ADC_BITS } {
  # Procedure called to update ADC_BITS when any of the dependent parameters in the arguments change
  set_property range "2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24" ${PARAM_VALUE.L9_ADC_BITS}
}

proc validate_PARAM_VALUE.L9_ADC_BITS { PARAM_VALUE.L9_ADC_BITS } {
	# Procedure called to validate ADC_BITS
	return true
}

proc update_PARAM_VALUE.L9_TYPE { PARAM_VALUE.L9_TYPE } {
	# Procedure called to update TYPE when any of the dependent parameters in the arguments change
  set_property range "VOLTAGE,CURRENT" ${PARAM_VALUE.L9_TYPE}
}

proc validate_PARAM_VALUE.L9_TYPE { PARAM_VALUE.L9_TYPE } {
	# Procedure called to validate TYPE
	return true
}

proc update_PARAM_VALUE.L9_CODE { PARAM_VALUE.L9_CODE } {
	# Procedure called to update CODE when any of the dependent parameters in the arguments change
  set_property range "USB,BTC" ${PARAM_VALUE.L9_CODE}
}

proc validate_PARAM_VALUE.L9_CODE { PARAM_VALUE.L9_CODE } {
	# Procedure called to validate CODE
	return true
}

# Lane 11

proc update_PARAM_VALUE.L10_USE_DSP { PARAM_VALUE.L10_USE_DSP } {
	# Procedure called to update USE_DSP when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.L10_USE_DSP { PARAM_VALUE.L10_USE_DSP } {
	# Procedure called to validate USE_DSP
	return true
}

proc update_PARAM_VALUE.L10_ADC_BITS { PARAM_VALUE.L10_ADC_BITS } {
  # Procedure called to update ADC_BITS when any of the dependent parameters in the arguments change
  set_property range "2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24" ${PARAM_VALUE.L10_ADC_BITS}
}

proc validate_PARAM_VALUE.L10_ADC_BITS { PARAM_VALUE.L10_ADC_BITS } {
	# Procedure called to validate ADC_BITS
	return true
}

proc update_PARAM_VALUE.L10_TYPE { PARAM_VALUE.L10_TYPE } {
	# Procedure called to update TYPE when any of the dependent parameters in the arguments change
  set_property range "VOLTAGE,CURRENT" ${PARAM_VALUE.L10_TYPE}
}

proc validate_PARAM_VALUE.L10_TYPE { PARAM_VALUE.L10_TYPE } {
	# Procedure called to validate TYPE
	return true
}

proc update_PARAM_VALUE.L10_CODE { PARAM_VALUE.L10_CODE } {
	# Procedure called to update CODE when any of the dependent parameters in the arguments change
  set_property range "USB,BTC" ${PARAM_VALUE.L10_CODE}
}

proc validate_PARAM_VALUE.L10_CODE { PARAM_VALUE.L10_CODE } {
	# Procedure called to validate CODE
	return true
}

# Lane 12

proc update_PARAM_VALUE.L11_USE_DSP { PARAM_VALUE.L11_USE_DSP } {
	# Procedure called to update USE_DSP when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.L11_USE_DSP { PARAM_VALUE.L11_USE_DSP } {
	# Procedure called to validate USE_DSP
	return true
}

proc update_PARAM_VALUE.L11_ADC_BITS { PARAM_VALUE.L11_ADC_BITS } {
  # Procedure called to update ADC_BITS when any of the dependent parameters in the arguments change
  set_property range "2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24" ${PARAM_VALUE.L11_ADC_BITS}
}

proc validate_PARAM_VALUE.L11_ADC_BITS { PARAM_VALUE.L11_ADC_BITS } {
	# Procedure called to validate ADC_BITS
	return true
}

proc update_PARAM_VALUE.L11_TYPE { PARAM_VALUE.L11_TYPE } {
	# Procedure called to update TYPE when any of the dependent parameters in the arguments change
  set_property range "VOLTAGE,CURRENT" ${PARAM_VALUE.L11_TYPE}
}

proc validate_PARAM_VALUE.L11_TYPE { PARAM_VALUE.L11_TYPE } {
	# Procedure called to validate TYPE
	return true
}

proc update_PARAM_VALUE.L11_CODE { PARAM_VALUE.L11_CODE } {
	# Procedure called to update CODE when any of the dependent parameters in the arguments change
  set_property range "USB,BTC" ${PARAM_VALUE.L11_CODE}
}

proc validate_PARAM_VALUE.L11_CODE { PARAM_VALUE.L11_CODE } {
	# Procedure called to validate CODE
	return true
}

# Lane 13

proc update_PARAM_VALUE.L12_USE_DSP { PARAM_VALUE.L12_USE_DSP } {
	# Procedure called to update USE_DSP when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.L12_USE_DSP { PARAM_VALUE.L12_USE_DSP } {
	# Procedure called to validate USE_DSP
	return true
}

proc update_PARAM_VALUE.L12_ADC_BITS { PARAM_VALUE.L12_ADC_BITS } {
  # Procedure called to update ADC_BITS when any of the dependent parameters in the arguments change
  set_property range "2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24" ${PARAM_VALUE.L12_ADC_BITS}
}

proc validate_PARAM_VALUE.L12_ADC_BITS { PARAM_VALUE.L12_ADC_BITS } {
	# Procedure called to validate ADC_BITS
	return true
}

proc update_PARAM_VALUE.L12_TYPE { PARAM_VALUE.L12_TYPE } {
	# Procedure called to update TYPE when any of the dependent parameters in the arguments change
  set_property range "VOLTAGE,CURRENT" ${PARAM_VALUE.L12_TYPE}
}

proc validate_PARAM_VALUE.L12_TYPE { PARAM_VALUE.L12_TYPE } {
	# Procedure called to validate TYPE
	return true
}

proc update_PARAM_VALUE.L12_CODE { PARAM_VALUE.L12_CODE } {
	# Procedure called to update CODE when any of the dependent parameters in the arguments change
  set_property range "USB,BTC" ${PARAM_VALUE.L12_CODE}
}

proc validate_PARAM_VALUE.L12_CODE { PARAM_VALUE.L12_CODE } {
	# Procedure called to validate CODE
	return true
}

# Lane 14

proc update_PARAM_VALUE.L13_USE_DSP { PARAM_VALUE.L13_USE_DSP } {
	# Procedure called to update USE_DSP when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.L13_USE_DSP { PARAM_VALUE.L13_USE_DSP } {
	# Procedure called to validate USE_DSP
	return true
}

proc update_PARAM_VALUE.L13_ADC_BITS { PARAM_VALUE.L13_ADC_BITS } {
  # Procedure called to update ADC_BITS when any of the dependent parameters in the arguments change
  set_property range "2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24" ${PARAM_VALUE.L13_ADC_BITS}
}

proc validate_PARAM_VALUE.L13_ADC_BITS { PARAM_VALUE.L13_ADC_BITS } {
	# Procedure called to validate ADC_BITS
	return true
}

proc update_PARAM_VALUE.L13_TYPE { PARAM_VALUE.L13_TYPE } {
	# Procedure called to update TYPE when any of the dependent parameters in the arguments change
  set_property range "VOLTAGE,CURRENT" ${PARAM_VALUE.L13_TYPE}
}

proc validate_PARAM_VALUE.L13_TYPE { PARAM_VALUE.L13_TYPE } {
	# Procedure called to validate TYPE
	return true
}

proc update_PARAM_VALUE.L13_CODE { PARAM_VALUE.L13_CODE } {
	# Procedure called to update CODE when any of the dependent parameters in the arguments change
  set_property range "USB,BTC" ${PARAM_VALUE.L13_CODE}
}

proc validate_PARAM_VALUE.L13_CODE { PARAM_VALUE.L13_CODE } {
	# Procedure called to validate CODE
	return true
}

# Lane 15

proc update_PARAM_VALUE.L14_USE_DSP { PARAM_VALUE.L14_USE_DSP } {
	# Procedure called to update USE_DSP when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.L14_USE_DSP { PARAM_VALUE.L14_USE_DSP } {
	# Procedure called to validate USE_DSP
	return true
}

proc update_PARAM_VALUE.L14_ADC_BITS { PARAM_VALUE.L14_ADC_BITS } {
  # Procedure called to update ADC_BITS when any of the dependent parameters in the arguments change
  set_property range "2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24" ${PARAM_VALUE.L14_ADC_BITS}
}

proc validate_PARAM_VALUE.L14_ADC_BITS { PARAM_VALUE.L14_ADC_BITS } {
	# Procedure called to validate ADC_BITS
	return true
}

proc update_PARAM_VALUE.L14_TYPE { PARAM_VALUE.L14_TYPE } {
	# Procedure called to update TYPE when any of the dependent parameters in the arguments change
  set_property range "VOLTAGE,CURRENT" ${PARAM_VALUE.L14_TYPE}
}

proc validate_PARAM_VALUE.L14_TYPE { PARAM_VALUE.L14_TYPE } {
	# Procedure called to validate TYPE
	return true
}

proc update_PARAM_VALUE.L14_CODE { PARAM_VALUE.L14_CODE } {
	# Procedure called to update CODE when any of the dependent parameters in the arguments change
  set_property range "USB,BTC" ${PARAM_VALUE.L14_CODE}
}

proc validate_PARAM_VALUE.L14_CODE { PARAM_VALUE.L14_CODE } {
	# Procedure called to validate CODE
	return true
}

# Lane 16

proc update_PARAM_VALUE.L15_USE_DSP { PARAM_VALUE.L15_USE_DSP } {
	# Procedure called to update USE_DSP when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.L15_USE_DSP { PARAM_VALUE.L15_USE_DSP } {
	# Procedure called to validate USE_DSP
	return true
}

proc update_PARAM_VALUE.L15_ADC_BITS { PARAM_VALUE.L15_ADC_BITS } {
  # Procedure called to update ADC_BITS when any of the dependent parameters in the arguments change
  set_property range "2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24" ${PARAM_VALUE.L15_ADC_BITS}
}

proc validate_PARAM_VALUE.L15_ADC_BITS { PARAM_VALUE.L15_ADC_BITS } {
	# Procedure called to validate ADC_BITS
	return true
}

proc update_PARAM_VALUE.L15_TYPE { PARAM_VALUE.L15_TYPE } {
	# Procedure called to update TYPE when any of the dependent parameters in the arguments change
  set_property range "VOLTAGE,CURRENT" ${PARAM_VALUE.L15_TYPE}
}

proc validate_PARAM_VALUE.L15_TYPE { PARAM_VALUE.L15_TYPE } {
	# Procedure called to validate TYPE
	return true
}

proc update_PARAM_VALUE.L15_CODE { PARAM_VALUE.L15_CODE } {
	# Procedure called to update CODE when any of the dependent parameters in the arguments change
  set_property range "USB,BTC" ${PARAM_VALUE.L15_CODE}
}

proc validate_PARAM_VALUE.L15_CODE { PARAM_VALUE.L15_CODE } {
	# Procedure called to validate CODE
	return true
}

######### update MODELPARAM VALUES ######################
proc update_MODELPARAM_VALUE.C_S_AXI_ADDR_WIDTH { MODELPARAM_VALUE.C_S_AXI_ADDR_WIDTH PARAM_VALUE.C_S_AXI_ADDR_WIDTH } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.C_S_AXI_ADDR_WIDTH}] ${MODELPARAM_VALUE.C_S_AXI_ADDR_WIDTH}
}

proc update_MODELPARAM_VALUE.C_S_AXI_DATA_WIDTH { MODELPARAM_VALUE.C_S_AXI_DATA_WIDTH PARAM_VALUE.C_S_AXI_DATA_WIDTH } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.C_S_AXI_DATA_WIDTH}] ${MODELPARAM_VALUE.C_S_AXI_DATA_WIDTH}
}

proc update_MODELPARAM_VALUE.NUM_CHANNELS { MODELPARAM_VALUE.NUM_CHANNELS PARAM_VALUE.NUM_CHANNELS } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.NUM_CHANNELS}] ${MODELPARAM_VALUE.NUM_CHANNELS}
}

# Lane 0

proc update_MODELPARAM_VALUE.L0_USE_DSP { MODELPARAM_VALUE.L0_USE_DSP PARAM_VALUE.L0_USE_DSP } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  #set USE_DSP {${PARAM_VALUE.L0_USE_DSP} ? 1 : 0}
  set_property value [get_property value ${PARAM_VALUE.L0_USE_DSP}] ${MODELPARAM_VALUE.L0_USE_DSP}
  #set_property value $USE_DSP ${MODELPARAM_VALUE.L0_USE_DSP}
}

proc update_MODELPARAM_VALUE.L0_ADC_BITS { MODELPARAM_VALUE.L0_ADC_BITS PARAM_VALUE.L0_ADC_BITS } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  #set USE_DSP {${PARAM_VALUE.L1_USE_DSP} ? 1 : 0}
  set_property value [get_property value ${PARAM_VALUE.L0_ADC_BITS}] ${MODELPARAM_VALUE.L0_ADC_BITS}
  #set_property value $USE_DSP ${MODELPARAM_VALUE.L1_USE_DSP}
}

proc update_MODELPARAM_VALUE.L0_TYPE { MODELPARAM_VALUE.L0_TYPE PARAM_VALUE.L0_TYPE } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L0_TYPE}] ${MODELPARAM_VALUE.L0_TYPE}
}

proc update_MODELPARAM_VALUE.L0_CODE { MODELPARAM_VALUE.L0_CODE PARAM_VALUE.L0_CODE } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L0_CODE}] ${MODELPARAM_VALUE.L0_CODE}
}

# Lane 1

proc update_MODELPARAM_VALUE.L1_USE_DSP { MODELPARAM_VALUE.L1_USE_DSP PARAM_VALUE.L1_USE_DSP } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L1_USE_DSP}] ${MODELPARAM_VALUE.L1_USE_DSP}
}

proc update_MODELPARAM_VALUE.L1_ADC_BITS { MODELPARAM_VALUE.L1_ADC_BITS PARAM_VALUE.L1_ADC_BITS } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L1_ADC_BITS}] ${MODELPARAM_VALUE.L1_ADC_BITS}
}

proc update_MODELPARAM_VALUE.L1_TYPE { MODELPARAM_VALUE.L1_TYPE PARAM_VALUE.L1_TYPE } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L1_TYPE}] ${MODELPARAM_VALUE.L1_TYPE}
}

proc update_MODELPARAM_VALUE.L1_CODE { MODELPARAM_VALUE.L1_CODE PARAM_VALUE.L1_CODE } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L1_CODE}] ${MODELPARAM_VALUE.L1_CODE}
}

# Lane 2

proc update_MODELPARAM_VALUE.L2_USE_DSP { MODELPARAM_VALUE.L2_USE_DSP PARAM_VALUE.L2_USE_DSP } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L2_USE_DSP}] ${MODELPARAM_VALUE.L2_USE_DSP}
}

proc update_MODELPARAM_VALUE.L2_ADC_BITS { MODELPARAM_VALUE.L2_ADC_BITS PARAM_VALUE.L2_ADC_BITS } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L2_ADC_BITS}] ${MODELPARAM_VALUE.L2_ADC_BITS}
}

proc update_MODELPARAM_VALUE.L2_TYPE { MODELPARAM_VALUE.L2_TYPE PARAM_VALUE.L2_TYPE } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L2_TYPE}] ${MODELPARAM_VALUE.L2_TYPE}
}

proc update_MODELPARAM_VALUE.L2_CODE { MODELPARAM_VALUE.L2_CODE PARAM_VALUE.L2_CODE } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L2_CODE}] ${MODELPARAM_VALUE.L2_CODE}
}


# Lane 3

proc update_MODELPARAM_VALUE.L3_USE_DSP { MODELPARAM_VALUE.L3_USE_DSP PARAM_VALUE.L3_USE_DSP } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L3_USE_DSP}] ${MODELPARAM_VALUE.L3_USE_DSP}
}

proc update_MODELPARAM_VALUE.L3_ADC_BITS { MODELPARAM_VALUE.L3_ADC_BITS PARAM_VALUE.L3_ADC_BITS } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L3_ADC_BITS}] ${MODELPARAM_VALUE.L3_ADC_BITS}
}

proc update_MODELPARAM_VALUE.L3_TYPE { MODELPARAM_VALUE.L3_TYPE PARAM_VALUE.L3_TYPE } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L3_TYPE}] ${MODELPARAM_VALUE.L3_TYPE}
}

proc update_MODELPARAM_VALUE.L3_CODE { MODELPARAM_VALUE.L3_CODE PARAM_VALUE.L3_CODE } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L3_CODE}] ${MODELPARAM_VALUE.L3_CODE}
}

# Lane 4

proc update_MODELPARAM_VALUE.L4_USE_DSP { MODELPARAM_VALUE.L4_USE_DSP PARAM_VALUE.L4_USE_DSP } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L4_USE_DSP}] ${MODELPARAM_VALUE.L4_USE_DSP}
}

proc update_MODELPARAM_VALUE.L4_ADC_BITS { MODELPARAM_VALUE.L4_ADC_BITS PARAM_VALUE.L4_ADC_BITS } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L4_ADC_BITS}] ${MODELPARAM_VALUE.L4_ADC_BITS}
}

proc update_MODELPARAM_VALUE.L4_TYPE { MODELPARAM_VALUE.L4_TYPE PARAM_VALUE.L4_TYPE } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L4_TYPE}] ${MODELPARAM_VALUE.L4_TYPE}
}

proc update_MODELPARAM_VALUE.L4_CODE { MODELPARAM_VALUE.L4_CODE PARAM_VALUE.L4_CODE } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L4_CODE}] ${MODELPARAM_VALUE.L4_CODE}
}

# Lane 5

proc update_MODELPARAM_VALUE.L5_USE_DSP { MODELPARAM_VALUE.L5_USE_DSP PARAM_VALUE.L5_USE_DSP } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L5_USE_DSP}] ${MODELPARAM_VALUE.L5_USE_DSP}
}

proc update_MODELPARAM_VALUE.L5_ADC_BITS { MODELPARAM_VALUE.L5_ADC_BITS PARAM_VALUE.L5_ADC_BITS } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L5_ADC_BITS}] ${MODELPARAM_VALUE.L5_ADC_BITS}
}

proc update_MODELPARAM_VALUE.L5_TYPE { MODELPARAM_VALUE.L5_TYPE PARAM_VALUE.L5_TYPE } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L5_TYPE}] ${MODELPARAM_VALUE.L5_TYPE}
}

proc update_MODELPARAM_VALUE.L5_CODE { MODELPARAM_VALUE.L5_CODE PARAM_VALUE.L5_CODE } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L5_CODE}] ${MODELPARAM_VALUE.L5_CODE}
}

# Lane 6

proc update_MODELPARAM_VALUE.L6_USE_DSP { MODELPARAM_VALUE.L6_USE_DSP PARAM_VALUE.L6_USE_DSP } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L6_USE_DSP}] ${MODELPARAM_VALUE.L6_USE_DSP}
}

proc update_MODELPARAM_VALUE.L6_ADC_BITS { MODELPARAM_VALUE.L6_ADC_BITS PARAM_VALUE.L6_ADC_BITS } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L6_ADC_BITS}] ${MODELPARAM_VALUE.L6_ADC_BITS}
}

proc update_MODELPARAM_VALUE.L6_TYPE { MODELPARAM_VALUE.L6_TYPE PARAM_VALUE.L6_TYPE } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L6_TYPE}] ${MODELPARAM_VALUE.L6_TYPE}
}

proc update_MODELPARAM_VALUE.L6_CODE { MODELPARAM_VALUE.L6_CODE PARAM_VALUE.L6_CODE } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L6_CODE}] ${MODELPARAM_VALUE.L6_CODE}
}

# Lane 7

proc update_MODELPARAM_VALUE.L7_USE_DSP { MODELPARAM_VALUE.L7_USE_DSP PARAM_VALUE.L7_USE_DSP } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L7_USE_DSP}] ${MODELPARAM_VALUE.L7_USE_DSP}
}

proc update_MODELPARAM_VALUE.L7_ADC_BITS { MODELPARAM_VALUE.L7_ADC_BITS PARAM_VALUE.L7_ADC_BITS } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L7_ADC_BITS}] ${MODELPARAM_VALUE.L7_ADC_BITS}
}

proc update_MODELPARAM_VALUE.L7_TYPE { MODELPARAM_VALUE.L7_TYPE PARAM_VALUE.L7_TYPE } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L7_TYPE}] ${MODELPARAM_VALUE.L7_TYPE}
}

proc update_MODELPARAM_VALUE.L7_CODE { MODELPARAM_VALUE.L7_CODE PARAM_VALUE.L7_CODE } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L7_CODE}] ${MODELPARAM_VALUE.L7_CODE}
}

# Lane 8

proc update_MODELPARAM_VALUE.L8_USE_DSP { MODELPARAM_VALUE.L8_USE_DSP PARAM_VALUE.L8_USE_DSP } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L8_USE_DSP}] ${MODELPARAM_VALUE.L8_USE_DSP}
}

proc update_MODELPARAM_VALUE.L8_ADC_BITS { MODELPARAM_VALUE.L8_ADC_BITS PARAM_VALUE.L8_ADC_BITS } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L8_ADC_BITS}] ${MODELPARAM_VALUE.L8_ADC_BITS}
}

proc update_MODELPARAM_VALUE.L8_TYPE { MODELPARAM_VALUE.L8_TYPE PARAM_VALUE.L8_TYPE } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L8_TYPE}] ${MODELPARAM_VALUE.L8_TYPE}
}

proc update_MODELPARAM_VALUE.L8_CODE { MODELPARAM_VALUE.L8_CODE PARAM_VALUE.L8_CODE } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L8_CODE}] ${MODELPARAM_VALUE.L8_CODE}
}

# Lane 9

proc update_MODELPARAM_VALUE.L9_USE_DSP { MODELPARAM_VALUE.L9_USE_DSP PARAM_VALUE.L9_USE_DSP } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L9_USE_DSP}] ${MODELPARAM_VALUE.L9_USE_DSP}
}

proc update_MODELPARAM_VALUE.L9_ADC_BITS { MODELPARAM_VALUE.L9_ADC_BITS PARAM_VALUE.L9_ADC_BITS } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L9_ADC_BITS}] ${MODELPARAM_VALUE.L9_ADC_BITS}
}

proc update_MODELPARAM_VALUE.L9_TYPE { MODELPARAM_VALUE.L9_TYPE PARAM_VALUE.L9_TYPE } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L9_TYPE}] ${MODELPARAM_VALUE.L9_TYPE}
}

proc update_MODELPARAM_VALUE.L9_CODE { MODELPARAM_VALUE.L9_CODE PARAM_VALUE.L9_CODE } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L9_CODE}] ${MODELPARAM_VALUE.L9_CODE}
}

# Lane 10

proc update_MODELPARAM_VALUE.L10_USE_DSP { MODELPARAM_VALUE.L10_USE_DSP PARAM_VALUE.L10_USE_DSP } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L10_USE_DSP}] ${MODELPARAM_VALUE.L10_USE_DSP}
}

proc update_MODELPARAM_VALUE.L10_ADC_BITS { MODELPARAM_VALUE.L10_ADC_BITS PARAM_VALUE.L10_ADC_BITS } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L10_ADC_BITS}] ${MODELPARAM_VALUE.L10_ADC_BITS}
}

proc update_MODELPARAM_VALUE.L10_TYPE { MODELPARAM_VALUE.L10_TYPE PARAM_VALUE.L10_TYPE } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L10_TYPE}] ${MODELPARAM_VALUE.L10_TYPE}
}

proc update_MODELPARAM_VALUE.L10_CODE { MODELPARAM_VALUE.L10_CODE PARAM_VALUE.L10_CODE } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L10_CODE}] ${MODELPARAM_VALUE.L10_CODE}
}

# Lane 11

proc update_MODELPARAM_VALUE.L11_USE_DSP { MODELPARAM_VALUE.L11_USE_DSP PARAM_VALUE.L11_USE_DSP } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L11_USE_DSP}] ${MODELPARAM_VALUE.L11_USE_DSP}
}

proc update_MODELPARAM_VALUE.L11_ADC_BITS { MODELPARAM_VALUE.L11_ADC_BITS PARAM_VALUE.L11_ADC_BITS } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L11_ADC_BITS}] ${MODELPARAM_VALUE.L11_ADC_BITS}
}

proc update_MODELPARAM_VALUE.L11_TYPE { MODELPARAM_VALUE.L11_TYPE PARAM_VALUE.L11_TYPE } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L11_TYPE}] ${MODELPARAM_VALUE.L11_TYPE}
}

proc update_MODELPARAM_VALUE.L11_CODE { MODELPARAM_VALUE.L11_CODE PARAM_VALUE.L11_CODE } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L11_CODE}] ${MODELPARAM_VALUE.L11_CODE}
}

# Lane 12

proc update_MODELPARAM_VALUE.L12_USE_DSP { MODELPARAM_VALUE.L12_USE_DSP PARAM_VALUE.L12_USE_DSP } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L12_USE_DSP}] ${MODELPARAM_VALUE.L12_USE_DSP}
}

proc update_MODELPARAM_VALUE.L12_ADC_BITS { MODELPARAM_VALUE.L12_ADC_BITS PARAM_VALUE.L12_ADC_BITS } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L12_ADC_BITS}] ${MODELPARAM_VALUE.L12_ADC_BITS}
}

proc update_MODELPARAM_VALUE.L12_TYPE { MODELPARAM_VALUE.L12_TYPE PARAM_VALUE.L12_TYPE } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L12_TYPE}] ${MODELPARAM_VALUE.L12_TYPE}
}

proc update_MODELPARAM_VALUE.L12_CODE { MODELPARAM_VALUE.L12_CODE PARAM_VALUE.L12_CODE } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L12_CODE}] ${MODELPARAM_VALUE.L12_CODE}
}

# Lane 13

proc update_MODELPARAM_VALUE.L13_USE_DSP { MODELPARAM_VALUE.L13_USE_DSP PARAM_VALUE.L13_USE_DSP } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L13_USE_DSP}] ${MODELPARAM_VALUE.L13_USE_DSP}
}

proc update_MODELPARAM_VALUE.L13_ADC_BITS { MODELPARAM_VALUE.L13_ADC_BITS PARAM_VALUE.L13_ADC_BITS } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L13_ADC_BITS}] ${MODELPARAM_VALUE.L13_ADC_BITS}
}

proc update_MODELPARAM_VALUE.L13_TYPE { MODELPARAM_VALUE.L13_TYPE PARAM_VALUE.L13_TYPE } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L13_TYPE}] ${MODELPARAM_VALUE.L13_TYPE}
}

proc update_MODELPARAM_VALUE.L13_CODE { MODELPARAM_VALUE.L13_CODE PARAM_VALUE.L13_CODE } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L13_CODE}] ${MODELPARAM_VALUE.L13_CODE}
}

# Lane 14

proc update_MODELPARAM_VALUE.L14_USE_DSP { MODELPARAM_VALUE.L14_USE_DSP PARAM_VALUE.L14_USE_DSP } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L14_USE_DSP}] ${MODELPARAM_VALUE.L14_USE_DSP}
}

proc update_MODELPARAM_VALUE.L14_ADC_BITS { MODELPARAM_VALUE.L14_ADC_BITS PARAM_VALUE.L14_ADC_BITS } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L14_ADC_BITS}] ${MODELPARAM_VALUE.L14_ADC_BITS}
}

proc update_MODELPARAM_VALUE.L14_TYPE { MODELPARAM_VALUE.L14_TYPE PARAM_VALUE.L14_TYPE } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L14_TYPE}] ${MODELPARAM_VALUE.L14_TYPE}
}

proc update_MODELPARAM_VALUE.L14_CODE { MODELPARAM_VALUE.L14_CODE PARAM_VALUE.L14_CODE } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L14_CODE}] ${MODELPARAM_VALUE.L14_CODE}
}

# Lane 15

proc update_MODELPARAM_VALUE.L15_USE_DSP { MODELPARAM_VALUE.L15_USE_DSP PARAM_VALUE.L15_USE_DSP } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L15_USE_DSP}] ${MODELPARAM_VALUE.L15_USE_DSP}
}

proc update_MODELPARAM_VALUE.L15_ADC_BITS { MODELPARAM_VALUE.L15_ADC_BITS PARAM_VALUE.L15_ADC_BITS } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE. L15_ADC_BITS}] ${MODELPARAM_VALUE.L15_ADC_BITS}
}

proc update_MODELPARAM_VALUE.L15_TYPE { MODELPARAM_VALUE.L15_TYPE PARAM_VALUE.L15_TYPE } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L15_TYPE}] ${MODELPARAM_VALUE.L15_TYPE}
}

proc update_MODELPARAM_VALUE.L15_CODE { MODELPARAM_VALUE.L15_CODE PARAM_VALUE.L15_CODE } {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.L15_CODE}] ${MODELPARAM_VALUE.L15_CODE}
}

proc update_MODELPARAM_VALUE.QSCALE_MAX_BITS { MODELPARAM_VALUE.QSCALE_MAX_BITS PARAM_VALUE.QSCALE_MAX_BITS} {
  # Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
  set_property value [get_property value ${PARAM_VALUE.QSCALE_MAX_BITS}] ${MODELPARAM_VALUE.QSCALE_MAX_BITS}
}

#############################################3
proc update_gui_for_PARAM_VALUE.NUM_CHANNELS {PARAM_VALUE.NUM_CHANNELS IPINST} {

  set num_channels [get_property value ${PARAM_VALUE.NUM_CHANNELS}]

  set list {1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16}
  set table [ipgui::get_tablespec -of $IPINST "Lane Table"]
  set index [tcl::lsearch $list $num_channels]
  set list [tcl::lreplace $list 0 $index]
  set list [regsub -all " " $list ","]
  set_property hidden_rows "$list" $table

}
