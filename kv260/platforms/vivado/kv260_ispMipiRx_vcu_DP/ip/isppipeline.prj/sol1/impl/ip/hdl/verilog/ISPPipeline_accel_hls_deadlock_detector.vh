
    wire dl_reset;
    wire dl_clock;
    assign dl_reset = ap_rst_n;
    assign dl_clock = ap_clk;
    wire [3:0] proc_0_data_FIFO_blk;
    wire [3:0] proc_0_data_PIPO_blk;
    wire [3:0] proc_0_start_FIFO_blk;
    wire [3:0] proc_0_TLF_FIFO_blk;
    wire [3:0] proc_0_input_sync_blk;
    wire [3:0] proc_0_output_sync_blk;
    wire [3:0] proc_dep_vld_vec_0;
    reg [3:0] proc_dep_vld_vec_0_reg;
    wire [3:0] in_chan_dep_vld_vec_0;
    wire [51:0] in_chan_dep_data_vec_0;
    wire [3:0] token_in_vec_0;
    wire [3:0] out_chan_dep_vld_vec_0;
    wire [12:0] out_chan_dep_data_0;
    wire [3:0] token_out_vec_0;
    wire dl_detect_out_0;
    wire dep_chan_vld_1_0;
    wire [12:0] dep_chan_data_1_0;
    wire token_1_0;
    wire dep_chan_vld_4_0;
    wire [12:0] dep_chan_data_4_0;
    wire token_4_0;
    wire dep_chan_vld_6_0;
    wire [12:0] dep_chan_data_6_0;
    wire token_6_0;
    wire dep_chan_vld_11_0;
    wire [12:0] dep_chan_data_11_0;
    wire token_11_0;
    wire [2:0] proc_1_data_FIFO_blk;
    wire [2:0] proc_1_data_PIPO_blk;
    wire [2:0] proc_1_start_FIFO_blk;
    wire [2:0] proc_1_TLF_FIFO_blk;
    wire [2:0] proc_1_input_sync_blk;
    wire [2:0] proc_1_output_sync_blk;
    wire [2:0] proc_dep_vld_vec_1;
    reg [2:0] proc_dep_vld_vec_1_reg;
    wire [7:0] in_chan_dep_vld_vec_1;
    wire [103:0] in_chan_dep_data_vec_1;
    wire [7:0] token_in_vec_1;
    wire [2:0] out_chan_dep_vld_vec_1;
    wire [12:0] out_chan_dep_data_1;
    wire [2:0] token_out_vec_1;
    wire dl_detect_out_1;
    wire dep_chan_vld_0_1;
    wire [12:0] dep_chan_data_0_1;
    wire token_0_1;
    wire dep_chan_vld_2_1;
    wire [12:0] dep_chan_data_2_1;
    wire token_2_1;
    wire dep_chan_vld_4_1;
    wire [12:0] dep_chan_data_4_1;
    wire token_4_1;
    wire dep_chan_vld_5_1;
    wire [12:0] dep_chan_data_5_1;
    wire token_5_1;
    wire dep_chan_vld_6_1;
    wire [12:0] dep_chan_data_6_1;
    wire token_6_1;
    wire dep_chan_vld_10_1;
    wire [12:0] dep_chan_data_10_1;
    wire token_10_1;
    wire dep_chan_vld_11_1;
    wire [12:0] dep_chan_data_11_1;
    wire token_11_1;
    wire dep_chan_vld_12_1;
    wire [12:0] dep_chan_data_12_1;
    wire token_12_1;
    wire [1:0] proc_2_data_FIFO_blk;
    wire [1:0] proc_2_data_PIPO_blk;
    wire [1:0] proc_2_start_FIFO_blk;
    wire [1:0] proc_2_TLF_FIFO_blk;
    wire [1:0] proc_2_input_sync_blk;
    wire [1:0] proc_2_output_sync_blk;
    wire [1:0] proc_dep_vld_vec_2;
    reg [1:0] proc_dep_vld_vec_2_reg;
    wire [0:0] in_chan_dep_vld_vec_2;
    wire [12:0] in_chan_dep_data_vec_2;
    wire [0:0] token_in_vec_2;
    wire [1:0] out_chan_dep_vld_vec_2;
    wire [12:0] out_chan_dep_data_2;
    wire [1:0] token_out_vec_2;
    wire dl_detect_out_2;
    wire dep_chan_vld_3_2;
    wire [12:0] dep_chan_data_3_2;
    wire token_3_2;
    wire [1:0] proc_3_data_FIFO_blk;
    wire [1:0] proc_3_data_PIPO_blk;
    wire [1:0] proc_3_start_FIFO_blk;
    wire [1:0] proc_3_TLF_FIFO_blk;
    wire [1:0] proc_3_input_sync_blk;
    wire [1:0] proc_3_output_sync_blk;
    wire [1:0] proc_dep_vld_vec_3;
    reg [1:0] proc_dep_vld_vec_3_reg;
    wire [1:0] in_chan_dep_vld_vec_3;
    wire [25:0] in_chan_dep_data_vec_3;
    wire [1:0] token_in_vec_3;
    wire [1:0] out_chan_dep_vld_vec_3;
    wire [12:0] out_chan_dep_data_3;
    wire [1:0] token_out_vec_3;
    wire dl_detect_out_3;
    wire dep_chan_vld_2_3;
    wire [12:0] dep_chan_data_2_3;
    wire token_2_3;
    wire dep_chan_vld_4_3;
    wire [12:0] dep_chan_data_4_3;
    wire token_4_3;
    wire [3:0] proc_4_data_FIFO_blk;
    wire [3:0] proc_4_data_PIPO_blk;
    wire [3:0] proc_4_start_FIFO_blk;
    wire [3:0] proc_4_TLF_FIFO_blk;
    wire [3:0] proc_4_input_sync_blk;
    wire [3:0] proc_4_output_sync_blk;
    wire [3:0] proc_dep_vld_vec_4;
    reg [3:0] proc_dep_vld_vec_4_reg;
    wire [2:0] in_chan_dep_vld_vec_4;
    wire [38:0] in_chan_dep_data_vec_4;
    wire [2:0] token_in_vec_4;
    wire [3:0] out_chan_dep_vld_vec_4;
    wire [12:0] out_chan_dep_data_4;
    wire [3:0] token_out_vec_4;
    wire dl_detect_out_4;
    wire dep_chan_vld_0_4;
    wire [12:0] dep_chan_data_0_4;
    wire token_0_4;
    wire dep_chan_vld_3_4;
    wire [12:0] dep_chan_data_3_4;
    wire token_3_4;
    wire dep_chan_vld_5_4;
    wire [12:0] dep_chan_data_5_4;
    wire token_5_4;
    wire [2:0] proc_5_data_FIFO_blk;
    wire [2:0] proc_5_data_PIPO_blk;
    wire [2:0] proc_5_start_FIFO_blk;
    wire [2:0] proc_5_TLF_FIFO_blk;
    wire [2:0] proc_5_input_sync_blk;
    wire [2:0] proc_5_output_sync_blk;
    wire [2:0] proc_dep_vld_vec_5;
    reg [2:0] proc_dep_vld_vec_5_reg;
    wire [1:0] in_chan_dep_vld_vec_5;
    wire [25:0] in_chan_dep_data_vec_5;
    wire [1:0] token_in_vec_5;
    wire [2:0] out_chan_dep_vld_vec_5;
    wire [12:0] out_chan_dep_data_5;
    wire [2:0] token_out_vec_5;
    wire dl_detect_out_5;
    wire dep_chan_vld_4_5;
    wire [12:0] dep_chan_data_4_5;
    wire token_4_5;
    wire dep_chan_vld_6_5;
    wire [12:0] dep_chan_data_6_5;
    wire token_6_5;
    wire [5:0] proc_6_data_FIFO_blk;
    wire [5:0] proc_6_data_PIPO_blk;
    wire [5:0] proc_6_start_FIFO_blk;
    wire [5:0] proc_6_TLF_FIFO_blk;
    wire [5:0] proc_6_input_sync_blk;
    wire [5:0] proc_6_output_sync_blk;
    wire [5:0] proc_dep_vld_vec_6;
    reg [5:0] proc_dep_vld_vec_6_reg;
    wire [5:0] in_chan_dep_vld_vec_6;
    wire [77:0] in_chan_dep_data_vec_6;
    wire [5:0] token_in_vec_6;
    wire [5:0] out_chan_dep_vld_vec_6;
    wire [12:0] out_chan_dep_data_6;
    wire [5:0] token_out_vec_6;
    wire dl_detect_out_6;
    wire dep_chan_vld_0_6;
    wire [12:0] dep_chan_data_0_6;
    wire token_0_6;
    wire dep_chan_vld_1_6;
    wire [12:0] dep_chan_data_1_6;
    wire token_1_6;
    wire dep_chan_vld_5_6;
    wire [12:0] dep_chan_data_5_6;
    wire token_5_6;
    wire dep_chan_vld_9_6;
    wire [12:0] dep_chan_data_9_6;
    wire token_9_6;
    wire dep_chan_vld_11_6;
    wire [12:0] dep_chan_data_11_6;
    wire token_11_6;
    wire dep_chan_vld_12_6;
    wire [12:0] dep_chan_data_12_6;
    wire token_12_6;
    wire [0:0] proc_7_data_FIFO_blk;
    wire [0:0] proc_7_data_PIPO_blk;
    wire [0:0] proc_7_start_FIFO_blk;
    wire [0:0] proc_7_TLF_FIFO_blk;
    wire [0:0] proc_7_input_sync_blk;
    wire [0:0] proc_7_output_sync_blk;
    wire [0:0] proc_dep_vld_vec_7;
    reg [0:0] proc_dep_vld_vec_7_reg;
    wire [0:0] in_chan_dep_vld_vec_7;
    wire [12:0] in_chan_dep_data_vec_7;
    wire [0:0] token_in_vec_7;
    wire [0:0] out_chan_dep_vld_vec_7;
    wire [12:0] out_chan_dep_data_7;
    wire [0:0] token_out_vec_7;
    wire dl_detect_out_7;
    wire dep_chan_vld_8_7;
    wire [12:0] dep_chan_data_8_7;
    wire token_8_7;
    wire [0:0] proc_8_data_FIFO_blk;
    wire [0:0] proc_8_data_PIPO_blk;
    wire [0:0] proc_8_start_FIFO_blk;
    wire [0:0] proc_8_TLF_FIFO_blk;
    wire [0:0] proc_8_input_sync_blk;
    wire [0:0] proc_8_output_sync_blk;
    wire [0:0] proc_dep_vld_vec_8;
    reg [0:0] proc_dep_vld_vec_8_reg;
    wire [0:0] in_chan_dep_vld_vec_8;
    wire [12:0] in_chan_dep_data_vec_8;
    wire [0:0] token_in_vec_8;
    wire [0:0] out_chan_dep_vld_vec_8;
    wire [12:0] out_chan_dep_data_8;
    wire [0:0] token_out_vec_8;
    wire dl_detect_out_8;
    wire dep_chan_vld_7_8;
    wire [12:0] dep_chan_data_7_8;
    wire token_7_8;
    wire [1:0] proc_9_data_FIFO_blk;
    wire [1:0] proc_9_data_PIPO_blk;
    wire [1:0] proc_9_start_FIFO_blk;
    wire [1:0] proc_9_TLF_FIFO_blk;
    wire [1:0] proc_9_input_sync_blk;
    wire [1:0] proc_9_output_sync_blk;
    wire [1:0] proc_dep_vld_vec_9;
    reg [1:0] proc_dep_vld_vec_9_reg;
    wire [1:0] in_chan_dep_vld_vec_9;
    wire [25:0] in_chan_dep_data_vec_9;
    wire [1:0] token_in_vec_9;
    wire [1:0] out_chan_dep_vld_vec_9;
    wire [12:0] out_chan_dep_data_9;
    wire [1:0] token_out_vec_9;
    wire dl_detect_out_9;
    wire dep_chan_vld_6_9;
    wire [12:0] dep_chan_data_6_9;
    wire token_6_9;
    wire dep_chan_vld_10_9;
    wire [12:0] dep_chan_data_10_9;
    wire token_10_9;
    wire [2:0] proc_10_data_FIFO_blk;
    wire [2:0] proc_10_data_PIPO_blk;
    wire [2:0] proc_10_start_FIFO_blk;
    wire [2:0] proc_10_TLF_FIFO_blk;
    wire [2:0] proc_10_input_sync_blk;
    wire [2:0] proc_10_output_sync_blk;
    wire [2:0] proc_dep_vld_vec_10;
    reg [2:0] proc_dep_vld_vec_10_reg;
    wire [1:0] in_chan_dep_vld_vec_10;
    wire [25:0] in_chan_dep_data_vec_10;
    wire [1:0] token_in_vec_10;
    wire [2:0] out_chan_dep_vld_vec_10;
    wire [12:0] out_chan_dep_data_10;
    wire [2:0] token_out_vec_10;
    wire dl_detect_out_10;
    wire dep_chan_vld_9_10;
    wire [12:0] dep_chan_data_9_10;
    wire token_9_10;
    wire dep_chan_vld_11_10;
    wire [12:0] dep_chan_data_11_10;
    wire token_11_10;
    wire [4:0] proc_11_data_FIFO_blk;
    wire [4:0] proc_11_data_PIPO_blk;
    wire [4:0] proc_11_start_FIFO_blk;
    wire [4:0] proc_11_TLF_FIFO_blk;
    wire [4:0] proc_11_input_sync_blk;
    wire [4:0] proc_11_output_sync_blk;
    wire [4:0] proc_dep_vld_vec_11;
    reg [4:0] proc_dep_vld_vec_11_reg;
    wire [4:0] in_chan_dep_vld_vec_11;
    wire [64:0] in_chan_dep_data_vec_11;
    wire [4:0] token_in_vec_11;
    wire [4:0] out_chan_dep_vld_vec_11;
    wire [12:0] out_chan_dep_data_11;
    wire [4:0] token_out_vec_11;
    wire dl_detect_out_11;
    wire dep_chan_vld_0_11;
    wire [12:0] dep_chan_data_0_11;
    wire token_0_11;
    wire dep_chan_vld_1_11;
    wire [12:0] dep_chan_data_1_11;
    wire token_1_11;
    wire dep_chan_vld_6_11;
    wire [12:0] dep_chan_data_6_11;
    wire token_6_11;
    wire dep_chan_vld_10_11;
    wire [12:0] dep_chan_data_10_11;
    wire token_10_11;
    wire dep_chan_vld_12_11;
    wire [12:0] dep_chan_data_12_11;
    wire token_12_11;
    wire [2:0] proc_12_data_FIFO_blk;
    wire [2:0] proc_12_data_PIPO_blk;
    wire [2:0] proc_12_start_FIFO_blk;
    wire [2:0] proc_12_TLF_FIFO_blk;
    wire [2:0] proc_12_input_sync_blk;
    wire [2:0] proc_12_output_sync_blk;
    wire [2:0] proc_dep_vld_vec_12;
    reg [2:0] proc_dep_vld_vec_12_reg;
    wire [1:0] in_chan_dep_vld_vec_12;
    wire [25:0] in_chan_dep_data_vec_12;
    wire [1:0] token_in_vec_12;
    wire [2:0] out_chan_dep_vld_vec_12;
    wire [12:0] out_chan_dep_data_12;
    wire [2:0] token_out_vec_12;
    wire dl_detect_out_12;
    wire dep_chan_vld_6_12;
    wire [12:0] dep_chan_data_6_12;
    wire token_6_12;
    wire dep_chan_vld_11_12;
    wire [12:0] dep_chan_data_11_12;
    wire token_11_12;
    wire [12:0] dl_in_vec;
    wire dl_detect_out;
    wire token_clear;
    reg [12:0] origin;

    reg ap_done_reg_0;// for module grp_ISPpipeline_fu_189.AXIVideo2BayerMat_13_1080_1920_1_U0
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            ap_done_reg_0 <= 'b0;
        end
        else begin
            ap_done_reg_0 <= grp_ISPpipeline_fu_189.AXIVideo2BayerMat_13_1080_1920_1_U0.ap_done & ~grp_ISPpipeline_fu_189.AXIVideo2BayerMat_13_1080_1920_1_U0.ap_continue;
        end
    end

    reg ap_done_reg_1;// for module grp_ISPpipeline_fu_189.gaincontrol_3_13_1080_1920_1_U0
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            ap_done_reg_1 <= 'b0;
        end
        else begin
            ap_done_reg_1 <= grp_ISPpipeline_fu_189.gaincontrol_3_13_1080_1920_1_U0.ap_done & ~grp_ISPpipeline_fu_189.gaincontrol_3_13_1080_1920_1_U0.ap_continue;
        end
    end

    reg ap_done_reg_2;// for module grp_ISPpipeline_fu_189.demosaicing_3_13_17_1080_1920_1_false_U0
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            ap_done_reg_2 <= 'b0;
        end
        else begin
            ap_done_reg_2 <= grp_ISPpipeline_fu_189.demosaicing_3_13_17_1080_1920_1_false_U0.ap_done & ~grp_ISPpipeline_fu_189.demosaicing_3_13_17_1080_1920_1_false_U0.ap_continue;
        end
    end

    reg ap_done_reg_3;// for module grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            ap_done_reg_3 <= 'b0;
        end
        else begin
            ap_done_reg_3 <= grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.ap_done & ~grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.ap_continue;
        end
    end

    reg ap_done_reg_4;// for module grp_ISPpipeline_fu_189.xf_QuatizationDithering_17_9_1080_1920_256_1024_1_U0
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            ap_done_reg_4 <= 'b0;
        end
        else begin
            ap_done_reg_4 <= grp_ISPpipeline_fu_189.xf_QuatizationDithering_17_9_1080_1920_256_1024_1_U0.ap_done & ~grp_ISPpipeline_fu_189.xf_QuatizationDithering_17_9_1080_1920_256_1024_1_U0.ap_continue;
        end
    end

    reg ap_done_reg_5;// for module grp_ISPpipeline_fu_189.gammacorrection_9_9_1080_1920_1_U0
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            ap_done_reg_5 <= 'b0;
        end
        else begin
            ap_done_reg_5 <= grp_ISPpipeline_fu_189.gammacorrection_9_9_1080_1920_1_U0.ap_done & ~grp_ISPpipeline_fu_189.gammacorrection_9_9_1080_1920_1_U0.ap_continue;
        end
    end

    reg ap_done_reg_6;// for module grp_ISPpipeline_fu_189.ColorMat2AXIvideo_9_1080_1920_1_U0
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            ap_done_reg_6 <= 'b0;
        end
        else begin
            ap_done_reg_6 <= grp_ISPpipeline_fu_189.ColorMat2AXIvideo_9_1080_1920_1_U0.ap_done & ~grp_ISPpipeline_fu_189.ColorMat2AXIvideo_9_1080_1920_1_U0.ap_continue;
        end
    end

    reg ap_done_reg_7;// for module grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.grp_fifo_awb_17_17_1080_1920_1_s_fu_168.AWBhistogram_17_17_1080_1920_1_1_1024_U0
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            ap_done_reg_7 <= 'b0;
        end
        else begin
            ap_done_reg_7 <= grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.grp_fifo_awb_17_17_1080_1920_1_s_fu_168.AWBhistogram_17_17_1080_1920_1_1_1024_U0.ap_done & ~grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.grp_fifo_awb_17_17_1080_1920_1_s_fu_168.AWBhistogram_17_17_1080_1920_1_1_1024_U0.ap_continue;
        end
    end

    reg ap_done_reg_8;// for module grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.grp_fifo_awb_17_17_1080_1920_1_s_fu_168.AWBNormalization_17_17_1080_1920_1_1_1024_U0
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            ap_done_reg_8 <= 'b0;
        end
        else begin
            ap_done_reg_8 <= grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.grp_fifo_awb_17_17_1080_1920_1_s_fu_168.AWBNormalization_17_17_1080_1920_1_1_1024_U0.ap_done & ~grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.grp_fifo_awb_17_17_1080_1920_1_s_fu_168.AWBNormalization_17_17_1080_1920_1_1_1024_U0.ap_continue;
        end
    end

