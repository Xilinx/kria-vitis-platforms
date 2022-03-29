# Definitional proc to organize widgets for parameters.
proc init_gui { IPINST   } {
  #source_ipfile "bd/bd.tcl"

  ipgui::add_param $IPINST -name "Component_Name"
  #set BRAM_COUNT [xit::get_device_data D_BRAM_COUNT -of [xit::current_scope]]
  #send_msg INFO 301 "BRAM_COUNT $BRAM_COUNT "
  #set URAM_COUNT [xit::get_device_data D_URAM_COUNT -of [xit::current_scope]]
  #send_msg INFO 301 "URAM_COUNT $URAM_COUNT "
  #Adding Page
  set Page_0 [ipgui::add_page $IPINST -name "Basic_Configuration"]
  ipgui::add_static_text $IPINST -name "TITLE" -text  "NOTE: VCU Subsystem is runtime software controlled, configuration options set in this IP wizard are\n used to accurately estimate power consumption and calculate depth of optional Encoder Buffer. " -display_border false -parent $Page_0
  set Page_1 [ipgui::add_page $IPINST -name "Advanced Configuration"]
  set gui_text_basic [ipgui::add_group $IPINST -parent $Page_0 -name "Resource Summary" -layout horizontal]  
  set gui_text_advanced [ipgui::add_group $IPINST -parent $Page_1 -name "Resource Summary" -layout horizontal]  
  #ipgui::add_row $IPINST -parent ${gui_text_basic}
  #ipgui::add_dynamic_text $IPINST -name "ENCODER_BW_ESTIMATION" -tclproc encoder_bw_calc -display_border false -parent ${gui_text_basic}
  ipgui::add_row $IPINST -parent ${gui_text_basic}
  ipgui::add_dynamic_text $IPINST -name "ENCODER_BUFFER_ESTIMATION" -tclproc encoder_depth_calc -display_border false -parent ${gui_text_basic}
  ipgui::add_row $IPINST -parent ${gui_text_basic}
  #ipgui::add_dynamic_text $IPINST -name "DECODER_BW_ESTIMATION" -tclproc decoder_bw_calc -display_border false -parent ${gui_text_basic}
  #ipgui::add_row $IPINST -parent ${gui_text_basic}
  ipgui::add_row $IPINST -parent ${gui_text_advanced}
  ipgui::add_dynamic_text $IPINST -name "ENCODER_BUFFER_ESTIMATION2" -tclproc encoder_depth_calc -display_border false -parent ${gui_text_advanced}
  ipgui::add_row $IPINST -parent ${gui_text_advanced}


  ipgui::add_param $IPINST -name "TABLE_NO" -parent ${Page_1}
  set_property visible false [ipgui::get_guiparamspec TABLE_NO -of $IPINST]

  set encoder_config [ipgui::add_group $IPINST -parent $Page_0 -name "Encoder_Configuration"]  
  ipgui::add_param $IPINST -name "ENABLE_ENCODER" -parent $encoder_config -widget checkBox -layout horizontal
  ipgui::add_param $IPINST -name "NO_OF_STREAMS" -parent ${Page_0} 
#  ipgui::add_param $IPINST -name "NO_OF_STREAMS" -parent $encoder_config -widget comboBox 
#  set_property tooltip "Maximum Number of Encoder Streams that needs to be supported." [ipgui::get_guiparamspec  NO_OF_STREAMS -of $IPINST]
#  ipgui::add_row $IPINST -parent $encoder_config
     set encoder_param  [ipgui::add_group $IPINST -parent $encoder_config -name "Stream Details - Most Demanding Stream" -layout horizontal]  
     ipgui::add_param $IPINST -name "ENC_VIDEO_STANDARD" -parent ${encoder_param} -widget comboBox
     ipgui::add_param $IPINST -name "ENC_CODING_TYPE" -parent ${encoder_param} -widget comboBox
     ipgui::add_row $IPINST -parent ${encoder_param}
     ipgui::add_param $IPINST -name "ENC_FRAME_SIZE" -parent ${encoder_param} -widget comboBox
     ipgui::add_param $IPINST -name "ENC_FPS" -parent ${encoder_param} -widget comboBox
     ipgui::add_row $IPINST -parent ${encoder_param}
     ipgui::add_param $IPINST -name "ENC_COLOR_FORMAT" -parent ${encoder_param} -widget comboBox
     ipgui::add_param $IPINST -name "ENC_COLOR_DEPTH" -parent ${encoder_param} -widget comboBox
     ipgui::add_row $IPINST -parent ${encoder_param}
     set encoder_buffer_configuration  [ipgui::add_group $IPINST -parent $encoder_config -name " Encoder_Buffer" -layout horizontal]  
     ipgui::add_param $IPINST -name "ENC_BUFFER_EN" -parent ${encoder_buffer_configuration}
     ipgui::add_row $IPINST -parent ${encoder_buffer_configuration}
     ipgui::add_param $IPINST -name "ENC_BUFFER_TYPE" -parent ${encoder_buffer_configuration} -layout horizontal -widget comboBox
     ipgui::add_row $IPINST -parent ${encoder_buffer_configuration}
     #set_property display_name "OverRide Display" [ipgui::get_guiparamspec  ENC_BUFFER_TYPE -of $IPINST]
	 # tool-tips
     set_property tooltip "Enable/Disable VCU Encoder Engine – Either exposing associated interfaces to IPI design canvas for connection to system interfaces OR disabling and automatically clock gating to optimise power consumption" [ipgui::get_guiparamspec  ENABLE_ENCODER -of $IPINST]
     set_property tooltip "Compression standard to be used – AVC or HEVC. Where both shall be used, select HEVC." [ipgui::get_guiparamspec  ENC_VIDEO_STANDARD -of $IPINST]
     set_property tooltip "Group Of Frame Structure to use for encoding. “Intra” is for I-Only encoding, “Intra and Inter” allows full flexibility and the use of B and P frames, see Advanced configuration tab for further control and documentation for full explanation." [ipgui::get_guiparamspec  ENC_CODING_TYPE -of $IPINST]
     set_property tooltip "Video Resolution of each incoming stream. Where multiple streams are to be used set to most demanding stream size." [ipgui::get_guiparamspec  ENC_FRAME_SIZE -of $IPINST]
     set_property tooltip "Colour format of incoming stream. Where multiple streams are to be used set to most demanding." [ipgui::get_guiparamspec  ENC_COLOR_FORMAT -of $IPINST]
     set_property tooltip "Maximum frame rate of input stream(s)." [ipgui::get_guiparamspec  ENC_FPS -of $IPINST]
     set_property tooltip "Colour depth per Luminance/Chrominance  input sample in “bits”. Where multiple streams are to be used set to most demanding." [ipgui::get_guiparamspec  ENC_COLOR_DEPTH -of $IPINST]
     #set_property display_name "OverRide Display" [ipgui::get_guiparamspec  ENC_BUFFER_TYPE -of $IPINST]
     set_property tooltip  "Select PL memory resource type to use for Encoder Buffer" [ipgui::get_guiparamspec  ENC_BUFFER_TYPE -of $IPINST]
     set_property tooltip  "Reduce external memory bandwidth needs by provisioning Programmable Logic BRAM and/or URAM resource to model an OnChip Encoder Buffer. Does not apply in intra frame encoding only use case" [ipgui::get_guiparamspec  ENC_BUFFER_EN -of $IPINST]

  set decoder_config [ipgui::add_group $IPINST -parent $Page_0 -name "Decoder_Configuration" -layout horizontal]
  ipgui::add_param $IPINST -name "ENABLE_DECODER" -parent $decoder_config -widget checkBox
  ipgui::add_row $IPINST -parent $decoder_config
  ipgui::add_param $IPINST -name "NO_OF_DEC_STREAMS" -parent ${Page_0} 
#  ipgui::add_param $IPINST -name "NO_OF_DEC_STREAMS" -parent $decoder_config -widget comboBox 
#  set_property tooltip "Maximum Number of Decoder Streams that needs to be supported." [ipgui::get_guiparamspec  NO_OF_DEC_STREAMS -of $IPINST]
#  ipgui::add_row $IPINST -parent $decoder_config
    set decoder_param [ipgui::add_group $IPINST -parent $decoder_config -name "Stream Details - Most Demanding Stream" -layout horizontal]  
    ipgui::add_param $IPINST -name "DEC_VIDEO_STANDARD" -parent ${decoder_param} -widget comboBox
    ipgui::add_param $IPINST -name "DEC_CODING_TYPE" -parent ${decoder_param} -widget comboBox
    ipgui::add_row $IPINST -parent ${decoder_param}
    ipgui::add_param $IPINST -name "DEC_FRAME_SIZE" -parent ${decoder_param} -widget comboBox
    ipgui::add_param $IPINST -name "DEC_FPS" -parent ${decoder_param} -widget comboBox
    ipgui::add_row $IPINST -parent ${decoder_param}
    ipgui::add_param $IPINST -name "DEC_COLOR_FORMAT" -parent ${decoder_param} -widget comboBox
    ipgui::add_param $IPINST -name "DEC_COLOR_DEPTH" -parent ${decoder_param} -widget comboBox

    set_property tooltip  "Enable/Disable VCU Decoder Engine – Either exposing associated interfaces to IPI design canvas for connection to system interfaces OR disabling and automatically clock gating to optimise power consumption" [ipgui::get_guiparamspec  ENABLE_DECODER -of $IPINST]
    set_property tooltip  "Compression standard incoming streams, where both shall be used select HEVC." [ipgui::get_guiparamspec  DEC_VIDEO_STANDARD -of $IPINST]
    set_property tooltip  "Group Of Frame Structure to use for decoding. “Intra” is for I-Only decoding, “Intra and Inter” allows full flexibility and the use of B and P frames." [ipgui::get_guiparamspec  DEC_CODING_TYPE -of $IPINST]
    set_property tooltip  "Video Resolution of incoming encoded stream. Where multiple streams are to be used refer documentation to set to effective resolution." [ipgui::get_guiparamspec  DEC_FRAME_SIZE -of $IPINST]
    set_property tooltip  "Maximum effective frame rate of stream(s) to be decoded. Where multiple streams are to be used refer documentation to set to effective FPS." [ipgui::get_guiparamspec  DEC_FPS -of $IPINST]
    set_property tooltip  "Colour depth per Luminance/Chrominance  input sample in “bits”. Where multiple streams are to be used set to most demanding." [ipgui::get_guiparamspec  DEC_COLOR_DEPTH -of $IPINST]
    set_property tooltip  "Colour format of incoming stream. Where multiple streams are to be used set to most demanding." [ipgui::get_guiparamspec  DEC_COLOR_FORMAT -of $IPINST]

  set encoder_config_2 [ipgui::add_group $IPINST -parent $Page_1 -name "Encoder_Configuration"]  
     set enc_mem_manual_override [ipgui::add_group $IPINST -parent $encoder_config_2 -name "Encoder_Buffer" -layout horizontal]  
     ipgui::add_row $IPINST -parent ${enc_mem_manual_override}
     ipgui::add_param $IPINST -name "ENC_BUFFER_MANUAL_OVERRIDE" -parent ${enc_mem_manual_override} -layout horizontal -widget checkBox
     set_property tooltip "Please read VCU Manual before Override of Encoder Buffer" [ipgui::get_guiparamspec  ENC_BUFFER_MANUAL_OVERRIDE -of $IPINST]
     ipgui::add_row $IPINST -parent ${enc_mem_manual_override}
     ipgui::add_param $IPINST -name "ENC_BUFFER_SIZE" -parent ${enc_mem_manual_override}  -layout horizontal
     set enc_advanced_config [ipgui::add_group $IPINST -parent $encoder_config_2 -name "Compression Algorithm Feature Selection" -layout horizontal]  
     ipgui::add_row $IPINST -parent ${enc_advanced_config}
     ipgui::add_param $IPINST -name "ENC_BUFFER_B_FRAME" -parent ${enc_advanced_config} -layout horizontal -widget comboBox
     ipgui::add_row $IPINST -parent ${enc_advanced_config}
     ipgui::add_param $IPINST -name "ENC_BUFFER_MOTION_VEC_RANGE" -parent ${enc_advanced_config} -layout horizontal -widget comboBox
     ipgui::add_row $IPINST -parent ${enc_advanced_config}
     ipgui::add_param $IPINST -name "WPP_EN" -parent ${enc_advanced_config}  -layout horizontal -widget comboBox
     set_property visible false [ipgui::get_guiparamspec WPP_EN -of $IPINST]
     ipgui::add_row $IPINST -parent ${enc_advanced_config}
     #set_property enabled false [ipgui::get_groupspec Buffer_Manual_Override -of $IPINST]
     #set_property enabled false [ipgui::get_groupspec Buffer_Advanced_Configuration -of $IPINST]
     set_property tooltip  "Allows explicit control over depth of Encoder Buffer memory – In the default case the depth of the buffer is calculated based on the stream details entered under the Basic Configuration tab. ONLY override and manually set AFTER reading the Product Guide. How the memory is implemented remains a function of the “Memory Resource Type”  set on the Basic Tab" [ipgui::get_guiparamspec  ENC_BUFFER_MANUAL_OVERRIDE -of $IPINST]
     set_property tooltip  "Further provides use-case GOP structure detail's to the Vivado environment to allow accurate calculation of the  required Encoder Buffer memory depth and corresponding power consumption to be estimated through report_power." [ipgui::get_guiparamspec  ENC_BUFFER_B_FRAME -of $IPINST]
     set_property tooltip  "Provides Motion Vector search preconfigurations to the Vivado environment to allow accurate calculation of the required Encoder Buffer memory depth and corresponding power consumption to be estimated through report_power." [ipgui::get_guiparamspec  ENC_BUFFER_MOTION_VEC_RANGE -of $IPINST]


  set software_config [ipgui::add_group $IPINST -parent $Page_1 -name "Clock Configuration" -layout horizontal]  
  ipgui::add_row $IPINST -parent ${software_config}
  #ipgui::add_param $IPINST -name "PLL_BYPASS" -parent ${software_config} -layout horizontal -widget checkBox
  #set_property tooltip "Provides ability to bypass (and disable) VCU internal PLL, sourcing VCU Core and VCU MCU (MicroController) directly from Programmable Logic. Read Product Guide to understand clock frequency requirements and dependencies before selecting this option." [ipgui::get_guiparamspec  PLL_BYPASS -of $IPINST]
  ipgui::add_row $IPINST -parent ${software_config}
  # Based on Mrinal's feedback, we are removing the MCU_CLK from user input.
  # MCU_CLK is always assumed to be 444 MHz
  ipgui::add_param $IPINST -name "MCU_CLK" -parent ${software_config}
  set_property visible false [ipgui::get_guiparamspec MCU_CLK -of $IPINST]
  ipgui::add_param $IPINST -name "CORE_CLK" -parent ${software_config}
  set_property tooltip  "Clock freq MAX for -1 part is 667" [ipgui::get_guiparamspec  CORE_CLK -of $IPINST]
  ipgui::add_row $IPINST -parent ${software_config}

  ####### The following params are not visible in GUI..
  ipgui::add_param $IPINST -name "PLL_CLK" -parent ${software_config}
  ipgui::add_param $IPINST -name "ENC_CLK" -parent ${software_config}

  ipgui::add_param $IPINST -name "AXI_ENC_CLK" -parent ${software_config}
  ipgui::add_param $IPINST -name "AXI_DEC_CLK" -parent ${software_config}
  ipgui::add_param $IPINST -name "AXI_MCU_CLK" -parent ${software_config}

  ipgui::add_param $IPINST -name "AXI_ENC_BASE0" -parent ${software_config}
  ipgui::add_param $IPINST -name "AXI_ENC_BASE1" -parent ${software_config}
  ipgui::add_param $IPINST -name "AXI_DEC_BASE0" -parent ${software_config}
  ipgui::add_param $IPINST -name "AXI_DEC_BASE1" -parent ${software_config}
  ipgui::add_param $IPINST -name "AXI_MCU_BASE"  -parent ${software_config}

  ipgui::add_param $IPINST -name "AXI_ENC_RANGE0" -parent ${software_config}
  ipgui::add_param $IPINST -name "AXI_ENC_RANGE1" -parent ${software_config}
  ipgui::add_param $IPINST -name "AXI_DEC_RANGE0" -parent ${software_config}
  ipgui::add_param $IPINST -name "AXI_DEC_RANGE1" -parent ${software_config}
  ipgui::add_param $IPINST -name "AXI_MCU_RANGE"  -parent ${software_config}

  set_property visible false [ipgui::get_guiparamspec PLL_CLK -of $IPINST]
  set_property visible false [ipgui::get_guiparamspec ENC_CLK -of $IPINST]

  set_property visible false [ipgui::get_guiparamspec AXI_ENC_CLK -of $IPINST]
  set_property visible false [ipgui::get_guiparamspec AXI_DEC_CLK -of $IPINST]
  set_property visible false [ipgui::get_guiparamspec AXI_MCU_CLK -of $IPINST]

  set_property visible false [ipgui::get_guiparamspec AXI_ENC_BASE0 -of $IPINST]
  set_property visible false [ipgui::get_guiparamspec AXI_ENC_BASE1 -of $IPINST]
  set_property visible false [ipgui::get_guiparamspec AXI_DEC_BASE0 -of $IPINST]
  set_property visible false [ipgui::get_guiparamspec AXI_DEC_BASE1 -of $IPINST]
  set_property visible false [ipgui::get_guiparamspec AXI_MCU_BASE -of $IPINST]

  set_property visible false [ipgui::get_guiparamspec AXI_ENC_RANGE0 -of $IPINST]
  set_property visible false [ipgui::get_guiparamspec AXI_ENC_RANGE1 -of $IPINST]
  set_property visible false [ipgui::get_guiparamspec AXI_DEC_RANGE0 -of $IPINST]
  set_property visible false [ipgui::get_guiparamspec AXI_DEC_RANGE1 -of $IPINST]
  set_property visible false [ipgui::get_guiparamspec AXI_MCU_RANGE -of $IPINST]

}

