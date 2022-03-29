# IP Integrator’s bd/bd.tcl
# cellPath: path to the cell instance


proc clock_checks {cellPath otherInfo } {
  set ip [get_bd_cells $cellPath]
  #bd::send_msg -of $cellPath -type INFO -msg_id 17 -text " In post config..."
  set axi_lite_clk [expr [get_property CONFIG.FREQ_HZ [get_bd_pins $ip/s_axi_lite_aclk]] / 1000000 ]
  set axi_mcu_clk  [expr [get_property CONFIG.FREQ_HZ [get_bd_pins $ip/m_axi_mcu_aclk]] / 1000000 ]
  set u_pll_by_pass_en     [get_property CONFIG.PLL_BYPASS $ip]
  set u_core_clk   [get_property CONFIG.CORE_CLK $ip]
  set u_mcu_clk    [get_property CONFIG.MCU_CLK $ip]
  set u_dec_en     [get_property CONFIG.HDL_DECODER_EN $ip]
  set u_enc_en     [get_property CONFIG.HDL_ENCODER_EN $ip]
  if {$u_enc_en == 1} {
     set axi_enc_clk  [expr [get_property CONFIG.FREQ_HZ [get_bd_pins $ip/m_axi_enc_aclk]] / 1000000 ]
     #set p_enc_clk    [expr [get_property CONFIG.FREQ_HZ [get_bd_pins $ip/enc_buffer_clk]] / 1000000 ]
	 set p_enc_clk $axi_enc_clk
  } else {
     set axi_enc_clk 0
	 set p_enc_clk $axi_enc_clk
  }
  if {$u_dec_en == 1} {
     set axi_dec_clk  [expr [get_property CONFIG.FREQ_HZ [get_bd_pins $ip/m_axi_dec_aclk]] / 1000000 ]
  } else {
     set axi_dec_clk 0
  }
  set enc_clk      $p_enc_clk  
  if {$u_pll_by_pass_en == 1} {
     set p_mcu_clk    [expr [get_property CONFIG.FREQ_HZ [get_bd_pins $ip/mcu_clk]] / 1000000 ]
     set p_core_clk   [expr [get_property CONFIG.FREQ_HZ [get_bd_pins $ip/core_clk]] / 1000000 ]
     set core_clk   $p_core_clk 
     set mcu_clk    $p_mcu_clk  
	 set ref_clk    0
     #bd::send_msg -of $cellPath -type INFO -msg_id 27 -text " p_mcu_clk is $p_mcu_clk"
     #bd::send_msg -of $cellPath -type INFO -msg_id 27 -text " p_core_clk is $p_core_clk"
  } else {
     set ref_clk      [expr [get_property CONFIG.FREQ_HZ [get_bd_pins $ip/pll_ref_clk]] / 1000000.0 ]
     #bd::send_msg -of $cellPath -type INFO -msg_id 27 -text " PLL_Clk  from pin is $ref_clk"
     set core_clk   $u_core_clk 
     set mcu_clk    $u_mcu_clk  
  }
  #bd::send_msg -of $cellPath -type INFO -msg_id 27 -text " axi_lite_clk is $axi_lite_clk"
  #bd::send_msg -of $cellPath -type INFO -msg_id 27 -text " axi_mcu_clk is $axi_mcu_clk"
  #bd::send_msg -of $cellPath -type INFO -msg_id 27 -text " axi_enc_clk is $axi_enc_clk"
  #bd::send_msg -of $cellPath -type INFO -msg_id 27 -text " ref_clk is $ref_clk"
  #bd::send_msg -of $cellPath -type INFO -msg_id 27 -text " p_enc_clk is $p_enc_clk"
  #bd::send_msg -of $cellPath -type INFO -msg_id 27 -text " axi_dec_clk is $axi_dec_clk"
  #bd::send_msg -of $cellPath -type INFO -msg_id 27 -text " u_pll_by_pass_en is $u_pll_by_pass_en"
  #bd::send_msg -of $cellPath -type INFO -msg_id 27 -text " u_core_clk is $u_core_clk"
  #bd::send_msg -of $cellPath -type INFO -msg_id 27 -text " u_mcu_clk is $u_mcu_clk"
  #bd::send_msg -of $cellPath -type INFO -msg_id 27 -text " mcu_clk is $mcu_clk"
  #bd::send_msg -of $cellPath -type INFO -msg_id 27 -text " enc_clk is $enc_clk"
  #bd::send_msg -of $cellPath -type INFO -msg_id 27 -text " core_clk is $core_clk"

  #Limit checks
  if {($ref_clk < 27 || $ref_clk > 60) && ($u_pll_by_pass_en == 0) } {
     bd::send_msg -of $cellPath -type ERROR -msg_id 17 -text " VCU REF clock needs to be 27 - 60. Actual $ref_clk"
  }
  if {($enc_clk > 333 || $enc_clk < 50) && ($u_enc_en == 1) } {
     bd::send_msg -of $cellPath -type ERROR -msg_id 17 -text " VCU ENC Cache clock needs to be  50 - 333. Actual $enc_clk"
  }
#added by pruthvi. 
  
  if {[get_property CONFIG.PROJECT_PARAM.SPEEDGRADE $ip] == -1 && $core_clk > 667 } {
     bd::send_msg -of $cellPath -type ERROR -msg_id 17 -text "*** VCU Core clock needs to be < 667. Actual $core_clk ***"
  }
  if {$core_clk > 712 } {
     bd::send_msg -of $cellPath -type ERROR -msg_id 17 -text "*** VCU Core clock needs to be < 712. Actual $core_clk ***"
  }
  if {$mcu_clk > 444 } {
     bd::send_msg -of $cellPath -type ERROR -msg_id 17 -text " VCU MCU clock needs to be < 444. Actual $mcu_clk"
  }
  if {($axi_enc_clk > 333 || $axi_enc_clk < 50) && ($u_enc_en == 1)} {
     bd::send_msg -of $cellPath -type ERROR -msg_id 17 -text " VCU AXI ENC clock needs to be 50 - 333. Actual $axi_enc_clk"
  }
  if {($axi_dec_clk > 333 || $axi_dec_clk < 40) && ($u_dec_en == 1)} {
     bd::send_msg -of $cellPath -type ERROR -msg_id 17 -text " VCU AXI ENC clock needs to be 40 - 333. Actual $axi_dec_clk"
  }
  if {($enc_clk < $axi_enc_clk) && ($u_enc_en == 1) } {
     bd::send_msg -of $cellPath -type ERROR -msg_id 17 -text " VCU ENC clock ($enc_clk) needs to be higher than AXI ENC Clock ($axi_enc_clk)"
  }
  if {($axi_enc_clk / $core_clk  > 2) && ($u_enc_en == 1)} {
     bd::send_msg -of $cellPath -type ERROR -msg_id 17 -text " VCU AXI ENC clock / Core Clock should be less than 2 "
  }
  if {($axi_dec_clk / $core_clk  > 2) && ($u_dec_en == 1)} {
     bd::send_msg -of $cellPath -type ERROR -msg_id 17 -text " VCU AXI DEC clock / Core Clock should be less than 2 "
  }
  if {$axi_mcu_clk > 333 } {
     bd::send_msg -of $cellPath -type ERROR -msg_id 17 -text " VCU MCU AXI clock needs to be < 333. Actual $axi_mcu_clk"
  }
  if {$mcu_clk < $axi_mcu_clk } {
     bd::send_msg -of $cellPath -type ERROR -msg_id 17 -text " VCU MCU clock ($mcu_clk) needs to be > MCU AXI Clock ($axi_mcu_clk)"
  }
  set_property CONFIG.PLL_CLK $ref_clk $ip
  set_property CONFIG.ENC_CLK $enc_clk $ip

  set_property CONFIG.AXI_ENC_CLK $axi_enc_clk $ip
  set_property CONFIG.AXI_DEC_CLK $axi_dec_clk $ip
  set_property CONFIG.AXI_MCU_CLK $axi_mcu_clk $ip
#  bd::send_msg -of $cellPath -type INFO -msg_id 27 -text " PLL_Clk found as $ref_clk"
#  bd::send_msg -of $cellPath -type INFO -msg_id 27 -text " Enc_Clk found as $enc_clk"
}


