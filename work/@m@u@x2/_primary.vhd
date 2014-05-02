library verilog;
use verilog.vl_types.all;
entity MUX2 is
    generic(
        size            : integer := 32
    );
    port(
        SEL             : in     vl_logic;
        IN0             : in     vl_logic_vector;
        IN1             : in     vl_logic_vector;
        \OUT\           : out    vl_logic_vector
    );
end MUX2;
