`timescale 10ps / 1ps

module SFTUnit_TB;

	// Inputs
	reg [31:0]INA;
	reg signed [31:0]INB;
	reg [31:0]OPCODE;
	reg Cin;

	// Outputs
	wire [31:0] OUT;	
	wire Cout;

	SFTUnit uut(.INA(INA),.INB(INB),.OPCODE(OPCODE),.Cin(Cin),.OUT(OUT),.Cout(Cout));

	
	
	initial begin
		INA = 32'h00000050;
		INB = 32'h00000010;
		OPCODE = 32'he3a00050;
		Cin = 1'b0;

		
		
		#10000 $stop;$finish;
        
		// Add stimulus here

	end
      
endmodule

