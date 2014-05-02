`timescale 10ps / 1ps
// MUX2 --- ( .SEL(),.IN0(),.IN1(), .OUT() );
// DestADDGen  ---- (.OPCODE(),.GCnt_Out_EXE(),.DESTADD());
module DestADDGen( OPCODE, GCnt_Out_EXE,DESTADD);

input [31:0]OPCODE;
input GCnt_Out_EXE;
output reg [3:0]DESTADD;

wire [3:0]Decode;


I_Decode Shift_I_Decode(.OPCODE(OPCODE),.Dec(Decode));

always@(OPCODE,Decode,GCnt_Out_EXE)
	begin
		case(Decode)
		1 : DESTADD = OPCODE[19:16];
		2 : begin	
				if(GCnt_Out_EXE)
					DESTADD = OPCODE[19:16];
				else          
					DESTADD = OPCODE[15:12];
			end
		3 : DESTADD = OPCODE[15:12];
		4 : DESTADD = OPCODE[15:12];
		5 : DESTADD = OPCODE[15:12];
		6 : DESTADD = OPCODE[15:12];
		7 : DESTADD = OPCODE[15:12];
		8 : begin	
				if(OPCODE[20] == 1'b0)   //Store Instruction
					DESTADD = OPCODE[19:16];
				else if ((OPCODE[20] == 1'b1) & (GCnt_Out_EXE == 1'b0)) //Load Instruction  Cycle 0        
					DESTADD = OPCODE[15:12];
				else
					DESTADD = OPCODE[19:16];
			end
		9 : DESTADD = 4'bzzzzz;
		10: DESTADD = OPCODE;
		11: DESTADD = OPCODE[15:12];
		default: DESTADD = 4'bzzzzz;
		endcase
	end

endmodule