proc init_params {IPINST PARAM_VALUE.ENABLE_ENCODER  PARAM_VALUE.ENC_CODING_TYPE PARAM_VALUE.ENC_BUFFER_EN PARAM_VALUE.ENC_BUFFER_TYPE  PARAM_VALUE.ENC_BUFFER_SIZE PARAM_VALUE.ENC_BUFFER_MANUAL_OVERRIDE  PARAM_VALUE.ENC_BUFFER_B_FRAME  PARAM_VALUE.ENC_BUFFER_MOTION_VEC_RANGE  PARAM_VALUE.WPP_EN PARAM_VALUE.CORE_CLK   } {

   set ENABLE_ENCODER  [get_property value ${PARAM_VALUE.ENABLE_ENCODER}]
   set ENC_CODING_TYPE [get_property value ${PARAM_VALUE.ENC_CODING_TYPE}]
   set ENC_BUFFER_EN   [get_property value ${PARAM_VALUE.ENC_BUFFER_EN}]
   set ENC_BUFFER_MANUAL_OVERRIDE   [get_property value ${PARAM_VALUE.ENC_BUFFER_MANUAL_OVERRIDE}]

   if {$ENABLE_ENCODER == "false" || $ENABLE_ENCODER == "FALSE" || $ENC_CODING_TYPE == 0} {
    set_property  enabled false  ${PARAM_VALUE.ENC_BUFFER_EN} 
    set_property  enabled false  ${PARAM_VALUE.ENC_BUFFER_TYPE} 

    set_property  enabled false  ${PARAM_VALUE.ENC_BUFFER_MANUAL_OVERRIDE} 
    set_property  enabled false  ${PARAM_VALUE.ENC_BUFFER_SIZE} 
    set_property  enabled false  ${PARAM_VALUE.ENC_BUFFER_B_FRAME} 
    set_property  enabled false  ${PARAM_VALUE.ENC_BUFFER_MOTION_VEC_RANGE} 
    set_property  enabled false  ${PARAM_VALUE.WPP_EN} 
   } elseif { $ENC_BUFFER_EN == "false" || $ENC_BUFFER_EN == "FALSE"  } {
    set_property  enabled false  ${PARAM_VALUE.ENC_BUFFER_TYPE} 

    set_property  enabled false  ${PARAM_VALUE.ENC_BUFFER_MANUAL_OVERRIDE} 
    set_property  enabled false  ${PARAM_VALUE.ENC_BUFFER_SIZE} 
    set_property  enabled false  ${PARAM_VALUE.ENC_BUFFER_B_FRAME} 
    set_property  enabled false  ${PARAM_VALUE.ENC_BUFFER_MOTION_VEC_RANGE} 
    set_property  enabled false  ${PARAM_VALUE.WPP_EN} 
   } elseif { $ENC_BUFFER_MANUAL_OVERRIDE == 1 } {
    set_property  enabled false  ${PARAM_VALUE.ENC_BUFFER_B_FRAME} 
    set_property  enabled false  ${PARAM_VALUE.ENC_BUFFER_MOTION_VEC_RANGE} 
    set_property  enabled false  ${PARAM_VALUE.WPP_EN} 
   }

# added by pruthvi.
#  set CORE_CLOCK [get_property value ${PARAM_VALUE.CORE_CLK}]
#   if {[::xit::get_property PROJECT_PARAM.SPEEDGRADE] == -1 && $CORE_CLOCK > 666 } {
#    set_property errmsg " CORE CLK should be < 666 for -1 part"  
#	send_msg INFO 110 "core clk value should be < 66 for part -1. actual is $CORE_CLOCK"
#   } 
}

#######  decoder BW Calculation #########################
#proc decoder_bw_calc { PARAM_VALUE.DEC_VIDEO_STANDARD PARAM_VALUE.DEC_COLOR_DEPTH PARAM_VALUE.DEC_COLOR_FORMAT PARAM_VALUE.DEC_FRAME_SIZE PARAM_VALUE.ENABLE_DECODER PARAM_VALUE.DEC_FPS PARAM_VALUE.CORE_CLK } {
proc decoder_bw_calc { DEC_VIDEO_STANDARD DEC_COLOR_DEPTH DEC_COLOR_FORMAT DEC_FRAME_SIZE ENABLE_DECODER DEC_FPS CORE_CLK NO_OF_STREAMS DEC_CODING_TYPE NO_OF_DEC_STREAMS } {
     #send_msg INFO 306 "in dec_bw_calc 1 "

	 ##set DEC_CODING_TYPE [get_property value ${PARAM_VALUE.DEC_CODING_TYPE}]
	 ##set DEC_BUFFER_SIZE [get_property value ${PARAM_VALUE.DEC_BUFFER_SIZE}]
	 #set ENABLE_DECODER [get_property value ${PARAM_VALUE.ENABLE_DECODER}]
	 #set DEC_VIDEO_STANDARD [get_property value ${PARAM_VALUE.DEC_VIDEO_STANDARD}]
	 #set DEC_COLOR_DEPTH [get_property value ${PARAM_VALUE.DEC_COLOR_DEPTH}]
	 #set DEC_COLOR_FORMAT [get_property value ${PARAM_VALUE.DEC_COLOR_FORMAT}]
	 #set DEC_FRAME_SIZE [get_property value ${PARAM_VALUE.DEC_FRAME_SIZE}]
	 #set DEC_FPS [get_property value ${PARAM_VALUE.DEC_FPS}]
	 #set CLOCK [get_property value ${PARAM_VALUE.CORE_CLK}]
	 ##set DEC_BUFFER_EN [get_property value ${PARAM_VALUE.DEC_BUFFER_EN}]
	 ##set DEC_BUFFER_B_FRAME [get_property value ${PARAM_VALUE.DEC_BUFFER_B_FRAME}]

     set X_VALUE   [calc_frame_size_x $DEC_FRAME_SIZE]
     set Y_VALUE   [calc_frame_size_y $DEC_FRAME_SIZE]
  	 set width     [expr (($X_VALUE + 63) / 64)*64 ]
  	 set height    [expr (($Y_VALUE + 7) / 8)*8 ]
  	 set width_MB  [expr (($X_VALUE + 15) / 16) ]
  	 set height_MB [expr (($Y_VALUE + 15) / 16) ]
     set FPS       [calc_fps $DEC_FPS]


     set rec_bw [expr $width * $height * $FPS]
     # 0 -> 4:2:0
  	 if { $DEC_COLOR_FORMAT == 0 } {set rec_bw [ expr ($rec_bw *3 /2) ] }
     # 1 -> 4:2:2
  	 if { $DEC_COLOR_FORMAT == 1 } {set rec_bw [ expr $rec_bw *2 ] }


	 #10 bit
     if { $DEC_COLOR_DEPTH  == 1 } {set rec_bw [ expr ($rec_bw * 4 / 3) ] }
     set ref_bw [ expr ($rec_bw * 5 / 2) ]
     set int_bw [ expr ($width_MB * $height_MB * $FPS / 2 ) ]


     # DEC_VIDEO_STANDARD == 0 is HEVC
	 if { $DEC_VIDEO_STANDARD ==  1 } {
        if { $DEC_COLOR_FORMAT == 2  }  {
           set int_bw [expr $int_bw * 1424 ]
		} elseif {  $DEC_COLOR_FORMAT == 1  } {
           set int_bw [expr $int_bw * 1136 ]
		} else {
           set int_bw [expr $int_bw * 816 ]
		}
	 } else {
        if { $DEC_COLOR_FORMAT == 2  }  {
           set int_bw [expr $int_bw * 1360 ]
		} elseif {  $DEC_COLOR_FORMAT == 1  } {
           set int_bw [expr $int_bw * 1104 ]
		} else {
           set int_bw [expr $int_bw * 848 ]
		}
	 }


	 set mv_bw [expr ($width_MB * $height_MB * $FPS * 16) ]
	 if { $DEC_VIDEO_STANDARD ==  1 } {
         set mv_bw [ expr $mv_bw * 8 ]
	 }

     #set bit_bw [ expr  ... can be neglected is the comment..
     set others_bw [ expr 52680 * $FPS ]

	 # -- round to MBytes/sec
	 set rec_bw    [expr ($rec_bw + 500000)  / 1000000 ]
	 set ref_bw    [expr ($ref_bw + 500000)  / 1000000 ]
	 set int_bw    [expr ($int_bw + 500000)  / 1000000 ]
	 set mv_bw     [expr ($mv_bw  + 500000)  / 1000000 ]
	 #set bit_bw    [expr ($bit_bw + 500000)  / 1000000 ]
	 set others_bw [expr ($others_bw + 500000)  / 1000000 ]



     #send_msg INFO 306 "in  FPS is $FPS "
     #send_msg INFO 306 "in  rec_bw is $rec_bw "
     #send_msg INFO 306 "in  ref_bw is $ref_bw "
     #send_msg INFO 306 "in  int_bw is $int_bw "
     #send_msg INFO 306 "in  mv_bw  is $mv_bw "
     #send_msg INFO 306 "in  others_bw  is $others_bw "

     set read_bw    [expr ($ref_bw  + $int_bw + $mv_bw + $others_bw) ] 
	 set write_bw   [expr ($rec_bw  + $int_bw + $mv_bw ) ] 
	 set total_bw   [expr ($read_bw + $write_bw) ]

	 # Intra only mode
     set read_bw_a    [expr ($int_bw + $others_bw) ]
	 set write_bw_a   [expr ($rec_bw  + $int_bw ) ]
	 set total_bw_a   [expr ($read_bw_a + $write_bw_a) ]

     #multistream scaling -- now decoder is aggregate value.. hence no scaling
	 set read_bw     [expr $read_bw*($NO_OF_DEC_STREAMS +0) ]
	 set write_bw    [expr $write_bw*($NO_OF_DEC_STREAMS +0) ]
	 set read_bw_a   [expr $read_bw_a*($NO_OF_DEC_STREAMS +0) ]
	 set write_bw_a  [expr $write_bw_a*($NO_OF_DEC_STREAMS +0) ]

	 #set read_bw     [ expr $read_bw ]
	 #set write_bw    [ expr $write_bw ]
	 #set read_bw_a   [ expr $read_bw_a ]
	 #set write_bw_a  [ expr $write_bw_a ]
     
     if {$ENABLE_DECODER == "false" || $ENABLE_DECODER == "FALSE" } {
          set read_bw 0
          set write_bw 0
          set read_bw_a 0
          set write_bw_a 0
	 }

     if { $DEC_CODING_TYPE == 0 } {
        set BW_msg "Decoder Bandwidth - READ $read_bw_a MBytes/Sec & WRITE $write_bw_a MBytes/Sec"
	 } else {
        set BW_msg "Decoder Bandwidth - READ $read_bw MBytes/Sec & WRITE $write_bw MBytes/Sec"
	 }

     return $BW_msg

}

proc encoder_bw_calc { ENC_CODING_TYPE ENC_VIDEO_STANDARD ENC_COLOR_DEPTH ENC_COLOR_FORMAT ENC_FRAME_SIZE ENABLE_ENCODER ENC_FPS CORE_CLK ENC_BUFFER_EN  ENC_BUFFER_B_FRAME NO_OF_STREAMS } {

	 #set ENABLE_ENCODER [get_property value ${PARAM_VALUE.ENABLE_ENCODER}]
	 #set ENC_VIDEO_STANDARD [get_property value ${PARAM_VALUE.ENC_VIDEO_STANDARD}]
	 #set ENC_COLOR_DEPTH [get_property value ${PARAM_VALUE.ENC_COLOR_DEPTH}]
	 #set ENC_COLOR_FORMAT [get_property value ${PARAM_VALUE.ENC_COLOR_FORMAT}]
	 #set ENC_FRAME_SIZE [get_property value ${PARAM_VALUE.ENC_FRAME_SIZE}]
	 #set ENC_FPS [get_property value ${PARAM_VALUE.ENC_FPS}]
	 #set CLOCK [get_property value ${PARAM_VALUE.CORE_CLK}]
	 #set ENC_BUFFER_EN [get_property value ${PARAM_VALUE.ENC_BUFFER_EN}]
	 #set ENC_BUFFER_B_FRAME [get_property value ${PARAM_VALUE.ENC_BUFFER_B_FRAME}]
	 set CLOCK $CORE_CLK

     set X_VALUE [calc_frame_size_x $ENC_FRAME_SIZE]
     set Y_VALUE [calc_frame_size_y $ENC_FRAME_SIZE]
 
 
     if {$ENC_BUFFER_B_FRAME == 0} {set B_FRAMES 0}
     if {$ENC_BUFFER_B_FRAME == 1} {set B_FRAMES 1}
     if {$ENC_BUFFER_B_FRAME == 2} {set B_FRAMES 1}
       
     if {$ENC_BUFFER_B_FRAME == 0} {set HIER_B 0}
     if {$ENC_BUFFER_B_FRAME == 1} {set HIER_B 0}
     if {$ENC_BUFFER_B_FRAME == 2} {set HIER_B 1}
  
     set width_LCU [expr (($X_VALUE + 31) / 32) ]
     set height_LCU [expr (($Y_VALUE + 31) / 32) ]
  	 set width  [expr (($X_VALUE + 63) / 64)*64 ]
  	 set height  [expr (($Y_VALUE + 7) / 8)*8 ]
  	 set NO_OF_CORES [calc_num_cores $ENC_FRAME_SIZE $ENC_FPS $CLOCK ]
     set FPS [calc_fps $ENC_FPS]
  

     #send_msg INFO 201 "in calcu BW  TOTAL_BYTES  width $width height $height FPS $FPS "
  
  	 set source_bw [expr $width * $height * $FPS]
       # 0 -> 4:2:0
  	 if { $ENC_COLOR_FORMAT == 0 } {set source_bw [ expr ($source_bw *3 /2) ] }
       # 1 -> 4:2:2
  	 if { $ENC_COLOR_FORMAT == 1 } {set source_bw [ expr $source_bw *2 ] }
     if { $ENC_COLOR_DEPTH  == 1 } {set source_bw [ expr ($source_bw *4 / 3) ] }
  
     set ref_bw [expr $width * $height * $FPS]
       # 0 -> 4:2:0
  	 if { $ENC_COLOR_FORMAT == 0 } {set ref_bw [ expr ($ref_bw *3 /2) ] }
       # 1 -> 4:2:2
  	 if { $ENC_COLOR_FORMAT == 1 } {set ref_bw [ expr $ref_bw *2 ] }
     if { $ENC_COLOR_DEPTH == 1 } {set ref_bw  [ expr ($ref_bw *10 / 8) ] }
  	 if { $B_FRAMES == 1 } {
        if { $ENC_BUFFER_EN == "TRUE" || $ENC_BUFFER_EN == "true" } {
              set ref_bw [ expr ($ref_bw * 2) ]
  		} else {
              set ref_bw [ expr ($ref_bw * 5) ]
  		}
  	 } else {
          if { $ENC_BUFFER_EN == "FALSE" || $ENC_BUFFER_EN == "false" } {
              set ref_bw [ expr ($ref_bw * 7) / 2 ]
  		  } else {
              set ref_bw [expr $ref_bw * 1]
		  }
  	 }
        
     if { $B_FRAMES == 0 || $HIER_B == 1 } {
        set rec_bw [expr $width * $height * $FPS]
          # 0 -> 4:2:0
  	    if { $ENC_COLOR_FORMAT == 0 } { set rec_bw [ expr ($rec_bw *3 /2) ] }
          # 1 -> 4:2:2
  	    if { $ENC_COLOR_FORMAT == 1 }  { set rec_bw [ expr $rec_bw *2 ] }
        if { $ENC_COLOR_DEPTH == 1 }  {set rec_bw [ expr ($rec_bw *10 / 8) ] }
  	 } else {
          set rec_bw 0
  	 }
  
     # ENC_VIDEO_STANDARD == 0 is HEVC
  	 if { $ENC_VIDEO_STANDARD == 1 && $NO_OF_CORES > 1 } {
         set intermediate_bw [expr ((($X_VALUE + 15)/ 16 ) * (($Y_VALUE +15 ) / 16) ) * $FPS]
  		 if { $ENC_COLOR_FORMAT == 1 } {
             if { $ENC_COLOR_DEPTH == 1  } {
                  set intermediate_bw [expr $intermediate_bw * 1296 ]
  			 } else {
                  set intermediate_bw [expr $intermediate_bw * 1168 ]
  			 }
  		 } else {
             if { $ENC_COLOR_DEPTH == 1  } {
                  set intermediate_bw [expr $intermediate_bw * 1008 ]
  			 } else {
                  set intermediate_bw [expr $intermediate_bw * 944 ]
  			 }
  		 }
         set intermediate_bw [expr $intermediate_bw * 4 / 10 ]
  	 } else {
         set intermediate_bw 0
  	 }
  
     set mv_bw [expr ((($X_VALUE + 15)/ 16 ) * (($Y_VALUE +15 ) / 16) ) * $FPS]
     # ENC_VIDEO_STANDARD == 0 is HEVC
  	 if { $ENC_VIDEO_STANDARD == 1 } {
        set mv_bw [expr $mv_bw * 32 ]
  	 } else {
        set mv_bw [expr $mv_bw * 16 ]
  	 }
  
     set bitstream_bw [expr ((($X_VALUE + 31)/ 32) * (($Y_VALUE +31 ) / 32) ) * $FPS]
     set bitstream_bw [expr ($bitstream_bw + 122400 -1) / 122400]
     set bitstream_bw [expr ($bitstream_bw * 30000000) / 8]
  
  	 set others_bw  [expr 64 + 256 + 25344]
  	 if { $ENC_VIDEO_STANDARD == 1 } {
          set others_bw [expr $others_bw + (($X_VALUE + 15) / 16) * (($Y_VALUE + 15)/16)]
  	 } else {
          set others_bw [expr $others_bw + (($X_VALUE + 31) / 32) * (($Y_VALUE + 31)/32)]
  	 }
  	 set others_bw  [expr $others_bw * $FPS]
  
  	 set source_bw [expr ($source_bw + 500000) / 1000000]
     set ref_bw  [expr ($ref_bw + 500000) / 1000000]
     set rec_bw [expr ($rec_bw + 500000) / 1000000]
     set intermediate_bw [expr ($intermediate_bw + 500000) / 1000000]
     set mv_bw [expr ($mv_bw + 500000) / 1000000]
     set bitstream_bw [expr ($bitstream_bw + 500000) / 1000000]
     set others_bw [expr ($others_bw + 500000) / 1000000]
  
     #send_msg INFO 201 "in calcu BW  source_bw $source_bw "
     #send_msg INFO 201 "in calcu BW  ref_bw $ref_bw "
     #send_msg INFO 201 "in calcu BW  rec_bw $rec_bw "
     #send_msg INFO 201 "in calcu BW  intermediate_bw $intermediate_bw "
     #send_msg INFO 201 "in calcu BW  mv_bw $mv_bw "
     #send_msg INFO 201 "in calcu BW  bitstream_bw $bitstream_bw "
     #send_msg INFO 201 "in calcu BW  others_bw $others_bw "

	 if { $ENC_CODING_TYPE == 1 } {
        set read_bw [expr $source_bw + $ref_bw + $intermediate_bw + $mv_bw + $others_bw]
	 } else {
        set read_bw [expr $source_bw + $intermediate_bw + $others_bw]
	 }

     set write_bw [expr $rec_bw + $intermediate_bw + $bitstream_bw]
     if {$B_FRAMES == 0 || $HIER_B == 1} {
         set write_bw [expr $write_bw + $mv_bw]
  	 }
     set total_bw  [expr $read_bw + $write_bw]
  
  
  	if {$ENABLE_ENCODER == "FALSE" || $ENABLE_ENCODER == "false"} {
        set read_bw 0
        set write_bw 0
 	}
    #multistream upscaling
    set read_bw  [ expr $read_bw*($NO_OF_STREAMS + 0) ]
    set write_bw [ expr $write_bw*($NO_OF_STREAMS + 0) ]

    return "Encoder Bandwidth - READ $read_bw MBytes/Sec & WRITE $write_bw MBytes/Sec\n"
}

