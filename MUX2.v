`timescale 10ps / 1ps
// MUX2 --- ( .SEL(),.IN0(),.IN1(), .OUT() );
module MUX2( SEL, IN0,IN1, OUT );

parameter size = 32;
input SEL;
input [size-1:0] IN0;
input [size-1:0] IN1;
output reg [size-1:0] OUT;



always@(SEL,IN0,IN1)
	begin
	 case (SEL) 
	   1'b0 : OUT = IN0;
	   1'b1 : OUT = IN1;
	endcase
end 

endmodule
