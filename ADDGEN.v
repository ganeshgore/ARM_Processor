`timescale 10ps / 1ps
// ADDGEN  ----- (.INA(), .INB(), .OPCODE(), .OUT())
module ADDGEN( INA, INB, OPCODE, OUT);

input [31:0] INA;
input [31:0] INB;
input [31:0] OPCODE;
output reg [31:0] OUT;



always@(INA,INB,OPCODE)
	begin
	 if (OPCODE[27:26] == 2'b01)		//Address Generation 32BIT
		begin
			if(OPCODE[25] == 1'b1 && OPCODE[23] == 1'b1)
				OUT <= INB + OPCODE[11:0];
			else if(OPCODE[25] == 1'b1 && OPCODE[23] == 1'b0)
				OUT <= INB - OPCODE[11:0];
			else if(OPCODE[25] == 1'b0 && OPCODE[23] == 1'b0)
				OUT <= INB - INA;
			else if(OPCODE[25] == 1'b0 && OPCODE[23] == 1'b1)
				OUT <= INB + INA;
			else
				OUT <= 32'bx;		
		end
	else if(OPCODE[27:26] == 2'b00)
		begin
			if(OPCODE[7:4] == 4'b1011 && OPCODE[22] == 1'b1)
				OUT <= INB + {OPCODE[11:7],OPCODE[3:0]};
			else if(OPCODE[7:4] == 4'b1011 && OPCODE[22] == 1'b0)
				OUT <= INB + INA;
			else if(OPCODE[7:4] == 4'b11x1)
				OUT <= INB + {OPCODE[11:7],OPCODE[3:0]};
			else
				OUT <= 32'bx;
		end
	else if(OPCODE[27:25] == 3'b101)
		begin
			OUT <= INB + INA;
		end
	else
		OUT <= 32'bx;
end 

endmodule