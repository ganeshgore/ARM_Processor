`timescale 10ps / 1ps
// REG2  xxx (.clk(),.rst(),.IN(),.OUT());
module REG2(clk,rst,IN, OUT);

input wire clk;
input wire rst;
input wire [1:0]IN;
output reg [1:0]OUT;

always@(posedge clk)
begin
	if(rst == 1)
		OUT <= 2'b00;
	else
			OUT <= IN;
end

endmodule
