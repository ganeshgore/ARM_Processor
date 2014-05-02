`timescale 10ps / 1ps
// REG32  xxx (.clk(),.rst(),.load(),.IN(),.OUT());
module REG5(clk,rst,load,IN, OUT);

input wire clk;input wire rst;input wire load;input wire [4:0]IN;output reg [4:0]OUT;


always@(posedge clk)
begin
	if(rst == 1)
		OUT <= 5'bzzzzz;
	else
		if (load == 1)
			OUT <= IN;
end

endmodule
