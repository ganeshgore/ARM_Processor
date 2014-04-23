library verilog;
use verilog.vl_types.all;
entity I_Cache is
    generic(
        DATA_WIDTH      : integer := 32;
        ADDR_WIDTH      : integer := 32
    );
    port(
        ADD             : in     vl_logic_vector(31 downto 0);
        \OUT\           : out    vl_logic_vector(31 downto 0)
    );
end I_Cache;
