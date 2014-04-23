library verilog;
use verilog.vl_types.all;
entity CondChk is
    port(
        OPCODE          : in     vl_logic_vector(3 downto 0);
        CPSR            : in     vl_logic_vector(3 downto 0);
        \OUT\           : out    vl_logic
    );
end CondChk;
