/*******************************************************************************
// © Copyright 2009 - 2022 Xilinx, Inc.
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