#######  Encoder Buffer Depth Calculation ###############
proc encoder_depth_calc {PARAM_VALUE.ENC_BUFFER_SIZE PARAM_VALUE.ENC_CODING_TYPE  PARAM_VALUE.ENC_VIDEO_STANDARD PARAM_VALUE.ENC_COLOR_DEPTH PARAM_VALUE.ENC_COLOR_FORMAT PARAM_VALUE.ENC_FRAME_SIZE PARAM_VALUE.ENABLE_ENCODER PARAM_VALUE.ENC_FPS PARAM_VALUE.CORE_CLK PARAM_VALUE.ENC_BUFFER_EN  PARAM_VALUE.ENC_BUFFER_B_FRAME PARAM_VALUE.ENC_MEM_BRAM_USED  PARAM_VALUE.ENC_MEM_URAM_USED PARAM_VALUE.DEC_VIDEO_STANDARD PARAM_VALUE.DEC_FRAME_SIZE PARAM_VALUE.ENABLE_DECODER PARAM_VALUE.DEC_FPS  PARAM_VALUE.DEC_COLOR_DEPTH PARAM_VALUE.DEC_COLOR_FORMAT PARAM_VALUE.NO_OF_STREAMS PARAM_VALUE.NO_OF_DEC_STREAMS PARAM_VALUE.DEC_CODING_TYPE } {

     #send_msg INFO 201 "in calcu BW  TOTAL_BYTES "

	 set ENC_CODING_TYPE [get_property value ${PARAM_VALUE.ENC_CODING_TYPE}]
	 set ENC_BUFFER_SIZE [get_property value ${PARAM_VALUE.ENC_BUFFER_SIZE}]
	 set ENABLE_ENCODER [get_property value ${PARAM_VALUE.ENABLE_ENCODER}]
	 set ENC_VIDEO_STANDARD [get_property value ${PARAM_VALUE.ENC_VIDEO_STANDARD}]
	 set ENC_COLOR_DEPTH [get_property value ${PARAM_VALUE.ENC_COLOR_DEPTH}]
	 set ENC_COLOR_FORMAT [get_property value ${PARAM_VALUE.ENC_COLOR_FORMAT}]
	 set ENC_FRAME_SIZE [get_property value ${PARAM_VALUE.ENC_FRAME_SIZE}]
	 set ENC_FPS [get_property value ${PARAM_VALUE.ENC_FPS}]
	 set CLOCK [get_property value ${PARAM_VALUE.CORE_CLK}]
	 set ENC_BUFFER_EN [get_property value ${PARAM_VALUE.ENC_BUFFER_EN}]
	 set ENC_BUFFER_B_FRAME [get_property value ${PARAM_VALUE.ENC_BUFFER_B_FRAME}]
	 set NO_OF_STREAMS [get_property value ${PARAM_VALUE.NO_OF_STREAMS}]
	 set NO_OF_DEC_STREAMS [get_property value ${PARAM_VALUE.NO_OF_DEC_STREAMS}]
     set ret_str [encoder_bw_calc $ENC_CODING_TYPE  $ENC_VIDEO_STANDARD  $ENC_COLOR_DEPTH   $ENC_COLOR_FORMAT   $ENC_FRAME_SIZE $ENABLE_ENCODER  $ENC_FPS  $CLOCK  $ENC_BUFFER_EN  $ENC_BUFFER_B_FRAME $NO_OF_STREAMS ]

	 set ENABLE_DECODER [get_property value ${PARAM_VALUE.ENABLE_DECODER}]
	 set DEC_VIDEO_STANDARD [get_property value ${PARAM_VALUE.DEC_VIDEO_STANDARD}]
	 set DEC_COLOR_DEPTH [get_property value ${PARAM_VALUE.DEC_COLOR_DEPTH}]
	 set DEC_COLOR_FORMAT [get_property value ${PARAM_VALUE.DEC_COLOR_FORMAT}]
	 set DEC_FRAME_SIZE [get_property value ${PARAM_VALUE.DEC_FRAME_SIZE}]
	 set DEC_FPS [get_property value ${PARAM_VALUE.DEC_FPS}]
	 set DEC_CODING_TYPE [get_property value ${PARAM_VALUE.DEC_CODING_TYPE}]
	 #set CLOCK [get_property value ${PARAM_VALUE.CORE_CLK}]
	 #set DEC_BUFFER_EN [get_property value ${PARAM_VALUE.DEC_BUFFER_EN}]


     #send_msg INFO 603 "in decoder BW  call $DEC_VIDEO_STANDARD $DEC_COLOR_DEPTH $DEC_COLOR_FORMAT $DEC_FRAME_SIZE $ENABLE_DECODER $DEC_FPS $CLOCK"
     set ret_str_dec [decoder_bw_calc $DEC_VIDEO_STANDARD $DEC_COLOR_DEPTH $DEC_COLOR_FORMAT $DEC_FRAME_SIZE $ENABLE_DECODER $DEC_FPS $CLOCK $NO_OF_STREAMS $DEC_CODING_TYPE $NO_OF_DEC_STREAMS ]
     #send_msg INFO 603  "return is $ret_str_dec "

	 #set ENABLE_ENCODER [get_property value ${PARAM_VALUE.ENABLE_ENCODER}]
	 #set ENC_BUFFER_EN [get_property value ${PARAM_VALUE.ENC_BUFFER_EN}]
	 if {($ENABLE_ENCODER == "TRUE" || $ENABLE_ENCODER == "true") && ( $ENC_BUFFER_EN == "true" || $ENC_BUFFER_EN == "TRUE") } {
	    set ENC_MEM_BRAM_USED [get_property value ${PARAM_VALUE.ENC_MEM_BRAM_USED}]
	    set ENC_MEM_URAM_USED [get_property value ${PARAM_VALUE.ENC_MEM_URAM_USED}]
	 } else {
	    set ENC_MEM_BRAM_USED 0
	    set ENC_MEM_URAM_USED 0
	 }
     #send_msg INFO 201 "in calcu depth  TOTAL_BYTES $ENC_MEM_BRAM_USED "
	 set TOTAL_MEM [expr ($ENC_MEM_BRAM_USED + $ENC_MEM_URAM_USED)]
     return "Encoder Buffer Size - $ENC_BUFFER_SIZE KBytes \n $ret_str $ret_str_dec "
     #return "Encoder Buffer Size - $ENC_BUFFER_SIZE KBytes \n $ret_str $ret_str_dec "
}



proc update_PARAM_VALUE.ENC_MEM_URAM_USED { PARAM_VALUE.ENC_MEM_URAM_USED PARAM_VALUE.ENC_BUFFER_SIZE_ACTUAL PARAM_VALUE.ENC_COLOR_DEPTH PARAM_VALUE.TABLE_NO PARAM_VALUE.ENC_BUFFER_TYPE } {
	 set ENC_BUFFER_TYPE [get_property value ${PARAM_VALUE.ENC_BUFFER_TYPE}]
	 set ENC_BUFFER_SIZE_ACTUAL [get_property value ${PARAM_VALUE.ENC_BUFFER_SIZE_ACTUAL}]
	 if { $ENC_BUFFER_TYPE == 0} {
 	    set_property value $ENC_BUFFER_SIZE_ACTUAL ${PARAM_VALUE.ENC_MEM_URAM_USED}
	 } else {
 	    set_property value 0 ${PARAM_VALUE.ENC_MEM_URAM_USED}
	 }
}

proc update_PARAM_VALUE.ENC_MEM_BRAM_USED { PARAM_VALUE.ENC_MEM_BRAM_USED PARAM_VALUE.ENC_BUFFER_SIZE_ACTUAL PARAM_VALUE.ENC_COLOR_DEPTH PARAM_VALUE.TABLE_NO PARAM_VALUE.ENC_BUFFER_TYPE } {
	 set ENC_BUFFER_SIZE_ACTUAL [get_property value ${PARAM_VALUE.ENC_BUFFER_SIZE_ACTUAL}]
	 set ENC_COLOR_DEPTH [get_property value ${PARAM_VALUE.ENC_COLOR_DEPTH}]
	 set TABLE_NO        [get_property value ${PARAM_VALUE.TABLE_NO}]
	 set ENC_BUFFER_TYPE [get_property value ${PARAM_VALUE.ENC_BUFFER_TYPE}]
	 if { $ENC_BUFFER_TYPE == 1} {
 	     set_property value $ENC_BUFFER_SIZE_ACTUAL ${PARAM_VALUE.ENC_MEM_BRAM_USED}
	 } else {
 	     set_property value 0 ${PARAM_VALUE.ENC_MEM_BRAM_USED}
	 }
     #send_msg INFO 201 "in ENC_MEM_BRAM_USED  TOTAL_BYTES $TOTAL_BYTES "
}


proc update_MODELPARAM_VALUE.ENABLEENCODER { MODELPARAM_VALUE.ENABLEENCODER  PARAM_VALUE.ENABLE_ENCODER  } {
   set ENABLE_ENCODER [get_property value ${PARAM_VALUE.ENABLE_ENCODER}]
   if {$ENABLE_ENCODER == "false" || $ENABLE_ENCODER == "FALSE" } {
      set_property value "FALSE" ${MODELPARAM_VALUE.ENABLEENCODER}
   } else {
      set_property value "TRUE" ${MODELPARAM_VALUE.ENABLEENCODER}
   }
}

proc update_MODELPARAM_VALUE.ENABLEDECODER { MODELPARAM_VALUE.ENABLEDECODER  PARAM_VALUE.ENABLE_DECODER  } {
   set ENABLE_DECODER [get_property value ${PARAM_VALUE.ENABLE_DECODER}]
   if {$ENABLE_DECODER == "false" || $ENABLE_DECODER == "FALSE" } {
      set_property value "FALSE" ${MODELPARAM_VALUE.ENABLEDECODER}
   } else {
      set_property value "TRUE" ${MODELPARAM_VALUE.ENABLEDECODER}
   }
}

proc update_MODELPARAM_VALUE.ENCODERCODING { MODELPARAM_VALUE.ENCODERCODING  PARAM_VALUE.ENC_VIDEO_STANDARD  } {
   set ENC_VIDEO_STANDARD [get_property value ${PARAM_VALUE.ENC_VIDEO_STANDARD}]
   if { $ENC_VIDEO_STANDARD == 0 } {
      set_property value "H.265" ${MODELPARAM_VALUE.ENCODERCODING}
   } elseif { $ENC_VIDEO_STANDARD == 1 } {
      set_property value "H.264" ${MODELPARAM_VALUE.ENCODERCODING}
   } else {
      # error message
   }
}

proc update_MODELPARAM_VALUE.DECODERCODING { MODELPARAM_VALUE.DECODERCODING PARAM_VALUE.DEC_VIDEO_STANDARD  } {
   set DEC_VIDEO_STANDARD [get_property value ${PARAM_VALUE.DEC_VIDEO_STANDARD}]
   if { $DEC_VIDEO_STANDARD == 0 } {
      set_property value "H.265" ${MODELPARAM_VALUE.ENCODERCODING}
   } elseif { $DEC_VIDEO_STANDARD == 1 } {
      set_property value "H.264" ${MODELPARAM_VALUE.ENCODERCODING}
   } else {
      # error message
   }
}

proc update_MODELPARAM_VALUE.ENCODERCODINGTYPE { MODELPARAM_VALUE.ENCODERCODINGTYPE  PARAM_VALUE.ENC_CODING_TYPE  } {
   set ENC_CODING_TYPE [get_property value ${PARAM_VALUE.ENC_CODING_TYPE}]
   if { $ENC_CODING_TYPE == 0 } {
      set_property value "INTRA_ONLY" ${MODELPARAM_VALUE.ENCODERCODINGTYPE}
   } elseif { $ENC_CODING_TYPE == 1 } {
      set_property value "INTRA_AND_INTER" ${MODELPARAM_VALUE.ENCODERCODINGTYPE}
   } else {
      # error message
   }
}

proc update_MODELPARAM_VALUE.DECODERCODINGTYPE { MODELPARAM_VALUE.DECODERCODINGTYPE PARAM_VALUE.DEC_CODING_TYPE  } {
   set DEC_CODING_TYPE [get_property value ${PARAM_VALUE.DEC_CODING_TYPE}]
   if { $DEC_CODING_TYPE == 0 } {
      set_property value "INTRA_ONLY" ${MODELPARAM_VALUE.DECODERCODINGTYPE}
   } elseif { $DEC_CODING_TYPE == 1 } {
      set_property value "INTRA_AND_INTER" ${MODELPARAM_VALUE.DECODERCODINGTYPE}
   } else {
      # error message
   }
}

proc update_MODELPARAM_VALUE.HDL_MAX_NUM_CORES { MODELPARAM_VALUE.HDL_MAX_NUM_CORES PARAM_VALUE.NO_OF_STREAMS PARAM_VALUE.ENC_FPS PARAM_VALUE.CORE_CLK  PARAM_VALUE.ENC_FRAME_SIZE  } {

   set NO_OF_STREAMS [get_property value ${PARAM_VALUE.NO_OF_STREAMS}]

   set FRAME_SIZE [get_property value ${PARAM_VALUE.ENC_FRAME_SIZE}]
   set CLOCK [get_property value ${PARAM_VALUE.CORE_CLK}]
   set FPS [get_property value ${PARAM_VALUE.ENC_FPS}]

   set C [calc_num_cores $FRAME_SIZE $FPS $CLOCK ] 
   set num_concurrent_streams [ expr ((4)/$C) ]
   set NO_OF_STREAMS [ expr ($NO_OF_STREAMS + 0)]

   #send_msg INFO 201 " NO_OF_STREAMS $NO_OF_STREAMS, num_concurrent_streams $num_concurrent_streams "
   if { $num_concurrent_streams  > $NO_OF_STREAMS } {
       set num_concurrent_streams $NO_OF_STREAMS
   }

   set HDL_MAX_NUM_CORES [ expr  $num_concurrent_streams * $C]
   if { $HDL_MAX_NUM_CORES > 4 } {
      # error message..
   }

   set_property value $HDL_MAX_NUM_CORES ${MODELPARAM_VALUE.HDL_MAX_NUM_CORES}
}

proc update_MODELPARAM_VALUE.HDL_NUM_CONCURRENT_STREAMS { MODELPARAM_VALUE.HDL_NUM_CONCURRENT_STREAMS PARAM_VALUE.NO_OF_STREAMS PARAM_VALUE.ENC_FPS PARAM_VALUE.CORE_CLK  PARAM_VALUE.ENC_FRAME_SIZE  } {
   set NO_OF_STREAMS [get_property value ${PARAM_VALUE.NO_OF_STREAMS}]
   set FRAME_SIZE [get_property value ${PARAM_VALUE.ENC_FRAME_SIZE}]
   set CLOCK [get_property value ${PARAM_VALUE.CORE_CLK}]
   set FPS [get_property value ${PARAM_VALUE.ENC_FPS}]

   set C [calc_num_cores $FRAME_SIZE $FPS $CLOCK ] 
   set HDL_NUM_CONCURRENT_STREAMS [ expr ((4)/$C) ]
   set NO_OF_STREAMS [expr $NO_OF_STREAMS + 0]
   if { $HDL_NUM_CONCURRENT_STREAMS  > $NO_OF_STREAMS } {
       set HDL_NUM_CONCURRENT_STREAMS $NO_OF_STREAMS
   }

   set_property value $HDL_NUM_CONCURRENT_STREAMS ${MODELPARAM_VALUE.HDL_NUM_CONCURRENT_STREAMS}
}

proc update_MODELPARAM_VALUE.HDL_NUM_STREAMS { MODELPARAM_VALUE.HDL_NUM_STREAMS PARAM_VALUE.NO_OF_STREAMS } {
   set NO_OF_STREAMS [get_property value ${PARAM_VALUE.NO_OF_STREAMS}]
   set NO_OF_STREAMS [expr $NO_OF_STREAMS + 0]
   set_property value $NO_OF_STREAMS ${MODELPARAM_VALUE.HDL_NUM_STREAMS}
}

proc update_MODELPARAM_VALUE.ENCHORRESOLUTION { MODELPARAM_VALUE.ENCHORRESOLUTION MODELPARAM_VALUE.HDL_FRAME_SIZE_X } {
   set FRAME_SIZE [get_property value ${MODELPARAM_VALUE.HDL_FRAME_SIZE_X}]
   set_property value $FRAME_SIZE ${MODELPARAM_VALUE.ENCHORRESOLUTION}
}

proc update_MODELPARAM_VALUE.ENCVERRESOLUTION { MODELPARAM_VALUE.ENCVERRESOLUTION MODELPARAM_VALUE.HDL_FRAME_SIZE_Y } {
   set FRAME_SIZE [get_property value ${MODELPARAM_VALUE.HDL_FRAME_SIZE_Y}]
   set_property value $FRAME_SIZE ${MODELPARAM_VALUE.ENCVERRESOLUTION}
}

proc update_MODELPARAM_VALUE.DECHORRESOLUTION { MODELPARAM_VALUE.DECHORRESOLUTION MODELPARAM_VALUE.HDL_DEC_FRAME_SIZE_X } {
   set FRAME_SIZE [get_property value ${MODELPARAM_VALUE.HDL_DEC_FRAME_SIZE_X}]
   set_property value $FRAME_SIZE ${MODELPARAM_VALUE.DECHORRESOLUTION}
}

proc update_MODELPARAM_VALUE.DECVERRESOLUTION { MODELPARAM_VALUE.DECVERRESOLUTION MODELPARAM_VALUE.HDL_DEC_FRAME_SIZE_Y } {
   set FRAME_SIZE [get_property value ${MODELPARAM_VALUE.HDL_DEC_FRAME_SIZE_Y}]
   set_property value $FRAME_SIZE ${MODELPARAM_VALUE.DECVERRESOLUTION}
}

proc update_MODELPARAM_VALUE.ENCODERCOLORDEPTH { MODELPARAM_VALUE.ENCODERCOLORDEPTH  PARAM_VALUE.ENC_COLOR_DEPTH  } {
   set ENC_COLOR_DEPTH [get_property value ${PARAM_VALUE.ENC_COLOR_DEPTH}]
   if { $ENC_COLOR_DEPTH == 0 } {
      set_property value 8 ${MODELPARAM_VALUE.ENCODERCOLORDEPTH}
   } elseif {  $ENC_COLOR_DEPTH == 1 } {
      set_property value 10 ${MODELPARAM_VALUE.ENCODERCOLORDEPTH}
   } else {
      # error message
   }
}

proc update_MODELPARAM_VALUE.ENCODERCHROMAFORMAT { MODELPARAM_VALUE.ENCODERCHROMAFORMAT  PARAM_VALUE.ENC_COLOR_FORMAT  } {
   set ENC_COLOR_FORMAT [get_property value ${PARAM_VALUE.ENC_COLOR_FORMAT}]
   if { $ENC_COLOR_FORMAT == 0 } {
      set_property value "4_2_0" ${MODELPARAM_VALUE.ENCODERCOLORDEPTH}
   } elseif {  $ENC_COLOR_FORMAT == 1 } {
      set_property value "4_2_2" ${MODELPARAM_VALUE.ENCODERCOLORDEPTH}
   } elseif {  $ENC_COLOR_FORMAT == 2 } {
      set_property value "4_0_0" ${MODELPARAM_VALUE.ENCODERCOLORDEPTH}
   } else {
      # error message
   }
}

