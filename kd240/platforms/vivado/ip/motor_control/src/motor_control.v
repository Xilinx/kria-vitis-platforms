// Copyright (C) 2022, Advanced Micro Devices, Inc.
// SPDX-License-Identifier: Apache-2.0

// This module drives the Texas Instruments LM5106 gate driver chip 
// enable pin. The enable pin is driven if enabled by the  
// SW application and if no current/voltage limits are violated 

// The module also has the following functions

// 1. Generate an interrupt if there is phase current imbalance

// 2. Enable MUX select lines
// Voltage_sel = 0 - Voltage from ADC
// Voltage_sel = 1 - Voltage from Virtual model
// 
// current_sel = 0 - Phase current from ADC
// current_sel = 1 - Phase current from Virtual model

// Speed_select = 0 - from encoder
// Speed_select = 1 - from virtual model


`timescale 1 ns / 1 ps

  module motor_control #
  (
    // Users to add parameters here

    // User parameters ends
    // Do not modify the parameters beyond this line


    // Parameters of Axi Slave Bus Interface s_axi_cntrl
    parameter integer C_S_AXI_CNTRL_DATA_WIDTH  = 32,
    parameter integer C_S_AXI_CNTRL_ADDR_WIDTH  = 5,

    // Parameters for AXI stream
    parameter integer Q_INTEGER = 7,
    parameter integer Q_FRACTIONAL = 16,
    parameter integer C_S_AXIS_TDATA_WIDTH  = 1 + Q_INTEGER + Q_FRACTIONAL 

  )
  (
    // Users to add ports here

    // User ports ends
    // Do not modify the ports beyond this line

    output wire  gate_drive_en,
    output wire  interrupt,
    output wire  voltage_sel,
    output wire  current_sel,
    output wire  speed_sel,
        
    input wire   phase_a_oc_err, 
    input wire   phase_b_oc_err,
    input wire   phase_c_oc_err,
    input wire   phase_a_uc_err, 
    input wire   phase_b_uc_err,
    input wire   phase_c_uc_err,
    
    input wire   dc_link_oc_err,
    input wire   dc_link_ov_err,
    input wire   dc_link_uc_err,
    input wire   dc_link_uv_err,
         
    // Ports of Axi Slave Bus Interface s_axi_cntrl
    input wire  s_axi_cntrl_clk,
    input wire  s_axi_cntrl_resetn,
    input wire [C_S_AXI_CNTRL_ADDR_WIDTH-1 : 0] s_axi_cntrl_awaddr,
    input wire [2 : 0] s_axi_cntrl_awprot,
    input wire  s_axi_cntrl_awvalid,
    output wire  s_axi_cntrl_awready,
    input wire [C_S_AXI_CNTRL_DATA_WIDTH-1 : 0] s_axi_cntrl_wdata,
    input wire [(C_S_AXI_CNTRL_DATA_WIDTH/8)-1 : 0] s_axi_cntrl_wstrb,
    input wire  s_axi_cntrl_wvalid,
    output wire  s_axi_cntrl_wready,
    output wire [1 : 0] s_axi_cntrl_bresp,
    output wire  s_axi_cntrl_bvalid,
    input wire  s_axi_cntrl_bready,
    input wire [C_S_AXI_CNTRL_ADDR_WIDTH-1 : 0] s_axi_cntrl_araddr,
    input wire [2 : 0] s_axi_cntrl_arprot,
    input wire  s_axi_cntrl_arvalid,
    output wire  s_axi_cntrl_arready,
    output wire [C_S_AXI_CNTRL_DATA_WIDTH-1 : 0] s_axi_cntrl_rdata,
    output wire [1 : 0] s_axi_cntrl_rresp,
    output wire  s_axi_cntrl_rvalid,
    input wire  s_axi_cntrl_rready,
    
     
    // Ports of Axi Slave Bus Interface s_axis_phase_a_i
    input wire [C_S_AXIS_TDATA_WIDTH-1 : 0] s_axis_phase_a_i_tdata,
    input wire  s_axis_phase_a_i_tvalid,
    output wire  s_axis_phase_a_i_tready,

    // Ports of Axi Slave Bus Interface s_axis_phase_b_i
    input wire [C_S_AXIS_TDATA_WIDTH-1 : 0] s_axis_phase_b_i_tdata,
    input wire  s_axis_phase_b_i_tvalid,
    output wire  s_axis_phase_b_i_tready,

    // Ports of Axi Slave Bus Interface s_axis_phase_c_i
    input wire [C_S_AXIS_TDATA_WIDTH-1 : 0] s_axis_phase_c_i_tdata,
    input wire  s_axis_phase_c_i_tvalid,
    output wire  s_axis_phase_c_i_tready
    
  );
  

  wire signed [C_S_AXIS_TDATA_WIDTH-1:0] phase_imbal_limit_q;
  wire [31:0] phase_imbal_limit;
  wire phase_imbal_err;
  wire [31:0] phase_imbal_clr;
  wire phase_imbal_err_en;
  wire gate_drive_en_sw;
  
  reg phase_imbal;
   
  reg signed [C_S_AXIS_TDATA_WIDTH:0] phase_a_i;
  reg signed [C_S_AXIS_TDATA_WIDTH:0] phase_b_i;
  reg signed [C_S_AXIS_TDATA_WIDTH:0] phase_c_i;
  
//C_S_AXIS_TDATA_WIDTH  = 1 (Sign bit) + Q_INTEGER + Q_FRACTIONAL
//Max width supported on the limit register is 32 bit, so C_S_AXIS_TDATA_WIDTH cannot be more than 32-bits
  assign phase_imbal_limit_q[C_S_AXIS_TDATA_WIDTH-1:0] = phase_imbal_limit[C_S_AXIS_TDATA_WIDTH-1:0];
      
// Instantiation of Axi Bus Interface s_axi_cntrl
  motor_control_s_axi_cntrl # ( 
    .C_S_AXI_DATA_WIDTH(C_S_AXI_CNTRL_DATA_WIDTH),
    .C_S_AXI_ADDR_WIDTH(C_S_AXI_CNTRL_ADDR_WIDTH)
  ) motor_control_s_axi_cntrl_inst (
    .S_AXI_ACLK(s_axi_cntrl_clk),
    .S_AXI_ARESETN(s_axi_cntrl_resetn),
    .S_AXI_AWADDR(s_axi_cntrl_awaddr),
    .S_AXI_AWPROT(s_axi_cntrl_awprot),
    .S_AXI_AWVALID(s_axi_cntrl_awvalid),
    .S_AXI_AWREADY(s_axi_cntrl_awready),
    .S_AXI_WDATA(s_axi_cntrl_wdata),
    .S_AXI_WSTRB(s_axi_cntrl_wstrb),
    .S_AXI_WVALID(s_axi_cntrl_wvalid),
    .S_AXI_WREADY(s_axi_cntrl_wready),
    .S_AXI_BRESP(s_axi_cntrl_bresp),
    .S_AXI_BVALID(s_axi_cntrl_bvalid),
    .S_AXI_BREADY(s_axi_cntrl_bready),
    .S_AXI_ARADDR(s_axi_cntrl_araddr),
    .S_AXI_ARPROT(s_axi_cntrl_arprot),
    .S_AXI_ARVALID(s_axi_cntrl_arvalid),
    .S_AXI_ARREADY(s_axi_cntrl_arready),
    .S_AXI_RDATA(s_axi_cntrl_rdata),
    .S_AXI_RRESP(s_axi_cntrl_rresp),
    .S_AXI_RVALID(s_axi_cntrl_rvalid),
    .S_AXI_RREADY(s_axi_cntrl_rready),
    .phase_imbal_limit (phase_imbal_limit),
    .phase_imbal_err_en(phase_imbal_err_en),
    .phase_imbal_clr(phase_imbal_clr),
    .phase_imbal_err(phase_imbal_err),
    .gate_drive_en_sw(gate_drive_en_sw),
    .speed_sel (speed_sel),
    .current_sel (current_sel),
    .voltage_sel (voltage_sel),
    .disable_interrupt (disable_interrupt)
    
  );

  always @( posedge s_axi_cntrl_clk )
  begin
    if ( s_axi_cntrl_resetn == 1'b0 )
      begin
        phase_a_i  <= 0;
      end 
    else
      begin    
        
        if (s_axis_phase_a_i_tvalid)
          phase_a_i <= s_axis_phase_a_i_tdata;
      end
  end    
  
  always @( posedge s_axi_cntrl_clk )
  begin
    if ( s_axi_cntrl_resetn == 1'b0 )
      begin
        phase_b_i  <= 0;
      end 
    else
      begin    
       if (s_axis_phase_b_i_tvalid)
          phase_b_i <= s_axis_phase_b_i_tdata;          
       end
  end    
  
  always @( posedge s_axi_cntrl_clk )
  begin
    if ( s_axi_cntrl_resetn == 1'b0 )
      begin
        phase_c_i  <= 0;
      end 
    else
      begin    
        
        if (s_axis_phase_c_i_tvalid)
          phase_c_i <= s_axis_phase_c_i_tdata;  
      end
  end    
  
  // The current imbalance is a check of the summation of the three phase currents. 
  // An ideal motor would have: Phase A + Phase B + Phase C = 0A. 
  // If Phase A + Phase B + Phase C > limit then set this bit
  // Note this is just a warning and doesn't disable the gate drive  
  
  always @( posedge s_axi_cntrl_clk )
    begin
      if ( s_axi_cntrl_resetn == 1'b0 )
        begin
          phase_imbal  <= 1'b0;
        end 
      else
        begin                   
          // When adding 2 negative number say in Q7.Q16 format the resultant value
          // may need an extra bit on the MSB side 
          // Not accouting for overflow when adding negative numbers
          // as not expecting a value of -2^8
          if ((phase_a_i + phase_b_i + phase_c_i > phase_imbal_limit_q) && (phase_imbal_err_en == 1'b1))
            phase_imbal <= 1'b1;               
          else if (phase_imbal_clr == 32'h0bad00ff)
            phase_imbal <= 1'b0;
          // Hold error until clear bit is set
          else
            phase_imbal <= phase_imbal;
  
        end
  end    
  
  
  assign phase_imbal_err = phase_imbal;  
  assign interrupt = (disable_interrupt == 1'b1) ? 1'b0: phase_imbal;
  
  // If any current or voltage limit is violated disable gate drive enable
    assign gate_drive_en = gate_drive_en_sw & (~(phase_a_oc_err | phase_b_oc_err | phase_c_oc_err | dc_link_oc_err | dc_link_ov_err |
                                                 phase_a_uc_err | phase_b_uc_err | phase_c_uc_err));
  
  
  assign s_axis_phase_a_i_tready = 1'b1;
  assign s_axis_phase_b_i_tready = 1'b1;
  assign s_axis_phase_c_i_tready = 1'b1;
 
  endmodule
