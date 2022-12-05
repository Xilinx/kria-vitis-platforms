// Copyright (C) 2022, Advanced Micro Devices, Inc.
// SPDX-License-Identifier: Apache-2.0

// This module controls the over/under fault generation and is instantiated
// one per channel.  It is kept generic so it can be used for voltage or
// current.

`timescale 1 ns / 1 ps

  module motor_fault_gen #(
    parameter integer SIGNED = 1
  )
  (
    input wire  clk,
    input wire  resetn,
    input wire [31:0] data_value,
    input wire [31:0] over_limit,
    input wire [31:0] under_limit,
    input wire [31:0] over_limit_enable,
    input wire [31:0] under_limit_enable,
    input wire [31:0] clear_over_error,
    input wire [31:0] clear_under_error,
    output wire [31:0] error_status
  );

  reg over_limit_err;
  reg under_limit_err;

  // Values on AXIS are in Q15.Q16 mode
  // Q16 LSB [15:0] used for decimal portion
  // Q15 MSB [30:16] used for integer portion
  // bit[31] indicates sign 1'b1: -ve ; 1'b0: +ve 

  //           <------------ integer ----------->  <---------------- decimal ----------------->
  //  _______  _______  _______  _______  _______   _______  _______  _______  _______  _______
  // |       ||       ||       ||       ||       | |       ||       ||       ||       ||       | 
  // |  Sign ||  2^15 ||  .... ||   2^1 ||  2^0  |.|  2^-1 ||  ---  || 2^-14 || 2^-15 || 2^-16 | 
  // |_______||_______||_______||_______||_______| |_______||_______||_______||_______||_______|
  //

  // Declare as signed (for both signed or unsigned case)
    wire signed [32:0]  data_value_i;
    wire signed [32:0]  over_limit_i;
    wire signed [32:0]  under_limit_i;

    // Use extra bit to make effectively switch between signed an unsigned
    assign data_value_i = (SIGNED) ? {data_value[31], data_value} : {1'b0, data_value};
    assign over_limit_i = (SIGNED) ? {over_limit[31], over_limit} : {1'b0, over_limit};
    assign under_limit_i = (SIGNED) ? {under_limit[31], under_limit} : {1'b0, under_limit};

    always @( posedge clk )
    begin
      if ( resetn == 1'b0 )
        begin
          over_limit_err <= 1'b0;
        end
      else
        begin
        // set error if value read is more than the programmed maximum limit value
        // since the values are signed numbers the check will work for both
        // postive and neg numbers
          if ((data_value_i > over_limit_i) && (over_limit_enable == 1'b1))
            over_limit_err <= 1'b1;
          else if (clear_over_error == 32'h0BAD00FF)
            over_limit_err <= 1'b0;
          // Hold error until clear 
          else
            over_limit_err <= over_limit_err;
        end
    end

    always @( posedge clk )
    begin
      if ( resetn == 1'b0 )
        begin
          under_limit_err <= 1'b0;
        end
      else
        begin
        // set error if value read is less than the programmed minimum limit value
        // since the values are signed numbers the check will work for both
        // postive and neg numbers
          if ((data_value_i < under_limit_i) && (under_limit_enable == 1'b1))
            under_limit_err <= 1'b1;
          else if (clear_under_error == 32'h0BAD00FF)
            under_limit_err <= 1'b0;
          // Hold error until clear
          else
            under_limit_err <= under_limit_err;
        end
    end

  assign error_status = {30'b0, under_limit_err, over_limit_err};

  endmodule
