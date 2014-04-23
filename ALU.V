`timescale 10ps / 1ps
// module ALU(.Ain(),.Bin(),.Cin(),.ALUout(),.Cout(),.Vout(),.OP());
module ALU(Ain,Bin,Cin,ALUout,Cout,Vout,OPCODE);
input [31:0]Ain,Bin;
input [31:0]OPCODE;
input Cin;
output reg [31:0]ALUout;
output reg Cout;
output Vout;

reg [3:0]OP_temp;
wire [3:0] Decode;

// always@(OPCODE)
// begin
// if((OPCODE[27:25] == 3'b001) || ((OPCODE[27:25] == 3'b000)&&(OPCODE[7:4] != 4'b1001)))
	// OP_temp = OPCODE[24:21];
// else if ((OPCODE[27:22] == 6'b000000)&&(OPCODE[7:4] == 4'b1001)) ///MULTIPLY
	// OP_temp = 4'B1101;
// else if ((OPCODE[27:22] == 5'b00001)&&(OPCODE[7:4] == 4'b1001))		//
	// OP_temp = 4'B0100;
// else
	// OP_temp = 4'B1101;
// end

I_Decode Shift_I_Decode(.OPCODE(OPCODE),.Dec(Decode));

always@(Decode)
	begin
	case(Decode)
		1:
			begin
				if(OPCODE[21] == 1'b0)
					OP_temp  = 4'B1101;
				else
					OP_temp = 4'B0100;
			end		
		7:OP_temp = OPCODE[24:21];
		11:OP_temp = 4'B1101;
		default:OP_temp=3'b000;
	endcase
	end
	
	

assign Vout = (Ain[31] & Bin[31] & ~ALUout[31] ) | (~Ain[31] & ~Bin[31] & ALUout[31] );
always@(OP_temp,Ain,Bin,Cin)
begin
case(OP_temp)
	4'B0000: {Cout,ALUout} = Ain & Bin;
	4'B0001: {Cout,ALUout} = Ain ^ Bin;
	4'B0010: {Cout,ALUout} = Ain - Bin;
	4'B0011: {Cout,ALUout} = Bin - Ain;
	4'B0100: {Cout,ALUout} = Ain + Bin;
	4'B0101: {Cout,ALUout} = Ain + Bin + Cin;
	4'B0110: {Cout,ALUout} = Ain - Bin + Cin - 1;
	4'B0111: {Cout,ALUout} = Bin - Ain + Cin - 1;
	4'B1000: {Cout,ALUout} = Ain & Bin;
	4'B1001: {Cout,ALUout} = Ain ^ Bin;
	4'B1010: {Cout,ALUout} = Ain - Bin;
	4'B1011: {Cout,ALUout} = Ain + Bin;
	4'B1100: {Cout,ALUout} = Ain | Bin;
	4'B1101: {Cout,ALUout} = {Cin,Bin};
	4'B1110: {Cout,ALUout} = Ain & ~(Bin);
	4'B1111: {Cout,ALUout} = ~({Cin,Bin});
endcase
// $display("ALUResult:%t\t%h\t%h\t%h\t%h", $time, Ain, Bin,ALUout,OPCODE);
end
endmodule
