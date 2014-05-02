`timescale 10ps / 1ps
// DecROM --- (.OPCODE(),.CntOut());
module MemROM(OPCODE,GCnt,GCnt2,CntOut);

input GCnt;
input GCnt2;
input [31:0] OPCODE;
output reg [8:0] CntOut;
wire [3:0] Decode;

// Control Out Bit Decription
// [7] MEM_FWD_Sel 
// [6:5] B_HW_W
// [4] READ_MEM
// [3:2] MEM_ADD_SUB
// [1] MEM_DIN_Sel
// [0] MEM_FWD_Sel 


// reg [2:0]CntOut_temp;


I_Decode Shift_I_Decode(.OPCODE(OPCODE),.Dec(Decode));

always@(Decode,OPCODE,GCnt,GCnt2)
	begin
	case(Decode)
		3:CntOut  = {1'b1, OPCODE[22], 6'b011000};
		7:CntOut  = 8'B000000;
		6:	begin
				if(OPCODE[20] == 1'b0)   //Store Instruction
					CntOut = {2'b10,OPCODE[22],!OPCODE[5],!GCnt,4'b0001};
				else  //Load Instriuction
					CntOut = {2'b10,OPCODE[5],!OPCODE[5],1'b0,4'b0001};
			end
		8:	begin
				if(OPCODE[20] == 1'b0)   //Store Instruction
					CntOut = {2'b00,OPCODE[22],1'b0,!GCnt,4'b0001};
				else  //Load Instriuction
					CntOut = {2'b00,OPCODE[22],1'b0,1'b0,4'b0001};
			end
		9:CntOut  = {1'b0,OPCODE[20],2'b00,!OPCODE[20],OPCODE[24],OPCODE[24],2'b00};
		10:CntOut  = {6'b00};
		default:CntOut  = 8'B0000000;
	endcase
	end
endmodule
