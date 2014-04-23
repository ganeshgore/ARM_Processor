library verilog;
use verilog.vl_types.all;
entity RegFile is
    generic(
        DATA_WIDTH      : integer := 32;
        REGFILE_WIDTH   : integer := 4
    );
    port(
        clk             : in     vl_logic;
        rst             : in     vl_logic;
        RADD1           : in     vl_logic_vector;
        RADD2           : in     vl_logic_vector;
        RADD3           : in     vl_logic_vector;
        RDAT1           : out    vl_logic_vector;
        RDAT2           : out    vl_logic_vector;
        RDAT3           : out    vl_logic_vector;
        WADD            : in     vl_logic_vector;
        WDAT            : in     vl_logic_vector;
        WEN             : in     vl_logic
    );
end RegFile;
