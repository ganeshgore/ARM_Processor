library verilog;
use verilog.vl_types.all;
entity MUX2 is
    port(
        SEL             : in     vl_logic;
        IN0             : in     vl_logic_vector(31 downto 0);
        IN1             : in     vl_logic_vector(31 downto 0);
        \OUT\           : out    vl_logic_vector(31 downto 0)
    );
end MUX2;