reg [15:0] trans_in_cnt_0;// for process grp_ISPpipeline_fu_189.AXIVideo2BayerMat_13_1080_1920_1_U0
always @(negedge dl_reset or posedge dl_clock) begin
    if (~dl_reset) begin
         trans_in_cnt_0 <= 16'h0;
    end
    else if (grp_ISPpipeline_fu_189.AXIVideo2BayerMat_13_1080_1920_1_U0.start_write == 1'b1) begin
        trans_in_cnt_0 <= trans_in_cnt_0 + 16'h1;
    end
    else begin
        trans_in_cnt_0 <= trans_in_cnt_0;
    end
end

reg [15:0] trans_out_cnt_0;// for process grp_ISPpipeline_fu_189.AXIVideo2BayerMat_13_1080_1920_1_U0
always @(negedge dl_reset or posedge dl_clock) begin
    if (~dl_reset) begin
         trans_out_cnt_0 <= 16'h0;
    end
    else if (grp_ISPpipeline_fu_189.AXIVideo2BayerMat_13_1080_1920_1_U0.ap_done == 1'b1 && grp_ISPpipeline_fu_189.AXIVideo2BayerMat_13_1080_1920_1_U0.ap_continue == 1'b1) begin
        trans_out_cnt_0 <= trans_out_cnt_0 + 16'h1;
    end
    else begin
        trans_out_cnt_0 <= trans_out_cnt_0;
    end
end

reg [15:0] trans_in_cnt_1;// for process grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0
always @(negedge dl_reset or posedge dl_clock) begin
    if (~dl_reset) begin
         trans_in_cnt_1 <= 16'h0;
    end
    else if (grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.start_write == 1'b1) begin
        trans_in_cnt_1 <= trans_in_cnt_1 + 16'h1;
    end
    else begin
        trans_in_cnt_1 <= trans_in_cnt_1;
    end
end

reg [15:0] trans_out_cnt_1;// for process grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0
always @(negedge dl_reset or posedge dl_clock) begin
    if (~dl_reset) begin
         trans_out_cnt_1 <= 16'h0;
    end
    else if (grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.ap_done == 1'b1 && grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.ap_continue == 1'b1) begin
        trans_out_cnt_1 <= trans_out_cnt_1 + 16'h1;
    end
    else begin
        trans_out_cnt_1 <= trans_out_cnt_1;
    end
end

    // Process: grp_ISPpipeline_fu_189.entry_proc_U0
    ISPPipeline_accel_hls_deadlock_detect_unit #(13, 0, 4, 4) ISPPipeline_accel_hls_deadlock_detect_unit_0 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_0),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_0),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_0),
        .token_in_vec(token_in_vec_0),
        .dl_detect_in(dl_detect_out),
        .origin(origin[0]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_0),
        .out_chan_dep_data(out_chan_dep_data_0),
        .token_out_vec(token_out_vec_0),
        .dl_detect_out(dl_in_vec[0]));

    assign proc_0_data_FIFO_blk[0] = 1'b0 | (~grp_ISPpipeline_fu_189.entry_proc_U0.rgain_c_blk_n) | (~grp_ISPpipeline_fu_189.entry_proc_U0.bgain_c_blk_n);
    assign proc_0_data_PIPO_blk[0] = 1'b0;
    assign proc_0_start_FIFO_blk[0] = 1'b0;
    assign proc_0_TLF_FIFO_blk[0] = 1'b0;
    assign proc_0_input_sync_blk[0] = 1'b0;
    assign proc_0_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_0[0] = dl_detect_out ? proc_dep_vld_vec_0_reg[0] : (proc_0_data_FIFO_blk[0] | proc_0_data_PIPO_blk[0] | proc_0_start_FIFO_blk[0] | proc_0_TLF_FIFO_blk[0] | proc_0_input_sync_blk[0] | proc_0_output_sync_blk[0]);
    assign proc_0_data_FIFO_blk[1] = 1'b0;
    assign proc_0_data_PIPO_blk[1] = 1'b0;
    assign proc_0_start_FIFO_blk[1] = 1'b0;
    assign proc_0_TLF_FIFO_blk[1] = 1'b0;
    assign proc_0_input_sync_blk[1] = 1'b0 | (grp_ISPpipeline_fu_189.ap_sync_entry_proc_U0_ap_ready & grp_ISPpipeline_fu_189.entry_proc_U0.ap_idle & ~grp_ISPpipeline_fu_189.ap_sync_ISPpipeline_Block_entry1_proc_U0_ap_ready);
    assign proc_0_output_sync_blk[1] = 1'b0;
    assign proc_dep_vld_vec_0[1] = dl_detect_out ? proc_dep_vld_vec_0_reg[1] : (proc_0_data_FIFO_blk[1] | proc_0_data_PIPO_blk[1] | proc_0_start_FIFO_blk[1] | proc_0_TLF_FIFO_blk[1] | proc_0_input_sync_blk[1] | proc_0_output_sync_blk[1]);
    assign proc_0_data_FIFO_blk[2] = 1'b0;
    assign proc_0_data_PIPO_blk[2] = 1'b0;
    assign proc_0_start_FIFO_blk[2] = 1'b0;
    assign proc_0_TLF_FIFO_blk[2] = 1'b0;
    assign proc_0_input_sync_blk[2] = 1'b0 | (grp_ISPpipeline_fu_189.ap_sync_entry_proc_U0_ap_ready & grp_ISPpipeline_fu_189.entry_proc_U0.ap_idle & ~grp_ISPpipeline_fu_189.ap_sync_function_awb_17_17_1080_1920_1_U0_ap_ready);
    assign proc_0_output_sync_blk[2] = 1'b0;
    assign proc_dep_vld_vec_0[2] = dl_detect_out ? proc_dep_vld_vec_0_reg[2] : (proc_0_data_FIFO_blk[2] | proc_0_data_PIPO_blk[2] | proc_0_start_FIFO_blk[2] | proc_0_TLF_FIFO_blk[2] | proc_0_input_sync_blk[2] | proc_0_output_sync_blk[2]);
    assign proc_0_data_FIFO_blk[3] = 1'b0;
    assign proc_0_data_PIPO_blk[3] = 1'b0;
    assign proc_0_start_FIFO_blk[3] = 1'b0;
    assign proc_0_TLF_FIFO_blk[3] = 1'b0;
    assign proc_0_input_sync_blk[3] = 1'b0 | (grp_ISPpipeline_fu_189.ap_sync_entry_proc_U0_ap_ready & grp_ISPpipeline_fu_189.entry_proc_U0.ap_idle & ~grp_ISPpipeline_fu_189.ap_sync_gammacorrection_9_9_1080_1920_1_U0_ap_ready);
    assign proc_0_output_sync_blk[3] = 1'b0;
    assign proc_dep_vld_vec_0[3] = dl_detect_out ? proc_dep_vld_vec_0_reg[3] : (proc_0_data_FIFO_blk[3] | proc_0_data_PIPO_blk[3] | proc_0_start_FIFO_blk[3] | proc_0_TLF_FIFO_blk[3] | proc_0_input_sync_blk[3] | proc_0_output_sync_blk[3]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_0_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_0_reg <= proc_dep_vld_vec_0;
        end
    end
    assign in_chan_dep_vld_vec_0[0] = dep_chan_vld_1_0;
    assign in_chan_dep_data_vec_0[12 : 0] = dep_chan_data_1_0;
    assign token_in_vec_0[0] = token_1_0;
    assign in_chan_dep_vld_vec_0[1] = dep_chan_vld_4_0;
    assign in_chan_dep_data_vec_0[25 : 13] = dep_chan_data_4_0;
    assign token_in_vec_0[1] = token_4_0;
    assign in_chan_dep_vld_vec_0[2] = dep_chan_vld_6_0;
    assign in_chan_dep_data_vec_0[38 : 26] = dep_chan_data_6_0;
    assign token_in_vec_0[2] = token_6_0;
    assign in_chan_dep_vld_vec_0[3] = dep_chan_vld_11_0;
    assign in_chan_dep_data_vec_0[51 : 39] = dep_chan_data_11_0;
    assign token_in_vec_0[3] = token_11_0;
    assign dep_chan_vld_0_4 = out_chan_dep_vld_vec_0[0];
    assign dep_chan_data_0_4 = out_chan_dep_data_0;
    assign token_0_4 = token_out_vec_0[0];
    assign dep_chan_vld_0_1 = out_chan_dep_vld_vec_0[1];
    assign dep_chan_data_0_1 = out_chan_dep_data_0;
    assign token_0_1 = token_out_vec_0[1];
    assign dep_chan_vld_0_6 = out_chan_dep_vld_vec_0[2];
    assign dep_chan_data_0_6 = out_chan_dep_data_0;
    assign token_0_6 = token_out_vec_0[2];
    assign dep_chan_vld_0_11 = out_chan_dep_vld_vec_0[3];
    assign dep_chan_data_0_11 = out_chan_dep_data_0;
    assign token_0_11 = token_out_vec_0[3];

    // Process: grp_ISPpipeline_fu_189.ISPpipeline_Block_entry1_proc_U0
    ISPPipeline_accel_hls_deadlock_detect_unit #(13, 1, 8, 3) ISPPipeline_accel_hls_deadlock_detect_unit_1 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_1),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_1),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_1),
        .token_in_vec(token_in_vec_1),
        .dl_detect_in(dl_detect_out),
        .origin(origin[1]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_1),
        .out_chan_dep_data(out_chan_dep_data_1),
        .token_out_vec(token_out_vec_1),
        .dl_detect_out(dl_in_vec[1]));

    assign proc_1_data_FIFO_blk[0] = 1'b0;
    assign proc_1_data_PIPO_blk[0] = 1'b0;
    assign proc_1_start_FIFO_blk[0] = 1'b0;
    assign proc_1_TLF_FIFO_blk[0] = 1'b0;
    assign proc_1_input_sync_blk[0] = 1'b0 | (grp_ISPpipeline_fu_189.ap_sync_ISPpipeline_Block_entry1_proc_U0_ap_ready & grp_ISPpipeline_fu_189.ISPpipeline_Block_entry1_proc_U0.ap_idle & ~grp_ISPpipeline_fu_189.ap_sync_entry_proc_U0_ap_ready);
    assign proc_1_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_1[0] = dl_detect_out ? proc_dep_vld_vec_1_reg[0] : (proc_1_data_FIFO_blk[0] | proc_1_data_PIPO_blk[0] | proc_1_start_FIFO_blk[0] | proc_1_TLF_FIFO_blk[0] | proc_1_input_sync_blk[0] | proc_1_output_sync_blk[0]);
    assign proc_1_data_FIFO_blk[1] = 1'b0;
    assign proc_1_data_PIPO_blk[1] = 1'b0;
    assign proc_1_start_FIFO_blk[1] = 1'b0;
    assign proc_1_TLF_FIFO_blk[1] = 1'b0;
    assign proc_1_input_sync_blk[1] = 1'b0 | (grp_ISPpipeline_fu_189.ap_sync_ISPpipeline_Block_entry1_proc_U0_ap_ready & grp_ISPpipeline_fu_189.ISPpipeline_Block_entry1_proc_U0.ap_idle & ~grp_ISPpipeline_fu_189.ap_sync_function_awb_17_17_1080_1920_1_U0_ap_ready);
    assign proc_1_output_sync_blk[1] = 1'b0;
    assign proc_dep_vld_vec_1[1] = dl_detect_out ? proc_dep_vld_vec_1_reg[1] : (proc_1_data_FIFO_blk[1] | proc_1_data_PIPO_blk[1] | proc_1_start_FIFO_blk[1] | proc_1_TLF_FIFO_blk[1] | proc_1_input_sync_blk[1] | proc_1_output_sync_blk[1]);
    assign proc_1_data_FIFO_blk[2] = 1'b0;
    assign proc_1_data_PIPO_blk[2] = 1'b0;
    assign proc_1_start_FIFO_blk[2] = 1'b0;
    assign proc_1_TLF_FIFO_blk[2] = 1'b0;
    assign proc_1_input_sync_blk[2] = 1'b0 | (grp_ISPpipeline_fu_189.ap_sync_ISPpipeline_Block_entry1_proc_U0_ap_ready & grp_ISPpipeline_fu_189.ISPpipeline_Block_entry1_proc_U0.ap_idle & ~grp_ISPpipeline_fu_189.ap_sync_gammacorrection_9_9_1080_1920_1_U0_ap_ready);
    assign proc_1_output_sync_blk[2] = 1'b0;
    assign proc_dep_vld_vec_1[2] = dl_detect_out ? proc_dep_vld_vec_1_reg[2] : (proc_1_data_FIFO_blk[2] | proc_1_data_PIPO_blk[2] | proc_1_start_FIFO_blk[2] | proc_1_TLF_FIFO_blk[2] | proc_1_input_sync_blk[2] | proc_1_output_sync_blk[2]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_1_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_1_reg <= proc_dep_vld_vec_1;
        end
    end
    assign in_chan_dep_vld_vec_1[0] = dep_chan_vld_0_1;
    assign in_chan_dep_data_vec_1[12 : 0] = dep_chan_data_0_1;
    assign token_in_vec_1[0] = token_0_1;
    assign in_chan_dep_vld_vec_1[1] = dep_chan_vld_2_1;
    assign in_chan_dep_data_vec_1[25 : 13] = dep_chan_data_2_1;
    assign token_in_vec_1[1] = token_2_1;
    assign in_chan_dep_vld_vec_1[2] = dep_chan_vld_4_1;
    assign in_chan_dep_data_vec_1[38 : 26] = dep_chan_data_4_1;
    assign token_in_vec_1[2] = token_4_1;
    assign in_chan_dep_vld_vec_1[3] = dep_chan_vld_5_1;
    assign in_chan_dep_data_vec_1[51 : 39] = dep_chan_data_5_1;
    assign token_in_vec_1[3] = token_5_1;
    assign in_chan_dep_vld_vec_1[4] = dep_chan_vld_6_1;
    assign in_chan_dep_data_vec_1[64 : 52] = dep_chan_data_6_1;
    assign token_in_vec_1[4] = token_6_1;
    assign in_chan_dep_vld_vec_1[5] = dep_chan_vld_10_1;
    assign in_chan_dep_data_vec_1[77 : 65] = dep_chan_data_10_1;
    assign token_in_vec_1[5] = token_10_1;
    assign in_chan_dep_vld_vec_1[6] = dep_chan_vld_11_1;
    assign in_chan_dep_data_vec_1[90 : 78] = dep_chan_data_11_1;
    assign token_in_vec_1[6] = token_11_1;
    assign in_chan_dep_vld_vec_1[7] = dep_chan_vld_12_1;
    assign in_chan_dep_data_vec_1[103 : 91] = dep_chan_data_12_1;
    assign token_in_vec_1[7] = token_12_1;
    assign dep_chan_vld_1_0 = out_chan_dep_vld_vec_1[0];
    assign dep_chan_data_1_0 = out_chan_dep_data_1;
    assign token_1_0 = token_out_vec_1[0];
    assign dep_chan_vld_1_6 = out_chan_dep_vld_vec_1[1];
    assign dep_chan_data_1_6 = out_chan_dep_data_1;
    assign token_1_6 = token_out_vec_1[1];
    assign dep_chan_vld_1_11 = out_chan_dep_vld_vec_1[2];
    assign dep_chan_data_1_11 = out_chan_dep_data_1;
    assign token_1_11 = token_out_vec_1[2];

    // Process: grp_ISPpipeline_fu_189.AXIVideo2BayerMat_13_1080_1920_1_U0
    ISPPipeline_accel_hls_deadlock_detect_unit #(13, 2, 1, 2) ISPPipeline_accel_hls_deadlock_detect_unit_2 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_2),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_2),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_2),
        .token_in_vec(token_in_vec_2),
        .dl_detect_in(dl_detect_out),
        .origin(origin[2]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_2),
        .out_chan_dep_data(out_chan_dep_data_2),
        .token_out_vec(token_out_vec_2),
        .dl_detect_out(dl_in_vec[2]));

    assign proc_2_data_FIFO_blk[0] = 1'b0;
    assign proc_2_data_PIPO_blk[0] = 1'b0;
    assign proc_2_start_FIFO_blk[0] = 1'b0;
    assign proc_2_TLF_FIFO_blk[0] = 1'b0 | (~grp_ISPpipeline_fu_189.imgInput1_rows_c21_channel_U.if_empty_n & grp_ISPpipeline_fu_189.AXIVideo2BayerMat_13_1080_1920_1_U0.ap_idle & ~grp_ISPpipeline_fu_189.imgInput1_rows_c21_channel_U.if_write) | (~grp_ISPpipeline_fu_189.imgInput1_cols_c22_channel_U.if_empty_n & grp_ISPpipeline_fu_189.AXIVideo2BayerMat_13_1080_1920_1_U0.ap_idle & ~grp_ISPpipeline_fu_189.imgInput1_cols_c22_channel_U.if_write);
    assign proc_2_input_sync_blk[0] = 1'b0;
    assign proc_2_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_2[0] = dl_detect_out ? proc_dep_vld_vec_2_reg[0] : (proc_2_data_FIFO_blk[0] | proc_2_data_PIPO_blk[0] | proc_2_start_FIFO_blk[0] | proc_2_TLF_FIFO_blk[0] | proc_2_input_sync_blk[0] | proc_2_output_sync_blk[0]);
    assign proc_2_data_FIFO_blk[1] = 1'b0 | (~grp_ISPpipeline_fu_189.AXIVideo2BayerMat_13_1080_1920_1_U0.grp_AXIVideo2BayerMat_13_1080_1920_1_Pipeline_loop_col_zxi2mat_fu_201.imgInput1_data238_blk_n) | (~grp_ISPpipeline_fu_189.AXIVideo2BayerMat_13_1080_1920_1_U0.imgInput1_rows_c_blk_n) | (~grp_ISPpipeline_fu_189.AXIVideo2BayerMat_13_1080_1920_1_U0.imgInput1_cols_c_blk_n);
    assign proc_2_data_PIPO_blk[1] = 1'b0;
    assign proc_2_start_FIFO_blk[1] = 1'b0 | (~grp_ISPpipeline_fu_189.start_for_blackLevelCorrection_13_1080_1920_1_16_15_1_9_U0_U.if_full_n & grp_ISPpipeline_fu_189.AXIVideo2BayerMat_13_1080_1920_1_U0.ap_start & ~grp_ISPpipeline_fu_189.AXIVideo2BayerMat_13_1080_1920_1_U0.real_start & (trans_in_cnt_0 == trans_out_cnt_0) & ~grp_ISPpipeline_fu_189.start_for_blackLevelCorrection_13_1080_1920_1_16_15_1_9_U0_U.if_read);
    assign proc_2_TLF_FIFO_blk[1] = 1'b0;
    assign proc_2_input_sync_blk[1] = 1'b0;
    assign proc_2_output_sync_blk[1] = 1'b0;
    assign proc_dep_vld_vec_2[1] = dl_detect_out ? proc_dep_vld_vec_2_reg[1] : (proc_2_data_FIFO_blk[1] | proc_2_data_PIPO_blk[1] | proc_2_start_FIFO_blk[1] | proc_2_TLF_FIFO_blk[1] | proc_2_input_sync_blk[1] | proc_2_output_sync_blk[1]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_2_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_2_reg <= proc_dep_vld_vec_2;
        end
    end
    assign in_chan_dep_vld_vec_2[0] = dep_chan_vld_3_2;
    assign in_chan_dep_data_vec_2[12 : 0] = dep_chan_data_3_2;
    assign token_in_vec_2[0] = token_3_2;
    assign dep_chan_vld_2_1 = out_chan_dep_vld_vec_2[0];
    assign dep_chan_data_2_1 = out_chan_dep_data_2;
    assign token_2_1 = token_out_vec_2[0];
    assign dep_chan_vld_2_3 = out_chan_dep_vld_vec_2[1];
    assign dep_chan_data_2_3 = out_chan_dep_data_2;
    assign token_2_3 = token_out_vec_2[1];

    // Process: grp_ISPpipeline_fu_189.blackLevelCorrection_13_1080_1920_1_16_15_1_9_U0
    ISPPipeline_accel_hls_deadlock_detect_unit #(13, 3, 2, 2) ISPPipeline_accel_hls_deadlock_detect_unit_3 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_3),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_3),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_3),
        .token_in_vec(token_in_vec_3),
        .dl_detect_in(dl_detect_out),
        .origin(origin[3]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_3),
        .out_chan_dep_data(out_chan_dep_data_3),
        .token_out_vec(token_out_vec_3),
        .dl_detect_out(dl_in_vec[3]));

    assign proc_3_data_FIFO_blk[0] = 1'b0 | (~grp_ISPpipeline_fu_189.blackLevelCorrection_13_1080_1920_1_16_15_1_9_U0.p_Src_rows_blk_n) | (~grp_ISPpipeline_fu_189.blackLevelCorrection_13_1080_1920_1_16_15_1_9_U0.p_Src_cols_blk_n) | (~grp_ISPpipeline_fu_189.blackLevelCorrection_13_1080_1920_1_16_15_1_9_U0.grp_blackLevelCorrection_13_1080_1920_1_16_15_1_9_Pipeline_VITIS_LOOP_91_1_fu_46.imgInput1_data238_blk_n);
    assign proc_3_data_PIPO_blk[0] = 1'b0;
    assign proc_3_start_FIFO_blk[0] = 1'b0 | (~grp_ISPpipeline_fu_189.start_for_blackLevelCorrection_13_1080_1920_1_16_15_1_9_U0_U.if_empty_n & grp_ISPpipeline_fu_189.blackLevelCorrection_13_1080_1920_1_16_15_1_9_U0.ap_idle & ~grp_ISPpipeline_fu_189.start_for_blackLevelCorrection_13_1080_1920_1_16_15_1_9_U0_U.if_write);
    assign proc_3_TLF_FIFO_blk[0] = 1'b0;
    assign proc_3_input_sync_blk[0] = 1'b0;
    assign proc_3_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_3[0] = dl_detect_out ? proc_dep_vld_vec_3_reg[0] : (proc_3_data_FIFO_blk[0] | proc_3_data_PIPO_blk[0] | proc_3_start_FIFO_blk[0] | proc_3_TLF_FIFO_blk[0] | proc_3_input_sync_blk[0] | proc_3_output_sync_blk[0]);
    assign proc_3_data_FIFO_blk[1] = 1'b0 | (~grp_ISPpipeline_fu_189.blackLevelCorrection_13_1080_1920_1_16_15_1_9_U0.grp_blackLevelCorrection_13_1080_1920_1_16_15_1_9_Pipeline_VITIS_LOOP_91_1_fu_46.imgInput2_data239_blk_n);
    assign proc_3_data_PIPO_blk[1] = 1'b0;
    assign proc_3_start_FIFO_blk[1] = 1'b0;
    assign proc_3_TLF_FIFO_blk[1] = 1'b0;
    assign proc_3_input_sync_blk[1] = 1'b0;
    assign proc_3_output_sync_blk[1] = 1'b0;
    assign proc_dep_vld_vec_3[1] = dl_detect_out ? proc_dep_vld_vec_3_reg[1] : (proc_3_data_FIFO_blk[1] | proc_3_data_PIPO_blk[1] | proc_3_start_FIFO_blk[1] | proc_3_TLF_FIFO_blk[1] | proc_3_input_sync_blk[1] | proc_3_output_sync_blk[1]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_3_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_3_reg <= proc_dep_vld_vec_3;
        end
    end
    assign in_chan_dep_vld_vec_3[0] = dep_chan_vld_2_3;
    assign in_chan_dep_data_vec_3[12 : 0] = dep_chan_data_2_3;
    assign token_in_vec_3[0] = token_2_3;
    assign in_chan_dep_vld_vec_3[1] = dep_chan_vld_4_3;
    assign in_chan_dep_data_vec_3[25 : 13] = dep_chan_data_4_3;
    assign token_in_vec_3[1] = token_4_3;
    assign dep_chan_vld_3_2 = out_chan_dep_vld_vec_3[0];
    assign dep_chan_data_3_2 = out_chan_dep_data_3;
    assign token_3_2 = token_out_vec_3[0];
    assign dep_chan_vld_3_4 = out_chan_dep_vld_vec_3[1];
    assign dep_chan_data_3_4 = out_chan_dep_data_3;
    assign token_3_4 = token_out_vec_3[1];

    // Process: grp_ISPpipeline_fu_189.gaincontrol_3_13_1080_1920_1_U0
    ISPPipeline_accel_hls_deadlock_detect_unit #(13, 4, 3, 4) ISPPipeline_accel_hls_deadlock_detect_unit_4 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_4),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_4),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_4),
        .token_in_vec(token_in_vec_4),
        .dl_detect_in(dl_detect_out),
        .origin(origin[4]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_4),
        .out_chan_dep_data(out_chan_dep_data_4),
        .token_out_vec(token_out_vec_4),
        .dl_detect_out(dl_in_vec[4]));

    assign proc_4_data_FIFO_blk[0] = 1'b0;
    assign proc_4_data_PIPO_blk[0] = 1'b0;
    assign proc_4_start_FIFO_blk[0] = 1'b0;
    assign proc_4_TLF_FIFO_blk[0] = 1'b0 | (~grp_ISPpipeline_fu_189.imgInput2_rows_channel_U.if_empty_n & grp_ISPpipeline_fu_189.gaincontrol_3_13_1080_1920_1_U0.ap_idle & ~grp_ISPpipeline_fu_189.imgInput2_rows_channel_U.if_write) | (~grp_ISPpipeline_fu_189.imgInput2_cols_channel_U.if_empty_n & grp_ISPpipeline_fu_189.gaincontrol_3_13_1080_1920_1_U0.ap_idle & ~grp_ISPpipeline_fu_189.imgInput2_cols_channel_U.if_write);
    assign proc_4_input_sync_blk[0] = 1'b0;
    assign proc_4_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_4[0] = dl_detect_out ? proc_dep_vld_vec_4_reg[0] : (proc_4_data_FIFO_blk[0] | proc_4_data_PIPO_blk[0] | proc_4_start_FIFO_blk[0] | proc_4_TLF_FIFO_blk[0] | proc_4_input_sync_blk[0] | proc_4_output_sync_blk[0]);
    assign proc_4_data_FIFO_blk[1] = 1'b0 | (~grp_ISPpipeline_fu_189.gaincontrol_3_13_1080_1920_1_U0.grp_gaincontrol_3_13_1080_1920_1_Pipeline_ColLoop_fu_82.imgInput2_data239_blk_n);
    assign proc_4_data_PIPO_blk[1] = 1'b0;
    assign proc_4_start_FIFO_blk[1] = 1'b0;
    assign proc_4_TLF_FIFO_blk[1] = 1'b0;
    assign proc_4_input_sync_blk[1] = 1'b0;
    assign proc_4_output_sync_blk[1] = 1'b0;
    assign proc_dep_vld_vec_4[1] = dl_detect_out ? proc_dep_vld_vec_4_reg[1] : (proc_4_data_FIFO_blk[1] | proc_4_data_PIPO_blk[1] | proc_4_start_FIFO_blk[1] | proc_4_TLF_FIFO_blk[1] | proc_4_input_sync_blk[1] | proc_4_output_sync_blk[1]);
    assign proc_4_data_FIFO_blk[2] = 1'b0 | (~grp_ISPpipeline_fu_189.gaincontrol_3_13_1080_1920_1_U0.grp_gaincontrol_3_13_1080_1920_1_Pipeline_ColLoop_fu_82.gain_out_data240_blk_n);
    assign proc_4_data_PIPO_blk[2] = 1'b0;
    assign proc_4_start_FIFO_blk[2] = 1'b0;
    assign proc_4_TLF_FIFO_blk[2] = 1'b0;
    assign proc_4_input_sync_blk[2] = 1'b0;
    assign proc_4_output_sync_blk[2] = 1'b0;
    assign proc_dep_vld_vec_4[2] = dl_detect_out ? proc_dep_vld_vec_4_reg[2] : (proc_4_data_FIFO_blk[2] | proc_4_data_PIPO_blk[2] | proc_4_start_FIFO_blk[2] | proc_4_TLF_FIFO_blk[2] | proc_4_input_sync_blk[2] | proc_4_output_sync_blk[2]);
    assign proc_4_data_FIFO_blk[3] = 1'b0 | (~grp_ISPpipeline_fu_189.gaincontrol_3_13_1080_1920_1_U0.rgain_blk_n) | (~grp_ISPpipeline_fu_189.gaincontrol_3_13_1080_1920_1_U0.bgain_blk_n);
    assign proc_4_data_PIPO_blk[3] = 1'b0;
    assign proc_4_start_FIFO_blk[3] = 1'b0;
    assign proc_4_TLF_FIFO_blk[3] = 1'b0;
    assign proc_4_input_sync_blk[3] = 1'b0;
    assign proc_4_output_sync_blk[3] = 1'b0;
    assign proc_dep_vld_vec_4[3] = dl_detect_out ? proc_dep_vld_vec_4_reg[3] : (proc_4_data_FIFO_blk[3] | proc_4_data_PIPO_blk[3] | proc_4_start_FIFO_blk[3] | proc_4_TLF_FIFO_blk[3] | proc_4_input_sync_blk[3] | proc_4_output_sync_blk[3]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_4_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_4_reg <= proc_dep_vld_vec_4;
        end
    end
    assign in_chan_dep_vld_vec_4[0] = dep_chan_vld_0_4;
    assign in_chan_dep_data_vec_4[12 : 0] = dep_chan_data_0_4;
    assign token_in_vec_4[0] = token_0_4;
    assign in_chan_dep_vld_vec_4[1] = dep_chan_vld_3_4;
    assign in_chan_dep_data_vec_4[25 : 13] = dep_chan_data_3_4;
    assign token_in_vec_4[1] = token_3_4;
    assign in_chan_dep_vld_vec_4[2] = dep_chan_vld_5_4;
    assign in_chan_dep_data_vec_4[38 : 26] = dep_chan_data_5_4;
    assign token_in_vec_4[2] = token_5_4;
    assign dep_chan_vld_4_1 = out_chan_dep_vld_vec_4[0];
    assign dep_chan_data_4_1 = out_chan_dep_data_4;
    assign token_4_1 = token_out_vec_4[0];
    assign dep_chan_vld_4_3 = out_chan_dep_vld_vec_4[1];
    assign dep_chan_data_4_3 = out_chan_dep_data_4;
    assign token_4_3 = token_out_vec_4[1];
    assign dep_chan_vld_4_5 = out_chan_dep_vld_vec_4[2];
    assign dep_chan_data_4_5 = out_chan_dep_data_4;
    assign token_4_5 = token_out_vec_4[2];
    assign dep_chan_vld_4_0 = out_chan_dep_vld_vec_4[3];
    assign dep_chan_data_4_0 = out_chan_dep_data_4;
    assign token_4_0 = token_out_vec_4[3];

    // Process: grp_ISPpipeline_fu_189.demosaicing_3_13_17_1080_1920_1_false_U0
    ISPPipeline_accel_hls_deadlock_detect_unit #(13, 5, 2, 3) ISPPipeline_accel_hls_deadlock_detect_unit_5 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_5),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_5),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_5),
        .token_in_vec(token_in_vec_5),
        .dl_detect_in(dl_detect_out),
        .origin(origin[5]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_5),
        .out_chan_dep_data(out_chan_dep_data_5),
        .token_out_vec(token_out_vec_5),
        .dl_detect_out(dl_in_vec[5]));

    assign proc_5_data_FIFO_blk[0] = 1'b0;
    assign proc_5_data_PIPO_blk[0] = 1'b0;
    assign proc_5_start_FIFO_blk[0] = 1'b0;
    assign proc_5_TLF_FIFO_blk[0] = 1'b0 | (~grp_ISPpipeline_fu_189.gain_out_rows_channel_U.if_empty_n & grp_ISPpipeline_fu_189.demosaicing_3_13_17_1080_1920_1_false_U0.ap_idle & ~grp_ISPpipeline_fu_189.gain_out_rows_channel_U.if_write) | (~grp_ISPpipeline_fu_189.gain_out_cols_channel_U.if_empty_n & grp_ISPpipeline_fu_189.demosaicing_3_13_17_1080_1920_1_false_U0.ap_idle & ~grp_ISPpipeline_fu_189.gain_out_cols_channel_U.if_write);
    assign proc_5_input_sync_blk[0] = 1'b0;
    assign proc_5_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_5[0] = dl_detect_out ? proc_dep_vld_vec_5_reg[0] : (proc_5_data_FIFO_blk[0] | proc_5_data_PIPO_blk[0] | proc_5_start_FIFO_blk[0] | proc_5_TLF_FIFO_blk[0] | proc_5_input_sync_blk[0] | proc_5_output_sync_blk[0]);
    assign proc_5_data_FIFO_blk[1] = 1'b0 | (~grp_ISPpipeline_fu_189.demosaicing_3_13_17_1080_1920_1_false_U0.grp_demosaicing_3_13_17_1080_1920_1_false_Pipeline_Col_Loop_fu_322.gain_out_data240_blk_n);
    assign proc_5_data_PIPO_blk[1] = 1'b0;
    assign proc_5_start_FIFO_blk[1] = 1'b0;
    assign proc_5_TLF_FIFO_blk[1] = 1'b0;
    assign proc_5_input_sync_blk[1] = 1'b0;
    assign proc_5_output_sync_blk[1] = 1'b0;
    assign proc_dep_vld_vec_5[1] = dl_detect_out ? proc_dep_vld_vec_5_reg[1] : (proc_5_data_FIFO_blk[1] | proc_5_data_PIPO_blk[1] | proc_5_start_FIFO_blk[1] | proc_5_TLF_FIFO_blk[1] | proc_5_input_sync_blk[1] | proc_5_output_sync_blk[1]);
    assign proc_5_data_FIFO_blk[2] = 1'b0 | (~grp_ISPpipeline_fu_189.demosaicing_3_13_17_1080_1920_1_false_U0.grp_demosaicing_3_13_17_1080_1920_1_false_Pipeline_Col_Loop_fu_322.demosaic_out_data241_blk_n);
    assign proc_5_data_PIPO_blk[2] = 1'b0;
    assign proc_5_start_FIFO_blk[2] = 1'b0;
    assign proc_5_TLF_FIFO_blk[2] = 1'b0;
    assign proc_5_input_sync_blk[2] = 1'b0;
    assign proc_5_output_sync_blk[2] = 1'b0;
    assign proc_dep_vld_vec_5[2] = dl_detect_out ? proc_dep_vld_vec_5_reg[2] : (proc_5_data_FIFO_blk[2] | proc_5_data_PIPO_blk[2] | proc_5_start_FIFO_blk[2] | proc_5_TLF_FIFO_blk[2] | proc_5_input_sync_blk[2] | proc_5_output_sync_blk[2]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_5_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_5_reg <= proc_dep_vld_vec_5;
        end
    end
    assign in_chan_dep_vld_vec_5[0] = dep_chan_vld_4_5;
    assign in_chan_dep_data_vec_5[12 : 0] = dep_chan_data_4_5;
    assign token_in_vec_5[0] = token_4_5;
    assign in_chan_dep_vld_vec_5[1] = dep_chan_vld_6_5;
    assign in_chan_dep_data_vec_5[25 : 13] = dep_chan_data_6_5;
    assign token_in_vec_5[1] = token_6_5;
    assign dep_chan_vld_5_1 = out_chan_dep_vld_vec_5[0];
    assign dep_chan_data_5_1 = out_chan_dep_data_5;
    assign token_5_1 = token_out_vec_5[0];
    assign dep_chan_vld_5_4 = out_chan_dep_vld_vec_5[1];
    assign dep_chan_data_5_4 = out_chan_dep_data_5;
    assign token_5_4 = token_out_vec_5[1];
    assign dep_chan_vld_5_6 = out_chan_dep_vld_vec_5[2];
    assign dep_chan_data_5_6 = out_chan_dep_data_5;
    assign token_5_6 = token_out_vec_5[2];

    // Process: grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0
    ISPPipeline_accel_hls_deadlock_detect_unit #(13, 6, 6, 6) ISPPipeline_accel_hls_deadlock_detect_unit_6 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_6),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_6),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_6),
        .token_in_vec(token_in_vec_6),
        .dl_detect_in(dl_detect_out),
        .origin(origin[6]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_6),
        .out_chan_dep_data(out_chan_dep_data_6),
        .token_out_vec(token_out_vec_6),
        .dl_detect_out(dl_in_vec[6]));

    assign proc_6_data_FIFO_blk[0] = 1'b0;
    assign proc_6_data_PIPO_blk[0] = 1'b0;
    assign proc_6_start_FIFO_blk[0] = 1'b0;
    assign proc_6_TLF_FIFO_blk[0] = 1'b0 | (~grp_ISPpipeline_fu_189.demosaic_out_rows_channel_U.if_empty_n & grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.ap_idle & ~grp_ISPpipeline_fu_189.demosaic_out_rows_channel_U.if_write) | (~grp_ISPpipeline_fu_189.demosaic_out_cols_channel_U.if_empty_n & grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.ap_idle & ~grp_ISPpipeline_fu_189.demosaic_out_cols_channel_U.if_write) | (~grp_ISPpipeline_fu_189.ltm_in_rows_c23_channel_U.if_empty_n & grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.ap_idle & ~grp_ISPpipeline_fu_189.ltm_in_rows_c23_channel_U.if_write) | (~grp_ISPpipeline_fu_189.ltm_in_cols_c24_channel_U.if_empty_n & grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.ap_idle & ~grp_ISPpipeline_fu_189.ltm_in_cols_c24_channel_U.if_write) | (~grp_ISPpipeline_fu_189.thresh_loc_channel_U.if_empty_n & grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.ap_idle & ~grp_ISPpipeline_fu_189.thresh_loc_channel_U.if_write);
    assign proc_6_input_sync_blk[0] = 1'b0 | (grp_ISPpipeline_fu_189.ap_sync_function_awb_17_17_1080_1920_1_U0_ap_ready & grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.ap_idle & ~grp_ISPpipeline_fu_189.ap_sync_ISPpipeline_Block_entry1_proc_U0_ap_ready);
    assign proc_6_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_6[0] = dl_detect_out ? proc_dep_vld_vec_6_reg[0] : (proc_6_data_FIFO_blk[0] | proc_6_data_PIPO_blk[0] | proc_6_start_FIFO_blk[0] | proc_6_TLF_FIFO_blk[0] | proc_6_input_sync_blk[0] | proc_6_output_sync_blk[0]);
    assign proc_6_data_FIFO_blk[1] = 1'b0 | (~grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.grp_fifo_copy_17_17_1080_1920_1_s_fu_156.grp_fifo_copy_17_17_1080_1920_1_Pipeline_Col_Loop_fu_54.demosaic_out_data241_blk_n);
    assign proc_6_data_PIPO_blk[1] = 1'b0;
    assign proc_6_start_FIFO_blk[1] = 1'b0;
    assign proc_6_TLF_FIFO_blk[1] = 1'b0;
    assign proc_6_input_sync_blk[1] = 1'b0;
    assign proc_6_output_sync_blk[1] = 1'b0;
    assign proc_dep_vld_vec_6[1] = dl_detect_out ? proc_dep_vld_vec_6_reg[1] : (proc_6_data_FIFO_blk[1] | proc_6_data_PIPO_blk[1] | proc_6_start_FIFO_blk[1] | proc_6_TLF_FIFO_blk[1] | proc_6_input_sync_blk[1] | proc_6_output_sync_blk[1]);
    assign proc_6_data_FIFO_blk[2] = 1'b0 | (~grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.grp_fifo_copy_17_17_1080_1920_1_s_fu_156.grp_fifo_copy_17_17_1080_1920_1_Pipeline_Col_Loop_fu_54.ltm_in_data242_blk_n) | (~grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.ltm_in_rows_c_blk_n) | (~grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.ltm_in_cols_c_blk_n);
    assign proc_6_data_PIPO_blk[2] = 1'b0;
    assign proc_6_start_FIFO_blk[2] = 1'b0 | (~grp_ISPpipeline_fu_189.start_for_colorcorrectionmatrix_0_17_17_1080_1920_1_2_U0_U.if_full_n & grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.ap_start & ~grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.real_start & (trans_in_cnt_1 == trans_out_cnt_1) & ~grp_ISPpipeline_fu_189.start_for_colorcorrectionmatrix_0_17_17_1080_1920_1_2_U0_U.if_read);
    assign proc_6_TLF_FIFO_blk[2] = 1'b0;
    assign proc_6_input_sync_blk[2] = 1'b0;
    assign proc_6_output_sync_blk[2] = 1'b0;
    assign proc_dep_vld_vec_6[2] = dl_detect_out ? proc_dep_vld_vec_6_reg[2] : (proc_6_data_FIFO_blk[2] | proc_6_data_PIPO_blk[2] | proc_6_start_FIFO_blk[2] | proc_6_TLF_FIFO_blk[2] | proc_6_input_sync_blk[2] | proc_6_output_sync_blk[2]);
    assign proc_6_data_FIFO_blk[3] = 1'b0;
    assign proc_6_data_PIPO_blk[3] = 1'b0;
    assign proc_6_start_FIFO_blk[3] = 1'b0;
    assign proc_6_TLF_FIFO_blk[3] = 1'b0;
    assign proc_6_input_sync_blk[3] = 1'b0 | (grp_ISPpipeline_fu_189.ap_sync_function_awb_17_17_1080_1920_1_U0_ap_ready & grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.ap_idle & ~grp_ISPpipeline_fu_189.ap_sync_entry_proc_U0_ap_ready);
    assign proc_6_output_sync_blk[3] = 1'b0;
    assign proc_dep_vld_vec_6[3] = dl_detect_out ? proc_dep_vld_vec_6_reg[3] : (proc_6_data_FIFO_blk[3] | proc_6_data_PIPO_blk[3] | proc_6_start_FIFO_blk[3] | proc_6_TLF_FIFO_blk[3] | proc_6_input_sync_blk[3] | proc_6_output_sync_blk[3]);
    assign proc_6_data_FIFO_blk[4] = 1'b0;
    assign proc_6_data_PIPO_blk[4] = 1'b0;
    assign proc_6_start_FIFO_blk[4] = 1'b0;
    assign proc_6_TLF_FIFO_blk[4] = 1'b0;
    assign proc_6_input_sync_blk[4] = 1'b0 | (grp_ISPpipeline_fu_189.ap_sync_function_awb_17_17_1080_1920_1_U0_ap_ready & grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.ap_idle & ~grp_ISPpipeline_fu_189.ap_sync_gammacorrection_9_9_1080_1920_1_U0_ap_ready);
    assign proc_6_output_sync_blk[4] = 1'b0;
    assign proc_dep_vld_vec_6[4] = dl_detect_out ? proc_dep_vld_vec_6_reg[4] : (proc_6_data_FIFO_blk[4] | proc_6_data_PIPO_blk[4] | proc_6_start_FIFO_blk[4] | proc_6_TLF_FIFO_blk[4] | proc_6_input_sync_blk[4] | proc_6_output_sync_blk[4]);
    assign proc_6_data_FIFO_blk[5] = 1'b0;
    assign proc_6_data_PIPO_blk[5] = 1'b0;
    assign proc_6_start_FIFO_blk[5] = 1'b0;
    assign proc_6_TLF_FIFO_blk[5] = 1'b0;
    assign proc_6_input_sync_blk[5] = 1'b0;
    assign proc_6_output_sync_blk[5] = 1'b0 | (ap_done_reg_3 & grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.ap_done & ~grp_ISPpipeline_fu_189.ColorMat2AXIvideo_9_1080_1920_1_U0.ap_done);
    assign proc_dep_vld_vec_6[5] = dl_detect_out ? proc_dep_vld_vec_6_reg[5] : (proc_6_data_FIFO_blk[5] | proc_6_data_PIPO_blk[5] | proc_6_start_FIFO_blk[5] | proc_6_TLF_FIFO_blk[5] | proc_6_input_sync_blk[5] | proc_6_output_sync_blk[5]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_6_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_6_reg <= proc_dep_vld_vec_6;
        end
    end
    assign in_chan_dep_vld_vec_6[0] = dep_chan_vld_0_6;
    assign in_chan_dep_data_vec_6[12 : 0] = dep_chan_data_0_6;
    assign token_in_vec_6[0] = token_0_6;
    assign in_chan_dep_vld_vec_6[1] = dep_chan_vld_1_6;
    assign in_chan_dep_data_vec_6[25 : 13] = dep_chan_data_1_6;
    assign token_in_vec_6[1] = token_1_6;
    assign in_chan_dep_vld_vec_6[2] = dep_chan_vld_5_6;
    assign in_chan_dep_data_vec_6[38 : 26] = dep_chan_data_5_6;
    assign token_in_vec_6[2] = token_5_6;
    assign in_chan_dep_vld_vec_6[3] = dep_chan_vld_9_6;
    assign in_chan_dep_data_vec_6[51 : 39] = dep_chan_data_9_6;
    assign token_in_vec_6[3] = token_9_6;
    assign in_chan_dep_vld_vec_6[4] = dep_chan_vld_11_6;
    assign in_chan_dep_data_vec_6[64 : 52] = dep_chan_data_11_6;
    assign token_in_vec_6[4] = token_11_6;
    assign in_chan_dep_vld_vec_6[5] = dep_chan_vld_12_6;
    assign in_chan_dep_data_vec_6[77 : 65] = dep_chan_data_12_6;
    assign token_in_vec_6[5] = token_12_6;
    assign dep_chan_vld_6_1 = out_chan_dep_vld_vec_6[0];
    assign dep_chan_data_6_1 = out_chan_dep_data_6;
    assign token_6_1 = token_out_vec_6[0];
    assign dep_chan_vld_6_5 = out_chan_dep_vld_vec_6[1];
    assign dep_chan_data_6_5 = out_chan_dep_data_6;
    assign token_6_5 = token_out_vec_6[1];
    assign dep_chan_vld_6_9 = out_chan_dep_vld_vec_6[2];
    assign dep_chan_data_6_9 = out_chan_dep_data_6;
    assign token_6_9 = token_out_vec_6[2];
    assign dep_chan_vld_6_0 = out_chan_dep_vld_vec_6[3];
    assign dep_chan_data_6_0 = out_chan_dep_data_6;
    assign token_6_0 = token_out_vec_6[3];
    assign dep_chan_vld_6_11 = out_chan_dep_vld_vec_6[4];
    assign dep_chan_data_6_11 = out_chan_dep_data_6;
    assign token_6_11 = token_out_vec_6[4];
    assign dep_chan_vld_6_12 = out_chan_dep_vld_vec_6[5];
    assign dep_chan_data_6_12 = out_chan_dep_data_6;
    assign token_6_12 = token_out_vec_6[5];

    // Process: grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.grp_fifo_awb_17_17_1080_1920_1_s_fu_168.AWBhistogram_17_17_1080_1920_1_1_1024_U0
    ISPPipeline_accel_hls_deadlock_detect_unit #(13, 7, 1, 1) ISPPipeline_accel_hls_deadlock_detect_unit_7 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_7),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_7),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_7),
        .token_in_vec(token_in_vec_7),
        .dl_detect_in(dl_detect_out),
        .origin(origin[7]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_7),
        .out_chan_dep_data(out_chan_dep_data_7),
        .token_out_vec(token_out_vec_7),
        .dl_detect_out(dl_in_vec[7]));

    assign proc_7_data_FIFO_blk[0] = 1'b0 | (~grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.grp_fifo_awb_17_17_1080_1920_1_s_fu_168.AWBhistogram_17_17_1080_1920_1_1_1024_U0.grp_AWBhistogramkernel_17_17_1080_1920_1_21_1_1024_s_fu_64.grp_AWBhistogramkernel_17_17_1080_1920_1_21_1_1024_Pipeline_COL_LOOP_fu_284.impop_data1_blk_n);
    assign proc_7_data_PIPO_blk[0] = 1'b0;
    assign proc_7_start_FIFO_blk[0] = 1'b0;
    assign proc_7_TLF_FIFO_blk[0] = 1'b0;
    assign proc_7_input_sync_blk[0] = 1'b0 | (grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.grp_fifo_awb_17_17_1080_1920_1_s_fu_168.ap_sync_AWBhistogram_17_17_1080_1920_1_1_1024_U0_ap_ready & grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.grp_fifo_awb_17_17_1080_1920_1_s_fu_168.AWBhistogram_17_17_1080_1920_1_1_1024_U0.ap_idle & ~grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.grp_fifo_awb_17_17_1080_1920_1_s_fu_168.ap_sync_AWBNormalization_17_17_1080_1920_1_1_1024_U0_ap_ready);
    assign proc_7_output_sync_blk[0] = 1'b0 | (ap_done_reg_7 & grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.grp_fifo_awb_17_17_1080_1920_1_s_fu_168.AWBhistogram_17_17_1080_1920_1_1_1024_U0.ap_done & ~grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.grp_fifo_awb_17_17_1080_1920_1_s_fu_168.AWBNormalization_17_17_1080_1920_1_1_1024_U0.ap_done);
    assign proc_dep_vld_vec_7[0] = dl_detect_out ? proc_dep_vld_vec_7_reg[0] : (proc_7_data_FIFO_blk[0] | proc_7_data_PIPO_blk[0] | proc_7_start_FIFO_blk[0] | proc_7_TLF_FIFO_blk[0] | proc_7_input_sync_blk[0] | proc_7_output_sync_blk[0]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_7_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_7_reg <= proc_dep_vld_vec_7;
        end
    end
    assign in_chan_dep_vld_vec_7[0] = dep_chan_vld_8_7;
    assign in_chan_dep_data_vec_7[12 : 0] = dep_chan_data_8_7;
    assign token_in_vec_7[0] = token_8_7;
    assign dep_chan_vld_7_8 = out_chan_dep_vld_vec_7[0];
    assign dep_chan_data_7_8 = out_chan_dep_data_7;
    assign token_7_8 = token_out_vec_7[0];

    // Process: grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.grp_fifo_awb_17_17_1080_1920_1_s_fu_168.AWBNormalization_17_17_1080_1920_1_1_1024_U0
    ISPPipeline_accel_hls_deadlock_detect_unit #(13, 8, 1, 1) ISPPipeline_accel_hls_deadlock_detect_unit_8 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_8),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_8),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_8),
        .token_in_vec(token_in_vec_8),
        .dl_detect_in(dl_detect_out),
        .origin(origin[8]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_8),
        .out_chan_dep_data(out_chan_dep_data_8),
        .token_out_vec(token_out_vec_8),
        .dl_detect_out(dl_in_vec[8]));

    assign proc_8_data_FIFO_blk[0] = 1'b0 | (~grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.grp_fifo_awb_17_17_1080_1920_1_s_fu_168.AWBNormalization_17_17_1080_1920_1_1_1024_U0.grp_AWBNormalizationkernel_17_17_1080_1920_1_21_1_1024_2_s_fu_50.grp_AWBNormalizationkernel_17_17_1080_1920_1_21_1_1024_2_Pipeline_Col_Loop1_fu_294.impop_data1_blk_n);
    assign proc_8_data_PIPO_blk[0] = 1'b0;
    assign proc_8_start_FIFO_blk[0] = 1'b0;
    assign proc_8_TLF_FIFO_blk[0] = 1'b0;
    assign proc_8_input_sync_blk[0] = 1'b0 | (grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.grp_fifo_awb_17_17_1080_1920_1_s_fu_168.ap_sync_AWBNormalization_17_17_1080_1920_1_1_1024_U0_ap_ready & grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.grp_fifo_awb_17_17_1080_1920_1_s_fu_168.AWBNormalization_17_17_1080_1920_1_1_1024_U0.ap_idle & ~grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.grp_fifo_awb_17_17_1080_1920_1_s_fu_168.ap_sync_AWBhistogram_17_17_1080_1920_1_1_1024_U0_ap_ready);
    assign proc_8_output_sync_blk[0] = 1'b0 | (ap_done_reg_8 & grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.grp_fifo_awb_17_17_1080_1920_1_s_fu_168.AWBNormalization_17_17_1080_1920_1_1_1024_U0.ap_done & ~grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.grp_fifo_awb_17_17_1080_1920_1_s_fu_168.AWBhistogram_17_17_1080_1920_1_1_1024_U0.ap_done);
    assign proc_dep_vld_vec_8[0] = dl_detect_out ? proc_dep_vld_vec_8_reg[0] : (proc_8_data_FIFO_blk[0] | proc_8_data_PIPO_blk[0] | proc_8_start_FIFO_blk[0] | proc_8_TLF_FIFO_blk[0] | proc_8_input_sync_blk[0] | proc_8_output_sync_blk[0]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_8_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_8_reg <= proc_dep_vld_vec_8;
        end
    end
    assign in_chan_dep_vld_vec_8[0] = dep_chan_vld_7_8;
    assign in_chan_dep_data_vec_8[12 : 0] = dep_chan_data_7_8;
    assign token_in_vec_8[0] = token_7_8;
    assign dep_chan_vld_8_7 = out_chan_dep_vld_vec_8[0];
    assign dep_chan_data_8_7 = out_chan_dep_data_8;
    assign token_8_7 = token_out_vec_8[0];

    // Process: grp_ISPpipeline_fu_189.colorcorrectionmatrix_0_17_17_1080_1920_1_2_U0
    ISPPipeline_accel_hls_deadlock_detect_unit #(13, 9, 2, 2) ISPPipeline_accel_hls_deadlock_detect_unit_9 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_9),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_9),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_9),
        .token_in_vec(token_in_vec_9),
        .dl_detect_in(dl_detect_out),
        .origin(origin[9]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_9),
        .out_chan_dep_data(out_chan_dep_data_9),
        .token_out_vec(token_out_vec_9),
        .dl_detect_out(dl_in_vec[9]));

    assign proc_9_data_FIFO_blk[0] = 1'b0 | (~grp_ISPpipeline_fu_189.colorcorrectionmatrix_0_17_17_1080_1920_1_2_U0.p_src_mat_rows_blk_n) | (~grp_ISPpipeline_fu_189.colorcorrectionmatrix_0_17_17_1080_1920_1_2_U0.p_src_mat_cols_blk_n) | (~grp_ISPpipeline_fu_189.colorcorrectionmatrix_0_17_17_1080_1920_1_2_U0.grp_xfccmkernel_17_1080_1920_21_1_11_11_1920_2_s_fu_44.grp_xfccmkernel_17_1080_1920_21_1_11_11_1920_2_Pipeline_colLoop_fu_54.ltm_in_data242_blk_n);
    assign proc_9_data_PIPO_blk[0] = 1'b0;
    assign proc_9_start_FIFO_blk[0] = 1'b0 | (~grp_ISPpipeline_fu_189.start_for_colorcorrectionmatrix_0_17_17_1080_1920_1_2_U0_U.if_empty_n & grp_ISPpipeline_fu_189.colorcorrectionmatrix_0_17_17_1080_1920_1_2_U0.ap_idle & ~grp_ISPpipeline_fu_189.start_for_colorcorrectionmatrix_0_17_17_1080_1920_1_2_U0_U.if_write);
    assign proc_9_TLF_FIFO_blk[0] = 1'b0;
    assign proc_9_input_sync_blk[0] = 1'b0;
    assign proc_9_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_9[0] = dl_detect_out ? proc_dep_vld_vec_9_reg[0] : (proc_9_data_FIFO_blk[0] | proc_9_data_PIPO_blk[0] | proc_9_start_FIFO_blk[0] | proc_9_TLF_FIFO_blk[0] | proc_9_input_sync_blk[0] | proc_9_output_sync_blk[0]);
    assign proc_9_data_FIFO_blk[1] = 1'b0 | (~grp_ISPpipeline_fu_189.colorcorrectionmatrix_0_17_17_1080_1920_1_2_U0.grp_xfccmkernel_17_1080_1920_21_1_11_11_1920_2_s_fu_44.grp_xfccmkernel_17_1080_1920_21_1_11_11_1920_2_Pipeline_colLoop_fu_54.lsc_out_data243_blk_n);
    assign proc_9_data_PIPO_blk[1] = 1'b0;
    assign proc_9_start_FIFO_blk[1] = 1'b0;
    assign proc_9_TLF_FIFO_blk[1] = 1'b0;
    assign proc_9_input_sync_blk[1] = 1'b0;
    assign proc_9_output_sync_blk[1] = 1'b0;
    assign proc_dep_vld_vec_9[1] = dl_detect_out ? proc_dep_vld_vec_9_reg[1] : (proc_9_data_FIFO_blk[1] | proc_9_data_PIPO_blk[1] | proc_9_start_FIFO_blk[1] | proc_9_TLF_FIFO_blk[1] | proc_9_input_sync_blk[1] | proc_9_output_sync_blk[1]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_9_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_9_reg <= proc_dep_vld_vec_9;
        end
    end
    assign in_chan_dep_vld_vec_9[0] = dep_chan_vld_6_9;
    assign in_chan_dep_data_vec_9[12 : 0] = dep_chan_data_6_9;
    assign token_in_vec_9[0] = token_6_9;
    assign in_chan_dep_vld_vec_9[1] = dep_chan_vld_10_9;
    assign in_chan_dep_data_vec_9[25 : 13] = dep_chan_data_10_9;
    assign token_in_vec_9[1] = token_10_9;
    assign dep_chan_vld_9_6 = out_chan_dep_vld_vec_9[0];
    assign dep_chan_data_9_6 = out_chan_dep_data_9;
    assign token_9_6 = token_out_vec_9[0];
    assign dep_chan_vld_9_10 = out_chan_dep_vld_vec_9[1];
    assign dep_chan_data_9_10 = out_chan_dep_data_9;
    assign token_9_10 = token_out_vec_9[1];

    // Process: grp_ISPpipeline_fu_189.xf_QuatizationDithering_17_9_1080_1920_256_1024_1_U0
    ISPPipeline_accel_hls_deadlock_detect_unit #(13, 10, 2, 3) ISPPipeline_accel_hls_deadlock_detect_unit_10 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_10),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_10),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_10),
        .token_in_vec(token_in_vec_10),
        .dl_detect_in(dl_detect_out),
        .origin(origin[10]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_10),
        .out_chan_dep_data(out_chan_dep_data_10),
        .token_out_vec(token_out_vec_10),
        .dl_detect_out(dl_in_vec[10]));

    assign proc_10_data_FIFO_blk[0] = 1'b0;
    assign proc_10_data_PIPO_blk[0] = 1'b0;
    assign proc_10_start_FIFO_blk[0] = 1'b0;
    assign proc_10_TLF_FIFO_blk[0] = 1'b0 | (~grp_ISPpipeline_fu_189.lsc_out_rows_channel_U.if_empty_n & grp_ISPpipeline_fu_189.xf_QuatizationDithering_17_9_1080_1920_256_1024_1_U0.ap_idle & ~grp_ISPpipeline_fu_189.lsc_out_rows_channel_U.if_write) | (~grp_ISPpipeline_fu_189.lsc_out_cols_channel_U.if_empty_n & grp_ISPpipeline_fu_189.xf_QuatizationDithering_17_9_1080_1920_256_1024_1_U0.ap_idle & ~grp_ISPpipeline_fu_189.lsc_out_cols_channel_U.if_write);
    assign proc_10_input_sync_blk[0] = 1'b0;
    assign proc_10_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_10[0] = dl_detect_out ? proc_dep_vld_vec_10_reg[0] : (proc_10_data_FIFO_blk[0] | proc_10_data_PIPO_blk[0] | proc_10_start_FIFO_blk[0] | proc_10_TLF_FIFO_blk[0] | proc_10_input_sync_blk[0] | proc_10_output_sync_blk[0]);
    assign proc_10_data_FIFO_blk[1] = 1'b0 | (~grp_ISPpipeline_fu_189.xf_QuatizationDithering_17_9_1080_1920_256_1024_1_U0.grp_xf_QuatizationDithering_Pipeline_LOOP_ROW_LOOP_COL_fu_62.lsc_out_data243_blk_n);
    assign proc_10_data_PIPO_blk[1] = 1'b0;
    assign proc_10_start_FIFO_blk[1] = 1'b0;
    assign proc_10_TLF_FIFO_blk[1] = 1'b0;
    assign proc_10_input_sync_blk[1] = 1'b0;
    assign proc_10_output_sync_blk[1] = 1'b0;
    assign proc_dep_vld_vec_10[1] = dl_detect_out ? proc_dep_vld_vec_10_reg[1] : (proc_10_data_FIFO_blk[1] | proc_10_data_PIPO_blk[1] | proc_10_start_FIFO_blk[1] | proc_10_TLF_FIFO_blk[1] | proc_10_input_sync_blk[1] | proc_10_output_sync_blk[1]);
    assign proc_10_data_FIFO_blk[2] = 1'b0 | (~grp_ISPpipeline_fu_189.xf_QuatizationDithering_17_9_1080_1920_256_1024_1_U0.grp_xf_QuatizationDithering_Pipeline_LOOP_ROW_LOOP_COL_fu_62.aecin_data245_blk_n);
    assign proc_10_data_PIPO_blk[2] = 1'b0;
    assign proc_10_start_FIFO_blk[2] = 1'b0;
    assign proc_10_TLF_FIFO_blk[2] = 1'b0;
    assign proc_10_input_sync_blk[2] = 1'b0;
    assign proc_10_output_sync_blk[2] = 1'b0;
    assign proc_dep_vld_vec_10[2] = dl_detect_out ? proc_dep_vld_vec_10_reg[2] : (proc_10_data_FIFO_blk[2] | proc_10_data_PIPO_blk[2] | proc_10_start_FIFO_blk[2] | proc_10_TLF_FIFO_blk[2] | proc_10_input_sync_blk[2] | proc_10_output_sync_blk[2]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_10_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_10_reg <= proc_dep_vld_vec_10;
        end
    end
    assign in_chan_dep_vld_vec_10[0] = dep_chan_vld_9_10;
    assign in_chan_dep_data_vec_10[12 : 0] = dep_chan_data_9_10;
    assign token_in_vec_10[0] = token_9_10;
    assign in_chan_dep_vld_vec_10[1] = dep_chan_vld_11_10;
    assign in_chan_dep_data_vec_10[25 : 13] = dep_chan_data_11_10;
    assign token_in_vec_10[1] = token_11_10;
    assign dep_chan_vld_10_1 = out_chan_dep_vld_vec_10[0];
    assign dep_chan_data_10_1 = out_chan_dep_data_10;
    assign token_10_1 = token_out_vec_10[0];
    assign dep_chan_vld_10_9 = out_chan_dep_vld_vec_10[1];
    assign dep_chan_data_10_9 = out_chan_dep_data_10;
    assign token_10_9 = token_out_vec_10[1];
    assign dep_chan_vld_10_11 = out_chan_dep_vld_vec_10[2];
    assign dep_chan_data_10_11 = out_chan_dep_data_10;
    assign token_10_11 = token_out_vec_10[2];

    // Process: grp_ISPpipeline_fu_189.gammacorrection_9_9_1080_1920_1_U0
    ISPPipeline_accel_hls_deadlock_detect_unit #(13, 11, 5, 5) ISPPipeline_accel_hls_deadlock_detect_unit_11 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_11),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_11),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_11),
        .token_in_vec(token_in_vec_11),
        .dl_detect_in(dl_detect_out),
        .origin(origin[11]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_11),
        .out_chan_dep_data(out_chan_dep_data_11),
        .token_out_vec(token_out_vec_11),
        .dl_detect_out(dl_in_vec[11]));

    assign proc_11_data_FIFO_blk[0] = 1'b0;
    assign proc_11_data_PIPO_blk[0] = 1'b0;
    assign proc_11_start_FIFO_blk[0] = 1'b0;
    assign proc_11_TLF_FIFO_blk[0] = 1'b0 | (~grp_ISPpipeline_fu_189.aecin_rows_channel_U.if_empty_n & grp_ISPpipeline_fu_189.gammacorrection_9_9_1080_1920_1_U0.ap_idle & ~grp_ISPpipeline_fu_189.aecin_rows_channel_U.if_write) | (~grp_ISPpipeline_fu_189.aecin_cols_channel_U.if_empty_n & grp_ISPpipeline_fu_189.gammacorrection_9_9_1080_1920_1_U0.ap_idle & ~grp_ISPpipeline_fu_189.aecin_cols_channel_U.if_write);
    assign proc_11_input_sync_blk[0] = 1'b0 | (grp_ISPpipeline_fu_189.ap_sync_gammacorrection_9_9_1080_1920_1_U0_ap_ready & grp_ISPpipeline_fu_189.gammacorrection_9_9_1080_1920_1_U0.ap_idle & ~grp_ISPpipeline_fu_189.ap_sync_ISPpipeline_Block_entry1_proc_U0_ap_ready);
    assign proc_11_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_11[0] = dl_detect_out ? proc_dep_vld_vec_11_reg[0] : (proc_11_data_FIFO_blk[0] | proc_11_data_PIPO_blk[0] | proc_11_start_FIFO_blk[0] | proc_11_TLF_FIFO_blk[0] | proc_11_input_sync_blk[0] | proc_11_output_sync_blk[0]);
    assign proc_11_data_FIFO_blk[1] = 1'b0 | (~grp_ISPpipeline_fu_189.gammacorrection_9_9_1080_1920_1_U0.grp_gammacorrection_9_9_1080_1920_1_Pipeline_colLoop_fu_92.aecin_data245_blk_n);
    assign proc_11_data_PIPO_blk[1] = 1'b0;
    assign proc_11_start_FIFO_blk[1] = 1'b0;
    assign proc_11_TLF_FIFO_blk[1] = 1'b0;
    assign proc_11_input_sync_blk[1] = 1'b0;
    assign proc_11_output_sync_blk[1] = 1'b0;
    assign proc_dep_vld_vec_11[1] = dl_detect_out ? proc_dep_vld_vec_11_reg[1] : (proc_11_data_FIFO_blk[1] | proc_11_data_PIPO_blk[1] | proc_11_start_FIFO_blk[1] | proc_11_TLF_FIFO_blk[1] | proc_11_input_sync_blk[1] | proc_11_output_sync_blk[1]);
    assign proc_11_data_FIFO_blk[2] = 1'b0 | (~grp_ISPpipeline_fu_189.gammacorrection_9_9_1080_1920_1_U0.grp_gammacorrection_9_9_1080_1920_1_Pipeline_colLoop_fu_92.p_dst_data244_blk_n);
    assign proc_11_data_PIPO_blk[2] = 1'b0;
    assign proc_11_start_FIFO_blk[2] = 1'b0;
    assign proc_11_TLF_FIFO_blk[2] = 1'b0;
    assign proc_11_input_sync_blk[2] = 1'b0;
    assign proc_11_output_sync_blk[2] = 1'b0;
    assign proc_dep_vld_vec_11[2] = dl_detect_out ? proc_dep_vld_vec_11_reg[2] : (proc_11_data_FIFO_blk[2] | proc_11_data_PIPO_blk[2] | proc_11_start_FIFO_blk[2] | proc_11_TLF_FIFO_blk[2] | proc_11_input_sync_blk[2] | proc_11_output_sync_blk[2]);
    assign proc_11_data_FIFO_blk[3] = 1'b0;
    assign proc_11_data_PIPO_blk[3] = 1'b0;
    assign proc_11_start_FIFO_blk[3] = 1'b0;
    assign proc_11_TLF_FIFO_blk[3] = 1'b0;
    assign proc_11_input_sync_blk[3] = 1'b0 | (grp_ISPpipeline_fu_189.ap_sync_gammacorrection_9_9_1080_1920_1_U0_ap_ready & grp_ISPpipeline_fu_189.gammacorrection_9_9_1080_1920_1_U0.ap_idle & ~grp_ISPpipeline_fu_189.ap_sync_entry_proc_U0_ap_ready);
    assign proc_11_output_sync_blk[3] = 1'b0;
    assign proc_dep_vld_vec_11[3] = dl_detect_out ? proc_dep_vld_vec_11_reg[3] : (proc_11_data_FIFO_blk[3] | proc_11_data_PIPO_blk[3] | proc_11_start_FIFO_blk[3] | proc_11_TLF_FIFO_blk[3] | proc_11_input_sync_blk[3] | proc_11_output_sync_blk[3]);
    assign proc_11_data_FIFO_blk[4] = 1'b0;
    assign proc_11_data_PIPO_blk[4] = 1'b0;
    assign proc_11_start_FIFO_blk[4] = 1'b0;
    assign proc_11_TLF_FIFO_blk[4] = 1'b0;
    assign proc_11_input_sync_blk[4] = 1'b0 | (grp_ISPpipeline_fu_189.ap_sync_gammacorrection_9_9_1080_1920_1_U0_ap_ready & grp_ISPpipeline_fu_189.gammacorrection_9_9_1080_1920_1_U0.ap_idle & ~grp_ISPpipeline_fu_189.ap_sync_function_awb_17_17_1080_1920_1_U0_ap_ready);
    assign proc_11_output_sync_blk[4] = 1'b0;
    assign proc_dep_vld_vec_11[4] = dl_detect_out ? proc_dep_vld_vec_11_reg[4] : (proc_11_data_FIFO_blk[4] | proc_11_data_PIPO_blk[4] | proc_11_start_FIFO_blk[4] | proc_11_TLF_FIFO_blk[4] | proc_11_input_sync_blk[4] | proc_11_output_sync_blk[4]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_11_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_11_reg <= proc_dep_vld_vec_11;
        end
    end
    assign in_chan_dep_vld_vec_11[0] = dep_chan_vld_0_11;
    assign in_chan_dep_data_vec_11[12 : 0] = dep_chan_data_0_11;
    assign token_in_vec_11[0] = token_0_11;
    assign in_chan_dep_vld_vec_11[1] = dep_chan_vld_1_11;
    assign in_chan_dep_data_vec_11[25 : 13] = dep_chan_data_1_11;
    assign token_in_vec_11[1] = token_1_11;
    assign in_chan_dep_vld_vec_11[2] = dep_chan_vld_6_11;
    assign in_chan_dep_data_vec_11[38 : 26] = dep_chan_data_6_11;
    assign token_in_vec_11[2] = token_6_11;
    assign in_chan_dep_vld_vec_11[3] = dep_chan_vld_10_11;
    assign in_chan_dep_data_vec_11[51 : 39] = dep_chan_data_10_11;
    assign token_in_vec_11[3] = token_10_11;
    assign in_chan_dep_vld_vec_11[4] = dep_chan_vld_12_11;
    assign in_chan_dep_data_vec_11[64 : 52] = dep_chan_data_12_11;
    assign token_in_vec_11[4] = token_12_11;
    assign dep_chan_vld_11_1 = out_chan_dep_vld_vec_11[0];
    assign dep_chan_data_11_1 = out_chan_dep_data_11;
    assign token_11_1 = token_out_vec_11[0];
    assign dep_chan_vld_11_10 = out_chan_dep_vld_vec_11[1];
    assign dep_chan_data_11_10 = out_chan_dep_data_11;
    assign token_11_10 = token_out_vec_11[1];
    assign dep_chan_vld_11_12 = out_chan_dep_vld_vec_11[2];
    assign dep_chan_data_11_12 = out_chan_dep_data_11;
    assign token_11_12 = token_out_vec_11[2];
    assign dep_chan_vld_11_0 = out_chan_dep_vld_vec_11[3];
    assign dep_chan_data_11_0 = out_chan_dep_data_11;
    assign token_11_0 = token_out_vec_11[3];
    assign dep_chan_vld_11_6 = out_chan_dep_vld_vec_11[4];
    assign dep_chan_data_11_6 = out_chan_dep_data_11;
    assign token_11_6 = token_out_vec_11[4];

    // Process: grp_ISPpipeline_fu_189.ColorMat2AXIvideo_9_1080_1920_1_U0
    ISPPipeline_accel_hls_deadlock_detect_unit #(13, 12, 2, 3) ISPPipeline_accel_hls_deadlock_detect_unit_12 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_12),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_12),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_12),
        .token_in_vec(token_in_vec_12),
        .dl_detect_in(dl_detect_out),
        .origin(origin[12]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_12),
        .out_chan_dep_data(out_chan_dep_data_12),
        .token_out_vec(token_out_vec_12),
        .dl_detect_out(dl_in_vec[12]));

    assign proc_12_data_FIFO_blk[0] = 1'b0;
    assign proc_12_data_PIPO_blk[0] = 1'b0;
    assign proc_12_start_FIFO_blk[0] = 1'b0;
    assign proc_12_TLF_FIFO_blk[0] = 1'b0 | (~grp_ISPpipeline_fu_189.p_dst_rows_channel_U.if_empty_n & grp_ISPpipeline_fu_189.ColorMat2AXIvideo_9_1080_1920_1_U0.ap_idle & ~grp_ISPpipeline_fu_189.p_dst_rows_channel_U.if_write) | (~grp_ISPpipeline_fu_189.p_dst_cols_channel_U.if_empty_n & grp_ISPpipeline_fu_189.ColorMat2AXIvideo_9_1080_1920_1_U0.ap_idle & ~grp_ISPpipeline_fu_189.p_dst_cols_channel_U.if_write);
    assign proc_12_input_sync_blk[0] = 1'b0;
    assign proc_12_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_12[0] = dl_detect_out ? proc_dep_vld_vec_12_reg[0] : (proc_12_data_FIFO_blk[0] | proc_12_data_PIPO_blk[0] | proc_12_start_FIFO_blk[0] | proc_12_TLF_FIFO_blk[0] | proc_12_input_sync_blk[0] | proc_12_output_sync_blk[0]);
    assign proc_12_data_FIFO_blk[1] = 1'b0 | (~grp_ISPpipeline_fu_189.ColorMat2AXIvideo_9_1080_1920_1_U0.grp_ColorMat2AXIvideo_9_1080_1920_1_Pipeline_loop_col_mat2axi_fu_82.p_dst_data244_blk_n);
    assign proc_12_data_PIPO_blk[1] = 1'b0;
    assign proc_12_start_FIFO_blk[1] = 1'b0;
    assign proc_12_TLF_FIFO_blk[1] = 1'b0;
    assign proc_12_input_sync_blk[1] = 1'b0;
    assign proc_12_output_sync_blk[1] = 1'b0;
    assign proc_dep_vld_vec_12[1] = dl_detect_out ? proc_dep_vld_vec_12_reg[1] : (proc_12_data_FIFO_blk[1] | proc_12_data_PIPO_blk[1] | proc_12_start_FIFO_blk[1] | proc_12_TLF_FIFO_blk[1] | proc_12_input_sync_blk[1] | proc_12_output_sync_blk[1]);
    assign proc_12_data_FIFO_blk[2] = 1'b0;
    assign proc_12_data_PIPO_blk[2] = 1'b0;
    assign proc_12_start_FIFO_blk[2] = 1'b0;
    assign proc_12_TLF_FIFO_blk[2] = 1'b0;
    assign proc_12_input_sync_blk[2] = 1'b0;
    assign proc_12_output_sync_blk[2] = 1'b0 | (ap_done_reg_6 & grp_ISPpipeline_fu_189.ColorMat2AXIvideo_9_1080_1920_1_U0.ap_done & ~grp_ISPpipeline_fu_189.function_awb_17_17_1080_1920_1_U0.ap_done);
    assign proc_dep_vld_vec_12[2] = dl_detect_out ? proc_dep_vld_vec_12_reg[2] : (proc_12_data_FIFO_blk[2] | proc_12_data_PIPO_blk[2] | proc_12_start_FIFO_blk[2] | proc_12_TLF_FIFO_blk[2] | proc_12_input_sync_blk[2] | proc_12_output_sync_blk[2]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_12_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_12_reg <= proc_dep_vld_vec_12;
        end
    end
    assign in_chan_dep_vld_vec_12[0] = dep_chan_vld_6_12;
    assign in_chan_dep_data_vec_12[12 : 0] = dep_chan_data_6_12;
    assign token_in_vec_12[0] = token_6_12;
    assign in_chan_dep_vld_vec_12[1] = dep_chan_vld_11_12;
    assign in_chan_dep_data_vec_12[25 : 13] = dep_chan_data_11_12;
    assign token_in_vec_12[1] = token_11_12;
    assign dep_chan_vld_12_1 = out_chan_dep_vld_vec_12[0];
    assign dep_chan_data_12_1 = out_chan_dep_data_12;
    assign token_12_1 = token_out_vec_12[0];
    assign dep_chan_vld_12_11 = out_chan_dep_vld_vec_12[1];
    assign dep_chan_data_12_11 = out_chan_dep_data_12;
    assign token_12_11 = token_out_vec_12[1];
    assign dep_chan_vld_12_6 = out_chan_dep_vld_vec_12[2];
    assign dep_chan_data_12_6 = out_chan_dep_data_12;
    assign token_12_6 = token_out_vec_12[2];


`include "ISPPipeline_accel_hls_deadlock_report_unit.vh"
