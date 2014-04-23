library verilog;
use verilog.vl_types.all;
entity DecROM is
    port(
        OPCODE          : in     vl_logic_vector(31 downto 0);
        CntOut          : out    vl_logic_vector(5 downto 0)
    );
end DecROM;
