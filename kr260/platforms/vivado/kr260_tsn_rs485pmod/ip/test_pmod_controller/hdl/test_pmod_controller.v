// (C) Copyright 2020 - 2022 Xilinx, Inc.
// SPDX-License-Identifier: Apache-2.0

`timescale 1 ns / 1 ps

module test_pmod_controller #
(

	// Parameters of AXI Slave Bus Interface S00_AXI
	parameter integer C_S00_AXI_DATA_WIDTH	= 32,
	parameter integer C_S00_AXI_ADDR_WIDTH	= 5
)
(

	output wire pub1_txs,
	output wire lis1_rxs,
	output wire pub2_txb,
	output wire lis2_rxb,
	output wire pub3_txx,
	output wire lis3_rxx,

	input wire  in_tx_s,
	input wire  in_rx_s,
	input wire  in_tx_b,
	input wire  in_rx_b,
	input wire  in_tx_x,
	input wire  in_rx_x,

	input wire  s00_axi_aclk,
	input wire  s00_axi_aresetn,
	input wire [C_S00_AXI_ADDR_WIDTH-1 : 0] s00_axi_awaddr,
	input wire [2 : 0] s00_axi_awprot,
	input wire  s00_axi_awvalid,
	output wire  s00_axi_awready,
	input wire [C_S00_AXI_DATA_WIDTH-1 : 0] s00_axi_wdata,
	input wire [(C_S00_AXI_DATA_WIDTH/8)-1 : 0] s00_axi_wstrb,
	input wire  s00_axi_wvalid,
	output wire  s00_axi_wready,
	output wire [1 : 0] s00_axi_bresp,
	output wire  s00_axi_bvalid,
	input wire  s00_axi_bready,
	input wire [C_S00_AXI_ADDR_WIDTH-1 : 0] s00_axi_araddr,
	input wire [2 : 0] s00_axi_arprot,
	input wire  s00_axi_arvalid,
	output wire  s00_axi_arready,
	output wire [C_S00_AXI_DATA_WIDTH-1 : 0] s00_axi_rdata,
	output wire [1 : 0] s00_axi_rresp,
	output wire  s00_axi_rvalid,
	input wire  s00_axi_rready

);

//-- Number of Slave Registers 6
wire [C_S00_AXI_DATA_WIDTH-1:0]	slv_reg0;
wire [C_S00_AXI_DATA_WIDTH-1:0]	slv_reg1;
wire [C_S00_AXI_DATA_WIDTH-1:0]	slv_reg2;
wire [C_S00_AXI_DATA_WIDTH-1:0]	slv_reg3;
wire [C_S00_AXI_DATA_WIDTH-1:0]	slv_reg4;
wire [C_S00_AXI_DATA_WIDTH-1:0]	slv_reg5;
wire [C_S00_AXI_DATA_WIDTH-1:0]	slv_reg6;

wire slv_reg0_vld;
wire slv_reg1_vld;
wire slv_reg2_vld;
wire slv_reg3_vld;
wire slv_reg4_vld;
wire slv_reg5_vld;
wire slv_reg6_vld;

reg [7:0] count_1us;
reg en_1us;

reg [3:0] i1, i2, i3, i4, i5, i6;
reg r1;
reg r2;
reg r3;
reg r4;
reg r5;
reg r6;

reg send_r1;
reg send_r2;
reg send_r3;
reg send_r4;
reg send_r5;
reg send_r6;

assign pub1_txs = (slv_reg0[0] == 1'b1) ? in_tx_s : r1;
assign pub2_txb = (slv_reg0[0] == 1'b1) ? in_tx_b : r3;
assign pub3_txx = (slv_reg0[0] == 1'b1) ? in_tx_x : r5;

assign lis1_rxs = (slv_reg0[0] == 1'b1) ? in_rx_s : r2;
assign lis2_rxb = (slv_reg0[0] == 1'b1) ? in_rx_b : r4;
assign lis3_rxx = (slv_reg0[0] == 1'b1) ? in_rx_x : r6;



reg_interface_v1_0_S00_AXI # ( 
	.C_S_AXI_DATA_WIDTH(C_S00_AXI_DATA_WIDTH),
	.C_S_AXI_ADDR_WIDTH(C_S00_AXI_ADDR_WIDTH)
) reg_interface_v1_0_S00_AXI_inst (
	.S_AXI_ACLK(s00_axi_aclk),
	.S_AXI_ARESETN(s00_axi_aresetn),
	.S_AXI_AWADDR(s00_axi_awaddr),
	.S_AXI_AWPROT(s00_axi_awprot),
	.S_AXI_AWVALID(s00_axi_awvalid),
	.S_AXI_AWREADY(s00_axi_awready),
	.S_AXI_WDATA(s00_axi_wdata),
	.S_AXI_WSTRB(s00_axi_wstrb),
	.S_AXI_WVALID(s00_axi_wvalid),
	.S_AXI_WREADY(s00_axi_wready),
	.S_AXI_BRESP(s00_axi_bresp),
	.S_AXI_BVALID(s00_axi_bvalid),
	.S_AXI_BREADY(s00_axi_bready),
	.S_AXI_ARADDR(s00_axi_araddr),
	.S_AXI_ARPROT(s00_axi_arprot),
	.S_AXI_ARVALID(s00_axi_arvalid),
	.S_AXI_ARREADY(s00_axi_arready),
	.S_AXI_RDATA(s00_axi_rdata),
	.S_AXI_RRESP(s00_axi_rresp),
	.S_AXI_RVALID(s00_axi_rvalid),
	.S_AXI_RREADY(s00_axi_rready),

	.slv_reg0_out(slv_reg0),
	.slv_reg1_out(slv_reg1),
	.slv_reg2_out(slv_reg2),
	.slv_reg3_out(slv_reg3),
	.slv_reg4_out(slv_reg4),
	.slv_reg5_out(slv_reg5),
	.slv_reg6_out(slv_reg6),

	.slv_reg0_vld_out(slv_reg0_vld),
	.slv_reg1_vld_out(slv_reg1_vld),
	.slv_reg2_vld_out(slv_reg2_vld),
	.slv_reg3_vld_out(slv_reg3_vld),
	.slv_reg4_vld_out(slv_reg4_vld),
	.slv_reg5_vld_out(slv_reg5_vld),
	.slv_reg6_vld_out(slv_reg6_vld)

);

// User logic 

// Generate enable every 1 us
always @( posedge s00_axi_aclk )
begin
  if (s00_axi_aresetn == 1'b0)
    begin
      count_1us <= 0;
      en_1us <= 1'b0;
    end
  // drive enable every 1us 
  // s00_axi_aclk is 100 MHz = 10ns
  // 10ns x 100 count = 1000 ns = 1us
  // 100 count = h'64,
  else if (count_1us == 8'h63)
    begin
      count_1us <= 0;
      en_1us <= 1'b1;
    end
  else 
    begin
      count_1us <= count_1us + 1;
      en_1us <= 1'b0;
    end
end    

// Drive slv_reg data on the test mod pin
// serially every 1 us
always @( posedge s00_axi_aclk )
begin
  if ( s00_axi_aresetn == 1'b0)
    begin
      r1 <= 0;
      i1 <= 0;
      send_r1 <= 1'b0;
    end 
   else if (i1 == 8 && en_1us == 1'b1)
    begin
       i1 = 0;
       r1 <= 0; 
       send_r1 <= 1'b0;
    end    
   else if (send_r1 == 1'b1 && en_1us == 1'b1) 
    begin  
      r1 <= slv_reg1[i1];	     	      
      i1 <= i1+1;
    end
   // if in the middle of publishing data - send_r1 is 1'b1
   // ignore new register write.
   // ideally register write will not happen within 8us 
   // 8us = 8bit pattern, 1us per bit  
   else if (slv_reg1_vld == 1'b1 && send_r1== 1'b0)
    begin
      i1 = 0;
      send_r1 <= 1'b1; 
    end 
end   

always @( posedge s00_axi_aclk )
begin
  if ( s00_axi_aresetn == 1'b0 )
    begin
      r2 <= 0;
      i2 <= 0;
      send_r2 <= 1'b0; 
    end 
    else if (i2 == 8 && en_1us == 1'b1)
    begin
       i2 = 0;
       r2 <= 0; 
      send_r2 <= 1'b0; 
    end    
   else if (send_r2 == 1'b1 && en_1us == 1'b1) 
    begin    
      r2 <= slv_reg2[i2];	     	      
      i2 <= i2+1;
    end
   else if (slv_reg2_vld == 1'b1 && send_r2 == 1'b0)
    begin
      i2 = 0;
      send_r2 <= 1'b1; 
     end  
end   

always @( posedge s00_axi_aclk )
begin
  if ( s00_axi_aresetn == 1'b0 )
    begin
      r3 <= 0;
      i3 <= 0;
      send_r3 <= 1'b0; 
    end 
   else if (i3 == 8 && en_1us == 1'b1)
    begin
       i3 = 0;
       r3 <= 0; 
      send_r3 <= 1'b0; 
    end    
   else if (send_r3 == 1'b1 && en_1us == 1'b1) 
    begin    
      r3 <= slv_reg3[i3];	     	      
      i3 <= i3+1;
    end
   else if (slv_reg3_vld == 1'b1 && send_r3 == 1'b0)
    begin
      i3 = 0;
      send_r3 <= 1'b1; 
     end  
end   

always @( posedge s00_axi_aclk )
begin
  if ( s00_axi_aresetn == 1'b0 )
    begin
      r4 <= 0;
      i4 <= 0;
      send_r4 <= 1'b0; 
    end 
   else if (i4 == 8 && en_1us == 1'b1)
    begin
       i4 = 0;
       r4 <= 0; 
      send_r4 <= 1'b0; 
    end    
   else if (send_r4 == 1'b1 && en_1us == 1'b1) 
    begin    
      r4 <= slv_reg4[i4];	     	      
      i4 <= i4+1;
    end
   else if (slv_reg4_vld == 1'b1 && send_r4 == 1'b0)
    begin
      i4 = 0;
      send_r4 <= 1'b1; 
     end  
end   

always @( posedge s00_axi_aclk )
begin
  if ( s00_axi_aresetn == 1'b0 )
    begin
      r5 <= 0;
      i5 <= 0;
      send_r5 <= 1'b0; 
    end 
   else if (i5 == 8 && en_1us == 1'b1)
    begin
       i5 = 0;
       r5 <= 0; 
      send_r5 <= 1'b0; 
    end    
   else if (send_r5 == 1'b1 && en_1us == 1'b1) 
    begin    
      r5 <= slv_reg5[i5];	     	      
      i5 <= i5+1;
    end
   else if (slv_reg5_vld == 1'b1 && send_r5 == 1'b0)
    begin
      i5 = 0;
      send_r5 <= 1'b1; 
     end  
end   

always @(posedge s00_axi_aclk )
begin
  if (s00_axi_aresetn == 1'b0)
    begin
      r6 <= 0;
      i6 <= 0;
      send_r6 <= 1'b0; 
    end 
   else if (i6 == 8 && en_1us == 1'b1)
    begin
      i6 = 0;
      r6 <= 0; 
      send_r6 <= 1'b0; 
    end    
   //send each bit of the pattern in the slave register
   //every 1us
   else if (send_r6 == 1'b1 && en_1us == 1'b1) 
    begin    
      r6 <= slv_reg6[i6];	     	      
      i6 <= i6+1;
    end
   // if in the middle of publishing data - send_r6 is 1'b1
   // ignore new register write.
   // ideally register write will not happen within 8us 
   // 8us = 8bit pattern, 1us per bit 
   else if (slv_reg6_vld == 1'b1 && send_r6 == 1'b0)
    begin
      i6 = 0;
      send_r6 <= 1'b1; 
    end 
end   
	
	
endmodule
