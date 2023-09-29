// Copyright (C) 2022, Advanced Micro Devices, Inc.
// SPDX-License-Identifier: Apache-2.0

`timescale 1ns / 1ps

// This module drives the Texas Instruments LM5106 gate driver chip 
// input pin. The pin is driven based on phase inputs from the 
// PWM module 

module gate_driver
 (
    input  wire      sys_clk,
    input  wire      sys_rstn,
    input  wire      phase_upper,
    input  wire      phase_lower,
    output wire      gate_drive
 );

reg gate_drive_i;

always @(posedge sys_clk)
begin
  if (sys_rstn == 1'b0)
    gate_drive_i <= 1'b0;
  else
  begin
    if (phase_upper == 1'b1 && phase_lower == 1'b0)
      gate_drive_i <= 1'b1;
    else if (phase_upper == 1'b0 && phase_lower == 1'b1)
      gate_drive_i <= 1'b0;
    else
      gate_drive_i <= 1'b0;
  end
end    

assign gate_drive = gate_drive_i;

endmodule
