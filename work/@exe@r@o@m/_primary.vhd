library verilog;
use verilog.vl_types.all;
entity ExeROM is
    port(
        OPCODE          : in     vl_logic_vector(31 downto 0);
        GCnt            : in     vl_logic;
        GCnt2           : in     vl_logic;
        CntOut          : out    vl_logic_vector(13 downto 0)
    );
end ExeROM;