proc update_MODELPARAM_VALUE.DECODERNUMCORE { MODELPARAM_VALUE.DECODERNUMCORE PARAM_VALUE.DEC_FPS PARAM_VALUE.CORE_CLK  PARAM_VALUE.DEC_FRAME_SIZE  } {
   set DEC_FRAME_SIZE [get_property value ${PARAM_VALUE.DEC_FRAME_SIZE}]
   set CORE_CLK [get_property value ${PARAM_VALUE.CORE_CLK}]
   set DEC_FPS [get_property value ${PARAM_VALUE.DEC_FPS}]

   set NO_OF_CORES [calc_num_cores $DEC_FRAME_SIZE $DEC_FPS $CORE_CLK]
   set NO_OF_CORES [expr ($NO_OF_CORES / 2 ) ]
   set_property value $NO_OF_CORES ${MODELPARAM_VALUE.DECODERNUMCORE}
}

proc update_MODELPARAM_VALUE.ENCODERNUMCORES { MODELPARAM_VALUE.ENCODERNUMCORES PARAM_VALUE.ENC_FPS PARAM_VALUE.CORE_CLK  PARAM_VALUE.ENC_FRAME_SIZE  } {
   set ENC_FRAME_SIZE [get_property value ${PARAM_VALUE.ENC_FRAME_SIZE}]
   set CORE_CLK [get_property value ${PARAM_VALUE.CORE_CLK}]
   set ENC_FPS [get_property value ${PARAM_VALUE.ENC_FPS}]

   set NO_OF_CORES [calc_num_cores $ENC_FRAME_SIZE $ENC_FPS $CORE_CLK ] 
   set_property value $NO_OF_CORES ${MODELPARAM_VALUE.ENCODERNUMCORES}
}

proc update_MODELPARAM_VALUE.DECODERCHROMAFORMAT { MODELPARAM_VALUE.DECODERCHROMAFORMAT } {
   set_property value "4_2_2" ${MODELPARAM_VALUE.DECODERCHROMAFORMAT}
}

proc update_MODELPARAM_VALUE.DECODERCOLORDEPTH { MODELPARAM_VALUE.DECODERCOLORDEPTH } {
   set_property value 8 ${MODELPARAM_VALUE.DECODERCOLORDEPTH}
}

proc update_MODELPARAM_VALUE.CORECLKFREQ { MODELPARAM_VALUE.CORECLKFREQ PARAM_VALUE.CORE_CLK } {
   set_property value [get_property value ${PARAM_VALUE.CORE_CLK}] ${MODELPARAM_VALUE.CORECLKFREQ}
}

proc update_PARAM_VALUE.CORE_CLK { PARAM_VALUE.CORE_CLK PARAM_VALUE.PLL_BYPASS IPINST } {
   set PLL_BYPASS [get_property value ${PARAM_VALUE.PLL_BYPASS}]
   if {$PLL_BYPASS == 1 } {
      set_property enabled false ${PARAM_VALUE.CORE_CLK}
   } else {
      set_property enabled true ${PARAM_VALUE.CORE_CLK}
   }
}

proc update_PARAM_VALUE.ENC_VIDEO_STANDARD { PARAM_VALUE.ENC_VIDEO_STANDARD PARAM_VALUE.ENABLE_ENCODER IPINST } {
   set ENABLE_ENCODER [get_property value ${PARAM_VALUE.ENABLE_ENCODER}]
   if {$ENABLE_ENCODER == "false" || $ENABLE_ENCODER == "FALSE" } {
      set_property enabled false ${PARAM_VALUE.ENC_VIDEO_STANDARD}
   } else {
      set_property enabled true ${PARAM_VALUE.ENC_VIDEO_STANDARD}
   }
}

proc update_PARAM_VALUE.NO_OF_STREAMS { PARAM_VALUE.NO_OF_STREAMS PARAM_VALUE.ENABLE_ENCODER IPINST } {
   set ENABLE_ENCODER [get_property value ${PARAM_VALUE.ENABLE_ENCODER}]
   if {$ENABLE_ENCODER == "false" || $ENABLE_ENCODER == "FALSE" } {
      set_property enabled false ${PARAM_VALUE.NO_OF_STREAMS}
   } else {
      set_property enabled true ${PARAM_VALUE.NO_OF_STREAMS}
   }
}


proc update_PARAM_VALUE.ENC_CODING_TYPE { PARAM_VALUE.ENC_CODING_TYPE PARAM_VALUE.ENABLE_ENCODER IPINST } {
   set ENABLE_ENCODER [get_property value ${PARAM_VALUE.ENABLE_ENCODER}]
   if {$ENABLE_ENCODER == "false" || $ENABLE_ENCODER == "FALSE" } {
      set_property enabled false ${PARAM_VALUE.ENC_CODING_TYPE}
   } else {
      set_property enabled true ${PARAM_VALUE.ENC_CODING_TYPE}
   }
}

proc update_PARAM_VALUE.ENC_FRAME_SIZE { PARAM_VALUE.ENC_FRAME_SIZE PARAM_VALUE.ENABLE_ENCODER IPINST } {
   set ENABLE_ENCODER [get_property value ${PARAM_VALUE.ENABLE_ENCODER}]
   if {$ENABLE_ENCODER == "false" || $ENABLE_ENCODER == "FALSE" } {
      set_property enabled false ${PARAM_VALUE.ENC_FRAME_SIZE}
   } else {
      set_property enabled true ${PARAM_VALUE.ENC_FRAME_SIZE}
   }
}

proc update_PARAM_VALUE.ENC_COLOR_FORMAT { PARAM_VALUE.ENC_COLOR_FORMAT PARAM_VALUE.ENABLE_ENCODER IPINST } {
   set ENABLE_ENCODER [get_property value ${PARAM_VALUE.ENABLE_ENCODER}]
   if {$ENABLE_ENCODER == "false" || $ENABLE_ENCODER == "FALSE" } {
      set_property enabled false ${PARAM_VALUE.ENC_COLOR_FORMAT}
   } else {
      set_property enabled true ${PARAM_VALUE.ENC_COLOR_FORMAT}
   }
}

proc update_PARAM_VALUE.ENC_COLOR_DEPTH { PARAM_VALUE.ENC_COLOR_DEPTH PARAM_VALUE.ENABLE_ENCODER IPINST } {
   set ENABLE_ENCODER [get_property value ${PARAM_VALUE.ENABLE_ENCODER}]
   if {$ENABLE_ENCODER == "false" || $ENABLE_ENCODER == "FALSE" } {
      set_property enabled false ${PARAM_VALUE.ENC_COLOR_DEPTH}
   } else {
      set_property enabled true ${PARAM_VALUE.ENC_COLOR_DEPTH}
   }
}

proc update_PARAM_VALUE.ENC_BUFFER_EN { PARAM_VALUE.ENC_BUFFER_EN PARAM_VALUE.ENABLE_ENCODER PARAM_VALUE.ENC_CODING_TYPE IPINST } {
   set ENABLE_ENCODER [get_property value ${PARAM_VALUE.ENABLE_ENCODER}]
   set ENC_CODING_TYPE [get_property value ${PARAM_VALUE.ENC_CODING_TYPE}]
   # In Intra Frame Only model also disable buffer
   if {$ENABLE_ENCODER == "false" || $ENABLE_ENCODER == "FALSE" || $ENC_CODING_TYPE == 0} {
	  set_property value "false" ${PARAM_VALUE.ENC_BUFFER_EN}
      set_property enabled false ${PARAM_VALUE.ENC_BUFFER_EN}
   } else {
      set_property enabled true ${PARAM_VALUE.ENC_BUFFER_EN}
   }
}

proc update_PARAM_VALUE.ENC_BUFFER_TYPE { PARAM_VALUE.ENC_BUFFER_TYPE PARAM_VALUE.ENC_BUFFER_EN PARAM_VALUE.ENC_CODING_TYPE PARAM_VALUE.ENABLE_ENCODER IPINST } {
   set ENABLE_ENCODER [get_property value ${PARAM_VALUE.ENABLE_ENCODER}]
   set ENC_BUFFER_EN [get_property value ${PARAM_VALUE.ENC_BUFFER_EN}]
   set ENC_CODING_TYPE [get_property value ${PARAM_VALUE.ENC_CODING_TYPE}]
   # In Intra Frame Only model also disable buffer
   if {$ENABLE_ENCODER == "false" || $ENABLE_ENCODER == "FALSE" || $ENC_BUFFER_EN == "false" || $ENC_BUFFER_EN == "FALSE" || $ENC_CODING_TYPE == 0 } {
      set_property enabled false ${PARAM_VALUE.ENC_BUFFER_TYPE}
   } else {
      set_property enabled true ${PARAM_VALUE.ENC_BUFFER_TYPE}
      # TBD : default is URAM for now
      set_property value 0 ${PARAM_VALUE.ENC_BUFFER_TYPE}
   }
}

proc update_PARAM_VALUE.DEC_VIDEO_STANDARD { PARAM_VALUE.DEC_VIDEO_STANDARD PARAM_VALUE.ENABLE_DECODER IPINST } {
   set ENABLE_DECODER [get_property value ${PARAM_VALUE.ENABLE_DECODER}]
   if {$ENABLE_DECODER == "false" || $ENABLE_DECODER == "FALSE" } {
        set_property enabled false ${PARAM_VALUE.DEC_VIDEO_STANDARD}
   } else {
        set_property enabled true ${PARAM_VALUE.DEC_VIDEO_STANDARD}
   }
}

proc update_PARAM_VALUE.DEC_FRAME_SIZE { PARAM_VALUE.DEC_FRAME_SIZE PARAM_VALUE.ENABLE_DECODER IPINST } {
   set ENABLE_DECODER [get_property value ${PARAM_VALUE.ENABLE_DECODER}]
   if {$ENABLE_DECODER == "false" || $ENABLE_DECODER == "FALSE" } {
        set_property enabled false ${PARAM_VALUE.DEC_FRAME_SIZE}
   } else {
      set_property enabled true ${PARAM_VALUE.DEC_FRAME_SIZE}
   }
}

proc update_PARAM_VALUE.DEC_FPS { PARAM_VALUE.DEC_FPS PARAM_VALUE.ENABLE_DECODER PARAM_VALUE.DEC_FRAME_SIZE IPINST } {
	 set ENABLE_DECODER [get_property value ${PARAM_VALUE.ENABLE_DECODER}]
	 set DEC_FRAME_SIZE [get_property value ${PARAM_VALUE.DEC_FRAME_SIZE}]
	 if {$ENABLE_DECODER == "false" || $ENABLE_DECODER == "FALSE" } {
        set_property enabled false ${PARAM_VALUE.DEC_FPS}
	 } else {
        set_property enabled true ${PARAM_VALUE.DEC_FPS}
	    # If frame is 8K, FPS == 15
	    if { $DEC_FRAME_SIZE == 6 } {
          set_property value 0 ${PARAM_VALUE.DEC_FPS}
          set_property enabled false ${PARAM_VALUE.DEC_FPS}
        }
	 }
}

proc update_PARAM_VALUE.DEC_CODING_TYPE { PARAM_VALUE.DEC_CODING_TYPE PARAM_VALUE.ENABLE_DECODER IPINST } {
   set ENABLE_DECODER [get_property value ${PARAM_VALUE.ENABLE_DECODER}]
   if {$ENABLE_DECODER == "false" || $ENABLE_DECODER == "FALSE" } {
        set_property enabled false ${PARAM_VALUE.DEC_CODING_TYPE}
   } else {
        set_property enabled true ${PARAM_VALUE.DEC_CODING_TYPE}
   }
}

proc update_PARAM_VALUE.DEC_COLOR_FORMAT { PARAM_VALUE.DEC_COLOR_FORMAT PARAM_VALUE.ENABLE_DECODER IPINST } {
   set ENABLE_DECODER [get_property value ${PARAM_VALUE.ENABLE_DECODER}]
   if {$ENABLE_DECODER == "false" || $ENABLE_DECODER == "FALSE" } {
        set_property enabled false ${PARAM_VALUE.DEC_COLOR_FORMAT}
   } else {
        set_property enabled true ${PARAM_VALUE.DEC_COLOR_FORMAT}
   }
}

proc update_PARAM_VALUE.DEC_COLOR_DEPTH { PARAM_VALUE.DEC_COLOR_DEPTH PARAM_VALUE.ENABLE_DECODER IPINST } {
   set ENABLE_DECODER [get_property value ${PARAM_VALUE.ENABLE_DECODER}]
   if {$ENABLE_DECODER == "false" || $ENABLE_DECODER == "FALSE" } {
        set_property enabled false ${PARAM_VALUE.DEC_COLOR_DEPTH}
   } else {
        set_property enabled true ${PARAM_VALUE.DEC_COLOR_DEPTH}
   }
}

proc update_PARAM_VALUE.ENC_BUFFER_MANUAL_OVERRIDE { PARAM_VALUE.ENC_BUFFER_MANUAL_OVERRIDE PARAM_VALUE.ENC_CODING_TYPE  PARAM_VALUE.ENABLE_ENCODER PARAM_VALUE.ENC_BUFFER_EN IPINST } {
   set ENABLE_ENCODER [get_property value ${PARAM_VALUE.ENABLE_ENCODER}]
   set ENC_CODING_TYPE [get_property value ${PARAM_VALUE.ENC_CODING_TYPE}]
   set ENC_BUFFER_EN [get_property value ${PARAM_VALUE.ENC_BUFFER_EN}]
   if {$ENABLE_ENCODER == "false" || $ENABLE_ENCODER == "FALSE" || $ENC_CODING_TYPE == 0  || $ENC_BUFFER_EN == "false" || $ENC_BUFFER_EN == "FALSE" } {
      set_property enabled false ${PARAM_VALUE.ENC_BUFFER_MANUAL_OVERRIDE}
   } else {
      set_property enabled true ${PARAM_VALUE.ENC_BUFFER_MANUAL_OVERRIDE}
   }
}

proc update_PARAM_VALUE.WPP_EN { PARAM_VALUE.ENC_BUFFER_MANUAL_OVERRIDE PARAM_VALUE.ENABLE_ENCODER PARAM_VALUE.ENC_CODING_TYPE   PARAM_VALUE.WPP_EN IPINST PARAM_VALUE.ENC_BUFFER_EN  } {
   set ENABLE_ENCODER [get_property value ${PARAM_VALUE.ENABLE_ENCODER}]
   set ENC_CODING_TYPE [get_property value ${PARAM_VALUE.ENC_CODING_TYPE}]
   set ENC_BUFFER_MANUAL_OVERRIDE [get_property value ${PARAM_VALUE.ENC_BUFFER_MANUAL_OVERRIDE}]
   set ENC_BUFFER_EN [get_property value ${PARAM_VALUE.ENC_BUFFER_EN}]
   if {$ENABLE_ENCODER == "false" || $ENABLE_ENCODER == "FALSE" || $ENC_BUFFER_MANUAL_OVERRIDE == 1  || $ENC_CODING_TYPE == 0  || $ENC_BUFFER_EN == "false" || $ENC_BUFFER_EN == "FALSE" } {
       set_property enabled false ${PARAM_VALUE.WPP_EN}
	} else {
       set_property enabled true ${PARAM_VALUE.WPP_EN}
	}
}

proc update_PARAM_VALUE.ENC_BUFFER_MOTION_VEC_RANGE { PARAM_VALUE.ENC_BUFFER_MANUAL_OVERRIDE PARAM_VALUE.ENABLE_ENCODER  PARAM_VALUE.ENC_CODING_TYPE   PARAM_VALUE.ENC_BUFFER_MOTION_VEC_RANGE IPINST PARAM_VALUE.ENC_BUFFER_EN  } {
   set ENABLE_ENCODER [get_property value ${PARAM_VALUE.ENABLE_ENCODER}]
   set ENC_BUFFER_MANUAL_OVERRIDE [get_property value ${PARAM_VALUE.ENC_BUFFER_MANUAL_OVERRIDE}]
   set ENC_CODING_TYPE [get_property value ${PARAM_VALUE.ENC_CODING_TYPE}]
   set ENC_BUFFER_EN [get_property value ${PARAM_VALUE.ENC_BUFFER_EN}]
   if {$ENABLE_ENCODER == "false" || $ENABLE_ENCODER == "FALSE" || $ENC_BUFFER_MANUAL_OVERRIDE == 1 || $ENC_CODING_TYPE == 0  || $ENC_BUFFER_EN == "false" || $ENC_BUFFER_EN == "FALSE" } {
       set_property enabled false ${PARAM_VALUE.ENC_BUFFER_MOTION_VEC_RANGE}
#Made conditional Buffer size enable for Encoder case ### Bharath
       set_property value 0 ${PARAM_VALUE.ENC_BUFFER_MOTION_VEC_RANGE}
	} else {
       set_property enabled true ${PARAM_VALUE.ENC_BUFFER_MOTION_VEC_RANGE}
	}
}

proc update_PARAM_VALUE.ENC_BUFFER_B_FRAME { PARAM_VALUE.ENC_BUFFER_MANUAL_OVERRIDE PARAM_VALUE.ENABLE_ENCODER PARAM_VALUE.ENC_CODING_TYPE   PARAM_VALUE.ENC_BUFFER_B_FRAME IPINST PARAM_VALUE.ENC_BUFFER_EN  } {
   set ENABLE_ENCODER [get_property value ${PARAM_VALUE.ENABLE_ENCODER}]
   set ENC_BUFFER_MANUAL_OVERRIDE [get_property value ${PARAM_VALUE.ENC_BUFFER_MANUAL_OVERRIDE}]
   set ENC_CODING_TYPE [get_property value ${PARAM_VALUE.ENC_CODING_TYPE}]
   set ENC_BUFFER_EN [get_property value ${PARAM_VALUE.ENC_BUFFER_EN}]
   if {$ENABLE_ENCODER == "false" || $ENABLE_ENCODER == "FALSE" || $ENC_BUFFER_MANUAL_OVERRIDE == 1 || $ENC_CODING_TYPE == 0  || $ENC_BUFFER_EN == "false" || $ENC_BUFFER_EN == "FALSE" } {
       set_property enabled false ${PARAM_VALUE.ENC_BUFFER_B_FRAME}
#Made conditional Buffer size enable for Encoder case ### Bharath
       set_property value 0 ${PARAM_VALUE.ENC_BUFFER_B_FRAME}
	} else {
       set_property enabled true ${PARAM_VALUE.ENC_BUFFER_B_FRAME}
	}
}

proc validate_PARAM_VALUE.ENABLE_DECODER { PARAM_VALUE.ENABLE_DECODER } {
	# Procedure called to validate ENABLE_DECODER
	return true
}

proc validate_PARAM_VALUE.ENABLE_ENCODER { PARAM_VALUE.ENABLE_ENCODER } {
	# Procedure called to validate ENABLE_ENCODER
	return true
}

