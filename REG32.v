`timescale 10ps / 1ps
// REG32  xxx (.clk(),.rst(),.load(),.IN(),.OUT());
module REG32(clk,rst,load,IN, OUT);

input wire clk;input wire rst;input wire load;input wire [31:0]IN;output reg [31:0]OUT;


always@(posedge clk)
begin
	if(rst == 1)
		OUT <= 32'hFFFFFFFF;
	else
		if (load == 1)
			OUT <= IN;
end

endmodule
