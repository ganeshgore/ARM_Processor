`timescale 10ps / 1ps
// DecROM --- (.OPCODE(),.CntOut());
module MemROM(OPCODE,GCnt,CntOut);

input [1:0] GCnt;
input [31:0] OPCODE;
output reg [6:0] CntOut;
wire [3:0] Decode;

// Control Out Bit Decription
// CntOut[6:5] = B_HW_W;
// CntOut[4] = READ_MEM;
// CntOut[3:2] = MEM_ADD_SUB[2];
// CntOut[1] = MEM_DIN_Sel;
// CntOut[0] = MEM_FWD_Sel; 


// reg [2:0]CntOut_temp;


I_Decode Shift_I_Decode(.OPCODE(OPCODE),.Dec(Decode));

always@(Decode)
	begin
	case(Decode)
		7:CntOut  = 7'B000000;
		8:CntOut  = {OPCODE[22],1'b0,!OPCODE[20],!OPCODE[24],1'b0,1'b0,1'b1};
		9:CntOut  = {OPCODE[22],1'b0,!OPCODE[20],OPCODE[24],1'b0,1'b0,1'b0};
		10:CntOut  = {6'b00};
		default:CntOut  = 7'B000000;
	endcase
	end

// always@(CntOut_temp)
	// begin
		// case(CntOut_temp)	
			// 3'B001: CntOut  = 5'B00000;		//Default Selection
			// default: CntOut = 5'B00000;		//Default Selection
		// endcase
	// end
endmodule
