`timescale 10ps / 1ps
// REG32  xxx (.clk(),.rst(),.load(),.IN(),.OUT());
module REG4(clk,rst,load,IN, OUT);

input wire clk;


always@(posedge clk)
begin
	if(rst == 1)
		OUT <= 4'bzzzz;
	else
		if (load == 1)
			OUT <= IN;
end

endmodule