`timescale 10ps / 1ps
// REG2  xxx (.clk(),.rst(),.IN(),.OUT());
module REG1(clk,rst,IN, OUT);

input wire clk;
input wire rst;
input wire IN;
output reg OUT;

always@(posedge clk)
begin
	if(rst == 1)
		OUT <= 1'b0;
	else
		OUT <= IN;
end

endmodule
