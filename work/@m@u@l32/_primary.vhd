library verilog;
use verilog.vl_types.all;
entity MUL32 is
    port(
        INA             : in     vl_logic_vector(31 downto 0);
        INB             : in     vl_logic_vector(31 downto 0);
        SIGN            : in     vl_logic;
        \OUT\           : out    vl_logic_vector(63 downto 0)
    );
end MUL32;
