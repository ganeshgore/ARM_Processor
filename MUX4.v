`timescale 10ps / 1ps
module MUX4( SEL, IN0,IN1,IN2,IN3, OUT );

input [1:0] SEL;
input [31:0] IN0;
input [31:0] IN1;
input [31:0] IN2;
input [31:0] IN3;
output reg [31:0] OUT;


always@(SEL,IN0,IN1,IN2,IN3)
	begin
	 case (SEL) 
	   1'b00 : OUT = IN0;
	   1'b01 : OUT = IN1;
	   1'b10 : OUT = IN2;
	   1'b11 : OUT = IN3;
	endcase
end 

endmodule