proc update_PARAM_VALUE.TABLE_NO { PARAM_VALUE.ENC_BUFFER_EN  PARAM_VALUE.TABLE_NO PARAM_VALUE.ENC_COLOR_DEPTH PARAM_VALUE.ENABLE_ENCODER PARAM_VALUE.ENC_BUFFER_TYPE } {
	set COLOR_DEPTH [get_property value ${PARAM_VALUE.ENC_COLOR_DEPTH}]
	set ENABLE_ENCODER [get_property value ${PARAM_VALUE.ENABLE_ENCODER}]
	set ENC_BUFFER_EN [get_property value ${PARAM_VALUE.ENC_BUFFER_EN}]
    set ENC_BUFFER_TYPE [get_property value ${PARAM_VALUE.ENC_BUFFER_TYPE}] 
    if { ($ENC_BUFFER_EN == "TRUE" || $ENC_BUFFER_EN == "true") && ($ENABLE_ENCODER == "true" || $ENABLE_ENCODER == "TRUE") } {
      if {$COLOR_DEPTH == 0 && $ENC_BUFFER_TYPE != 2 } {
         # this is 8-bpp ,URAM only or BRAM only
 	     set_property value 2 ${PARAM_VALUE.TABLE_NO}
 	  } elseif {$COLOR_DEPTH == 1 && $ENC_BUFFER_TYPE != 2 } {
         # this is 10-bpp, URAM only or BRAM only
 	     set_property value 1 ${PARAM_VALUE.TABLE_NO}
 	  } elseif {$COLOR_DEPTH == 1 && $ENC_BUFFER_TYPE == 2 } {
         # this is 10-bpp, combo
 	     set_property value 3 ${PARAM_VALUE.TABLE_NO}
 	  } elseif {$COLOR_DEPTH == 0 && $ENC_BUFFER_TYPE == 2 } {
         # this is 8-bpp, combo
 	     set_property value 4 ${PARAM_VALUE.TABLE_NO}
 	  } else {
         #No Memory -> Table 0
 	     set_property value 0 ${PARAM_VALUE.TABLE_NO}
 	  }
	} else {
      #No Memory -> Table 0
 	  set_property value 0 ${PARAM_VALUE.TABLE_NO}
	}
}

proc calc_fps { FPS } {
	 if { $FPS == 0 } {
        set A_FPS 15
	 } elseif { $FPS == 1} {
        set A_FPS 30
	 } elseif { $FPS == 2} {
        set A_FPS 45
	 } else {
        set A_FPS 60
	 }
    return $A_FPS
}

# Number of Cores for single stream
proc calc_num_cores {  FRAME_SIZE FPS CLOCK  }  {
     set pixel_width   [calc_frame_size_x $FRAME_SIZE]
     set pixel_height  [calc_frame_size_y $FRAME_SIZE]  

     set width_LCU [expr ($pixel_width + 31) / 32]
     set height_LCU [expr ($pixel_height + 31) / 32]

     set A_FPS [calc_fps $FPS] 

     set num_core [expr ($width_LCU * $height_LCU *$A_FPS * 666 + 122400 * $CLOCK -1) / (122400 * $CLOCK)]
	 if { $num_core < [expr ($pixel_width + 4095) / 4096] } {
        set num_core [expr ($pixel_width + 4095) / 4096]
	 }
	 if { $num_core > 4 } {
        set num_core 4
	 }
     return $num_core
}

proc calc_mem_depth { FRAME_SIZE RANGE STANDARD CORE } {
     set width   [calc_frame_size_x $FRAME_SIZE]
     set height  [calc_frame_size_y $FRAME_SIZE]  

	 if { $STANDARD == 0 && $width == 1920 && $height == 1080 && $CORE == 1 && $RANGE == 0 } { set minSize 2432 }
     if { $STANDARD == 0 && $width == 1920 && $height == 1080 && $CORE == 1 && $RANGE == 1 } { set minSize 4480 }
     if { $STANDARD == 0 && $width == 1920 && $height == 1080 && $CORE == 1 && $RANGE == 2 } { set minSize 8448 }

	 if { $STANDARD == 0 && $width == 1920 && $height == 1080 && $CORE == 2 && $RANGE == 0 } { set minSize 3456 }
     if { $STANDARD == 0 && $width == 1920 && $height == 1080 && $CORE == 2 && $RANGE == 1 } { set minSize 6272 }
     if { $STANDARD == 0 && $width == 1920 && $height == 1080 && $CORE == 2 && $RANGE == 2 } { set minSize 11776 }

	 if { $STANDARD == 0 && $width == 1920 && $height == 1080 && $CORE == 3 && $RANGE == 0 } { set minSize 4480 }
     if { $STANDARD == 0 && $width == 1920 && $height == 1080 && $CORE == 3 && $RANGE == 1 } { set minSize 8064 }
     if { $STANDARD == 0 && $width == 1920 && $height == 1080 && $CORE == 3 && $RANGE == 2 } { set minSize 15104 }

	 if { $STANDARD == 0 && $width == 1920 && $height == 1080 && $CORE == 4 && $RANGE == 0 } { set minSize 4480 }
     if { $STANDARD == 0 && $width == 1920 && $height == 1080 && $CORE == 4 && $RANGE == 1 } { set minSize 10112 }
     if { $STANDARD == 0 && $width == 1920 && $height == 1080 && $CORE == 4 && $RANGE == 2 } { set minSize 18944 }

	 if { $STANDARD == 1 && $width == 1920 && $height == 1080 && $CORE == 1 && $RANGE == 0 } { set minSize 2224 }
     if { $STANDARD == 1 && $width == 1920 && $height == 1080 && $CORE == 1 && $RANGE == 1 } { set minSize 4208 }
     if { $STANDARD == 1 && $width == 1920 && $height == 1080 && $CORE == 1 && $RANGE == 2 } { set minSize 8112 }

	 if { $STANDARD == 1 && $width == 1920 && $height == 1080 && $CORE == 2 && $RANGE == 0 } { set minSize 2912 }
     if { $STANDARD == 1 && $width == 1920 && $height == 1080 && $CORE == 2 && $RANGE == 1 } { set minSize 5536 }
     if { $STANDARD == 1 && $width == 1920 && $height == 1080 && $CORE == 2 && $RANGE == 2 } { set minSize 10848 }

	 if { $STANDARD == 1 && $width == 1920 && $height == 1080 && $CORE == 3 && $RANGE == 0 } { set minSize 3600 }
     if { $STANDARD == 1 && $width == 1920 && $height == 1080 && $CORE == 3 && $RANGE == 1 } { set minSize 6864 }
     if { $STANDARD == 1 && $width == 1920 && $height == 1080 && $CORE == 3 && $RANGE == 2 } { set minSize 13584 }

	 if { $STANDARD == 1 && $width == 1920 && $height == 1080 && $CORE == 4 && $RANGE == 0 } { set minSize 4416 }
     if { $STANDARD == 1 && $width == 1920 && $height == 1080 && $CORE == 4 && $RANGE == 1 } { set minSize 8448 }
     if { $STANDARD == 1 && $width == 1920 && $height == 1080 && $CORE == 4 && $RANGE == 2 } { set minSize 16832 }

	 ## Second set
	 if { $STANDARD == 0 && $width == 1280 && $height == 720 && $CORE == 1 && $RANGE == 0 } { set minSize 1792 }
     if { $STANDARD == 0 && $width == 1280 && $height == 720 && $CORE == 1 && $RANGE == 1 } { set minSize 3200 }
     if { $STANDARD == 0 && $width == 1280 && $height == 720 && $CORE == 1 && $RANGE == 2 } { set minSize 5888 }

	 if { $STANDARD == 0 && $width == 1280 && $height == 720 && $CORE == 2 && $RANGE == 0 } { set minSize 2816 }
     if { $STANDARD == 0 && $width == 1280 && $height == 720 && $CORE == 2 && $RANGE == 1 } { set minSize 4992 }
     if { $STANDARD == 0 && $width == 1280 && $height == 720 && $CORE == 2 && $RANGE == 2 } { set minSize 9216 }

	 if { $STANDARD == 0 && $width == 1280 && $height == 720 && $CORE == 3 && $RANGE == 0 } { set minSize 3904 }
     if { $STANDARD == 0 && $width == 1280 && $height == 720 && $CORE == 3 && $RANGE == 1 } { set minSize 6912 }
     if { $STANDARD == 0 && $width == 1280 && $height == 720 && $CORE == 3 && $RANGE == 2 } { set minSize 12800 }

	 if { $STANDARD == 0 && $width == 1280 && $height == 720 && $CORE == 4 && $RANGE == 0 } { set minSize 4864 }
     if { $STANDARD == 0 && $width == 1280 && $height == 720 && $CORE == 4 && $RANGE == 1 } { set minSize 8576 }
     if { $STANDARD == 0 && $width == 1280 && $height == 720 && $CORE == 4 && $RANGE == 2 } { set minSize 15872 }

	 if { $STANDARD == 1 && $width == 1280 && $height == 720 && $CORE == 1 && $RANGE == 0 } { set minSize 1584 }
     if { $STANDARD == 1 && $width == 1280 && $height == 720 && $CORE == 1 && $RANGE == 1 } { set minSize 2918 }
     if { $STANDARD == 1 && $width == 1280 && $height == 720 && $CORE == 1 && $RANGE == 2 } { set minSize 5552 }

	 if { $STANDARD == 1 && $width == 1280 && $height == 720 && $CORE == 2 && $RANGE == 0 } { set minSize 2272 }
     if { $STANDARD == 1 && $width == 1280 && $height == 720 && $CORE == 2 && $RANGE == 1 } { set minSize 4256 }
     if { $STANDARD == 1 && $width == 1280 && $height == 720 && $CORE == 2 && $RANGE == 2 } { set minSize 8288 }

	 if { $STANDARD == 1 && $width == 1280 && $height == 720 && $CORE == 3 && $RANGE == 0 } { set minSize 3024 }
     if { $STANDARD == 1 && $width == 1280 && $height == 720 && $CORE == 3 && $RANGE == 1 } { set minSize 5712 }
     if { $STANDARD == 1 && $width == 1280 && $height == 720 && $CORE == 3 && $RANGE == 2 } { set minSize 11280 }

	 if { $STANDARD == 1 && $width == 1280 && $height == 720 && $CORE == 4 && $RANGE == 0 } { set minSize 3648 }
     if { $STANDARD == 1 && $width == 1280 && $height == 720 && $CORE == 4 && $RANGE == 1 } { set minSize 6912 }
     if { $STANDARD == 1 && $width == 1280 && $height == 720 && $CORE == 4 && $RANGE == 2 } { set minSize 13760 }

	 ## Third set
	 if { $STANDARD == 0 && $width == 3840 && $height == 2160 && $CORE == 1 && $RANGE == 0 } { set minSize 4352 }
     if { $STANDARD == 0 && $width == 3840 && $height == 2160 && $CORE == 1 && $RANGE == 1 } { set minSize 8320 }
     if { $STANDARD == 0 && $width == 3840 && $height == 2160 && $CORE == 1 && $RANGE == 2 } { set minSize 16128 }

	 if { $STANDARD == 0 && $width == 3840 && $height == 2160 && $CORE == 2 && $RANGE == 0 } { set minSize 5376 }
     if { $STANDARD == 0 && $width == 3840 && $height == 2160 && $CORE == 2 && $RANGE == 1 } { set minSize 10112 }
     if { $STANDARD == 0 && $width == 3840 && $height == 2160 && $CORE == 2 && $RANGE == 2 } { set minSize 19456 }

	 if { $STANDARD == 0 && $width == 3840 && $height == 2160 && $CORE == 3 && $RANGE == 0 } { set minSize 6400 }
     if { $STANDARD == 0 && $width == 3840 && $height == 2160 && $CORE == 3 && $RANGE == 1 } { set minSize 11904 }
     if { $STANDARD == 0 && $width == 3840 && $height == 2160 && $CORE == 3 && $RANGE == 2 } { set minSize 22784 }

	 if { $STANDARD == 0 && $width == 3840 && $height == 2160 && $CORE == 4 && $RANGE == 0 } { set minSize 7424 }
     if { $STANDARD == 0 && $width == 3840 && $height == 2160 && $CORE == 4 && $RANGE == 1 } { set minSize 13696 }
     if { $STANDARD == 0 && $width == 3840 && $height == 2160 && $CORE == 4 && $RANGE == 2 } { set minSize 26112 }

	 if { $STANDARD == 1 && $width == 3840 && $height == 2160 && $CORE == 1 && $RANGE == 0 } { set minSize 4144 }
     if { $STANDARD == 1 && $width == 3840 && $height == 2160 && $CORE == 1 && $RANGE == 1 } { set minSize 8048 }
     if { $STANDARD == 1 && $width == 3840 && $height == 2160 && $CORE == 1 && $RANGE == 2 } { set minSize 15792 }

	 if { $STANDARD == 1 && $width == 3840 && $height == 2160 && $CORE == 2 && $RANGE == 0 } { set minSize 4832 }
     if { $STANDARD == 1 && $width == 3840 && $height == 2160 && $CORE == 2 && $RANGE == 1 } { set minSize 9376 }
     if { $STANDARD == 1 && $width == 3840 && $height == 2160 && $CORE == 2 && $RANGE == 2 } { set minSize 18528 }

	 if { $STANDARD == 1 && $width == 3840 && $height == 2160 && $CORE == 3 && $RANGE == 0 } { set minSize 5520 }
     if { $STANDARD == 1 && $width == 3840 && $height == 2160 && $CORE == 3 && $RANGE == 1 } { set minSize 10704 }
     if { $STANDARD == 1 && $width == 3840 && $height == 2160 && $CORE == 3 && $RANGE == 2 } { set minSize 21264 }

	 if { $STANDARD == 1 && $width == 3840 && $height == 2160 && $CORE == 4 && $RANGE == 0 } { set minSize 6208 }
     if { $STANDARD == 1 && $width == 3840 && $height == 2160 && $CORE == 4 && $RANGE == 1 } { set minSize 12032 }
     if { $STANDARD == 1 && $width == 3840 && $height == 2160 && $CORE == 4 && $RANGE == 2 } { set minSize 24000 }

	 ## Fourth set
	 if { $STANDARD == 0 && $width == 4096 && $height == 2160 && $CORE == 1 && $RANGE == 0 } { set minSize 4608 }
     if { $STANDARD == 0 && $width == 4096 && $height == 2160 && $CORE == 1 && $RANGE == 1 } { set minSize 8832 }
     if { $STANDARD == 0 && $width == 4096 && $height == 2160 && $CORE == 1 && $RANGE == 2 } { set minSize 17152 }

	 if { $STANDARD == 0 && $width == 4096 && $height == 2160 && $CORE == 2 && $RANGE == 0 } { set minSize 5632 }
     if { $STANDARD == 0 && $width == 4096 && $height == 2160 && $CORE == 2 && $RANGE == 1 } { set minSize 10624 }
     if { $STANDARD == 0 && $width == 4096 && $height == 2160 && $CORE == 2 && $RANGE == 2 } { set minSize 20480 }

	 if { $STANDARD == 0 && $width == 4096 && $height == 2160 && $CORE == 3 && $RANGE == 0 } { set minSize 6784 }
     if { $STANDARD == 0 && $width == 4096 && $height == 2160 && $CORE == 3 && $RANGE == 1 } { set minSize 12672 }
     if { $STANDARD == 0 && $width == 4096 && $height == 2160 && $CORE == 3 && $RANGE == 2 } { set minSize 24320 }

	 if { $STANDARD == 0 && $width == 4096 && $height == 2160 && $CORE == 4 && $RANGE == 0 } { set minSize 7680 }
     if { $STANDARD == 0 && $width == 4096 && $height == 2160 && $CORE == 4 && $RANGE == 1 } { set minSize 14208 }
     if { $STANDARD == 0 && $width == 4096 && $height == 2160 && $CORE == 4 && $RANGE == 2 } { set minSize 27136 }

	 if { $STANDARD == 1 && $width == 4096 && $height == 2160 && $CORE == 1 && $RANGE == 0 } { set minSize 4400 }
     if { $STANDARD == 1 && $width == 4096 && $height == 2160 && $CORE == 1 && $RANGE == 1 } { set minSize 8560 }
     if { $STANDARD == 1 && $width == 4096 && $height == 2160 && $CORE == 1 && $RANGE == 2 } { set minSize 16816 }

	 if { $STANDARD == 1 && $width == 4096 && $height == 2160 && $CORE == 2 && $RANGE == 0 } { set minSize 5088 }
     if { $STANDARD == 1 && $width == 4096 && $height == 2160 && $CORE == 2 && $RANGE == 1 } { set minSize 9888 }
     if { $STANDARD == 1 && $width == 4096 && $height == 2160 && $CORE == 2 && $RANGE == 2 } { set minSize 19552 }

	 if { $STANDARD == 1 && $width == 4096 && $height == 2160 && $CORE == 3 && $RANGE == 0 } { set minSize 5904 }
     if { $STANDARD == 1 && $width == 4096 && $height == 2160 && $CORE == 3 && $RANGE == 1 } { set minSize 11472 }
     if { $STANDARD == 1 && $width == 4096 && $height == 2160 && $CORE == 3 && $RANGE == 2 } { set minSize 22800 }

	 if { $STANDARD == 1 && $width == 4096 && $height == 2160 && $CORE == 4 && $RANGE == 0 } { set minSize 6464 }
     if { $STANDARD == 1 && $width == 4096 && $height == 2160 && $CORE == 4 && $RANGE == 1 } { set minSize 12544 }
     if { $STANDARD == 1 && $width == 4096 && $height == 2160 && $CORE == 4 && $RANGE == 2 } { set minSize 25024 }

	 ## Fifth set
	 if { $STANDARD == 0 && $width == 7680 && $height == 4320 && $CORE == 1 && $RANGE == 0 } { set minSize 8192 }
     if { $STANDARD == 0 && $width == 7680 && $height == 4320 && $CORE == 1 && $RANGE == 1 } { set minSize 16000 }
     if { $STANDARD == 0 && $width == 7680 && $height == 4320 && $CORE == 1 && $RANGE == 2 } { set minSize 31488 }

	 if { $STANDARD == 0 && $width == 7680 && $height == 4320 && $CORE == 2 && $RANGE == 0 } { set minSize 9216 }
     if { $STANDARD == 0 && $width == 7680 && $height == 4320 && $CORE == 2 && $RANGE == 1 } { set minSize 17792 }
     if { $STANDARD == 0 && $width == 7680 && $height == 4320 && $CORE == 2 && $RANGE == 2 } { set minSize 34816 }

	 if { $STANDARD == 0 && $width == 7680 && $height == 4320 && $CORE == 3 && $RANGE == 0 } { set minSize 10240 }
     if { $STANDARD == 0 && $width == 7680 && $height == 4320 && $CORE == 3 && $RANGE == 1 } { set minSize 19584 }
     if { $STANDARD == 0 && $width == 7680 && $height == 4320 && $CORE == 3 && $RANGE == 2 } { set minSize 38144 }

	 if { $STANDARD == 0 && $width == 7680 && $height == 4320 && $CORE == 4 && $RANGE == 0 } { set minSize 11264 }
     if { $STANDARD == 0 && $width == 7680 && $height == 4320 && $CORE == 4 && $RANGE == 1 } { set minSize 21376 }
     if { $STANDARD == 0 && $width == 7680 && $height == 4320 && $CORE == 4 && $RANGE == 2 } { set minSize 41472 }

	 if { $STANDARD == 1 && $width == 7680 && $height == 4320 && $CORE == 1 && $RANGE == 0 } { set minSize 7984 }
     if { $STANDARD == 1 && $width == 7680 && $height == 4320 && $CORE == 1 && $RANGE == 1 } { set minSize 15728 }
     if { $STANDARD == 1 && $width == 7680 && $height == 4320 && $CORE == 1 && $RANGE == 2 } { set minSize 31152 }

	 if { $STANDARD == 1 && $width == 7680 && $height == 4320 && $CORE == 2 && $RANGE == 0 } { set minSize 8672 }
     if { $STANDARD == 1 && $width == 7680 && $height == 4320 && $CORE == 2 && $RANGE == 1 } { set minSize 17056 }
     if { $STANDARD == 1 && $width == 7680 && $height == 4320 && $CORE == 2 && $RANGE == 2 } { set minSize 33888 }

	 if { $STANDARD == 1 && $width == 7680 && $height == 4320 && $CORE == 3 && $RANGE == 0 } { set minSize 9360 }
     if { $STANDARD == 1 && $width == 7680 && $height == 4320 && $CORE == 3 && $RANGE == 1 } { set minSize 18384 }
     if { $STANDARD == 1 && $width == 7680 && $height == 4320 && $CORE == 3 && $RANGE == 2 } { set minSize 36624 }

	 if { $STANDARD == 1 && $width == 7680 && $height == 4320 && $CORE == 4 && $RANGE == 0 } { set minSize 10048 }
     if { $STANDARD == 1 && $width == 7680 && $height == 4320 && $CORE == 4 && $RANGE == 1 } { set minSize 19712 }
     if { $STANDARD == 1 && $width == 7680 && $height == 4320 && $CORE == 4 && $RANGE == 2 } { set minSize 39360 }
     
	 return $minSize
}


