`timescale 10ps / 1ps
// MUL32(.INA(),.INB(),.SIGN(),.OUT());
module MUL32(INA,INB,SIGN,OUT);

input [31:0] INA;
input [31:0] INB;
input SIGN;
output reg [63:0] OUT;

wire signed INA_SIGN;
wire signed INB_SIGN;

assign INA_SIGN = INA;
assign INB_SIGN = INB;

always@(INA,INB)
	begin
		if (SIGN)
			OUT = INA_SIGN*INB_SIGN;
		else
			OUT = INA*INB;
		// $display("MULTIPLIER:%t\t%h\t%h\t%h", $time, INA, INB,OUT);
	end


endmodule