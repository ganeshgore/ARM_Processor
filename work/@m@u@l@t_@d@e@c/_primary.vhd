library verilog;
use verilog.vl_types.all;
entity MULT_DEC is
    port(
        clk             : in     vl_logic;
        rst             : in     vl_logic;
        load            : in     vl_logic;
        OPCODE          : in     vl_logic_vector(15 downto 0);
        INC             : in     vl_logic;
        \Out\           : out    vl_logic_vector(3 downto 0);
        NoAdd           : out    vl_logic
    );
end MULT_DEC;