# kamalesh
proc validate_PARAM_VALUE.NO_OF_DEC_STREAMS { PARAM_VALUE.DEC_FRAME_SIZE PARAM_VALUE.DEC_FPS  PARAM_VALUE.NO_OF_DEC_STREAMS PARAM_VALUE.CORE_CLK } {

	 set DEC_FRAME_SIZE [get_property value ${PARAM_VALUE.DEC_FRAME_SIZE}]
	 set DEC_FPS [get_property value ${PARAM_VALUE.DEC_FPS}]
     set A_FPS [calc_fps $DEC_FPS] 
	 set NO_OF_DEC_STREAMS [get_property value ${PARAM_VALUE.NO_OF_DEC_STREAMS}]
	 set CORE_CLK [get_property value ${PARAM_VALUE.CORE_CLK}]

     set X_VALUE [calc_frame_size_x $DEC_FRAME_SIZE]
     set Y_VALUE [calc_frame_size_y $DEC_FRAME_SIZE]
    # old eqn 10dec
    #set num_cores_req [ expr ($X_VALUE*$Y_VALUE*$A_FPS*($NO_OF_DEC_STREAMS + 0)/(3840*2160*30))]
    # old eqn 10dec modified
    #set num_cores_req [ expr ($X_VALUE*$Y_VALUE*$A_FPS*($NO_OF_DEC_STREAMS + 0)/(3840*2160*30.0))]
    # added new eqn on 10dec given by fredric for floating point issue
	 set num_cores_req [ expr ($X_VALUE*$Y_VALUE*$A_FPS*($NO_OF_DEC_STREAMS + 0)*666 / (3840*2160*30.0*$CORE_CLK))]
	 
	 #send_msg INFO 306 "num_cores_req $num_cores_req X_VALUE $X_VALUE Y_VALUE $Y_VALUE DEC_FPS $A_FPS NO_OF_DEC_STREAMS $NO_OF_DEC_STREAMS "

	 if { $num_cores_req > 2 } {
       set_property errmsg "This configuration exceeds the VCU capabilities !"  ${PARAM_VALUE.NO_OF_DEC_STREAMS}
	   return false
	 } else {
	   return true
	 }
}

# kamalesh
proc validate_PARAM_VALUE.NO_OF_STREAMS { PARAM_VALUE.ENC_FRAME_SIZE PARAM_VALUE.ENC_FPS  PARAM_VALUE.NO_OF_STREAMS PARAM_VALUE.CORE_CLK } {

	 set ENC_FRAME_SIZE [get_property value ${PARAM_VALUE.ENC_FRAME_SIZE}]
	 set ENC_FPS [get_property value ${PARAM_VALUE.ENC_FPS}]
     set A_FPS [calc_fps $ENC_FPS] 
	 set NO_OF_STREAMS [get_property value ${PARAM_VALUE.NO_OF_STREAMS}]
	 set CORE_CLK [get_property value ${PARAM_VALUE.CORE_CLK}]

     set X_VALUE [calc_frame_size_x $ENC_FRAME_SIZE]
     set Y_VALUE [calc_frame_size_y $ENC_FRAME_SIZE]

    #set num_cores_req [ expr ($X_VALUE*$Y_VALUE*$A_FPS*($NO_OF_STREAMS + 1)/(1920*1088*60))]
    # added new eqn pruthvi (CR-1018128):
	#set num_cores_req [ expr ($X_VALUE*$Y_VALUE*$A_FPS*($NO_OF_STREAMS + 0)*666 + (1920*1088*60*$CORE_CLK) - 1) / (1920*1088*60*$CORE_CLK)]
    # old eqn 10dec modified
	#set num_cores_req [ expr ($X_VALUE*$Y_VALUE*$A_FPS*($NO_OF_STREAMS + 0)*666 + (1920*1088*60.0*$CORE_CLK) - 1) / (1920*1088*60.0*$CORE_CLK)]
   # added new eqn on 10dec given by fredric for floating point issue
	 set num_cores_req [ expr ($X_VALUE*$Y_VALUE*$A_FPS*($NO_OF_STREAMS + 0)*666 / (1920*1088*60.0*$CORE_CLK))]
    
	#send_msg INFO 306 "num_cores_req $num_cores_req X_VALUE $X_VALUE Y_VALUE $Y_VALUE ENC_FPS $A_FPS NO_OF_STREAMS $NO_OF_STREAMS CORE_CLK $CORE_CLK "

	 if { $num_cores_req > 4 } {
       set_property errmsg "This configuration exceeds the VCU capabilities !"  ${PARAM_VALUE.NO_OF_STREAMS}
	   return false
	 } else {
	   return true
	 }
}

proc validate_PARAM_VALUE.ENC_BUFFER_SIZE { PARAM_VALUE.ENC_BUFFER_SIZE PARAM_VALUE.ENC_BUFFER_TYPE  PARAM_VALUE.ENC_COLOR_DEPTH } {

	set ENC_BUFFER_SIZE [get_property value ${PARAM_VALUE.ENC_BUFFER_SIZE}]
	set ENC_BUFFER_TYPE [get_property value ${PARAM_VALUE.ENC_BUFFER_TYPE}]
	set ENC_COLOR_DEPTH [get_property value ${PARAM_VALUE.ENC_COLOR_DEPTH}]

    # URAM
	if {$ENC_BUFFER_TYPE == 0 } {
       set URAM_COUNT [xit::get_device_data D_URAM_COUNT -of [xit::current_scope]]
       set MEMORY_SIZE_MAX [expr $URAM_COUNT* 288 * 1 / 8 ]
       #send_msg INFO 306 "URAM_COUNT $URAM_COUNT  MEMORY_SIZE_MAX is $MEMORY_SIZE_MAX in KBytes "
	}
	#BRAM
	if {$ENC_BUFFER_TYPE == 1 } {
       set version1 [version -short]
       #send_msg INFO 306 " Version no $version1"
       set list1 [split $version1 .]
       set year [lindex $list1 0] 
       set quarter [lindex $list1 1] 
       set BRAM_COUNT 1880
       if {$year <= 2015 } {
          set BRAM_COUNT 1880
          #send_msg INFO 306 " Version no1 $version1"
       } elseif {$year == 2016 && $quarter < 3 } {
          set BRAM_COUNT 1880
          #send_msg INFO 306 " Version no2 $version1"
       } elseif {$year == 2016 && $quarter >= 3} {
          set BRAM_COUNT [xit::get_device_data D_BRAM_COUNT -of [xit::current_scope]]
          #send_msg INFO 306 " Version no3 $version1"
       } else {
          set BRAM_COUNT [xit::get_device_data D_BRAM_COUNT -of [xit::current_scope]]
          #send_msg INFO 306 " Version no4 $version1"
       }
       set MEMORY_SIZE_MAX [expr $BRAM_COUNT* 36 * 1 /8 ]
       #send_msg INFO 306 "BRAM_COUNT $BRAM_COUNT  MEMORY_SIZE_MAX is $MEMORY_SIZE_MAX in KBytes "
	}
	#COMBO 
	if {$ENC_BUFFER_TYPE == 2} {
       set URAM_COUNT [xit::get_device_data D_URAM_COUNT -of [xit::current_scope]]
       set MEMORY_SIZE_MAX_U [expr $URAM_COUNT* 288 * 1 / 8 ]
       set BRAM_COUNT [xit::get_device_data D_BRAM_COUNT -of [xit::current_scope]]
       set MEMORY_SIZE_MAX_B [expr $BRAM_COUNT* 36 * 1 /8 ]
	   set MEMORY_SIZE_MAX [expr $MEMORY_SIZE_MAX_U + $MEMORY_SIZE_MAX_B]
	}

	set ratio [expr $ENC_BUFFER_SIZE * 100  / $MEMORY_SIZE_MAX ]
	if { $ratio <= 65 } {
	   return true
	} elseif { $ratio > 65 && $ratio < 100   } {
       #send_msg CRITICAL_WARNING 1 "Memory usage is $ratio. Can cause timing issues"
	   return true
	} else {
       set_property errmsg "Memory requested exceeds capacity of device"  ${PARAM_VALUE.ENC_BUFFER_SIZE}
	   return false
	}
}


proc l2c_depth_calc_multiStream { FRAME_SIZE FPS CLOCK COLOR_FORMAT ENC_BUFFER_B_FRAME VERTICAL_RANGE VIDEO_STANDARD NO_OF_STREAMS } {

     set W  [calc_frame_size_x $FRAME_SIZE]
     # Number of Cores for Single Stream
	 set C [calc_num_cores $FRAME_SIZE $FPS $CLOCK ] 
     set T [ expr (((($W + 63)/64)+ $C -1)/$C)*64]  
	 if { $VIDEO_STANDARD == 0 } { set  B 32  }
	 if { $VIDEO_STANDARD == 1 } { set  B 16  }
	 # Horizontal Motion vec Range
	 if { $VERTICAL_RANGE == 0 } { 
	        set V 16 
	        set H 128
	 }
	 if { $VERTICAL_RANGE == 1 } { 
	        set V 32 
	        set H 192 
	 }
	 if { $VERTICAL_RANGE == 2 } { 
	        set V 64 
	        set H 256 
	 }
	 #set V $VERTICAL_RANGE
	 if { $COLOR_FORMAT == 0 } {  set Ch 1.5 }
	 if { $COLOR_FORMAT == 1 } {  set Ch 2  }
	 if { $COLOR_FORMAT == 2 } {  set Ch 1  }
	 set ST [expr ($C*(2*$V*$T + $B*(2*($H+$B)))+(2*$V+$B)*2*$H*($C-1) + $C*8*64*8 + $C*64*$B)*$Ch ]
     
	 if { $C > 1 } { set C_Adj 4 } else { set C_Adj 0 }
     #send_msg INFO 201 " C $C , T $T, Ch $Ch, V $V, B $B, C_Adj $C_Adj, H $H"

	 set sectionA [ expr ((2*$C_Adj + 1)*$B+2*$H + 63)/64 ]
	 set sectionB [ expr (($W + 63)/64)]
	 set sectionC [ expr (($C_Adj*$C*$B + 63)/64)]

	 set SW [ expr ((($C*$B*$sectionA*64) + 2*($V*$sectionB*64-$sectionC*64) + $C*8*64*8 + $C*64*$B)*$Ch)]
     #send_msg INFO 201 " SW $SW "
     #send_msg INFO 201 " ST $ST "

	 #($C*$B*ROUNDUP(((2*  $C_Adj  )*$B+2*$H)/64,0)*64 + 2*($V*ROUNDUP($W/64,0)*64-ROUNDUP(  $C_Adj  *$C*$B/64,0)*64) + $C*8*64*8 + $C*64*$B)*$Ch
	 #(B33*B34*ROUNDUP(((2*(IF(B33>1,4,0)+1))*B34+2*B35)/64,0)*64 + 2*(B36*ROUNDUP(B31/64,0)*64-ROUNDUP(IF(B33>1,4,0)*B33*B34/64,0)*64) + B33*8*64*8 + B33*64*B34)*B37

	 set lineDepth [ expr (($ST + 31)/32)]
     if { $ENC_BUFFER_B_FRAME == 0 } { set L2C_Depth $lineDepth }
     if { $ENC_BUFFER_B_FRAME == 1 } { set L2C_Depth [expr $lineDepth * 2] }
     if { $ENC_BUFFER_B_FRAME == 2 } { set L2C_Depth [expr $lineDepth * 2] }
     #send_msg INFO 201 " L2C_Depth $L2C_Depth "

	 set num_concurrent_streams [ expr ((4)/$C) ]
	 set NO_OF_STREAMS [ expr ($NO_OF_STREAMS + 0)]

     #send_msg INFO 201 " NO_OF_STREAMS $NO_OF_STREAMS, num_concurrent_streams $num_concurrent_streams "
	 if { $num_concurrent_streams  > $NO_OF_STREAMS } {
        set num_concurrent_streams $NO_OF_STREAMS
	 }
	 set L2C_Depth_multiStream [ expr ($L2C_Depth * $num_concurrent_streams) ]
     #send_msg INFO 201 " L2C_Depth_multiStream $L2C_Depth_multiStream "
     return $L2C_Depth_multiStream

}

# not used now..
proc l2c_depth_calc { FRAME_SIZE FPS CLOCK COLOR_FORMAT ENC_BUFFER_B_FRAME VERTICAL_RANGE VIDEO_STANDARD  NO_OF_STREAMS } {

	set NO_OF_CORES [calc_num_cores $FRAME_SIZE $FPS $CLOCK ] 
    #send_msg INFO 201 " NO_OF_CORES $NO_OF_CORES "

	set min_L2C_Depth [calc_mem_depth $FRAME_SIZE $VERTICAL_RANGE $VIDEO_STANDARD $NO_OF_CORES]
    #send_msg INFO 201 " min_L2C_Depth $min_L2C_Depth "
	set chroma_L2C_Depth $min_L2C_Depth
	if { $COLOR_FORMAT == 0 } {  set chroma_L2C_Depth [expr (($min_L2C_Depth * 3) + 1) /2 ] }
	if { $COLOR_FORMAT == 1 } {  set chroma_L2C_Depth [expr ($min_L2C_Depth * 2) ] }
	if { $COLOR_FORMAT == 2 } {  set chroma_L2C_Depth $min_L2C_Depth }
    #send_msg INFO 201 " chroma_L2C_Depth $chroma_L2C_Depth "

	set L2C_Depth $chroma_L2C_Depth
    if { $ENC_BUFFER_B_FRAME == 0 } { set L2C_Depth $chroma_L2C_Depth }
    if { $ENC_BUFFER_B_FRAME == 1 } { set L2C_Depth [expr $chroma_L2C_Depth * 2] }
    if { $ENC_BUFFER_B_FRAME == 2 } { set L2C_Depth [expr $chroma_L2C_Depth * 2] }
    #send_msg INFO 201 " L2C_Depth $L2C_Depth "
    return $L2C_Depth

}

proc update_PARAM_VALUE.ENC_BUFFER_SIZE_ACTUAL { PARAM_VALUE.NO_OF_STREAMS PARAM_VALUE.ENC_BUFFER_TYPE PARAM_VALUE.ENC_BUFFER_B_FRAME PARAM_VALUE.ENC_VIDEO_STANDARD PARAM_VALUE.CORE_CLK  PARAM_VALUE.ENC_FPS PARAM_VALUE.ENC_CODING_TYPE PARAM_VALUE.ENC_BUFFER_MANUAL_OVERRIDE PARAM_VALUE.ENABLE_ENCODER PARAM_VALUE.ENC_BUFFER_SIZE_ACTUAL PARAM_VALUE.ENC_COLOR_DEPTH PARAM_VALUE.ENC_BUFFER_MOTION_VEC_RANGE PARAM_VALUE.ENC_FRAME_SIZE PARAM_VALUE.ENC_COLOR_FORMAT PARAM_VALUE.ENC_BUFFER_EN } {
	# Procedure called to update ENC_BUFFER_SIZE_ACTUAL when any of the dependent parameters in the arguments change
    set ENABLE_ENCODER [get_property value ${PARAM_VALUE.ENABLE_ENCODER}]
    set ENC_BUFFER_MANUAL_OVERRIDE [get_property value ${PARAM_VALUE.ENC_BUFFER_MANUAL_OVERRIDE}]
    set ENC_CODING_TYPE [get_property value ${PARAM_VALUE.ENC_CODING_TYPE}]
    set ENC_BUFFER_EN [get_property value ${PARAM_VALUE.ENC_BUFFER_EN}]
    set FPS [get_property value ${PARAM_VALUE.ENC_FPS}]
    set CLOCK [get_property value ${PARAM_VALUE.CORE_CLK}]
    set ENC_BUFFER_TYPE [get_property value ${PARAM_VALUE.ENC_BUFFER_TYPE}]
    set NO_OF_STREAMS [get_property value ${PARAM_VALUE.NO_OF_STREAMS}]

	set COLOR_DEPTH [get_property value ${PARAM_VALUE.ENC_COLOR_DEPTH}]
	set VERTICAL_RANGE [get_property value ${PARAM_VALUE.ENC_BUFFER_MOTION_VEC_RANGE}]
	set FRAME_SIZE [get_property value ${PARAM_VALUE.ENC_FRAME_SIZE}]
	set COLOR_FORMAT [get_property value ${PARAM_VALUE.ENC_COLOR_FORMAT}]
	set VIDEO_STANDARD [get_property value ${PARAM_VALUE.ENC_VIDEO_STANDARD}]
	set ENC_BUFFER_B_FRAME [get_property value ${PARAM_VALUE.ENC_BUFFER_B_FRAME}]

	if { $COLOR_DEPTH == 0 && $ENC_BUFFER_TYPE == 0 } {
	   # URAM 8 bpc
       set MULTI_ACTUAL 288 
	} elseif  { $COLOR_DEPTH == 1 && $ENC_BUFFER_TYPE == 0 } {
	   # URAM 10 bpc
       set MULTI_ACTUAL 360 
	} elseif  { $COLOR_DEPTH == 0 && $ENC_BUFFER_TYPE == 1 } {
	   # BRAM 8 bpc. assuming 4kx9
       set MULTI_ACTUAL 261 
	} elseif  { $COLOR_DEPTH == 1 && $ENC_BUFFER_TYPE == 1 } {
	   # BRAM 10 bpc
       set MULTI_ACTUAL 324 
	} elseif  { $ENC_BUFFER_TYPE == 2 }  {
	   # COMBO. TODO
       set MULTI_ACTUAL 340
	}


	#set L2C_Depth [ l2c_depth_calc  $FRAME_SIZE $FPS $CLOCK $COLOR_FORMAT $ENC_BUFFER_B_FRAME $VERTICAL_RANGE $VIDEO_STANDARD $NO_OF_STREAMS ]
	set L2C_Depth_multiStream [ l2c_depth_calc_multiStream $FRAME_SIZE $FPS $CLOCK $COLOR_FORMAT $ENC_BUFFER_B_FRAME $VERTICAL_RANGE $VIDEO_STANDARD $NO_OF_STREAMS ]


    if { $ENABLE_ENCODER == "false" || $ENABLE_ENCODER == "FALSE" || $ENC_CODING_TYPE == 0 || $ENC_BUFFER_EN == "false" || $ENC_BUFFER_EN == "FALSE" } {
 	   set_property value 0 ${PARAM_VALUE.ENC_BUFFER_SIZE_ACTUAL}
	} else {
 	   set_property value [expr ((($L2C_Depth_multiStream * $MULTI_ACTUAL) + (8*1024) -1)/(8*1024))] ${PARAM_VALUE.ENC_BUFFER_SIZE_ACTUAL}
	}

}


