/*******************************************************************************
// Copyright (C) 2023, Advanced Micro Devices, Inc.
// SPDX-License-Identifier: Apache-2.0

*******************************************************************************/
//
// This module changes a data vector from one clock domain to another, safely.
// It holds the data in the old clock domain when valid signal
// is high, then sends a synchronization signal across to the new clock
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

module synchronizer_vector_vld #
(
    parameter DATA_WIDTH = 1,
    parameter INIT_VAL = 0
)
(
    input                        data_in_clk,
    input       [DATA_WIDTH-1:0] data_in,
    input                        data_in_vld,
    input                        data_out_clk,
    output reg [DATA_WIDTH-1:0]  data_out,
    output reg                   data_out_vld,
    output wire                  busy
);


(* ASYNC_REG = "TRUE", SHIFT_EXTRACT = "NO" *) reg change_sync_1reg = INIT_VAL;
(* ASYNC_REG = "TRUE", SHIFT_EXTRACT = "NO" *) reg change_sync_2reg = INIT_VAL;
(* ASYNC_REG = "TRUE", SHIFT_EXTRACT = "NO" *) reg done_sync_1reg = INIT_VAL;
(* ASYNC_REG = "TRUE", SHIFT_EXTRACT = "NO" *) reg done_sync_2reg = INIT_VAL;
reg [DATA_WIDTH-1:0] hold_reg = {DATA_WIDTH{INIT_VAL}}; //initially

parameter IDLE = 0, HOLD = 1, RELEASE = 2;
reg [1:0] state = IDLE;  //initially in IDLE
reg done_new, done_new_reg = 1'b0;
//reg done_old = 1'b0;
wire new_clk;
wire old_clk;

assign new_clk = data_out_clk;
assign old_clk = data_in_clk;

always @(posedge old_clk)
begin
  case (state)

    IDLE: if (data_in_vld)
            state <= HOLD;
    HOLD: if (done_sync_2reg)
            state <= RELEASE;
    RELEASE: if (!done_sync_2reg)
            state <= IDLE;
    default: state <= IDLE;
  endcase
end

// Register input data to hold for clock crossing
always @(posedge old_clk)
begin
  if (data_in_vld)
      hold_reg <= data_in;
end

// Register enable signal twice for metastability crossing into new clock domain
always @(posedge new_clk)
begin
  change_sync_1reg <= (state == HOLD);
  change_sync_2reg <= change_sync_1reg;
  // use for edge detect
  done_new <= change_sync_2reg;
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
  if (change_sync_2reg && !done_new)
    begin
      data_out <= hold_reg;
      data_out_vld <= 1'b1;
    end
  else
    data_out_vld <= 1'b0;
end

assign busy = (state != IDLE);


endmodule

