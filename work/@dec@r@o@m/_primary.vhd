library verilog;
use verilog.vl_types.all;
entity DecROM is
    port(
        OPCODE          : in     vl_logic_vector(31 downto 0);
        GCnt            : in     vl_logic;
        CntOut          : out    vl_logic_vector(8 downto 0)
    );
end DecROM;
