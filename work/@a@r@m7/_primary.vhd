library verilog;
use verilog.vl_types.all;
entity ARM7 is
    generic(
        O               : integer := 28;
        C               : integer := 29;
        Z               : integer := 30;
        N               : integer := 31
    );
    port(
        clk             : in     vl_logic;
        rst             : in     vl_logic;
        OUTPORT         : out    vl_logic_vector(7 downto 0);
        INPORT          : in     vl_logic_vector(7 downto 0)
    );
end ARM7;