proc update_PARAM_VALUE.ENC_BUFFER_SIZE { PARAM_VALUE.NO_OF_STREAMS PARAM_VALUE.ENC_BUFFER_TYPE PARAM_VALUE.ENC_BUFFER_B_FRAME PARAM_VALUE.ENC_VIDEO_STANDARD PARAM_VALUE.CORE_CLK  PARAM_VALUE.ENC_FPS PARAM_VALUE.ENC_CODING_TYPE PARAM_VALUE.ENC_BUFFER_MANUAL_OVERRIDE PARAM_VALUE.ENABLE_ENCODER PARAM_VALUE.ENC_BUFFER_SIZE PARAM_VALUE.ENC_COLOR_DEPTH PARAM_VALUE.ENC_BUFFER_MOTION_VEC_RANGE PARAM_VALUE.ENC_FRAME_SIZE PARAM_VALUE.ENC_COLOR_FORMAT PARAM_VALUE.ENC_BUFFER_EN } {
	# Procedure called to update ENC_BUFFER_SIZE when any of the dependent parameters in the arguments change
    set ENABLE_ENCODER [get_property value ${PARAM_VALUE.ENABLE_ENCODER}]
    set ENC_BUFFER_MANUAL_OVERRIDE [get_property value ${PARAM_VALUE.ENC_BUFFER_MANUAL_OVERRIDE}]
    set ENC_CODING_TYPE [get_property value ${PARAM_VALUE.ENC_CODING_TYPE}]
    set ENC_BUFFER_EN [get_property value ${PARAM_VALUE.ENC_BUFFER_EN}]
    set FPS [get_property value ${PARAM_VALUE.ENC_FPS}]
    set CLOCK [get_property value ${PARAM_VALUE.CORE_CLK}]
    set ENC_BUFFER_TYPE [get_property value ${PARAM_VALUE.ENC_BUFFER_TYPE}]
    set NO_OF_STREAMS [get_property value ${PARAM_VALUE.NO_OF_STREAMS}]

	set COLOR_DEPTH [get_property value ${PARAM_VALUE.ENC_COLOR_DEPTH}]
	set VERTICAL_RANGE [get_property value ${PARAM_VALUE.ENC_BUFFER_MOTION_VEC_RANGE}]
	set FRAME_SIZE [get_property value ${PARAM_VALUE.ENC_FRAME_SIZE}]
	set COLOR_FORMAT [get_property value ${PARAM_VALUE.ENC_COLOR_FORMAT}]
	set VIDEO_STANDARD [get_property value ${PARAM_VALUE.ENC_VIDEO_STANDARD}]
	set ENC_BUFFER_B_FRAME [get_property value ${PARAM_VALUE.ENC_BUFFER_B_FRAME}]

	if { $COLOR_DEPTH == 0 } {
       set MULT 256  
	} else {
       set MULT 320
	}

	#set L2C_Depth [ l2c_depth_calc  $FRAME_SIZE $FPS $CLOCK $COLOR_FORMAT $ENC_BUFFER_B_FRAME $VERTICAL_RANGE $VIDEO_STANDARD $NO_OF_STREAMS ]
	set L2C_Depth_multiStream [ l2c_depth_calc_multiStream $FRAME_SIZE $FPS $CLOCK $COLOR_FORMAT $ENC_BUFFER_B_FRAME $VERTICAL_RANGE $VIDEO_STANDARD $NO_OF_STREAMS ]

    if { $ENABLE_ENCODER == "false" || $ENABLE_ENCODER == "FALSE" || $ENC_CODING_TYPE == 0 || $ENC_BUFFER_EN == "false" || $ENC_BUFFER_EN == "FALSE" } {
       set_property enabled true ${PARAM_VALUE.ENC_BUFFER_SIZE}
 	   set_property value 0 ${PARAM_VALUE.ENC_BUFFER_SIZE}
	} else {
       set_property enabled true ${PARAM_VALUE.ENC_BUFFER_SIZE}
 	   set_property value [expr ((($L2C_Depth_multiStream * $MULT) + (8*1024) -1)/(8*1024))] ${PARAM_VALUE.ENC_BUFFER_SIZE}
	}

    if {$ENABLE_ENCODER == "false" || $ENABLE_ENCODER == "FALSE" || $ENC_BUFFER_MANUAL_OVERRIDE == 0 || $ENC_CODING_TYPE == 0 || $ENC_BUFFER_EN == "false" || $ENC_BUFFER_EN == "FALSE"  } {
	  #set_property value 0 ${PARAM_VALUE.ENC_BUFFER_SIZE}
      set_property enabled false ${PARAM_VALUE.ENC_BUFFER_SIZE}
    } else {
      set_property enabled true ${PARAM_VALUE.ENC_BUFFER_SIZE}
   }
}


proc update_MODELPARAM_VALUE.HDL_WPP_EN { PARAM_VALUE.WPP_EN MODELPARAM_VALUE.HDL_WPP_EN} {
    set_property value [get_property value ${PARAM_VALUE.WPP_EN}] ${MODELPARAM_VALUE.HDL_WPP_EN}
}


proc update_MODELPARAM_VALUE.HDL_ENC_BUFFER_MOTION_VEC_RANGE { PARAM_VALUE.ENC_BUFFER_MOTION_VEC_RANGE MODELPARAM_VALUE.HDL_ENC_BUFFER_MOTION_VEC_RANGE} {
    set_property value [get_propert value ${PARAM_VALUE.ENC_BUFFER_MOTION_VEC_RANGE}] ${MODELPARAM_VALUE.HDL_ENC_BUFFER_MOTION_VEC_RANGE}
}


proc update_MODELPARAM_VALUE.HDL_ENC_BUFFER_B_FRAME { PARAM_VALUE.ENC_BUFFER_B_FRAME MODELPARAM_VALUE.HDL_ENC_BUFFER_B_FRAME} {
    set_property value [get_property value ${PARAM_VALUE.ENC_BUFFER_B_FRAME}] ${MODELPARAM_VALUE.HDL_ENC_BUFFER_B_FRAME}
}

proc update_MODELPARAM_VALUE.HDL_ENC_BUFFER_MANUAL_OVERRIDE { PARAM_VALUE.ENC_BUFFER_MANUAL_OVERRIDE MODELPARAM_VALUE.HDL_ENC_BUFFER_MANUAL_OVERRIDE} {
	set  ENC_BUFFER_MANUAL_OVERRIDE [get_property value ${PARAM_VALUE.ENC_BUFFER_MANUAL_OVERRIDE}] 
	#send_msg INFO 2001 " ENC_BUFFER_MANUAL_OVERRIDE param here ..$ENC_BUFFER_MANUAL_OVERRIDE "
    set_property value $ENC_BUFFER_MANUAL_OVERRIDE ${MODELPARAM_VALUE.HDL_ENC_BUFFER_MANUAL_OVERRIDE}
	set  HDL_ENC_BUFFER_MANUAL_OVERRIDE [get_property value ${MODELPARAM_VALUE.HDL_ENC_BUFFER_MANUAL_OVERRIDE}] 
	#send_msg INFO 2001 " HDL_ENC_BUFFER_MANUAL_OVERRIDE param here ..$HDL_ENC_BUFFER_MANUAL_OVERRIDE "
}

proc update_MODELPARAM_VALUE.HDL_DEC_VIDEO_STANDARD { PARAM_VALUE.DEC_VIDEO_STANDARD MODELPARAM_VALUE.HDL_DEC_VIDEO_STANDARD} {
    set_property value [get_property value ${PARAM_VALUE.DEC_VIDEO_STANDARD}] ${MODELPARAM_VALUE.HDL_DEC_VIDEO_STANDARD}
}

# TBD.. fix the size correctly.. shld be derived
proc update_MODELPARAM_VALUE.HDL_DEC_FRAME_SIZE_Y { MODELPARAM_VALUE.HDL_DEC_FRAME_SIZE_Y PARAM_VALUE.DEC_FRAME_SIZE} {
    set FRAME_SIZE [get_property value ${PARAM_VALUE.DEC_FRAME_SIZE}]
	if { $FRAME_SIZE == 2 } {
      set Y_VALUE 720;
	} elseif { $FRAME_SIZE == 3 } {
      set Y_VALUE 1080;
	} elseif { $FRAME_SIZE == 4 } {
      set Y_VALUE 2160;
	} elseif { $FRAME_SIZE == 5 } {
      set Y_VALUE 2160;
	} elseif { $FRAME_SIZE == 6 } {
      set Y_VALUE 4320;
	} elseif {$FRAME_SIZE == 0} {
	  set Y_VALUE 480;
	} elseif {$FRAME_SIZE == 1} {
	  set Y_VALUE 576;
	}
    set_property value ${Y_VALUE} ${MODELPARAM_VALUE.HDL_DEC_FRAME_SIZE_Y}
}

# TBD.. fix the size correctly.. shld be derived
proc update_MODELPARAM_VALUE.HDL_DEC_FRAME_SIZE_X {MODELPARAM_VALUE.HDL_DEC_FRAME_SIZE_X PARAM_VALUE.DEC_FRAME_SIZE } {
    set FRAME_SIZE [get_property value ${PARAM_VALUE.DEC_FRAME_SIZE}]
	if { $FRAME_SIZE == 2 } {
      set X_VALUE 1280;
	} elseif { $FRAME_SIZE == 3 } {
      set X_VALUE 1920;
	} elseif { $FRAME_SIZE == 4 } {
      set X_VALUE 3840;
	} elseif { $FRAME_SIZE == 5 } {
      set X_VALUE 4096;
	} elseif { $FRAME_SIZE == 6 } {
      set X_VALUE 7680;
	} elseif {$FRAME_SIZE == 0} {
	  set X_VALUE 854;
	} elseif {$FRAME_SIZE == 1} {
	  set X_VALUE 1024;
	}
    set_property value ${X_VALUE} ${MODELPARAM_VALUE.HDL_DEC_FRAME_SIZE_X}
}

proc update_MODELPARAM_VALUE.HDL_DEC_FPS { PARAM_VALUE.DEC_FPS MODELPARAM_VALUE.HDL_DEC_FPS} {
    set FPS [get_property value ${PARAM_VALUE.DEC_FPS}]
	set HDL_FPS 60
    set HDL_FPS [calc_fps $FPS]
    set_property value ${HDL_FPS} ${MODELPARAM_VALUE.HDL_DEC_FPS}
}

proc update_MODELPARAM_VALUE.HDL_VIDEO_STANDARD { PARAM_VALUE.ENC_VIDEO_STANDARD MODELPARAM_VALUE.HDL_VIDEO_STANDARD} {
    set_property value [get_property value ${PARAM_VALUE.ENC_VIDEO_STANDARD}] ${MODELPARAM_VALUE.HDL_VIDEO_STANDARD}
}

proc update_MODELPARAM_VALUE.HDL_CODING_TYPE { PARAM_VALUE.ENC_CODING_TYPE MODELPARAM_VALUE.HDL_CODING_TYPE } {
    set_property value [get_property value ${PARAM_VALUE.ENC_CODING_TYPE}] ${MODELPARAM_VALUE.HDL_CODING_TYPE}
}

proc update_MODELPARAM_VALUE.HDL_DEC_CODING_TYPE { PARAM_VALUE.DEC_CODING_TYPE MODELPARAM_VALUE.HDL_DEC_CODING_TYPE } {
    set_property value [get_property value ${PARAM_VALUE.DEC_CODING_TYPE}] ${MODELPARAM_VALUE.HDL_DEC_CODING_TYPE}
}

proc update_MODELPARAM_VALUE.HDL_ENC_BUFFER_EN { PARAM_VALUE.ENC_BUFFER_EN MODELPARAM_VALUE.HDL_ENC_BUFFER_EN } {
	set ENC_BUFFER_EN [get_property value ${PARAM_VALUE.ENC_BUFFER_EN}]
    if { $ENC_BUFFER_EN == "TRUE" || $ENC_BUFFER_EN == "true" } {
      set_property value 1 ${MODELPARAM_VALUE.HDL_ENC_BUFFER_EN}
	} else {
      set_property value 0 ${MODELPARAM_VALUE.HDL_ENC_BUFFER_EN}
	}
}

proc update_MODELPARAM_VALUE.HDL_PLL_CLK_HI { PARAM_VALUE.PLL_CLK MODELPARAM_VALUE.HDL_PLL_CLK_HI} {
	set PLL_CLK [get_property value ${PARAM_VALUE.PLL_CLK}]
	# convert to 2 decimal places
	set PLL_CLK_FLOAT [expr {double(round(100*$PLL_CLK))/100}]
	set HDL_PLL_CLK_HI [expr {floor($PLL_CLK_FLOAT)}]
	set HDL_PLL_CLK_LO [expr (($PLL_CLK_FLOAT - $HDL_PLL_CLK_HI)*100)]
	#send_msg INFO 201 "HDL_PLL_CLK_HI here ..$HDL_PLL_CLK_HI"
    set_property value ${HDL_PLL_CLK_HI} ${MODELPARAM_VALUE.HDL_PLL_CLK_HI}
}

proc update_MODELPARAM_VALUE.HDL_PLL_CLK_LO { PARAM_VALUE.PLL_CLK MODELPARAM_VALUE.HDL_PLL_CLK_LO} {
	set PLL_CLK [get_property value ${PARAM_VALUE.PLL_CLK}]
	# convert to 2 decimal places
	set PLL_CLK_FLOAT [expr {double(round(100*$PLL_CLK))/100}]
	set HDL_PLL_CLK_HI [expr {floor($PLL_CLK_FLOAT)}]
	set HDL_PLL_CLK_LO [expr (($PLL_CLK_FLOAT - $HDL_PLL_CLK_HI)*100)]
	#send_msg INFO 201 "HDL_PLL_CLK_LO here ..$HDL_PLL_CLK_LO"
    set_property value ${HDL_PLL_CLK_LO} ${MODELPARAM_VALUE.HDL_PLL_CLK_LO}
}

proc update_MODELPARAM_VALUE.HDL_PLL_BYPASS {PARAM_VALUE.PLL_BYPASS MODELPARAM_VALUE.HDL_PLL_BYPASS } {
	set PLL_BYPASS [get_property value ${PARAM_VALUE.PLL_BYPASS}]
    set_property value ${PLL_BYPASS} ${MODELPARAM_VALUE.HDL_PLL_BYPASS}
	#send_msg INFO 2001 "Model param here ..$PLL_BYPASS "
    #set data_aa get_property width [ipx::get_address_blocks M_AXI_DEC0]
	#send_msg INFO 201 "Addr param here ..$data_aa "
}

proc update_MODELPARAM_VALUE.HDL_AXI_MCU_RANGE {PARAM_VALUE.AXI_MCU_RANGE MODELPARAM_VALUE.HDL_AXI_MCU_RANGE } {
   set_property value [get_property value ${PARAM_VALUE.AXI_MCU_RANGE}] ${MODELPARAM_VALUE.HDL_AXI_MCU_RANGE}
}

proc update_MODELPARAM_VALUE.HDL_AXI_DEC_RANGE0 {PARAM_VALUE.AXI_DEC_RANGE0 MODELPARAM_VALUE.HDL_AXI_DEC_RANGE0 } {
   set_property value [get_property value ${PARAM_VALUE.AXI_DEC_RANGE0}] ${MODELPARAM_VALUE.HDL_AXI_DEC_RANGE0}
}

proc update_MODELPARAM_VALUE.HDL_AXI_DEC_RANGE1 {PARAM_VALUE.AXI_DEC_RANGE1 MODELPARAM_VALUE.HDL_AXI_DEC_RANGE1 } {
   set_property value [get_property value ${PARAM_VALUE.AXI_DEC_RANGE1}] ${MODELPARAM_VALUE.HDL_AXI_DEC_RANGE1}
}

proc update_MODELPARAM_VALUE.HDL_AXI_ENC_RANGE0 {PARAM_VALUE.AXI_ENC_RANGE0 MODELPARAM_VALUE.HDL_AXI_ENC_RANGE0 } {
   set_property value [get_property value ${PARAM_VALUE.AXI_ENC_RANGE0}] ${MODELPARAM_VALUE.HDL_AXI_ENC_RANGE0}
}

proc update_MODELPARAM_VALUE.HDL_AXI_ENC_RANGE1 {PARAM_VALUE.AXI_ENC_RANGE1 MODELPARAM_VALUE.HDL_AXI_ENC_RANGE1 } {
   set_property value [get_property value ${PARAM_VALUE.AXI_ENC_RANGE1}] ${MODELPARAM_VALUE.HDL_AXI_ENC_RANGE1}
}

