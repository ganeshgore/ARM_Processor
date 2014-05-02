`timescale 10ps / 1ps
// module FWD_UNIT(.R1_In(),.R2_In(),.R3_In(),.DestAddEXE(),.DestAddMEM(),.DestAddWB(),.OP1FWD_Sel(),.OP2FWD_Sel(),.OP3FWD_Sel(),.Stall());
module FWD_UNIT(R1_In,R2_In,R3_In,DestAddEXE,DestAddMEM,DestAddWB,OP1FWD_Sel,OP2FWD_Sel,OP3FWD_Sel,Stall);

input [3:0]R1_In;
input [3:0]R2_In;
input [3:0]R3_In;

input [4:0]DestAddEXE;
input [3:0]DestAddMEM;
input [3:0]DestAddWB;

output reg [1:0]OP1FWD_Sel;
output reg [1:0]OP2FWD_Sel;
output reg [1:0]OP3FWD_Sel;
output Stall;

reg Stall0,Stall1,Stall2;

assign Stall = Stall0 | Stall1 | Stall2;

always@(R1_In,DestAddEXE,DestAddMEM,DestAddWB)
	begin
	 case(R1_In) 
	   DestAddEXE[3:0] : begin
						if(DestAddEXE[4])
							Stall0 = 1;
						else
							begin
								Stall0 = 0;
								OP1FWD_Sel = 2'b01;
							end
						end
	   DestAddMEM[3:0]: begin Stall0 = 0; OP1FWD_Sel = 2'b10; end
	   DestAddWB[3:0]: begin Stall0 = 0; OP1FWD_Sel = 2'b11; end
	   default: begin Stall0 = 0; OP1FWD_Sel = 2'b00; end
	endcase
	end 


always@(R2_In,DestAddEXE,DestAddMEM,DestAddWB)
	begin
	 case(R2_In) 
	   DestAddEXE[3:0] : begin
						if(DestAddEXE[4])
							Stall1 = 1;
						else
							begin
								Stall1 = 0;
								OP2FWD_Sel = 2'b01;
							end
						end
	   DestAddMEM[3:0] : begin Stall1 = 0; OP2FWD_Sel = 2'b10; end
	   DestAddWB[3:0]  : begin Stall1 = 0; OP2FWD_Sel = 2'b11; end
	   default: begin Stall1 = 0;OP2FWD_Sel = 2'b00; end
	endcase
end
	

always@(R3_In,DestAddEXE,DestAddMEM,DestAddWB)
	begin
	 case(R3_In) 
	   DestAddEXE[3:0] : begin
						if(DestAddEXE[4])
							Stall2 = 1;
						else
							begin
								Stall2 = 0;
								OP3FWD_Sel = 2'b01;
							end
						end
	   DestAddMEM[3:0] : begin Stall2 = 0; OP3FWD_Sel = 2'b10; end
	   DestAddWB[3:0]  : begin Stall2 = 0; OP3FWD_Sel = 2'b11; end
	   default: begin Stall2 = 0; OP3FWD_Sel = 2'b00; end
	endcase
end 

endmodule
