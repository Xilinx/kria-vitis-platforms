// --(C) Copyright 2020 - 2021 Xilinx, Inc. 
// --Copyright (C) 2022 - 2023, Advanced Micro Devices, Inc 
// --SPDX-License-Identifier: Apache-2.0


`timescale 1ns / 1ps
/////////////////////////////////////////////////////////////////
// Module Name: dff_en_reset_vector
/////////////////////////////////////////////////////////////////
module dff_en_reset_vector #(parameter SIZE = 4, DELAY = 3)(
    input wire [SIZE-1:0] d,
    input wire clk,
    input wire en,
    input wire reset,
    output reg [SIZE-1:0] q
    );
    
    always @(posedge clk)
    begin 
        if(reset)
            q<= #DELAY 0;
        else if(en)
            q<= #DELAY d;            
    end
    
endmodule