proc post_config_ip {cellPath otherInfo } {
# Any updates to interface properties based on user configuration
# Not needed, can be removed
   # Kamalesh. not sure if we need to call here.
   # clock_checks $cellPath $otherInfo
}

proc init { cellPath otherInfo } {
  bd::send_msg -of $cellPath -type INFO -msg_id 17 -text ": Init "
  bd::mark_propagate_overrideable [get_bd_cells $cellPath] {PLL_CLK ENC_CLK AXI_ENC_CLK AXI_DEC_CLK AXI_MCU_CLK}
}


proc vcu_get_actual_value { addr_range } {
   regsub -all "K" $addr_range "" result1 
   regsub -all "M" $result1 "" result2 
   regsub -all "G" $result2 "" result3 
   return $result3
}

proc vcu_get_KMG { addr_range } {
    if { [string match "K" $addr_range ]} {
        return "1"
	} elseif { [string match "M" $addr_range  ]} {
        return "2"
	} elseif { [string match "G" $addr_range ] } {
        return "3"
	} else {
        return "0"
	}
}


proc chop_base_addr { cellPath addr_in } {
    if { [regsub -all "0000$" $addr_in "" result] == 1 } {
      #regsub -all "0x" $result "" res
      return [expr $result]
	} else {
      bd::send_msg -of $cellPath -type ERROR -msg_id 36 -text " LSB 16 bits of addr are expected to be zero, but got $addr_in"
	  return 0
	}
}

