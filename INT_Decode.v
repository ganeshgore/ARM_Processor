`timescale 10ps / 1ps
// I_Decode -- (.OPCODE(),.Dec());
module I_Decode(OPCODE,Dec);

input [31:0]OPCODE;
output reg [3:0]Dec;

always@(OPCODE)
	begin
	case(OPCODE[27:26])
	2'b10:
			// Branch or Block Data Instruction
			begin
				if(OPCODE[25] == 0)
					Dec = 4'd9;		//-----------Block Data Transfer
				else
					Dec = 4'd10;		//-----------Branch Instruction
			end
	2'b01:
			// Load Store
			begin
				if(OPCODE[4] == 0)
					Dec	= 4'd8;		//-----------Load Store Intruction
				else
					Dec = 4'b0;
			end
	2'b00:
			//Regular Instruction
			begin
				if(OPCODE[25] == 1'b0 && OPCODE[7]==1'b1 && OPCODE[4]==1'b1)
					begin
						if(OPCODE[11:4] == 8'b00001001 && OPCODE[24] == 1'b1)
							Dec = 4'd3; 		//-----------Single Data Swap
						else if(OPCODE[11:4] == 8'b00001011 && OPCODE[22] == 1'b0)
							Dec = 4'd4;		//-----------Half word Data Transfer Register offeset 
						else if(OPCODE[7:4] == 4'b1001 && OPCODE[23] == 1'b0)
							Dec = 4'd1; 		//-----------Multiply
						else if(OPCODE[7:4] == 4'b1001 && OPCODE[23] == 1'b1)
							Dec = 4'd2; 		//-----------Multiply Long
						else if(OPCODE[7:4] == 4'b1011 && OPCODE[22] == 1'b1)
							Dec = 4'd5; 		//-----------Half Word Data Transfer Immidiate 
						else if((OPCODE[7:6] == 2'b11)&&(OPCODE[4]==1'b1))
							Dec = 4'd6; 		//-----------Signed Data Transfer
						else
							Dec = 1'b0;
					end
				else
					if (OPCODE[25] == 1'b1)
						Dec	= 4'd11;				//-----------Data Processing PSR Transfer	
					else	
						Dec = 4'd7;
					
			end
	default : Dec=3'b0 ;//NoTValid
	endcase
end 

endmodule