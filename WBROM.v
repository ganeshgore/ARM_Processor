`timescale 10ps / 1ps
// DecROM --- (.OPCODE(),.CntOut());
module WBROM(OPCODE,GCnt,CntOut);

input [1:0] GCnt;
input [31:0] OPCODE;
output reg [4:0] CntOut;
wire [3:0] Decode;

// Control Out Bit Decription
// CntOut[4:2]  = REG_WADD_SEl;
// CntOut[1] 	= REG_WDAT_Sel;
// CntOut[0] 	= REG_W; 


reg [2:0]CntOut_temp;

I_Decode Shift_I_Decode(.OPCODE(OPCODE),.Dec(Decode));

always@(Decode,GCnt,OPCODE)
	begin
	case(Decode)
		1:CntOut  = 5'B01001;
		7:CntOut  = 5'B00001;
		8:begin
				if(OPCODE[20] == 1'b0)
					CntOut  = {4'B0110,OPCODE[21]};
				else
					CntOut  = 5'B00011;
			end
		9:
			begin
					CntOut = {OPCODE[24],2'b10,1'b1,1'b1};
			end
		10:CntOut = {4'B1010,OPCODE[24]};
		11:CntOut = 5'B00001;
		default:CntOut = 5'B00000;
	endcase
	end
endmodule
