`timescale 10ps / 1ps
// DecROM --- (.OPCODE(),.CntOut());
module DecROM(OPCODE,CntOut);

input [31:0] OPCODE;
output reg [5:0] CntOut;
wire [3:0] Decode;


// Control Out Bit Decription
// CntOut[5:4] = GCnt_IN;
// CntOut[3] = R2_Sel;
// CntOut[2] = R3_Sel;
// CntOut[1] = OP1_Sel;
// CntOut[0] = OP3_Sel; 


reg [2:0]CntOut_temp;

I_Decode Shift_I_Decode(.OPCODE(OPCODE),.Dec(Decode));

always@(Decode)
	begin
	case(Decode)
		1:CntOut_temp  = 3'B010;
		7:CntOut_temp  = 3'B001;
		8:CntOut_temp  = 3'B011;
		9:CntOut_temp  = 3'B101;
		10:CntOut_temp = 3'B100;
		default:CntOut_temp=3'b000;
	endcase
	end
	
	
always@(CntOut_temp)
	begin
		case(CntOut_temp)	
							 //3210
			3'B001: CntOut = 6'B000000;		//Default Selection
			3'B010: CntOut = 6'B000100;		//Default Selection
			3'B011: CntOut = 6'B001000;		//Default Selection
			3'B100: CntOut = 6'B000001;		//Default Selection
			3'B101: CntOut = 6'B110000;		//Default Selection
			default: CntOut = 6'B000000;		//Default Selection
		endcase
	end
endmodule
