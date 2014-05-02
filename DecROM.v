`timescale 10ps / 1ps
// DecROM --- (.OPCODE(),.CntOut());
module DecROM(OPCODE,GCnt,CntOut);

input [31:0] OPCODE;
input GCnt;
output reg [8:0] CntOut;
wire [3:0] Decode;


// Control Out Bit Decription
// [8] DestSel
// [7] DestADD_Val
// [6] GCnt_In_Dec
// [5] load_mul_unit
// [4] INC_Order
// [3] R3_Sel
// [2] R2_Sel
// [1] R1_Sel
// [0] OP3_Sel 




I_Decode Shift_I_Decode(.OPCODE(OPCODE),.Dec(Decode));

always@(Decode,OPCODE,GCnt)
	begin
	case(Decode)
		1:CntOut  = 9'b000001000;
		2: begin
			if(OPCODE[21] == 1'b0)
				CntOut  ={2'b00,GCnt,6'B001000};
			else
				CntOut  ={2'b00,GCnt,2'b00,GCnt,3'b000};
			end
		3:CntOut  = 9'b000000000;
		7:CntOut  = 9'B000000000;
		11:CntOut = 9'B000000000;
		6:begin
				if(OPCODE[20] == 1'b0)   //Store Instruction
					CntOut = {2'b00,!(OPCODE[24]|GCnt),6'b000100};
				else  //Load Instriuction
					CntOut = {2'b01,(OPCODE[21]|(!OPCODE[24]))&(!GCnt),6'b000000};
		end
		8:begin
				if(OPCODE[20] == 1'b0)   //Store Instruction
					CntOut = {2'b00,!(OPCODE[24]|GCnt),6'b000100};
				else  //Load Instriuction
					CntOut = {2'b01,(OPCODE[21]|(!OPCODE[24]))&(!GCnt),6'b000000};
		end
		9:CntOut  = {1'b1,OPCODE[20],!GCnt,1'b1,OPCODE[23],4'b0010};
		10:CntOut = 9'b000000001;
		default:CntOut=9'b000000000;
	endcase
	end

endmodule
