`timescale 10ps / 1ps
module ARM7(clk,rst,OUTPORT,INPORT);

parameter O = 28; 
parameter C = 29; 
parameter Z = 30; 
parameter N = 31; 

// Input Output Declaration
input wire clk;
input wire rst;
input  [7:0]INPORT;
output [7:0]OUTPORT;


// Interanal Variable and Signals Declaration
	// Fetch Stage Wires
	wire [31:0]PC_4,PC_OUT,I_OUT,PC_in;
	wire PC_SEL;
	
	// Decode Stage Out
	wire [31:0]INT_REG_OUT;
	wire [31:0]RDAT1,RDAT2,RDAT3,PC_DECODE;
	
	wire [31:0]OP1_IN,OP2_IN,OP3_IN;
	
	// Execute Stage Out
	wire [31:0]OP1_OUT,OP2_OUT,OP3_OUT;
	wire [31:0]OP_EXE_OUT,ALUout,CPSR_Out,SFTout,MEM_ADD,MULOUT,Bin;
	reg  [31:0]EXEOUT,Ain,MEM_MUX_OUT;
	wire [63:0]MULOp;
	
	// Memory Stage Out
	wire [31:0]OP_MEM_OUT,ALUOutReg;
	reg [31:0] MADD;
	
	// Write Back Stage Out
	wire [31:0]OP_WB_OUT,WDAT;
	wire WEN;
	
reg  [3:0]WADD;


wire [31:0]WB_IN,MEM_DIN,RD_IN,RD_Out,MEM_ADD_OUT,MEM_DATA_out,MEM_Out,WB_Out;

	
// Control Signals
	// Decoder Stage Controls
	wire R2_Sel,R3_Sel,OP1_Sel,OP3_Sel;
	wire [5:0]Dec_CntWord;
	wire [3:0]R2_IN,R3_IN;
	
	// Execute Stage Controls
	wire HLT_F_D,Br_EN,RD_IN_Sel,MULSFT_EN,Bin_Sel,SFTCout,Cin_Sel,EX_ADDSUB,LOAD_CPSR,CondOut;
	wire [1:0]EXE_ADD_SEL,Ain_Sel,EX_OUT_SEL,GCnt_IN,GCnt_Out,GCnt_Out_MEM,GCnt_Out_WB;
	wire [12:0]Exe_CntWord;
	wire [31:0]MEM_ADD_IN;

	// Memoery Stage Controls
	wire READ_MEM,MEM_DIN_Sel,MEM_FWD_Sel;
	wire [1:0]B_HW_W,MEM_ADD_SUB;
	wire [6:0]Mem_CntWord;
	
	// Memoery Stage Controls
	wire REG_WDAT_Sel,REG_W,ADD_Order ;
	wire [4:0]WB_CntWord;
	wire [3:0]MUL_ADD;
	wire [1:0]REG_WADD_SEl;
	
	
// Register load Signals (Pipeline Stage)
	// Fetch Stage Controls
	wire PC_LOAD,INT_REG_LOAD;
	// Decoder Stage Controls
	// Execute Stage Controls
	// MEMORY Stage Controls

	// WB Stage Controls
	wire OP_EXE_LOAD,OP_MEM_LOAD,OP_WB_LOAD,OP1_LOAD,WB_LOAD,OP2_LOAD,OP3_LOAD,MEM_DATA_OUT_LOAD,PC_DEC_LOAD,MEM_ADD_LOAD;


assign PC_4 = PC_OUT+4;
assign PC_LOAD = HLT_F_D;
assign INT_REG_LOAD = HLT_F_D;
assign OP_EXE_LOAD = HLT_F_D;
assign OP_MEM_LOAD = 1;
assign PC_DEC_LOAD = HLT_F_D;
assign OP_WB_LOAD  = 1;
assign ALUOutReg_LOAD  = 1;
assign WB_LOAD  = 1;
assign OP1_LOAD  = HLT_F_D;
assign OP2_LOAD  = HLT_F_D;
assign OP3_LOAD  = HLT_F_D;
assign MEM_ADD_LOAD = 1;
assign MEM_DATA_OUT_LOAD  = 1;


// Program Counter
REG32 PGM_CNT (clk,rst,PC_LOAD,PC_in, PC_OUT);
assign PC_in =  (PC_SEL) ?  MEM_ADD:PC_4; 
// Instruction Cache
I_Cache INST_Cache(.ADD(PC_OUT),.OUT(I_OUT));

REG32  PC_Dec (.clk(clk),.rst(rst),.load(PC_DEC_LOAD),.IN(PC_4),.OUT(PC_DECODE));  //OP_x_From_Decode

// **********************************DECODING STAGE*************************************
// Instruction register
REG32  INST_REG (.clk(clk),.rst(rst|PC_SEL),.load(INT_REG_LOAD),.IN(I_OUT),.OUT(INT_REG_OUT));

RegFile RegFile (.clk(clk),.rst(rst),.RADD1(INT_REG_OUT[3:0]),.RADD2(R2_IN),.RADD3(R3_IN),
						  .RDAT1(RDAT1),.RDAT2(RDAT2),.RDAT3(RDAT3),
						  . WADD(WADD),. WDAT(WDAT),.WEN(WEN));



DecROM Dec_ROM(.OPCODE(INT_REG_OUT),.CntOut(Dec_CntWord));					  
assign {GCnt_IN,R2_Sel,R3_Sel,OP1_Sel,OP3_Sel} = Dec_CntWord;

// Control Muxes
assign R2_IN = (R2_Sel) ? INT_REG_OUT[15:12]:INT_REG_OUT[11:8];
assign R3_IN = (R3_Sel) ? INT_REG_OUT[15:12]:INT_REG_OUT[19:16];

assign OP1_IN = (OP1_Sel) ? 32'b0:RDAT1;
assign OP2_IN = RDAT2;
assign OP3_IN = (OP3_Sel) ? PC_DECODE:RDAT3;


						  
REG32  OP1 (.clk(clk),.rst(rst),.load(OP1_LOAD),.IN(OP1_IN),.OUT(OP1_OUT));  //OP_x_From_Decode
REG32  OP2 (.clk(clk),.rst(rst),.load(OP2_LOAD),.IN(OP2_IN),.OUT(OP2_OUT));  //OP_x_From_Decode
REG32  OP3 (.clk(clk),.rst(rst),.load(OP3_LOAD),.IN(OP3_IN),.OUT(OP3_OUT));  //OP_x_From_Decode

// **********************************EXECUTION STAGE************************************* 
REG32  OP_EXE (.clk(clk),.rst(rst|PC_SEL),.load(OP_EXE_LOAD),.IN(INT_REG_OUT),.OUT(OP_EXE_OUT));  // Copy OPCODE

CPSRREG CPSR (.clk(clk),.rst(rst),.load(LOAD_CPSR&CondOut),.IN(EXEOUT),.Vin(Vout),.OUT(CPSR_Out),.Cin(Cout));

SFTUnit SFTUnit1(.INA(OP1_OUT),.INB(OP2_OUT),.OPCODE(OP_EXE_OUT),.Cin(1'b0),.OUT(SFTout),.Cout(SFTCout));

CondChk Cond_Chk(.OPCODE(OP_EXE_OUT[31:28]),.CPSR(CPSR_Out[31:28]),.OUT(CondOut));

MUL32 Multiplier (.INA(OP1_OUT),.INB(OP2_OUT),.OUT(MULOp));

ALU    My_ALU  (.Ain(Ain),.Bin(Bin),.Cin(Cin),.ALUout(ALUout),.Cout(Cout),.Vout(Vout),.OPCODE(OP_EXE_OUT));

REG32 	ALUReg (.clk(clk),.rst(rst),.load(ALUOutReg_LOAD),.IN(EXEOUT),.OUT(ALUOutReg));  // Copy ALUOutput
REG32 	OP2_Reg (.clk(clk),.rst(rst),.load(ALUOutReg_LOAD),.IN(RD_IN),.OUT(RD_Out));  // Copy RD_(OP2)_Out

assign {Br_EN,RD_IN_SEL,MULSFT_EN,Ain_Sel, Bin_Sel, Cin_Sel, EX_OUT_SEL,EXE_ADD_SEL,EX_ADDSUB,LOAD_CPSR} = Exe_CntWord;
ExeROM Exe_ROM(.OPCODE(OP_EXE_OUT),.GCnt(GCnt_Out),.CntOut(Exe_CntWord));

Counter2 GCounter(.clk(clk),.rst(rst),.IN(GCnt_IN),.Out(GCnt_Out));

assign HLT_F_D = !(GCnt_Out[0] | GCnt_Out[1]);


assign MULOUT = (MULSFT_EN) ? MULOp[63:32]:MULOp[31:0];
assign Cin = (Cin_Sel) ? 1'b0:SFTCout;

assign PC_SEL = Br_EN & CondOut;

assign RD_IN = (RD_IN_SEL) ? OP1_OUT:OP2_OUT; 
assign Bin = (Bin_Sel) ? MULOUT:OP3_OUT;

always@(EX_OUT_SEL,CPSR_Out,ALUout,SFTout)
	begin
		case(EX_OUT_SEL)
			3'b00: EXEOUT = ALUout;
			3'b01: EXEOUT = SFTout;
			3'b10: EXEOUT = CPSR_Out;
			3'b11: EXEOUT = OP3_OUT;
			default: EXEOUT = 4'b0;
		endcase
	end

always@(Ain_Sel,ALUOutReg,OP3_OUT,SFTout)
	begin
		case(Ain_Sel)
			3'b00: Ain = SFTout;
			3'b01: Ain = OP3_OUT;
			3'b10: Ain = ALUOutReg;
			default: Ain = 4'b0;
		endcase
	end

		// Execution Stage Memory Address Calculation
ADDGEN  ADD_GEN (.INA(SFTout), .INB(OP3_OUT), .OPCODE(OP_EXE_OUT), .OUT(MEM_ADD));

always@(EXE_ADD_SEL,MEM_ADD,OP3_OUT,MEM_ADD_OUT)
	begin
		case(EXE_ADD_SEL)
			3'b00: MEM_MUX_OUT = MEM_ADD;
			3'b01: MEM_MUX_OUT = OP3_OUT;
			3'b10: MEM_MUX_OUT = MEM_ADD_OUT;
			default: MEM_MUX_OUT = 4'b0;
		endcase
	end

assign MEM_ADD_IN = (EX_ADDSUB) ? (MEM_MUX_OUT + 4):(MEM_MUX_OUT-4);

// **********************************MEMORY STAGE************************************* 
REG32  OP_MEM (.clk(clk),.rst(!CondOut),.load(OP_MEM_LOAD),.IN(OP_EXE_OUT),.OUT(OP_MEM_OUT));   //COPY OPCODE
REG32  WBReg (.clk(clk),.rst(rst),.load(WB_LOAD),.IN(WB_IN),.OUT(WB_Out));  // Copy ALUOutput
REG32  WBDATAReg (.clk(clk),.rst(rst),.load(MEM_DATA_OUT_LOAD),.IN(MEM_Out),.OUT(MEM_DATA_out));  // Copy ALUOutput
REG32  MEM_ADD_Reg (.clk(clk),.rst(rst),.load(MEM_ADD_LOAD),.IN(MEM_ADD_IN),.OUT(MEM_ADD_OUT));  // Copy Mem Address

REG2  REG_MEM (.clk(clk),.rst(rst),.IN(GCnt_Out),.OUT(GCnt_Out_MEM));// Copy Count to next Stage MEMORY

DATA_Cache DataMem(.MADD(MADD),.MDATA(MEM_DIN),.MDOUT(MEM_Out),.type(B_HW_W),.RW(READ_MEM),.OUTPORT(OUTPORT),.INPORT(INPORT));

assign {B_HW_W,READ_MEM,MEM_ADD_SUB,MEM_DIN_Sel,MEM_FWD_Sel} = Mem_CntWord;
MemROM Mem_ROM(.OPCODE(OP_MEM_OUT),.GCnt(GCnt_Out_MEM),.CntOut(Mem_CntWord));

assign WB_IN = (MEM_FWD_Sel) ? MEM_ADD_OUT:ALUOutReg;
assign MEM_DIN = (MEM_DIN_Sel) ? ALUOutReg:RD_Out;

always@(MEM_ADD_SUB,MEM_ADD_OUT)
begin
	case(MEM_ADD_SUB)
	2'b00:MADD =  MEM_ADD_OUT -4;
	2'b10:MADD =  MEM_ADD_OUT +4;
	default:MADD =  MEM_ADD_OUT;
	endcase
end

// assign MADD = (MEM_ADD_SUB) ? (MEM_ADD_OUT+4):(MEM_ADD_OUT-4);

// **********************************WriteBACK STAGE*************************************
REG32  OP_WB  (.clk(clk),.rst(rst),.load(OP_WB_LOAD),.IN(OP_MEM_OUT),.OUT(OP_WB_OUT));     //COPY OPCODE

REG2  REG_WB (.clk(clk),.rst(rst),.IN(GCnt_Out_MEM),.OUT(GCnt_Out_WB));// Copy Count to next Stage WB

assign {ADD_Order,REG_WADD_SEl,REG_WDAT_Sel,REG_W } = WB_CntWord;
WBROM WB_ROM(.OPCODE(OP_WB_OUT),.GCnt(GCnt_Out_WB),.CntOut(WB_CntWord));

MULT_DEC MULT_DEC0(.OPCODE(OP_WB_OUT[15:0]), .Cnt(GCnt_Out_WB), .Out(MUL_ADD) ,.Order(ADD_Order));

assign WDAT = (REG_WDAT_Sel) ? MEM_DATA_out:WB_Out;
assign WEN = REG_W;
 
 
always@(OP_WB_OUT,REG_WADD_SEl,MUL_ADD)
	begin
		case(REG_WADD_SEl)
			2'b00: WADD = OP_WB_OUT[15:12];
			2'b01: WADD = OP_WB_OUT[19:16];
			2'b10: WADD = MUL_ADD;
			2'b11: WADD = 4'b1110;
			default: WADD = 4'b0;
		endcase
	end
endmodule
