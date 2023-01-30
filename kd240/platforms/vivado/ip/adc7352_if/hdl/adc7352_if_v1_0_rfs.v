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
localparam integer QUIET = 0;

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
    .new_clk    (~SCLK), // Shouldn't this be CLK?
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


/*******************************************************************************
// © Copyright 2010 - 2022 Xilinx, Inc.
// Copyright (C) 2022, Advanced Micro Devices, Inc.
// SPDX-License-Identifier: Apache-2.0

*******************************************************************************/
//
// This module changes signals from one domain to another, when the signals
// have no relation to each other.  For signals that are vectors, use the 
// synchronizer_vector module instead.
//
// This module has variable width to synchronize several signals using
// one instantiation of the module.  Uses a simple double register scheme to
// change clock domains.
//
`timescale 1ps / 1ps

module synchronizer_simple #
(
   parameter DATA_WIDTH = 1,
   parameter INIT_VAL = 0
)
(
   input       [DATA_WIDTH-1:0] data_in,
   input                        new_clk,
   output wire [DATA_WIDTH-1:0] data_out
);

(* ASYNC_REG = "TRUE", SHIFT_EXTRACT = "NO" *) reg [DATA_WIDTH-1:0] sync_1reg = {DATA_WIDTH{INIT_VAL}};
(* ASYNC_REG = "TRUE", SHIFT_EXTRACT = "NO" *) reg [DATA_WIDTH-1:0] sync_2reg = {DATA_WIDTH{INIT_VAL}};

// Register twice for metastability
always @(posedge new_clk)
begin
  sync_1reg <= data_in;
  sync_2reg <= sync_1reg;
end

assign data_out = sync_2reg;

endmodule



/*******************************************************************************
// Â© Copyright 2010 - 2022 Xilinx, Inc.
// Copyright (C) 2022, Advanced Micro Devices, Inc.
// SPDX-License-Identifier: Apache-2.0

*******************************************************************************/
//
// This module changes a data vector from one clock domain to another, safely.
// It holds the data in the old clock domain when a change in the data value
// is detected, then sends a synchronization signal across to the new clock
// domain (using two flops in the new clock domain for metastability).  When
// the synchronization signal has been seen by the second flop, the held data
// is clocked once in the new clock domain and is ready to use.
//
// This method delays the data by several clocks into the new domain, and can miss
// intermediate values of the data, if the data changes every clock in the old
// domain, for instance.  However, for data that only accumulates, such as status
// bits, or for configuration instructions from software that changes infrequently,
// there will be no problems using this method.
//
`timescale 1ps / 1ps

module synchronizer_vector #
(
    parameter DATA_WIDTH = 1,
    parameter INIT_VAL = 0
)
(
    input                        old_clk,
    input       [DATA_WIDTH-1:0] data_in,
    input                        new_clk,
    output wire [DATA_WIDTH-1:0] data_out
);


(* ASYNC_REG = "TRUE", SHIFT_EXTRACT = "NO" *) reg change_sync_1reg = INIT_VAL;
(* ASYNC_REG = "TRUE", SHIFT_EXTRACT = "NO" *) reg change_sync_2reg = INIT_VAL;
(* ASYNC_REG = "TRUE", SHIFT_EXTRACT = "NO" *) reg done_sync_1reg = INIT_VAL;
(* ASYNC_REG = "TRUE", SHIFT_EXTRACT = "NO" *) reg done_sync_2reg = INIT_VAL;
(* ASYNC_REG = "TRUE", SHIFT_EXTRACT = "NO" *) reg [DATA_WIDTH-1:0] data_reg = {DATA_WIDTH{INIT_VAL}}; //initially
reg [DATA_WIDTH-1:0] hold_reg = {DATA_WIDTH{INIT_VAL}}; //initially
wire data_changed;

parameter IDLE = 0, HOLD = 1;
reg state = IDLE;  //initially in IDLE


always @(posedge old_clk)
begin
  case (state)
  
    IDLE: if (data_changed)
            state <= HOLD;
    HOLD: if (done_sync_2reg)
            state <= IDLE;
  endcase
end

// Register input data to detect change in incoming data, hold when
// change is detected.
always @(posedge old_clk)
begin
  if (state == IDLE)
    hold_reg <= data_in;
end

assign data_changed = (data_in !== hold_reg) ? 1'b1 : 1'b0;

// Register enable signal twice for metastability crossing into new clock domain
always @(posedge new_clk)
begin
  change_sync_1reg <= (state == HOLD);
  change_sync_2reg <= change_sync_1reg;
end

// Register twice for metastability, done signal back to old clock domain
always @(posedge old_clk)
begin
  done_sync_1reg <= change_sync_2reg;
  done_sync_2reg <= done_sync_1reg;
end


// When change signal has been registered twice, allow data to cross from old
// clock domain to new clock domain.  The data should not have changed for 
// two of the new clock cycles, so should be quite stable.
always @(posedge new_clk)
begin
  if (change_sync_2reg)
    data_reg <= hold_reg; 
end

assign data_out = data_reg;

endmodule



