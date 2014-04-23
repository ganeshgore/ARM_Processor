`timescale 10ps / 1ps
// REG32  xxx (.clk(),.rst(),.load(),.IN(),.OUT());
module CPSRREG(clk,rst,load,IN,Vin,Cin,OUT);

input wire clk;input wire rst;input wire load;
input wire Cin;
input wire Vin;input wire [31:0]IN;output reg [31:0]OUT;

wire O;
wire C;
wire Z;
wire N;

assign O = Vin;
assign C = Cin;
assign Z = (IN == 32'h0000) ? 1:0;
assign N = IN[31];

always@(posedge clk)
begin
	if(rst == 1)
		OUT = 32'h0000;
	else
		if (load == 1)
			OUT = {O, C, Z, N,4'b0000,24'h000};
end

endmodule
