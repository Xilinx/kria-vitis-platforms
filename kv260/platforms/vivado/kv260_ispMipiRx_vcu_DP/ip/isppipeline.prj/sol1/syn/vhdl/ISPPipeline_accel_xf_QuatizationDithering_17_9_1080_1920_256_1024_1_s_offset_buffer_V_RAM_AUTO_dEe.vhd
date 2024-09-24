-- ==============================================================
-- Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.1 (64-bit)
-- Tool Version Limit: 2022.04
-- Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
-- ==============================================================
--
library ieee; 
use ieee.std_logic_1164.all; 
use ieee.std_logic_unsigned.all;

entity ISPPipeline_accel_xf_QuatizationDithering_17_9_1080_1920_256_1024_1_s_offset_buffer_V_RAM_AUTO_dEe is 
    generic(
            MEM_TYPE    : string := "auto"; 
            DataWidth     : integer := 9; 
            AddressWidth     : integer := 11; 
            AddressRange    : integer := 1920
    ); 
    port (
          address0     : in std_logic_vector(AddressWidth-1 downto 0); 
          ce0       : in std_logic; 
          d0        : in std_logic_vector(DataWidth-1 downto 0); 
          we0       : in std_logic; 
          address1     : in std_logic_vector(AddressWidth-1 downto 0); 
          ce1       : in std_logic; 
          q1        : out std_logic_vector(DataWidth-1 downto 0);
          reset      : in std_logic; 
          clk        : in std_logic 
    ); 
end entity; 


architecture rtl of ISPPipeline_accel_xf_QuatizationDithering_17_9_1080_1920_256_1024_1_s_offset_buffer_V_RAM_AUTO_dEe is 

signal address1_tmp : std_logic_vector(AddressWidth-1 downto 0); 
type mem_array is array (0 to AddressRange-1) of std_logic_vector (DataWidth-1 downto 0); 
signal ram : mem_array;

attribute syn_ramstyle : string; 
attribute syn_ramstyle of ram : signal is "auto";
attribute ram_style : string;
attribute ram_style of ram : signal is MEM_TYPE;

begin 



p_memory_access_0: process (clk)  
begin 
    if (clk'event and clk = '1') then
        if (ce0 = '1') then 
            if (we0 = '1') then 
                ram(CONV_INTEGER(address0)) <= d0; 
            end if;
        end if;
    end if;
end process;

memory_access_guard_1: process (address1) 
begin
      address1_tmp <= address1;
--synthesis translate_off
      if (CONV_INTEGER(address1) > AddressRange-1) then
           address1_tmp <= (others => '0');
      else 
           address1_tmp <= address1;
      end if;
--synthesis translate_on
end process;

p_memory_access_1: process (clk)  
begin 
    if (clk'event and clk = '1') then
        if (ce1 = '1') then 
            q1 <= ram(CONV_INTEGER(address1_tmp));
        end if;
    end if;
end process;


end rtl;

