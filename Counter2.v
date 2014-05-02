`timescale 10ps / 1ps
// Counter2 (.clk(clk),.rst(rst),.IN(GCnt_IN),.Out(GCnt_Out))
// module Counter2( .clk(), .rst(), .T(), .Out() );
module Counter2( clk, rst, In, Out );

input clk;
input rst;
input In;
output reg Out;

always@(posedge clk)
	begin
	if(rst == 1)
		Out <= 1'b0;
	else
		if(In == 1'b1)
			Out <= !Out;
end 

endmodule
