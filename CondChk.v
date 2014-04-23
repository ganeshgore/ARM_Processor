`timescale 10ps / 1ps
// CondChk ---- ( .OPCODE(),.CPSR(),.OUT());
module CondChk( OPCODE,CPSR,OUT);

input [3:0] OPCODE;
input [3:0] CPSR;
output reg OUT;


always@(OPCODE,CPSR)
	begin
	 case (OPCODE) 
	   4'd00 : OUT = CPSR[1];
	   4'd01 : OUT = !CPSR[1];
	   4'd02 : OUT = CPSR[2];
	   4'd03 : OUT = !CPSR[2];
	   4'd04 : OUT = CPSR[0];
	   4'd05 : OUT = !CPSR[0];
	   4'd06 : OUT = CPSR[3];
	   4'd07 : OUT = !CPSR[3];
	   4'd08 : OUT = CPSR[2] & !CPSR[1];
	   4'd09 : OUT = !CPSR[2] & CPSR[1];
	   4'd10 : OUT = !(CPSR[0] ^ CPSR[3]);
	   4'd11 : OUT = CPSR[0] ^ CPSR[3];
	   4'd12 : OUT = !CPSR[1] & (!(CPSR[0] ^ CPSR[3]));
	   4'd13 : OUT = CPSR[1] & ((CPSR[0] ^ CPSR[3]));
	   default: OUT = 1;
	endcase
end 

endmodule