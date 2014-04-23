library verilog;
use verilog.vl_types.all;
entity MUX4 is
    port(
        SEL             : in     vl_logic_vector(1 downto 0);
        IN0             : in     vl_logic_vector(31 downto 0);
        IN1             : in     vl_logic_vector(31 downto 0);
        IN2             : in     vl_logic_vector(31 downto 0);
        IN3             : in     vl_logic_vector(31 downto 0);
        \OUT\           : out    vl_logic_vector(31 downto 0)
    );
end MUX4;
