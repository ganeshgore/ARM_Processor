`timescale 10ps / 1ps
// MUL32(.INA(),.INB(),.OUT());
module MUL32(INA,INB,OUT);

input [31:0] INA;
input [31:0] INB;
output reg [63:0] OUT;

always@(INA,INB)
	begin
		OUT = INA*INB;
		// $display("MULTIPLIER:%t\t%h\t%h\t%h", $time, INA, INB,OUT);
	end


endmodule