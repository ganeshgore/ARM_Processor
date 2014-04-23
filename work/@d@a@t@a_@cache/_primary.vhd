library verilog;
use verilog.vl_types.all;
entity DATA_Cache is
    generic(
        DATA_WIDTH      : integer := 8;
        ADDR_WIDTH      : integer := 32
    );
    port(
        MADD            : in     vl_logic_vector(31 downto 0);
        MDATA           : in     vl_logic_vector(31 downto 0);
        MDOUT           : out    vl_logic_vector(31 downto 0);
        \type\          : in     vl_logic_vector(1 downto 0);
        RW              : in     vl_logic;
        OUTPORT         : out    vl_logic_vector(7 downto 0);
        INPORT          : in     vl_logic_vector(7 downto 0)
    );
end DATA_Cache;
