library verilog;
use verilog.vl_types.all;
entity REG2 is
    port(
        clk             : in     vl_logic;
        rst             : in     vl_logic;
        \IN\            : in     vl_logic_vector(1 downto 0);
        \OUT\           : out    vl_logic_vector(1 downto 0)
    );
end REG2;