proc chop_range_addr { cellPath addr_in } {
    if { [regsub -all "0000$" $addr_in "" result] == 1 } {
	  if { [regsub -linestop "0000" $result "" result2] == 1 } {
         #regsub -all "0x" $result2 "" res
         return [expr $result2]
	  } else {
		 # Kamalesh.. this also is not required now.
         #bd::send_msg -of $cellPath -type ERROR -msg_id 36 -text " MSB 16 bits of range are expected to be zero, but got $addr_in"
	     return 0
	  }
	} else {
      bd::send_msg -of $cellPath -type ERROR -msg_id 36 -text " LSB 16 bits of range are expected to be zero, but got $addr_in"
	  return 0
	}
}

proc addr_range_extract { cellPath otherInfo } {
   set ip [get_bd_cells $cellPath]
   set u_dec_en     [get_property CONFIG.HDL_DECODER_EN $ip]
   set u_enc_en     [get_property CONFIG.HDL_ENCODER_EN $ip]
   if {$u_enc_en == 1} {
      set enc0_base [get_property offset [get_bd_addr_segs $ip/Data/SEG_M_AXI_ENC0_Reg]]
      set enc1_base [get_property offset [get_bd_addr_segs $ip/Data/SEG_M_AXI_ENC1_Reg]]
      set enc0_range [get_property range [get_bd_addr_segs $ip/Data/SEG_M_AXI_ENC0_Reg]]
      set enc1_range [get_property range [get_bd_addr_segs $ip/Data/SEG_M_AXI_ENC1_Reg]]
      set_property CONFIG.AXI_ENC_BASE0 [chop_base_addr $cellPath $enc0_base] $ip
      set_property CONFIG.AXI_ENC_BASE1 [chop_base_addr $cellPath $enc1_base] $ip
      set_property CONFIG.AXI_ENC_RANGE0 [chop_range_addr $cellPath $enc0_range] $ip
      set_property CONFIG.AXI_ENC_RANGE1 [chop_range_addr $cellPath $enc1_range] $ip
   } else {
	   # what needs to be done for other mode.. ??
   }
   if {$u_dec_en == 1} {
      set dec0_base [get_property offset [get_bd_addr_segs $ip/Data/SEG_M_AXI_DEC0_Reg]]
      set dec1_base [get_property offset [get_bd_addr_segs $ip/Data/SEG_M_AXI_DEC1_Reg]]
      set dec0_range [get_property range [get_bd_addr_segs $ip/Data/SEG_M_AXI_DEC0_Reg]]
      set dec1_range [get_property range [get_bd_addr_segs $ip/Data/SEG_M_AXI_DEC1_Reg]]
      set_property CONFIG.AXI_DEC_BASE0 [chop_base_addr $cellPath $dec0_base] $ip
      set_property CONFIG.AXI_DEC_BASE1 [chop_base_addr $cellPath $dec1_base] $ip
      set_property CONFIG.AXI_DEC_RANGE0 [chop_range_addr $cellPath $dec0_range] $ip
      set_property CONFIG.AXI_DEC_RANGE1 [chop_range_addr $cellPath $dec1_range] $ip
   } else {
	   # what needs to be done for other mode.. ??
   }
   set mcu_base  [get_property offset [get_bd_addr_segs $ip/Data/SEG_M_AXI_MCU_Reg]]
   set mcu_range  [get_property range [get_bd_addr_segs $ip/Data/SEG_M_AXI_MCU_Reg]]
   set_property CONFIG.AXI_MCU_BASE  [chop_base_addr $cellPath $mcu_base]  $ip
   set_property CONFIG.AXI_MCU_RANGE  [chop_range_addr $cellPath $mcu_range] $ip

   #set mcu_range_KMG  [vcu_get_KMG $mcu_range ]
   #set enc0_range_KMG [vcu_get_KMG $enc0_range ]
   #set enc1_range_KMG [vcu_get_KMG $enc1_range ]
   #set dec0_range_KMG [vcu_get_KMG $dec0_range ]
   #set dec1_range_KMG [vcu_get_KMG $dec1_range ]

   #set mcu_range_value  [vcu_get_actual_value $mcu_range ]
   #set enc0_range_value [vcu_get_actual_value $enc0_range ]
   #set enc1_range_value [vcu_get_actual_value $enc1_range ]
   #set dec0_range_value [vcu_get_actual_value $dec0_range ]
   #set dec1_range_value [vcu_get_actual_value $dec1_range ]

   #set mcu_range_f  [concat $mcu_range_value $mcu_range_KMG]
   #set enc0_range_f [concat $enc0_range_value $enc0_range_KMG]
   #set enc1_range_f [concat $enc1_range_value $enc1_range_KMG]
   #set dec0_range_f [concat $dec0_range_value $dec0_range_KMG]
   #set dec1_range_f [concat $dec1_range_value $dec1_range_KMG]

   #bd::send_msg -of $cellPath -type INFO -msg_id 17 -text "Propagate  - $mcu_range_KMG, $mcu_range_value -> $mcu_range {$mcu_range_f}"
   #bd::send_msg -of $cellPath -type INFO -msg_id 17 -text "Propagate  - $enc0_range_KMG, $enc0_range_value -> $enc0_range {$enc0_range_f}"
   #bd::send_msg -of $cellPath -type INFO -msg_id 17 -text "Propagate  - $dec1_range_KMG, $dec1_range_value -> $dec1_range {$dec1_range_f}"


}

