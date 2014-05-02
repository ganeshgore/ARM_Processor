`timescale 10ps / 1ps
// Counter2 (.clk(clk),.rst(rst),.IN(GCnt_IN),.Out(GCnt_Out))
module Counter2( clk, rst,IN, Out );

input clk;
input rst;
input [1:0] IN;
output reg [1:0] Out;

always@(posedge clk)
	begin
	if(rst == 1)
		Out <= 2'b00;
	else
		if(Out == 2'b00 && IN != 2'b00)
			Out <= IN;
		else
			if (Out == 2'b00)
				Out <= 2'b00;
			else
				Out <= Out-1;
end 

endmodule