library verilog;
use verilog.vl_types.all;
entity SFTUnit is
    port(
        INA             : in     vl_logic_vector(31 downto 0);
        INB             : in     vl_logic_vector(31 downto 0);
        OPCODE          : in     vl_logic_vector(31 downto 0);
        Cin             : in     vl_logic;
        TMP_CR          : in     vl_logic;
        \OUT\           : out    vl_logic_vector(31 downto 0);
        Cout            : out    vl_logic
    );
end SFTUnit;
