/*******************************************************************************
// © Copyright 2010 - 2022 Xilinx, Inc.
// Copyright (C) 2023, Advanced Micro Devices, Inc.
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

