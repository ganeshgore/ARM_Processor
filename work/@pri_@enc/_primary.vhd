library verilog;
use verilog.vl_types.all;
entity Pri_Enc is
    port(
        encoder_in      : in     vl_logic_vector(15 downto 0);
        binary_out      : out    vl_logic_vector(3 downto 0)
    );
end Pri_Enc;
