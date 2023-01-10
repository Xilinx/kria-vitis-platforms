// Copyright (C) 2022, Advanced Micro Devices, Inc.
// SPDX-License-Identifier: Apache-2.0

// This implements the AXI control and status registers for the ADC
// Hub.  One instance of this module is used for all ADC channels.

`timescale 1 ns / 1 ps

module adc_hub_axi_cntrl #
(

    parameter integer NUM_CHANNELS = 1,

    // Width of S_AXI data bus
    parameter integer C_S_AXI_DATA_WIDTH  = 32,
    // Width of S_AXI address bus
    parameter integer C_S_AXI_ADDR_WIDTH  = 12

)
(
    // Users to add ports here

    output  wire signed [(32*NUM_CHANNELS)-1:0] over_limit,
    output  wire signed [(32*NUM_CHANNELS)-1:0] under_limit,
    output  wire [(32*NUM_CHANNELS)-1:0] over_limit_enable,
    output  wire [(32*NUM_CHANNELS)-1:0] under_limit_enable,
    output  wire [(32*NUM_CHANNELS)-1:0] global_irq_disable,
    output  wire [(32*NUM_CHANNELS)-1:0] scale_factor,
    output  wire [(32*NUM_CHANNELS)-1:0] num_samples,
    input   wire [(32*NUM_CHANNELS)-1:0] error_status,
    input   wire [(32*NUM_CHANNELS)-1:0] raw_adc,
    input   wire [(32*NUM_CHANNELS)-1:0] qscale_data,
    input   wire [NUM_CHANNELS-1:0] data_signed,
    input   wire [NUM_CHANNELS-1:0] data_type,
    output  wire [(32*NUM_CHANNELS)-1:0] clear_under_error,
    output  wire [(32*NUM_CHANNELS)-1:0] clear_over_error,
    output  wire [(32*NUM_CHANNELS)-1:0] offset,

    // Global Clock Signal
    input wire  S_AXI_ACLK,
    // Global Reset Signal. This Signal is Active LOW
    input wire  S_AXI_ARESETN,
    // Write address (issued by master, acceped by Slave)
    input wire [C_S_AXI_ADDR_WIDTH-1 : 0] S_AXI_AWADDR,
    // Write channel Protection type. This signal indicates the
        // privilege and security level of the transaction, and whether
        // the transaction is a data access or an instruction access.
    input wire [2 : 0] S_AXI_AWPROT,
    // Write address valid. This signal indicates that the master signaling
        // valid write address and control information.
    input wire  S_AXI_AWVALID,
    // Write address ready. This signal indicates that the slave is ready
        // to accept an address and associated control signals.
    output wire  S_AXI_AWREADY,
    // Write data (issued by master, acceped by Slave) 
    input wire [C_S_AXI_DATA_WIDTH-1 : 0] S_AXI_WDATA,
    // Write strobes. This signal indicates which byte lanes hold
        // valid data. There is one write strobe bit for each eight
        // bits of the write data bus.    
    input wire [(C_S_AXI_DATA_WIDTH/8)-1 : 0] S_AXI_WSTRB,
    // Write valid. This signal indicates that valid write
        // data and strobes are available.
    input wire  S_AXI_WVALID,
    // Write ready. This signal indicates that the slave
        // can accept the write data.
    output wire  S_AXI_WREADY,
    // Write response. This signal indicates the status
        // of the write transaction.
    output wire [1 : 0] S_AXI_BRESP,
    // Write response valid. This signal indicates that the channel
        // is signaling a valid write response.
    output wire  S_AXI_BVALID,
    // Response ready. This signal indicates that the master
        // can accept a write response.
    input wire  S_AXI_BREADY,
    // Read address (issued by master, acceped by Slave)
    input wire [C_S_AXI_ADDR_WIDTH-1 : 0] S_AXI_ARADDR,
    // Protection type. This signal indicates the privilege
        // and security level of the transaction, and whether the
        // transaction is a data access or an instruction access.
    input wire [2 : 0] S_AXI_ARPROT,
    // Read address valid. This signal indicates that the channel
        // is signaling valid read address and control information.
    input wire  S_AXI_ARVALID,
    // Read address ready. This signal indicates that the slave is
        // ready to accept an address and associated control signals.
    output wire  S_AXI_ARREADY,
    // Read data (issued by slave)
    output wire [C_S_AXI_DATA_WIDTH-1 : 0] S_AXI_RDATA,
    // Read response. This signal indicates the status of the
        // read transfer.
    output wire [1 : 0] S_AXI_RRESP,
    // Read valid. This signal indicates that the channel is
        // signaling the required read data.
    output wire  S_AXI_RVALID,
    // Read ready. This signal indicates that the master can
        // accept the read data and response information.
    input wire  S_AXI_RREADY
);

  // AXI4LITE signals
  reg [C_S_AXI_ADDR_WIDTH-1 : 0]  axi_awaddr;
  reg   axi_awready;
  reg   axi_wready;
  reg [1 : 0]   axi_bresp;
  reg   axi_bvalid;
  reg [C_S_AXI_ADDR_WIDTH-1 : 0]  axi_araddr;
  reg   axi_arready;
  reg [C_S_AXI_DATA_WIDTH-1 : 0]  axi_rdata;
  reg [1 : 0]   axi_rresp;
  reg   axi_rvalid;

  // Example-specific design signals
  // local parameter for addressing 32 bit / 64 bit C_S_AXI_DATA_WIDTH
  // ADDR_LSB is used for addressing 32/64 bit registers/memories
  // ADDR_LSB = 2 for 32 bits (n downto 2)
  // ADDR_LSB = 3 for 64 bits (n downto 3)
  localparam integer ADDR_LSB = (C_S_AXI_DATA_WIDTH/32) + 1;
  localparam integer OPT_MEM_ADDR_BITS = 3;
  // CHAN_BITS = [CHAN_BIT_MAX:CHAN_BIT_MIN] - which channel is being addressed (16 supported)
  localparam integer CHAN_BIT_MAX = 11;
  localparam integer CHAN_BIT_MIN = 8;

  //----------------------------------------------
  //-- Signals for user logic register space example
  //------------------------------------------------
  //-- Read/Write
  reg [(C_S_AXI_DATA_WIDTH*NUM_CHANNELS)-1:0] slv_reg0;  // over_limit
  reg [(C_S_AXI_DATA_WIDTH*NUM_CHANNELS)-1:0] slv_reg1;  // under_limit
  reg [(C_S_AXI_DATA_WIDTH*NUM_CHANNELS)-1:0] slv_reg2;  // scale_factor
  reg [(C_S_AXI_DATA_WIDTH*NUM_CHANNELS)-1:0] slv_reg3;  // num_samples
  reg [(C_S_AXI_DATA_WIDTH*NUM_CHANNELS)-1:0] slv_reg7;  // clear_over_error
  reg [(C_S_AXI_DATA_WIDTH*NUM_CHANNELS)-1:0] slv_reg8;  // clear_under_error
  reg [(C_S_AXI_DATA_WIDTH*NUM_CHANNELS)-1:0] slv_reg9;  // over limit enable
  reg [(C_S_AXI_DATA_WIDTH*NUM_CHANNELS)-1:0] slv_reg10; // under limit enable
  reg [(C_S_AXI_DATA_WIDTH*NUM_CHANNELS)-1:0] slv_reg14; // global_irq_disable
  reg [(C_S_AXI_DATA_WIDTH*NUM_CHANNELS)-1:0] slv_reg15; // offset
  // Read-only
  wire [(C_S_AXI_DATA_WIDTH*NUM_CHANNELS)-1:0] slv_reg4; // raw_data
  wire [(C_S_AXI_DATA_WIDTH*NUM_CHANNELS)-1:0] slv_reg5; // qscale_data
  wire [(C_S_AXI_DATA_WIDTH*NUM_CHANNELS)-1:0] slv_reg6; // error_status
  wire [(C_S_AXI_DATA_WIDTH*NUM_CHANNELS)-1:0] slv_reg11; // signed/unsigned
  wire [(C_S_AXI_DATA_WIDTH*NUM_CHANNELS)-1:0] slv_reg12; // type voltage/current
  wire [(C_S_AXI_DATA_WIDTH*NUM_CHANNELS)-1:0] slv_reg13; // num_channels



  reg [(C_S_AXI_DATA_WIDTH*NUM_CHANNELS)-1:0] reg_data_out;

  wire  slv_reg_rden;
  wire  slv_reg_wren;

  integer  byte_index;
  reg  aw_en;
  wire [3:0] rd_chan;
  wire [3:0] wr_chan;

  genvar chan;

  // I/O Connections assignments

  assign S_AXI_AWREADY  = axi_awready;
  assign S_AXI_WREADY = axi_wready;
  assign S_AXI_BRESP  = axi_bresp;
  assign S_AXI_BVALID = axi_bvalid;
  assign S_AXI_ARREADY  = axi_arready;
  assign S_AXI_RDATA  = axi_rdata;
  assign S_AXI_RRESP  = axi_rresp;
  assign S_AXI_RVALID = axi_rvalid;

  // Implement axi_awready generation
  // axi_awready is asserted for one S_AXI_ACLK clock cycle when both
  // S_AXI_AWVALID and S_AXI_WVALID are asserted. axi_awready is
  // de-asserted when reset is low.

  always @( posedge S_AXI_ACLK )
  begin
    if ( S_AXI_ARESETN == 1'b0 )
      begin
        axi_awready <= 1'b0;
        aw_en <= 1'b1;
      end 
    else
      begin    
        if (~axi_awready && S_AXI_AWVALID && S_AXI_WVALID && aw_en)
          begin
            // slave is ready to accept write address when 
            // there is a valid write address and write data
            // on the write address and data bus. This design 
            // expects no outstanding transactions. 
            axi_awready <= 1'b1;
            aw_en <= 1'b0;
          end
          else if (S_AXI_BREADY && axi_bvalid)
              begin
                aw_en <= 1'b1;
                axi_awready <= 1'b0;
              end
        else
          begin
            axi_awready <= 1'b0;
          end
      end
  end

  // Implement axi_awaddr latching
  // This process is used to latch the address when both
  // S_AXI_AWVALID and S_AXI_WVALID are valid.

  always @( posedge S_AXI_ACLK )
  begin
    if ( S_AXI_ARESETN == 1'b0 )
      begin
        axi_awaddr <= 0;
      end 
    else
      begin    
        if (~axi_awready && S_AXI_AWVALID && S_AXI_WVALID && aw_en)
          begin
            // Write Address latching
            axi_awaddr <= S_AXI_AWADDR;
          end
      end
  end

  // Implement axi_wready generation
  // axi_wready is asserted for one S_AXI_ACLK clock cycle when both
  // S_AXI_AWVALID and S_AXI_WVALID are asserted. axi_wready is
  // de-asserted when reset is low. 

  always @( posedge S_AXI_ACLK )
  begin
    if ( S_AXI_ARESETN == 1'b0 )
      begin
        axi_wready <= 1'b0;
      end 
    else
      begin    
        if (~axi_wready && S_AXI_WVALID && S_AXI_AWVALID && aw_en )
          begin
            // slave is ready to accept write data when
            // there is a valid write address and write data
            // on the write address and data bus. This design
            // expects no outstanding transactions.
            axi_wready <= 1'b1;
          end
        else
          begin
            axi_wready <= 1'b0;
          end
      end 
  end

  // Implement memory mapped register select and write logic generation
  // The write data is accepted and written to memory mapped registers when
  // axi_awready, S_AXI_WVALID, axi_wready and S_AXI_WVALID are asserted.
  // Write strobes are used to select byte enables of slave registers while writing.
  // These registers are cleared when reset (active low) is applied.
  // Slave register write enable is asserted when valid address and data are available
  // and the slave is ready to accept the write address and write data.
  assign slv_reg_wren = axi_wready && S_AXI_WVALID && axi_awready && S_AXI_AWVALID;

  assign rd_chan = axi_araddr[CHAN_BIT_MAX:CHAN_BIT_MIN];
  assign wr_chan = axi_awaddr[CHAN_BIT_MAX:CHAN_BIT_MIN];

  // Implement write response logic generation
  // The write response and response valid signals are asserted by the slave
  // when axi_wready, S_AXI_WVALID, axi_wready and S_AXI_WVALID are asserted.
  // This marks the acceptance of address and indicates the status of
  // write transaction.

  always @( posedge S_AXI_ACLK )
  begin
    if ( S_AXI_ARESETN == 1'b0 )
      begin
        axi_bvalid  <= 0;
        axi_bresp   <= 2'b0;
      end 
    else
      begin    
        if (axi_awready && S_AXI_AWVALID && ~axi_bvalid && axi_wready && S_AXI_WVALID)
          begin
            // indicates a valid write response is available
            axi_bvalid <= 1'b1;
            axi_bresp  <= 2'b0; // 'OKAY' response
          end                   // work error responses in future
        else
          begin
            if (S_AXI_BREADY && axi_bvalid) 
              //check if bready is asserted while bvalid is high)
              //(there is a possibility that bready is always asserted high)
              begin
                axi_bvalid <= 1'b0;
              end
          end
      end
  end

  // Implement axi_arready generation
  // axi_arready is asserted for one S_AXI_ACLK clock cycle when
  // S_AXI_ARVALID is asserted. axi_awready is
  // de-asserted when reset (active low) is asserted.
  // The read address is also latched when S_AXI_ARVALID is
  // asserted. axi_araddr is reset to zero on reset assertion.

  always @( posedge S_AXI_ACLK )
  begin
    if ( S_AXI_ARESETN == 1'b0 )
      begin
        axi_arready <= 1'b0;
        axi_araddr  <= 32'b0;
      end
    else
      begin
        if (~axi_arready && S_AXI_ARVALID)
          begin
            // indicates that the slave has acceped the valid read address
            axi_arready <= 1'b1;
            // Read address latching
            axi_araddr  <= S_AXI_ARADDR;
          end
        else
          begin
            axi_arready <= 1'b0;
          end
      end
  end

  // Implement axi_arvalid generation
  // axi_rvalid is asserted for one S_AXI_ACLK clock cycle when both
  // S_AXI_ARVALID and axi_arready are asserted. The slave registers
  // data are available on the axi_rdata bus at this instance. The
  // assertion of axi_rvalid marks the validity of read data on the
  // bus and axi_rresp indicates the status of read transaction.axi_rvalid
  // is deasserted on reset (active low). axi_rresp and axi_rdata are
  // cleared to zero on reset (active low).
  always @( posedge S_AXI_ACLK )
  begin
    if ( S_AXI_ARESETN == 1'b0 )
      begin
        axi_rvalid <= 0;
        axi_rresp  <= 0;
      end 
    else
      begin
        if (axi_arready && S_AXI_ARVALID && ~axi_rvalid)
          begin
            // Valid read data is available at the read data bus
            axi_rvalid <= 1'b1;
            axi_rresp  <= 2'b0; // 'OKAY' response
          end
        else if (axi_rvalid && S_AXI_RREADY)
          begin
            // Read data is accepted by the master
            axi_rvalid <= 1'b0;
          end
      end
  end

  // Implement memory mapped register select and read logic generation
  // Slave register read enable is asserted when valid address is available
  // and the slave is ready to accept the read address.
  assign slv_reg_rden = axi_arready & S_AXI_ARVALID & ~axi_rvalid;

  // Output register or memory read data
  always @( posedge S_AXI_ACLK )
  begin
    if ( S_AXI_ARESETN == 1'b0 )
      begin
        axi_rdata  <= 0;
      end 
    else
      begin
        // When there is a valid read address (S_AXI_ARVALID) with
        // acceptance of read address by the slave (axi_arready),
        // output the read data
        if (slv_reg_rden)
          begin
            axi_rdata <= reg_data_out[rd_chan*C_S_AXI_DATA_WIDTH+:C_S_AXI_DATA_WIDTH];  // register read data
          end
      end
  end

generate
  for (chan = 0 ; chan < NUM_CHANNELS ; chan = chan + 1)
    begin
      assign over_limit[(32*chan)+:32] = slv_reg0[(32*chan)+:32];
      assign under_limit[(32*chan)+:32] = slv_reg1[(32*chan)+:32];
      assign scale_factor[(32*chan)+:32] = slv_reg2[(32*chan)+:32];
      assign num_samples[(32*chan)+:32] = slv_reg3[(32*chan)+:32];
      assign slv_reg4[(32*chan)+:32] = raw_adc[(32*chan)+:32];
      assign slv_reg5[(32*chan)+:32] = qscale_data[(32*chan)+:32];
      assign slv_reg6[(32*chan)+:32] = error_status[(32*chan)+:32];
      assign clear_over_error[(32*chan)+:32] = slv_reg7[(32*chan)+:32];
      assign clear_under_error[(32*chan)+:32] = slv_reg8[(32*chan)+:32];
      assign over_limit_enable[(32*chan)+:32] = slv_reg9[(32*chan)+:32]; 
      assign under_limit_enable[(32*chan)+:32] = slv_reg10[(32*chan)+:32]; 
      assign slv_reg11[(32*chan)+:32] = data_signed[chan];
      assign slv_reg12[(32*chan)+:32] = data_type[chan];
      assign slv_reg13[(32*chan)+:32] = NUM_CHANNELS;
      assign global_irq_disable[(32*chan)+:32] = slv_reg14[(32*chan)+:32];
      assign offset[(32*chan)+:32] = slv_reg15[(32*chan)+:32];

      always @(*)
      begin
        // Address decoding for reading registers
        case ( axi_araddr[ADDR_LSB+OPT_MEM_ADDR_BITS:ADDR_LSB] )
          4'h0   : reg_data_out[(32*chan)+:32] <= slv_reg0[(32*chan)+:32];
          4'h1   : reg_data_out[(32*chan)+:32] <= slv_reg1[(32*chan)+:32];
          4'h2   : reg_data_out[(32*chan)+:32] <= slv_reg2[(32*chan)+:32];
          4'h3   : reg_data_out[(32*chan)+:32] <= slv_reg3[(32*chan)+:32];
          4'h4   : reg_data_out[(32*chan)+:32] <= slv_reg4[(32*chan)+:32];
          4'h5   : reg_data_out[(32*chan)+:32] <= slv_reg5[(32*chan)+:32];
          4'h6   : reg_data_out[(32*chan)+:32] <= slv_reg6[(32*chan)+:32];
          4'h7   : reg_data_out[(32*chan)+:32] <= slv_reg7[(32*chan)+:32];
          4'h8   : reg_data_out[(32*chan)+:32] <= slv_reg8[(32*chan)+:32];
          4'h9   : reg_data_out[(32*chan)+:32] <= slv_reg9[(32*chan)+:32];
          4'hA   : reg_data_out[(32*chan)+:32] <= slv_reg10[(32*chan)+:32];
          4'hB   : reg_data_out[(32*chan)+:32] <= slv_reg11[(32*chan)+:32];
          4'hC   : reg_data_out[(32*chan)+:32] <= slv_reg12[(32*chan)+:32];
          4'hD   : reg_data_out[(32*chan)+:32] <= slv_reg13[(32*chan)+:32];
          4'hE   : reg_data_out[(32*chan)+:32] <= slv_reg14[(32*chan)+:32];
          4'hF   : reg_data_out[(32*chan)+:32] <= slv_reg15[(32*chan)+:32];
          default : reg_data_out[(32*chan)+:32] <= 0;
        endcase
      end

      // registers 0 to 1 defined in Q15.Q16 format
      // Q16 LSB [15:0] used for decimal portion
      // Q15 MSB [30:16] used for integer portion
      // [31] to indicate sign 1'b1:-ve 1'b0: +ve
      // Example 1.88 A = 32'h0001_E148
      // .88/2^-16 = d'57672 = h'E148
      // 2^-16 is the resolution for LSB
      always @( posedge S_AXI_ACLK )
        begin
          if ( S_AXI_ARESETN == 1'b0 )
            begin
              slv_reg0[(32*chan)+:32] <= 32'h0; // over_limit
              slv_reg1[(32*chan)+:32] <= 32'h0; // under_limit
              slv_reg2[(32*chan)+:32] <= 32'h00000320; // scale_factor (don't set over 0x1000 or Q-Scale will fail 24 bit max)
              slv_reg3[(32*chan)+:32] <= 32'h00000004; // num_samples default value 4
              //slv_reg4[(32*chan)+:32] <= 32'h0; // raw_adc
              //slv_reg5[(32*chan)+:32] <= 32'h0; // qscale_data
              //slv_reg6 <= 0;  // error_status
              slv_reg7[(32*chan)+:32] <= 0;
              slv_reg8[(32*chan)+:32] <= 0;
              slv_reg9[(32*chan)+:32] <= 0;
              slv_reg10[(32*chan)+:32] <= 0; 
              //slv_reg11[(32*chan)+:32] <= 0; // signed/unsigned data
              //slv_reg12[(32*chan)+:32] <= 0; // voltage/current data
              //slv_reg13[(32*chan)+:32] <= 0; // Num Channels (per channel, but same info on each)
              slv_reg14[(32*chan)+:32] <= 32'h0; // global_irq_disable (per channel, but only need one set)
              slv_reg15[(32*chan)+:32] <= 32'h0; // offset
            end 
          else 
            begin
              // auto-clear these registers
              slv_reg7[(32*chan)+:32] <= 0;
              slv_reg8[(32*chan)+:32] <= 0;

              if (slv_reg_wren)
                begin
                  if (chan == wr_chan)
                    case ( axi_awaddr[ADDR_LSB+OPT_MEM_ADDR_BITS:ADDR_LSB] )
                      4'h0:
                        for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                          if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                            // Respective byte enables are asserted as per write strobes
                            // Slave register 0
                            slv_reg0[(C_S_AXI_DATA_WIDTH*chan)+(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                          end  
                      4'h1:
                        for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                          if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                            // Respective byte enables are asserted as per write strobes
                            // Slave register 1
                            slv_reg1[(C_S_AXI_DATA_WIDTH*chan)+(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                          end  
                      4'h2:
                        for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                          if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                            // Respective byte enables are asserted as per write strobes
                            // Slave register 2
                            slv_reg2[(C_S_AXI_DATA_WIDTH*chan)+(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                          end  
                      4'h3:
                        for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                          if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                            // Respective byte enables are asserted as per write strobes
                            // Slave register 3
                            slv_reg3[(C_S_AXI_DATA_WIDTH*chan)+(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                          end
                  /* 4 - 6 are read only
                      4'h4:
                        for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                          if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                            // Respective byte enables are asserted as per write strobes
                            // Slave register 4
                            slv_reg4[(C_S_AXI_DATA_WIDTH*chan)+(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                          end
                      4'h5:
                        for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                          if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                            // Respective byte enables are asserted as per write strobes
                            // Slave register 5
                            slv_reg5[(C_S_AXI_DATA_WIDTH*chan)+(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                          end
                      // Slave reg 6 is a read only register
                      4'h6:
                        for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                          if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                            // Respective byte enables are asserted as per write strobes
                            // Slave register 6
                            slv_reg6[(C_S_AXI_DATA_WIDTH*chan)+(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                          end
                      */
                      4'h7:
                        for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                          if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                            // Respective byte enables are asserted as per write strobes
                            // Slave register 7
                            slv_reg7[(C_S_AXI_DATA_WIDTH*chan)+(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                          end
                      
                      4'h8:
                        for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                          if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                            // Respective byte enables are asserted as per write strobes
                            // Slave register 8
                            slv_reg8[(C_S_AXI_DATA_WIDTH*chan)+(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                          end
                      4'h9:
                        for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                          if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                            // Respective byte enables are asserted as per write strobes
                            // Slave register 9
                            slv_reg9[(C_S_AXI_DATA_WIDTH*chan)+(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                          end
                      4'hA: 
                        for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                          if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                            // Respective byte enables are asserted as per write strobes
                            // Slave register 10
                            slv_reg10[(C_S_AXI_DATA_WIDTH*chan)+(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                          end

                      //4'hB: // Read only for slv_reg11
                      //4'hC: // Read only for slv_reg12
                      //4'hD: // Read only for slv_reg13
                      4'hE:
                        for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                          if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                            // Respective byte enables are asserted as per write strobes
                            // Slave register 14
                            slv_reg14[(C_S_AXI_DATA_WIDTH*chan)+(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                        end  
                      4'hF:
                        for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
                          if ( S_AXI_WSTRB[byte_index] == 1 ) begin
                            // Respective byte enables are asserted as per write strobes
                            // Slave register 15
                            slv_reg15[(C_S_AXI_DATA_WIDTH*chan)+(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
                        end  
                      default : 
                        begin
                          slv_reg0[(32*chan)+:32] <= slv_reg0[(32*chan)+:32];
                          slv_reg1[(32*chan)+:32] <= slv_reg1[(32*chan)+:32];
                          slv_reg2[(32*chan)+:32] <= slv_reg2[(32*chan)+:32];
                          slv_reg3[(32*chan)+:32] <= slv_reg3[(32*chan)+:32];
                          //slv_reg4[(NUM_CHANNELS*chan)+:32] <= slv_reg4[(NUM_CHANNELS*chan)+:32];
                          //slv_reg5[(NUM_CHANNELS*chan)+:32] <= slv_reg5[(NUM_CHANNELS*chan)+:32];
                          //slv_reg6[(NUM_CHANNELS*chan)+:32] <= slv_reg6[(NUM_CHANNELS*chan)+:32];
                          slv_reg7[(32*chan)+:32] <= slv_reg7[(32*chan)+:32];
                          slv_reg8[(32*chan)+:32] <= slv_reg8[(32*chan)+:32];
                          slv_reg9[(32*chan)+:32] <= slv_reg9[(32*chan)+:32];
                          slv_reg10[(32*chan)+:32] <= slv_reg10[(32*chan)+:32];
                          //slv_reg11[(NUM_CHANNELS*chan)+:32] <= slv_reg11[(NUM_CHANNELS*chan)+:32];
                          //slv_reg12[(NUM_CHANNELS*chan)+:32] <= slv_reg12[(NUM_CHANNELS*chan)+:32];
                          //slv_reg13[(NUM_CHANNELS*chan)+:32] <= slv_reg13[(NUM_CHANNELS*chan)+:32];
                          slv_reg14[(32*chan)+:32] <= slv_reg14[(32*chan)+:32];
                          slv_reg15[(32*chan)+:32] <= slv_reg15[(32*chan)+:32];
                        end
                    endcase
                end
            end
        end
    end
endgenerate

endmodule


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

  // Values on AXIS are in Q8.Q16 mode (but are in 32 bit registers)
  // Q16 LSB [15:0] used for decimal portion
  // Q7 MSB [22:16] used for integer portion
  // bit[23] indicates sign 1'b1: -ve ; 1'b0: +ve 

  //           <------------ integer ----------->  <---------------- decimal ----------------->
  //  _______  _______  _______  _______  _______   _______  _______  _______  _______  _______
  // |       ||       ||       ||       ||       | |       ||       ||       ||       ||       | 
  // |  Sign ||  2^6  ||  .... ||   2^1 ||  2^0  |.|  2^-1 ||  ---  || 2^-14 || 2^-15 || 2^-16 | 
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


