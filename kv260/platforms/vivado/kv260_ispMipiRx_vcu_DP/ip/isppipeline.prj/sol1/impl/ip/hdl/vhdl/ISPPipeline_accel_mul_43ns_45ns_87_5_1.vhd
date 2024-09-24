-- ==============================================================
-- Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.1 (64-bit)
-- Version: 2022.1
-- Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
-- ==============================================================
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity ISPPipeline_accel_mul_43ns_45ns_87_5_1 is
generic (
    ID : INTEGER := 1;
    NUM_STAGE : INTEGER := 4;
    din0_WIDTH : INTEGER := 14;
    din1_WIDTH : INTEGER := 12;
    dout_WIDTH : INTEGER := 26);
port (
    clk: in std_logic;
    ce: in std_logic;
    reset: in std_logic;
    din0: in std_logic_vector(din0_WIDTH - 1 downto 0);
    din1: in std_logic_vector(din1_WIDTH - 1 downto 0);
    dout: out std_logic_vector(dout_WIDTH - 1 downto 0));
end entity;

architecture behav of ISPPipeline_accel_mul_43ns_45ns_87_5_1 is
    signal tmp_product : std_logic_vector(dout_WIDTH - 1 downto 0);
    signal a_i : std_logic_vector(din0_WIDTH - 1 downto 0);
    signal b_i : std_logic_vector(din1_WIDTH - 1 downto 0);
    signal p_tmp : std_logic_vector(dout_WIDTH - 1 downto 0);
    signal a_reg0 : std_logic_vector(din0_WIDTH - 1 downto 0);
    signal b_reg0 : std_logic_vector(din1_WIDTH - 1 downto 0);

    signal buff0 : std_logic_vector(dout_WIDTH - 1 downto 0);
    signal buff1 : std_logic_vector(dout_WIDTH - 1 downto 0);
    signal buff2 : std_logic_vector(dout_WIDTH - 1 downto 0);
begin
    a_i <= din0;
    b_i <= din1;
    dout <= p_tmp;

    p_tmp <= buff2;
    tmp_product <= std_logic_vector(resize(unsigned(a_reg0) * unsigned(b_reg0), dout_WIDTH));

    process(clk)
    begin
        if (clk'event and clk = '1') then
            if (ce = '1') then
                a_reg0 <= a_i;
                b_reg0 <= b_i;
                buff0 <= tmp_product;
                buff1 <= buff0;
                buff2 <= buff1;
            end if;
        end if;
    end process;
end architecture;
