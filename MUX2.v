`timescale 10ps / 1ps
module MUX2( SEL, IN0,IN1, OUT );

input SEL;
input [31:0] IN0;
input [31:0] IN1;
output reg [31:0] OUT;



always@(SEL,IN0,IN1)
	begin
	 case (SEL) 
	   1'b0 : OUT = IN0;
	   1'b1 : OUT = IN1;
	endcase
end 

endmodule