proc dump_verif_file {cellPath otherInfo } {
   set ip [get_bd_cells $cellPath]

   set ENABLE_DECODER 	[get_property CONFIG.ENABLE_DECODER $ip]
   set ENABLE_ENCODER  	[get_property CONFIG.ENABLE_ENCODER $ip]
   set ENC_BUFFER_SIZE  [get_property CONFIG.ENC_BUFFER_SIZE $ip]
   set ENC_COLOR_DEPTH  [get_property CONFIG.ENC_COLOR_DEPTH $ip]

   set ENC_BUFFER_MOTION_VEC_RANGE  	[get_property CONFIG.ENC_BUFFER_MOTION_VEC_RANGE $ip]
   set ENC_FRAME_SIZE  	    [get_property CONFIG.ENC_FRAME_SIZE $ip]
   set ENC_COLOR_FORMAT  	[get_property CONFIG.ENC_COLOR_FORMAT $ip]
   set ENC_FPS  	        [get_property CONFIG.ENC_FPS $ip]
   set ENC_VIDEO_STANDARD  	[get_property CONFIG.ENC_VIDEO_STANDARD $ip]

   set MCU_CLK  	        [get_property CONFIG.MCU_CLK $ip]
   set CORE_CLK  	        [get_property CONFIG.CORE_CLK $ip]
   set PLL_BYPASS  	        [get_property CONFIG.PLL_BYPASS $ip]
   set ENC_CLK  	        [get_property CONFIG.ENC_CLK $ip]
   set PLL_CLK  	        [get_property CONFIG.PLL_CLK $ip]

   set AXI_ENC_CLK  	    [get_property CONFIG.AXI_ENC_CLK $ip]
   set AXI_DEC_CLK  	    [get_property CONFIG.AXI_DEC_CLK $ip]
   set AXI_MCU_CLK  	    [get_property CONFIG.AXI_MCU_CLK $ip]

   set DEC_VIDEO_STANDARD  	[get_property CONFIG.DEC_VIDEO_STANDARD $ip]
   set DEC_FRAME_SIZE  	    [get_property CONFIG.DEC_FRAME_SIZE $ip]
   set DEC_FPS  	        [get_property CONFIG.DEC_FPS $ip]
   
   set filename "tcl_file.txt"

   set fileid [open $filename "w"]

    puts $fileid $ENABLE_DECODER
    puts $fileid $ENABLE_ENCODER
    puts $fileid $ENC_BUFFER_SIZE
    puts $fileid $ENC_COLOR_DEPTH

    puts $fileid $ENC_BUFFER_MOTION_VEC_RANGE
    puts $fileid $ENC_FRAME_SIZE
    puts $fileid $ENC_COLOR_FORMAT
    puts $fileid $ENC_FPS
    puts $fileid $ENC_VIDEO_STANDARD

    puts $fileid $MCU_CLK
    puts $fileid $CORE_CLK
    puts $fileid $PLL_BYPASS
    puts $fileid $ENC_CLK
    puts $fileid $PLL_CLK

    puts $fileid $AXI_ENC_CLK
    puts $fileid $AXI_DEC_CLK
    puts $fileid $AXI_MCU_CLK

    puts $fileid $DEC_VIDEO_STANDARD
    puts $fileid $DEC_FRAME_SIZE
    puts $fileid $DEC_FPS

    close $fileid
}


proc propagate {cellPath otherInfo } {
   clock_checks $cellPath $otherInfo
   #addr_range_extract $cellPath $otherInfo
   #dump_verif_file $cellPath $otherInfo
}

proc pre_propagate { cellPath otherInfo } {
  bd::send_msg -of $cellPath -type INFO -msg_id 17 -text ":Pre-Propagate  ."
}

proc post_propagate {cellPath otherInfo} {
  bd::send_msg -of $cellPath -type INFO -msg_id 17 -text ": Post Propogate  ."
}
