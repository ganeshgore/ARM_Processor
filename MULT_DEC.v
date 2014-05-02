`timescale 10ps / 1ps
// MULT_DEC ----- (clk(),.rst(),.load(),.OPCODE(),.INC(),.Out(),.Pat_Out(),.NoAdd());
module MULT_DEC(clk,rst,load,OPCODE, INC, Out ,NoAdd);
input clk;
input rst;
input load;
input [15:0]OPCODE;
input INC;
output [3:0]Out;
output NoAdd;



wire [15:0]Pat_Out;
reg [15:0]flip_OPCODE;
reg NoAdd_tmp;
reg NoAdd_seq;
reg [15:0]Store_Opcode;
wire [15:0]temp_OPCODE;
wire [15:0]Pri_In;
wire [15:0]Pat_out_Reg;
wire [3:0]reg_add;

integer n;

assign temp_OPCODE = (!NoAdd_tmp) ? OPCODE:Store_Opcode;


assign NoAdd = (Pat_Out == 0) ? 1'b0 : NoAdd_seq | load ;
assign Pri_In = (INC) ? flip_OPCODE:temp_OPCODE;
assign Out = reg_add;
assign Pat_Out = Pat_out_Reg&temp_OPCODE;

Pri_Enc  Pri_Enc0 (Pri_In ,reg_add);
hot_code hot_code0(reg_add,Pat_out_Reg);


always@(posedge clk)
begin
	if(rst == 1)
		begin
		Store_Opcode <= 0;
		NoAdd_seq <= 0;
		end
	else	
	begin
		if ((load == 1 )||(Store_Opcode != 0) )
			Store_Opcode <= Pat_Out;
	if(load == 1'b1)
		NoAdd_seq <= 1;
	else if(load != 1'b1 && (Pat_Out == 0)) 
		NoAdd_seq <= 0;
	end
end

always@(temp_OPCODE,Store_Opcode,temp_OPCODE)
begin
	for ( n=0 ; n <= 15 ; n=n+1 ) begin
	flip_OPCODE[n] <= temp_OPCODE[15-n];
	end
	if(Store_Opcode == 0)
		NoAdd_tmp <= 0;
	else
		NoAdd_tmp <= 1;
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



module Pri_Enc (encoder_in,binary_out);
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



module MULT_DEC_TB;

	// Inputs
	reg clk;
	reg rst;
	reg load;
	reg [15:0]OPCODE;
	reg INC;


	// Outputs
	wire [3:0]Out;
	wire  NoAdd;
	
	parameter clk_period = 4;
	
	MULT_DEC uut (
		.clk(clk),
		.rst(rst),
		.load(load),
		.OPCODE(OPCODE),
		.INC(INC),
		.Out(Out),
		.NoAdd(NoAdd)
	);

	
	initial clk = 0; 
	always #clk_period clk = ~clk;
	

	initial begin

		rst <= 1'b1;
		load <= 1'b0;
		INC <=	1'b0;
		OPCODE <= 16'h0054;
		
		#(4*clk_period) rst <= 1'b0;
		#(2*clk_period) load <= 1'b1;
		#(2*clk_period) load <= 1'b0;
		#(16*clk_period) 		
		#(2*clk_period) load <= 1'b1;
		#(2*clk_period) load <= 1'b0;
        // #(2*clk_period) OPCODE <= 16'h0000;
		#1000 $stop;$finish;

	end
      
endmodule