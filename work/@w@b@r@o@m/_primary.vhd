library verilog;
use verilog.vl_types.all;
entity WBROM is
    port(
        OPCODE          : in     vl_logic_vector(31 downto 0);
        GCnt            : in     vl_logic_vector(1 downto 0);
        CntOut          : out    vl_logic_vector(4 downto 0)
    );
end WBROM;
