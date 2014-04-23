`timescale 10ps / 1ps
// DecROM --- (.OPCODE(),.CntOut());
module ExeROM(OPCODE,GCnt,CntOut);

input [1:0] GCnt;
input [31:0] OPCODE;
output reg [12:0] CntOut;
wire [3:0] Decode;


// Control Out Bit Decription
// CntOut[12] 	= Br_EN
// CntOut[11] 	= RD_IN_SEL
// CntOut[10] 	= MULSFT_EN; 
// CntOut[9] 	= Ain_Sel; 
// CntOut[8:7]  = Bin_Sel; 
// CntOut[6]   	= Cin_Sel; 
// CntOut[5:4]  = EX_OUT_SEL; 
// CntOut[3:2] 	= EX_ADD_SEL; 
// CntOut[1]   	= EX_ADDSUB; 
// CntOut[0]   	= LOAD_CPSR; 


reg [2:0]CntOut_temp;

I_Decode Shift_I_Decode(.OPCODE(OPCODE),.Dec(Decode));

always@(Decode,GCnt)
	begin
	case(Decode)
		1:
			begin
				if(OPCODE[21] == 1'b0)
					CntOut  = 13'B0000100000000;
				else
					CntOut  = 13'B0000010000000;
			end
		7:CntOut  = {12'B000000000000,OPCODE[20]};
		8:CntOut  = 13'B0000000000010;
		9:
			begin
				if(GCnt == 2'b11)
					CntOut  = 13'B0000000000110;
				else
					CntOut  = 13'B0000000001010;
			end
		10:CntOut  = 13'B1000000110010;
		11:CntOut  = {12'B000000001000,OPCODE[20]};
		default:CntOut  = 13'B0000000000000; 
	endcase
	// $display("EXE_DECODE\tInstruction:%h\tDecode:%h", OPCODE, Decode);
	end

// always@(CntOut_temp)
	// begin
		// case(CntOut_temp)	
								// 109876543210
			// 3'B001: CntOut  = 12'B000000000000;		//Default Selection
			// 3'B010: CntOut  = 12'B000000010000;		//forward OP2 to next Stage
			// 3'B011: CntOut  = 12'B000100000000;		//Bypasss Multiplication through ALU
			// 3'B100: CntOut  = 12'B000010000000;		//ADD in ALU for MLA Instruction
			// 3'B101: CntOut  = 12'B000000000010;		//Address Genertation STR
			// default: CntOut = 12'B000000000000;		//Default Selection
		// endcase
	// end
endmodule
