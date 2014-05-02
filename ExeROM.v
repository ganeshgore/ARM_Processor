`timescale 10ps / 1ps
// DecROM --- (.OPCODE(),.CntOut());
module ExeROM(OPCODE,GCnt,GCnt2,CntOut);

input GCnt;
input GCnt2;
input [31:0] OPCODE;
output reg [13:0] CntOut;
wire [3:0] Decode;


// Control Out Bit Decription
// CntOut[13] 	= SIGN_MUL
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

always@(Decode,OPCODE,GCnt,GCnt2)
	begin
	case(Decode)
		1:
			begin
				if(OPCODE[21] == 1'b1)
					CntOut  = {OPCODE[20] , 12'b000010100000 , OPCODE[20]};   ///Multiply Short Accumulate
				else
					CntOut  = {OPCODE[20] , 12'b000010100000 , OPCODE[20]};	 ///Multiply Short 
			end
		2: 
			begin
				if(OPCODE[21] == 1'b1)
					CntOut  = {OPCODE[20] , 2'b00 , GCnt , 3'b010, !GCnt,  5'b00000 , OPCODE[20]};  /// Multiply Long Accumulate
				else
					CntOut  = {OPCODE[20] , 2'b00 , GCnt, 9'b010100000 , OPCODE[20]}; /// Multiply Long
			end
		3:CntOut  = {14'B00100000000000};
		7:CntOut  = {12'B000000000000,OPCODE[20]};
		6:begin
				if(OPCODE[20] == 1'b0)   //Store Instruction
					CntOut = {11'b00000000000,!(OPCODE[24]|GCnt),2'b10};
				else  //Load Instriuction
					CntOut = {11'b00000000000,!(OPCODE[24]|GCnt),2'b10};
			end
		8:begin
				if(OPCODE[20] == 1'b0)   //Store Instruction
					CntOut = {11'b00000000000,!(OPCODE[24]|GCnt),2'b10};
				else  //Load Instriuction
					CntOut = {11'b00000000000,!(OPCODE[24]|GCnt),2'b10};
			end
		9:CntOut  = {10'B000100000,GCnt,!GCnt,2'b10};
		10:CntOut  = 14'B01000000000000;
		11:CntOut  = {13'B000000000000,OPCODE[20]};
		default:CntOut  = 14'B0000000000000; 
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
