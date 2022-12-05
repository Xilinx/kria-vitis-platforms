// Copyright (C) 2022, Advanced Micro Devices, Inc.
// SPDX-License-Identifier: Apache-2.0

// This module controls one half of ADC7352.  The ADC7352
// actually contains two channels, but this module only exercises one
// of these channels.  Two instances can be used to control both.  This
// way it is easier to parameterize for the number of channels being used.
// SCLK is limited to 48 MHz which is the maximum speed that is supported
// by the ADC7352.

`timescale 1 ns / 1 ps

module adc7352_if #(
    parameter integer ADC_BITS = 12

)
(
	
  // Interface to ADC
  output wire CSn,
  input  wire SDATA,
  input  wire SCLK,

  // Interface to system
  input  wire CLK,
  input  wire UPDATE,
  input  wire SCLK_RESETn,
  input  wire RESETn,
  output wire  [ADC_BITS-1:0] DOUT,
  output wire  DOUT_VLD

);


localparam integer LEADING_ZEROS = 2-1;
localparam integer QUIET = 2;

// State machine states
localparam [3:0] 
  sm_IDLE = 4'b1000,
  sm_LDZ_A = 4'b0000,
  sm_ACQ_A = 4'b0100,
  sm_SEND_VLD = 4'b1001;

reg [3:0] sm = sm_IDLE; 
reg [3:0] cntdown = 'h0;
reg [ADC_BITS-1:0] data_a = 'h0;

wire shift;

reg update_reg = 1'b0;

wire update_sclk;
wire update_sclk_off;
wire vld_sync;
reg vld_sync_reg;
reg vld_reg;

reg [ADC_BITS-1:0] porta_hold;

assign CSn = sm[3];
assign shift = sm[2];
assign vld = sm[0];

assign DOUT_VLD = !vld_sync_reg && vld_sync;

always @(negedge SCLK)
begin
    if (SCLK_RESETn == 1'b0)
        porta_hold <= 'h0;
    else if (vld)
        porta_hold <= data_a;
end

synchronizer_vector #
(
    .DATA_WIDTH     (ADC_BITS)
)
sync_porta
(
    .old_clk        (~SCLK),
    .data_in        (porta_hold),
    .new_clk        (CLK),
    .data_out       (DOUT)
);

synchronizer_vector #
(
    .DATA_WIDTH     (1)
)
sync_vld
(
    .old_clk        (~SCLK),
    .data_in        (vld_reg),
    .new_clk        (CLK),
    .data_out       (vld_sync)
);

synchronizer_simple sync_sample
(
    .data_in    (update_reg),
    .new_clk    (~SCLK),
    .data_out   (update_sclk)
);

synchronizer_simple sync_sample_off
(
    .data_in    (update_sclk),
    .new_clk    (~SCLK),
    .data_out   (update_sclk_off)
);

always @(posedge CLK)
begin
  if (RESETn == 1'b0)
    update_reg <= 1'b0;
  else if (update_sclk_off)
    update_reg <= 1'b0;
  else if (UPDATE)
    update_reg <= 1'b1;
end

always @(negedge SCLK)
begin
  if (SCLK_RESETn == 1'b0)
    sm <= sm_IDLE;
  else
    case (sm) 
      sm_IDLE: begin
	// Implementing QUIET countdown
        if (cntdown > 0)
          cntdown <= cntdown - 1'b1;
        else if (update_sclk) begin
          cntdown <= LEADING_ZEROS;
          sm <= sm_LDZ_A;
        end
      end
      
      sm_LDZ_A:
        if (cntdown > 0)
          cntdown <= cntdown - 1'b1;
        else begin
          sm <= sm_ACQ_A;
          cntdown <= ADC_BITS-1; 
        end
        
      sm_ACQ_A:
        if (cntdown > 0)
          cntdown <= cntdown - 1'b1;
        else begin
          sm <= sm_SEND_VLD;
       end
      
      sm_SEND_VLD:
      begin
        sm <= sm_IDLE;
        cntdown <= QUIET;
      end
      default: sm <= sm_IDLE;
    endcase
end

always @(negedge SCLK)
begin
  if (SCLK_RESETn == 1'b0 || sm == sm_IDLE) 
    data_a <= 'h0;

  else if (shift) 
  begin
    data_a <= data_a << 1;
    data_a[0] <= SDATA;  
  end 
  else if (DOUT_VLD) 
    data_a <= 'h0;
  else
    data_a[0] <= SDATA;
end

always @(posedge CLK)
    vld_sync_reg <= vld_sync;

always @(negedge SCLK)
    vld_reg <= vld;

endmodule