proc update_MODELPARAM_VALUE.HDL_AXI_MCU_BASE {PARAM_VALUE.AXI_MCU_BASE MODELPARAM_VALUE.HDL_AXI_MCU_BASE } {
   set_property value [get_property value ${PARAM_VALUE.AXI_MCU_BASE}] ${MODELPARAM_VALUE.HDL_AXI_MCU_BASE}
}
proc update_MODELPARAM_VALUE.HDL_AXI_DEC_BASE1 {PARAM_VALUE.AXI_DEC_BASE1 MODELPARAM_VALUE.HDL_AXI_DEC_BASE1 } {
   set_property value [get_property value ${PARAM_VALUE.AXI_DEC_BASE1}] ${MODELPARAM_VALUE.HDL_AXI_DEC_BASE1}
}
proc update_MODELPARAM_VALUE.HDL_AXI_DEC_BASE0 {PARAM_VALUE.AXI_DEC_BASE0 MODELPARAM_VALUE.HDL_AXI_DEC_BASE0 } {
   set_property value [get_property value ${PARAM_VALUE.AXI_DEC_BASE0}] ${MODELPARAM_VALUE.HDL_AXI_DEC_BASE0}
}
proc update_MODELPARAM_VALUE.HDL_AXI_ENC_BASE1 {PARAM_VALUE.AXI_ENC_BASE1 MODELPARAM_VALUE.HDL_AXI_ENC_BASE1 } {
   set_property value [get_property value ${PARAM_VALUE.AXI_ENC_BASE1}] ${MODELPARAM_VALUE.HDL_AXI_ENC_BASE1}
}
proc update_MODELPARAM_VALUE.HDL_AXI_ENC_BASE0 {PARAM_VALUE.AXI_ENC_BASE0 MODELPARAM_VALUE.HDL_AXI_ENC_BASE0 } {
   set_property value [get_property value ${PARAM_VALUE.AXI_ENC_BASE0}] ${MODELPARAM_VALUE.HDL_AXI_ENC_BASE0}
}

## starting here, will need to handle fractional digits..
#
proc float2hex {value} {
       global tcl_platform
       send_msg INFO 603 " in flao2hex  update method .. $value "
       set sign [expr ($value < 0 ? 1 : 0)]
       if {$sign} {set value [expr -1.0 * $value]}
       if {[catch {
           set exponent [expr {int(floor(log($value)/0.69314718055994529))+127}]
           set mantissa [expr {($value/pow(2,double($exponent-127)))-1.}]
       }]} {
           set exponent 0
           set mantissa 0.0
       } else {
           if {$exponent < 0} {
               set exponent 0
               set mantissa 0.0
           } elseif {$exponent > 255} {
               error "value $value outside legal range for a float"
           }
       }
       set mantissa [expr int($mantissa * pow(2,23))]
       set result [format "0x%04X" [expr ($sign << 31) | ($exponent << 23) | ($mantissa)]]
       #if {$tcl_platform(byteOrder) == "littleEndian"} { set result [reverse4 $result] }
       return $result
}


proc update_MODELPARAM_VALUE.HDL_AXI_MCU_CLK {PARAM_VALUE.AXI_MCU_CLK MODELPARAM_VALUE.HDL_AXI_MCU_CLK } {
   set fromBD [get_property value ${PARAM_VALUE.AXI_MCU_CLK}]
   set hexFloatRep [float2hex  [expr $fromBD/100.0] ]
   set decVal [expr $hexFloatRep]
   send_msg INFO 603 " in HDL_AXI_MCU_CLK  update method ..$decVal "
   set_property value $decVal ${MODELPARAM_VALUE.HDL_AXI_MCU_CLK}
}

proc update_MODELPARAM_VALUE.HDL_AXI_DEC_CLK {PARAM_VALUE.AXI_DEC_CLK MODELPARAM_VALUE.HDL_AXI_DEC_CLK } {
   set fromBD [get_property value ${PARAM_VALUE.AXI_DEC_CLK}]
   set hexFloatRep [float2hex  [expr $fromBD/100.0] ]
   set decVal [expr $hexFloatRep]
   send_msg INFO 603 " in HDL_AXI_DEC_CLK  update method ..$decVal "
   set_property value $decVal ${MODELPARAM_VALUE.HDL_AXI_DEC_CLK}
}

proc update_MODELPARAM_VALUE.HDL_AXI_ENC_CLK {PARAM_VALUE.AXI_ENC_CLK MODELPARAM_VALUE.HDL_AXI_ENC_CLK } {
   set fromBD [get_property value ${PARAM_VALUE.AXI_ENC_CLK}]
   set hexFloatRep [float2hex  [expr $fromBD/100.0] ]
   set decVal [expr $hexFloatRep]
   send_msg INFO 603 " in HDL_AXI_ENC_CLK  update method ..$decVal "
   set_property value $decVal ${MODELPARAM_VALUE.HDL_AXI_ENC_CLK}
}

proc update_MODELPARAM_VALUE.HDL_ENC_CLK {PARAM_VALUE.ENC_CLK MODELPARAM_VALUE.HDL_ENC_CLK } {
   set ENC_CLK [get_property value ${PARAM_VALUE.ENC_CLK}]
   set_property value ${ENC_CLK} ${MODELPARAM_VALUE.HDL_ENC_CLK}
}

proc update_MODELPARAM_VALUE.HDL_MCU_CLK {PARAM_VALUE.MCU_CLK MODELPARAM_VALUE.HDL_MCU_CLK } {
	set MCU_CLK [get_property value ${PARAM_VALUE.MCU_CLK}]
    set_property value ${MCU_CLK} ${MODELPARAM_VALUE.HDL_MCU_CLK}
}


proc validate_MODELPARAM_VALUE.HDL_CORE_CLK { MODELPARAM_VALUE.HDL_CORE_CLK PARAM_VALUE.CORE_CLK} {
	set CORE_CLK [get_property value ${PARAM_VALUE.CORE_CLK}]
	 if {[::xit::get_property PROJECT_PARAM.SPEEDGRADE] == -1 && $CORE_CLK > 667 } {
       set_property errmsg "For SpeedGrade -1, Max core clock should be 667 !"  ${PARAM_VALUE.CORE_CLK}
     }
}

proc update_MODELPARAM_VALUE.HDL_CORE_CLK {PARAM_VALUE.CORE_CLK MODELPARAM_VALUE.HDL_CORE_CLK } {
	set CORE_CLK [get_property value ${PARAM_VALUE.CORE_CLK}]
   if {[::xit::get_property PROJECT_PARAM.SPEEDGRADE] == -1 && $CORE_CLK > 667 } {
       set_property value 667 ${MODELPARAM_VALUE.HDL_CORE_CLK}
    } else {
       set_property value ${CORE_CLK} ${MODELPARAM_VALUE.HDL_CORE_CLK}
      }
   }


proc calc_frame_size_x { FRAME_SIZE } {
	if { $FRAME_SIZE == 2 } {
      set X_VALUE 1280;
	} elseif { $FRAME_SIZE == 3 } {
      set X_VALUE 1920;
	} elseif { $FRAME_SIZE == 4 } {
      set X_VALUE 3840;
	} elseif { $FRAME_SIZE == 5 } {
      set X_VALUE 4096;
	} elseif { $FRAME_SIZE == 6 } {
      set X_VALUE 7680;
	} elseif { $FRAME_SIZE == 0 } {
      set X_VALUE 854;
	} elseif { $FRAME_SIZE == 1 } {
      set X_VALUE 1024;
	}
    return $X_VALUE
}

proc update_MODELPARAM_VALUE.HDL_FRAME_SIZE_X {MODELPARAM_VALUE.HDL_FRAME_SIZE_X PARAM_VALUE.ENC_FRAME_SIZE } {
    set FRAME_SIZE [get_property value ${PARAM_VALUE.ENC_FRAME_SIZE}]
    set X_VALUE [calc_frame_size_x $FRAME_SIZE]
    set_property value ${X_VALUE} ${MODELPARAM_VALUE.HDL_FRAME_SIZE_X}
}

proc update_PARAM_VALUE.ENC_FPS { PARAM_VALUE.ENC_FPS PARAM_VALUE.ENABLE_ENCODER PARAM_VALUE.ENC_FRAME_SIZE  } {
    set FRAME_SIZE [get_property value ${PARAM_VALUE.ENC_FRAME_SIZE}]
	set ENCODER_VALUE [get_property value ${PARAM_VALUE.ENABLE_ENCODER}]
    set FPS [get_property value ${PARAM_VALUE.ENC_FPS}]
	#send_msg INFO 201 " in ENC_FPS update method ..$FPS "
	if {( $ENCODER_VALUE == "true" ) || ( $ENCODER_VALUE == "TRUE" ) } {
	  # If frame is 8K, FPS == 15
	  if { $FRAME_SIZE == 6 } {
        set_property value 0 ${PARAM_VALUE.ENC_FPS}
        set_property enabled false ${PARAM_VALUE.ENC_FPS}
      } else {
        set_property enabled true ${PARAM_VALUE.ENC_FPS}
	  }
	} else {
       set_property enabled false ${PARAM_VALUE.ENC_FPS}
	}
}

proc update_MODELPARAM_VALUE.HDL_FPS {MODELPARAM_VALUE.HDL_FPS PARAM_VALUE.ENC_FPS } {
    set FPS [get_property value ${PARAM_VALUE.ENC_FPS}]
	set VALUE [calc_fps $FPS]
    set_property value ${VALUE} ${MODELPARAM_VALUE.HDL_FPS}
    #set HDL_FPS [get_property value ${MODELPARAM_VALUE.HDL_FPS}]
    #set ENC_FPS [get_property value ${PARAM_VALUE.ENC_FPS}]
	#send_msg INFO 21 "HDL_FPS $HDL_FPS "
	#send_msg INFO 21 "ENC_FPS $ENC_FPS "
}

proc update_MODELPARAM_VALUE.HDL_COLOR_FORMAT {PARAM_VALUE.ENC_COLOR_FORMAT MODELPARAM_VALUE.HDL_COLOR_FORMAT } {
    set COLOR_FORMAT [get_property value ${PARAM_VALUE.ENC_COLOR_FORMAT}]
    set_property value ${COLOR_FORMAT} ${MODELPARAM_VALUE.HDL_COLOR_FORMAT}
}

proc update_MODELPARAM_VALUE.HDL_DEC_COLOR_FORMAT {PARAM_VALUE.DEC_COLOR_FORMAT MODELPARAM_VALUE.HDL_DEC_COLOR_FORMAT } {
    set COLOR_FORMAT [get_property value ${PARAM_VALUE.DEC_COLOR_FORMAT}]
    set_property value ${COLOR_FORMAT} ${MODELPARAM_VALUE.HDL_DEC_COLOR_FORMAT}
}


proc calc_frame_size_y { FRAME_SIZE  } {
	if { $FRAME_SIZE == 2 } {
      set Y_VALUE 720;
	} elseif { $FRAME_SIZE == 3 } {
      set Y_VALUE 1080;
	} elseif { $FRAME_SIZE == 4 } {
      set Y_VALUE 2160;
	} elseif { $FRAME_SIZE == 5 } {
      set Y_VALUE 2160;
	} elseif { $FRAME_SIZE == 6 } {
      set Y_VALUE 4320;
	} elseif { $FRAME_SIZE == 0 } {
      set Y_VALUE 480;
	} elseif { $FRAME_SIZE == 1 } {
      set Y_VALUE 576;
	}
    return $Y_VALUE
}


proc update_MODELPARAM_VALUE.HDL_FRAME_SIZE_Y {PARAM_VALUE.ENC_FRAME_SIZE MODELPARAM_VALUE.HDL_FRAME_SIZE_Y } {
    set FRAME_SIZE [get_property value ${PARAM_VALUE.ENC_FRAME_SIZE}]
	set Y_VALUE [calc_frame_size_y $FRAME_SIZE]
    set_property value ${Y_VALUE} ${MODELPARAM_VALUE.HDL_FRAME_SIZE_Y}
}

proc update_MODELPARAM_VALUE.HDL_COLOR_DEPTH {PARAM_VALUE.ENC_COLOR_DEPTH MODELPARAM_VALUE.HDL_COLOR_DEPTH } {
    set COLOR_DEPTH [get_property value ${PARAM_VALUE.ENC_COLOR_DEPTH}]
    set_property value ${COLOR_DEPTH} ${MODELPARAM_VALUE.HDL_COLOR_DEPTH}
}

proc update_MODELPARAM_VALUE.HDL_DEC_COLOR_DEPTH {PARAM_VALUE.DEC_COLOR_DEPTH MODELPARAM_VALUE.HDL_DEC_COLOR_DEPTH } {
    set COLOR_DEPTH [get_property value ${PARAM_VALUE.DEC_COLOR_DEPTH}]
    set_property value ${COLOR_DEPTH} ${MODELPARAM_VALUE.HDL_DEC_COLOR_DEPTH}
}

proc update_MODELPARAM_VALUE.C_VCU_AXILITEAPB_DATA_WIDTH { MODELPARAM_VALUE.C_VCU_AXILITEAPB_DATA_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	# WARNING: There is no corresponding user parameter named "C_VCU_AXILITEAPB_DATA_WIDTH". Setting updated value from the model parameter.
set_property value 32 ${MODELPARAM_VALUE.C_VCU_AXILITEAPB_DATA_WIDTH}
}

proc update_MODELPARAM_VALUE.C_VCU_ENC0_DATA_WIDTH { MODELPARAM_VALUE.C_VCU_ENC0_DATA_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	# WARNING: There is no corresponding user parameter named "C_VCU_ENC0_DATA_WIDTH". Setting updated value from the model parameter.
set_property value 128 ${MODELPARAM_VALUE.C_VCU_ENC0_DATA_WIDTH}
}

proc update_MODELPARAM_VALUE.C_VCU_ENC1_DATA_WIDTH { MODELPARAM_VALUE.C_VCU_ENC1_DATA_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	# WARNING: There is no corresponding user parameter named "C_VCU_ENC1_DATA_WIDTH". Setting updated value from the model parameter.
set_property value 128 ${MODELPARAM_VALUE.C_VCU_ENC1_DATA_WIDTH}
}

proc update_MODELPARAM_VALUE.C_VCU_DEC0_DATA_WIDTH { MODELPARAM_VALUE.C_VCU_DEC0_DATA_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	# WARNING: There is no corresponding user parameter named "C_VCU_DEC0_DATA_WIDTH". Setting updated value from the model parameter.
set_property value 128 ${MODELPARAM_VALUE.C_VCU_DEC0_DATA_WIDTH}
}

proc update_MODELPARAM_VALUE.C_VCU_DEC1_DATA_WIDTH { MODELPARAM_VALUE.C_VCU_DEC1_DATA_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	# WARNING: There is no corresponding user parameter named "C_VCU_DEC1_DATA_WIDTH". Setting updated value from the model parameter.
set_property value 128 ${MODELPARAM_VALUE.C_VCU_DEC1_DATA_WIDTH}
}

proc update_MODELPARAM_VALUE.C_VCU_MCU_DATA_WIDTH { MODELPARAM_VALUE.C_VCU_MCU_DATA_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	# WARNING: There is no corresponding user parameter named "C_VCU_MCU_DATA_WIDTH". Setting updated value from the model parameter.
set_property value 32 ${MODELPARAM_VALUE.C_VCU_MCU_DATA_WIDTH}
}

proc update_MODELPARAM_VALUE.HDL_TABLE_NO { MODELPARAM_VALUE.HDL_TABLE_NO PARAM_VALUE.TABLE_NO } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.TABLE_NO}] ${MODELPARAM_VALUE.HDL_TABLE_NO}
}

proc update_MODELPARAM_VALUE.HDL_ENCODER_EN { MODELPARAM_VALUE.HDL_ENCODER_EN PARAM_VALUE.ENABLE_ENCODER } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set ENCODER_VALUE [get_property value ${PARAM_VALUE.ENABLE_ENCODER}]
	if {( $ENCODER_VALUE == "true" ) || ( $ENCODER_VALUE == "TRUE" ) } {
	   set_property value 1 ${MODELPARAM_VALUE.HDL_ENCODER_EN}
	} else {
	   set_property value 0 ${MODELPARAM_VALUE.HDL_ENCODER_EN}
	}
}

# TBD .. this has to be ENC_BUFFER_SIZE
proc update_MODELPARAM_VALUE.HDL_MEM_DEPTH { MODELPARAM_VALUE.HDL_MEM_DEPTH PARAM_VALUE.ENC_BUFFER_SIZE  PARAM_VALUE.TABLE_NO PARAM_VALUE.ENC_BUFFER_TYPE  PARAM_VALUE.ENC_COLOR_DEPTH } {
    # ENC Buffer Size in K Bytes 
	set ENC_BUFFER_SIZE [get_property value ${PARAM_VALUE.ENC_BUFFER_SIZE}]
	set ENC_BUFFER_TYPE [get_property value ${PARAM_VALUE.ENC_BUFFER_TYPE}]
	set ENC_COLOR_DEPTH [get_property value ${PARAM_VALUE.ENC_COLOR_DEPTH}]
	if  {$ENC_COLOR_DEPTH == 0} {
        set MULT 256
	} else {
        set MULT 320
	}
    #send_msg INFO 201 "in ENC_BUFFER_SIZE  TOTAL_BYTES $ENC_BUFFER_SIZE "
	#set TOTAL_BYTES [expr ( $ENC_BUFFER_SIZE * $MULT / (8*1024) )] 
	#Convert from KBytes to depth in words
	set mem_depth [expr $ENC_BUFFER_SIZE * 1024 * 8 / $MULT ]
    #send_msg INFO 201 "in mem_depth  in entries $mem_depth "
	set_property value $mem_depth ${MODELPARAM_VALUE.HDL_MEM_DEPTH}
}

proc update_MODELPARAM_VALUE.HDL_RAM_TYPE { MODELPARAM_VALUE.HDL_RAM_TYPE PARAM_VALUE.ENC_BUFFER_TYPE  } {
	set_property value [get_property value ${PARAM_VALUE.ENC_BUFFER_TYPE}] ${MODELPARAM_VALUE.HDL_RAM_TYPE}
}

proc update_MODELPARAM_VALUE.HDL_DECODER_EN { MODELPARAM_VALUE.HDL_DECODER_EN PARAM_VALUE.ENABLE_DECODER } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	# set_property value [get_property value ${PARAM_VALUE.ENABLE_DECODER}] ${MODELPARAM_VALUE.HDL_DECODER_EN}
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set DECODER_VALUE [get_property value ${PARAM_VALUE.ENABLE_DECODER}]
	#send_msg INFO 2001 "Model param here ..$DECODER_VALUE "
	if {( $DECODER_VALUE == "true" ) || ( $DECODER_VALUE == "TRUE" )} {
	   set_property value 1 ${MODELPARAM_VALUE.HDL_DECODER_EN}
       #set_property enabled true [ipgui::get_groupspec Decoder_Parameter -of $IPINST]
	} else {
	   set_property value 0 ${MODELPARAM_VALUE.HDL_DECODER_EN}
       #set_property enabled false [ipgui::get_groupspec Decoder_Parameter -of $IPINST]
	}
}

#proc update_MODELPARAM_VALUE.ENABLE_DEBUG_PORT { MODELPARAM_VALUE.ENABLE_DEBUG_PORT PARAM_VALUE.ENABLE_DEBUG_PORT } {
#	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
#	set_property value [get_property value ${PARAM_VALUE.ENABLE_DEBUG_PORT}] ${MODELPARAM_VALUE.ENABLE_DEBUG_PORT}
#	send_msg INFO 2 "Setting Enable Debug Port.."
#}

#proc update_MODELPARAM_VALUE.ENABLE_TEST_PORT { MODELPARAM_VALUE.ENABLE_TEST_PORT PARAM_VALUE.ENABLE_TEST_PORT } {
#	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
#	set_property value [get_property value ${PARAM_VALUE.ENABLE_TEST_PORT}] ${MODELPARAM_VALUE.ENABLE_TEST_PORT}
#}
