`timescale 10ps / 1ps
// MUX4 ---- (.SEL(),.IN0(),.IN1(),.IN2(),.IN3(),.OUT());
module MUX4( SEL, IN0,IN1,IN2,IN3, OUT );

parameter size = 32;

input [1:0] SEL;
input [size-1:0] IN0;
input [size-1:0] IN1;
input [size-1:0] IN2;
input [size-1:0] IN3;
output reg [size-1:0] OUT;


always@(SEL,IN0,IN1,IN2,IN3)
	begin
	 case (SEL) 
	   2'b00 : OUT = IN0;
	   2'b01 : OUT = IN1;
	   2'b10 : OUT = IN2;
	   2'b11 : OUT = IN3;
	endcase
end 

endmodule
