// Copyright (C) 2022, Advanced Micro Devices, Inc.
// SPDX-License-Identifier: Apache-2.0

// This module drives the Texas Instruments LM5106 gate driver chip 
// enable pin. The enable pin is driven if enabled by the  
// SW application and if no current/voltage limits are violated 

`timescale 1 ns / 1 ps

  module gate_driver_chk_en 
  (
 
    output wire  gate_drive_en,
   
    input wire   phase_a_oc_err, 
    input wire   phase_b_oc_err,
    input wire   phase_c_oc_err,
    input wire   phase_a_uc_err, 
    input wire   phase_b_uc_err,
    input wire   phase_c_uc_err,
    
    input wire   dc_link_oc_err,
    input wire   dc_link_ov_err,
     
    input wire   gate_drive_en_sw            
    
  );
  
  // If any current or voltage limit is violated disable gate drive enable
  assign gate_drive_en = gate_drive_en_sw & (~(phase_a_oc_err | phase_b_oc_err | phase_c_oc_err | 
                                               phase_a_uc_err | phase_b_uc_err | phase_c_uc_err |
                                               dc_link_oc_err | dc_link_ov_err ));
  
  

 
  endmodule
