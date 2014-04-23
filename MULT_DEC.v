`timescale 10ps / 1ps
// MULT_DEC ----- (.OPCODE(), .Cnt(), .Out() ,.Order());
module MULT_DEC(OPCODE, Cnt, Out ,Order);

input [15:0]OPCODE;
input [1:0]Cnt;
input Order;
output reg [3:0] Out;

wire [3:0]ADD0,ADD1,ADD2,ADD3;
wire [15:0]hot0,hot1,hot2,hot3;
wire [15:0]enc_in0,enc_in1,enc_in2,enc_in3;

Pri_Enc  Pri_Enc0(ADD0 ,OPCODE);
hot_code hot_code0(ADD0,hot0);

Pri_Enc  Pri_Enc1(ADD1,enc_in0);
hot_code hot_code1(ADD1,hot1);

Pri_Enc  Pri_Enc2(ADD2,enc_in1);
hot_code hot_code2(ADD2,hot2);

Pri_Enc  Pri_Enc3(ADD3,enc_in2);
hot_code hot_code3(ADD3,hot3);

assign enc_in0 = OPCODE  & (hot0);
assign enc_in1 = enc_in0 & (hot1);
assign enc_in2 = enc_in1 & (hot2);
assign enc_in3 = enc_in2 & (hot3);


always@(Cnt,ADD0,ADD1,ADD2,ADD3,Order)
	begin
	if(Order == 1)
	begin
		 case(Cnt)
		 2'b11:Out = ADD0;
		 2'b10:Out = ADD1;
		 2'b01:Out = ADD2;
		 default:Out = ADD3;
		 endcase
	end
	else
	begin
		case(Cnt)
		 2'b11:Out = ADD3;
		 2'b10:Out = ADD2;
		 2'b01:Out = ADD1;
		 default:Out = ADD0;
		 endcase
	end
end 


endmodule


module hot_code(bin,hot);
input [3:0]bin;
output reg [15:0]hot;
always@(bin)
begin
	case(bin)
	4'd00: hot = ~(16'd1);
	4'd01: hot = ~(16'd2);
	4'd02: hot = ~(16'd4);
	4'd03: hot = ~(16'd8);
	4'd04: hot = ~(16'd16);
	4'd05: hot = ~(16'd32);
	4'd06: hot = ~(16'd64);
	4'd07: hot = ~(16'd128);
	4'd08: hot = ~(16'd256);
	4'd09: hot = ~(16'd512);
	4'd10: hot = ~(16'd1024);
	4'd11: hot = ~(16'd2048);
	4'd12: hot = ~(16'd4096);
	4'd13: hot = ~(16'd8192);
	4'd14: hot = ~(16'd16384);
	4'd15: hot = ~(16'd32768);
	default: hot= 16'd0;
	endcase
end
endmodule



module Pri_Enc (binary_out ,encoder_in);
output [3:0] binary_out ; 
input [15:0] encoder_in ; 
     
reg [3:0] binary_out ;
      
always @ (encoder_in)
begin
   casex(encoder_in)
   16'bxxxxxxxxxxxxxxx1 : binary_out = 00;
   16'bxxxxxxxxxxxxxx10 : binary_out = 01;
   16'bxxxxxxxxxxxxx100 : binary_out = 02;
   16'bxxxxxxxxxxxx1000 : binary_out = 03;
   16'bxxxxxxxxxxx10000 : binary_out = 04;
   16'bxxxxxxxxxx100000 : binary_out = 05;
   16'bxxxxxxxxx1000000 : binary_out = 06;
   16'bxxxxxxxx10000000 : binary_out = 07;
   16'bxxxxxxx100000000 : binary_out = 08;
   16'bxxxxxx1000000000 : binary_out = 09;
   16'bxxxxx10000000000 : binary_out = 10;
   16'bxxxx100000000000 : binary_out = 11;
   16'bxxx1000000000000 : binary_out = 12;
   16'bxx10000000000000 : binary_out = 13;
   16'bx100000000000000 : binary_out = 14;
   16'b1000000000000000 : binary_out = 15;
   default : binary_out = 0;
   endcase
end

endmodule  