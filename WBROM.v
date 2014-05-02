`timescale 10ps / 1ps
// DecROM --- (.OPCODE(),.CntOut());
module WBROM(OPCODE,GCnt,CntOut);

input GCnt;
input [31:0] OPCODE;
output reg [2:0] CntOut;
wire [3:0] Decode;

// Control Out Bit Decription
// CntOut[2]  = WB_FWD_SEL;
// CntOut[1] 	= REG_WDAT_Sel;
// CntOut[0] 	= REG_W; 


reg [2:0]CntOut_temp;

I_Decode Shift_I_Decode(.OPCODE(OPCODE),.Dec(Decode));

always@(Decode,GCnt,OPCODE)
	begin
	case(Decode)
		1:CntOut  = 3'B001;
		2:CntOut  = 3'B001;
		3:CntOut  = 3'B111;
		7:CntOut  = 3'B00001;
		6:	begin
				if(OPCODE[20] == 1'b0)   //Store Instruction
					CntOut = {2'b00,((!GCnt & OPCODE[21])|(GCnt&(!OPCODE[24])))};
				else  //Load Instriuction
					CntOut = {!GCnt,!GCnt,1'b1};
			end
		8:	begin
				if(OPCODE[20] == 1'b0)   //Store Instruction
					CntOut = {2'b00,((!GCnt & OPCODE[21])|(GCnt&(!OPCODE[24])))};
				else  //Load Instriuction
					CntOut = {!GCnt,!GCnt,1'b1};
			end
		9: CntOut = {2'B11,OPCODE[20]};
		10:CntOut = {3'b000};
		11:CntOut = 5'B00001;
		default:CntOut = 5'B00000;
	endcase
	end
endmodule
