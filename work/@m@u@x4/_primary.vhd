library verilog;
use verilog.vl_types.all;
entity MUX4 is
    generic(
        size            : integer := 32
    );
    port(
        SEL             : in     vl_logic_vector(1 downto 0);
        IN0             : in     vl_logic_vector;
        IN1             : in     vl_logic_vector;
        IN2             : in     vl_logic_vector;
        IN3             : in     vl_logic_vector;
        \OUT\           : out    vl_logic_vector
    );
end MUX4;
