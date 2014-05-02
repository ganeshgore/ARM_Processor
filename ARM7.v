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
	wire [31:0]RDAT1,RDAT2,RDAT3,RDAT30,PC_DECODE;
	wire [1:0]OP1_FWD_Sel,OP2_FWD_Sel,OP3_FWD_Sel;
	wire [3:0]MUL_ADD,DestReg_Gen;
	wire [3:0]R1_In,R2_In,R3_In;
	
	wire [31:0]OP1_IN,OP2_IN,OP3_IN;
	
	// Execute Stage Out
	wire [31:0]OP1_OUT,OP2_OUT,OP3_OUT,Ain;
	wire [31:0]OP_EXE_OUT,ALUout,CPSR_Out,SFTout,MEM_ADD,MULOUT,Bin;
	reg  [31:0]EXEOUT,MEM_MUX_OUT;
	wire [63:0]MULOp;
	
	// Memory Stage Out
	wire [31:0]OP_MEM_OUT,ALUOutReg;
	reg [31:0] MADD;
	
	// Write Back Stage Out
	wire [31:0]OP_WB_OUT,WDAT;
	wire WEN;
	
    wire  [3:0]WADD;
	
	wire [31:0]WB_FWD,MEM_FWD;


wire [31:0]WB_IN,MEM_DIN,RD_IN,RD_Out,MEM_ADD_OUT,MEM_DATA_out,MEM_Out,WB_Out;

	
// Control Signals
	// Decoder Stage Controls
	wire R2_Sel,R3_Sel,OP1_Sel,OP3_Sel;
	wire [8:0]Dec_CntWord;
	wire [3:0]R1_IN,R2_IN,R3_IN,Dest_Reg_ADD;
	
	// Execute Stage Controls
	wire Br_EN,RD_IN_Sel,Ain_Sel,MULSFT_EN,SFTCout,Cin_Sel,EX_ADDSUB,LOAD_CPSR,CondOut,GCnt_Out_MEM,GCnt_Out_WB;
	wire [1:0]EXE_ADD_SEL,Bin_Sel,EX_OUT_SEL,GCnt_IN,GCnt_Out;
	wire [13:0]Exe_CntWord;
	wire [31:0]MEM_ADD_IN;

	// Memoery Stage Controls
	wire WRITE_MEM,MEM_DIN_Sel,MEM_FWD_Sel;
	wire [1:0]B_HW_W,MEM_ADD_SUB;
	wire [8:0]Mem_CntWord;
	
	// Memoery Stage Controls
	wire REG_WDAT_Sel,REG_W,ADD_Order ;
	wire [2:0]WB_CntWord;
	wire [1:0]REG_WADD_SEl;
	
	wire [4:0]DestAddEXE,EXE_STAGE_FWD;
	wire [3:0]DestAddMEM,DestAddWB;
// Register load Signals (Pipeline Stage)
	// Fetch Stage Controls
	wire PC_LOAD,INT_REG_LOAD;
	// Decoder Stage Controls
	// Execute Stage Controls
	// MEMORY Stage Controls

	// WB Stage Controls
	wire OP_EXE_LOAD,OP_MEM_LOAD,OP_WB_LOAD,OP1_LOAD,WB_LOAD,OP2_LOAD,OP3_LOAD,MEM_DATA_OUT_LOAD,PC_DEC_LOAD,MEM_ADD_LOAD;
	wire [15:0]Pat_Out;

//Control Wires
// wire FlushFetch,FlushDec,FlushExe,FlushMem,FlushWB,StallFetch,StallDec,StallExe,StallMem,StallWB;


assign PC_4 = PC_OUT+4;
assign PC_LOAD = 1;
assign INT_REG_LOAD = 1;
assign OP_EXE_LOAD = 1;
assign OP_MEM_LOAD = 1;
assign PC_DEC_LOAD = 1;
assign OP_WB_LOAD  = 1;
assign ALUOutReg_LOAD  = 1;
assign WB_LOAD  = 1;
assign OP1_LOAD  = 4;
assign OP2_LOAD  = 4;
assign OP3_LOAD  = 4;
assign MEM_ADD_LOAD = 1;
assign MEM_DATA_OUT_LOAD  = 1;



// **********************************Fetch STAGE*************************************
//Program Counter Input Select
assign PC_in =  (PC_SEL) ?  MEM_ADD:PC_4;
// Program Counter
REG32  PGM_CNT (.clk(clk),.rst(rst),.load(1'b1),.IN(PC_in),.OUT(PC_OUT));
// Instruction Cache
I_Cache INST_Cache(.ADD(PC_OUT),.OUT(I_OUT));



// **********************************DECODING STAGE*************************************
// Instruction register
REG32  INST_REG (.clk(clk),.rst(rst),.load(StallDec),.IN(I_OUT),.OUT(INT_REG_OUT));
//Program Counter Incrementing Counter
REG32  PC_Dec (.clk(clk),.rst(rst),.load(StallDec),.IN(PC_4),.OUT(PC_DECODE));  //OP_x_From_Decode
//Register File
RegFile RegFile (.clk(clk),.rst(rst),.RADD1(R1_IN),.RADD2(R2_IN),.RADD3(R3_IN),
						  .RDAT1(RDAT1),.RDAT2(RDAT2),.RDAT3(RDAT3),
						  . WADD(WADD),. WDAT(WDAT),.WEN(WEN));
						  
// Multiple Load Store Address generator
MULT_DEC MULT_DEC0(.clk(clk),.rst(rst),.load(load_mul_unit),.OPCODE(INT_REG_OUT[15:0]),.INC(1'b0),.Out(MUL_ADD),.NoAdd(NoAdd));

FWD_UNIT FWD_UNIT0(.R1_In(R1_IN),.R2_In(R2_IN),.R3_In(R3_IN),
				.DestAddEXE(EXE_STAGE_FWD),.DestAddMEM(DestAddMEM),.DestAddWB(DestAddWB),
				.OP1FWD_Sel(OP1_FWD_Sel),.OP2FWD_Sel(OP2_FWD_Sel),.OP3FWD_Sel(OP3_FWD_Sel),.Stall(Stall0));

assign EXE_STAGE_FWD = (!CondOut) ? 5'bzzzzz:DestAddEXE;
				
//Reg Out MUX
MUX4 MUX4OP1(.SEL(OP1_FWD_Sel),.IN0(RDAT1),.IN1(EXEOUT),.IN2(MEM_FWD),.IN3(WB_FWD),.OUT(OP1_IN));
MUX4 MUX4OP2(.SEL(OP2_FWD_Sel),.IN0(RDAT2),.IN1(EXEOUT),.IN2(MEM_FWD),.IN3(WB_FWD),.OUT(OP2_IN));
MUX2 MUX20  (.SEL(OP3_Sel),.IN0(RDAT3),.IN1(PC_DECODE),.OUT(RDAT30));
MUX4 MUX4OP3(.SEL(OP3_FWD_Sel),.IN0(RDAT30),.IN1(EXEOUT),.IN2(MEM_FWD),.IN3(WB_FWD),.OUT(OP3_IN));

//Reg Input Mux

MUX24 MUX2R1 (.SEL(R1_Sel),.IN0(INT_REG_OUT[3:0]),.IN1(MUL_ADD),.OUT(R1_IN));
MUX24 MUX2R2 (.SEL(R2_Sel),.IN0(INT_REG_OUT[11:8]),.IN1(INT_REG_OUT[15:12]),.OUT(R2_IN));
MUX24 MUX2R3 (.SEL(R3_Sel),.IN0(INT_REG_OUT[19:16]),.IN1(INT_REG_OUT[15:12]),.OUT(R3_IN));

//Destination Address Selection
MUX24 MUX2Dest (.SEL(DestSel),.IN0(DestReg_Gen),.IN1(MUL_ADD),.OUT(Dest_Reg_ADD));


//Destination address generator
DestADDGen  DestADDGen0(.OPCODE(INT_REG_OUT),.GCnt_Out_EXE(GCnt_Out_DEC),.DESTADD(DestReg_Gen));

//Stall Counter

REG1  StallDecReg (.clk(clk),.rst(rst),.IN(GCnt_In_Dec|NoAdd),.OUT(GCnt_Out_DEC));

REG1  StallEXE (.clk(clk),.rst(rst),.IN(GCnt_Out_DEC),.OUT(GCnt_Out_EXE));

//Reg Destination Address Forward
REG5  REG50 (.clk(clk),.rst(rst),.load(1'b1),.IN({DestADD_Val,Dest_Reg_ADD}),.OUT(DestAddEXE));

//Decode Stage Control Unit
DecROM Dec_ROM(.OPCODE(INT_REG_OUT),.GCnt(GCnt_Out_DEC),.CntOut(Dec_CntWord));					  
assign {DestSel,DestADD_Val,GCnt_In_Dec,load_mul_unit,INC_Order,R3_Sel,R2_Sel,R1_Sel,OP3_Sel} = Dec_CntWord;


//Stalling Decode Stage
assign StallDec = !(NoAdd | GCnt_In_Dec);

// **********************************EXECUTION STAGE************************************* 
//Opcode Load fro execution Stage
REG32  OP_EXE (.clk(clk),.rst(rst),.load(1'b1),.IN(INT_REG_OUT),.OUT(OP_EXE_OUT));  // Copy OPCODE

//Operands Load
REG32  OP1 (.clk(clk),.rst(rst),.load(1'b1),.IN(OP1_IN),.OUT(OP1_OUT));  //OP_From_Decode
REG32  OP2 (.clk(clk),.rst(rst),.load(1'b1),.IN(OP2_IN),.OUT(OP2_OUT));  //OP_From_Decode
REG32  OP3 (.clk(clk),.rst(rst),.load(1'b1),.IN(OP3_IN),.OUT(OP3_OUT));  //OP_From_Decode


//Predicateed Instruction Condition Checking Block
CondChk Cond_Chk(.OPCODE(OP_EXE_OUT[31:28]),.CPSR(CPSR_Out[31:28]),.OUT(CondOut));

//Status Register
CPSRREG CPSR (.clk(clk),.rst(rst),.load(LOAD_CPSR&CondOut),.IN(EXEOUT),.Vin(Vout),.OUT(CPSR_Out),.Cin(Cout));

//Shiting Unit
SFTUnit SFTUnit1(.INA(OP1_OUT),.INB(OP2_OUT),.OPCODE(OP_EXE_OUT),.Cin(CPSR_Out[29]),.TMP_CR(TEMP_CARRY),.OUT(SFTout),.Cout(SFTCout));

//Multiplier unit
MUL32 Multiplier (.INA(OP1_OUT),.INB(OP2_OUT),.SIGN(SIGN_MUL),.OUT(MULOp));

// ALU
ALU    My_ALU  (.Ain(Ain),.Bin(Bin),.Cin(Cin),.ALUout(ALUout),.Cout(Cout),.Vout(Vout),.OPCODE(OP_EXE_OUT));

// Temporary carry Bit
REG1  TempCarry (.clk(clk),.rst(rst),.IN(Cout),.OUT(TEMP_CARRY));


//Reg Destination Address Forward
REG4  REG4EXE (.clk(clk),.rst(!CondOut),.load(1'b1),.IN(DestAddEXE[3:0]),.OUT(DestAddMEM));


//Execution Stage Control Unit
assign {SIGN_MUL,Br_EN,RD_IN_SEL,MULSFT_EN,Ain_Sel, Bin_Sel, Cin_Sel, EX_OUT_SEL,EXE_ADD_SEL,EX_ADDSUB,LOAD_CPSR} = Exe_CntWord;
ExeROM Exe_ROM(.OPCODE(OP_EXE_OUT),.GCnt(GCnt_Out_EXE),.GCnt2(GCnt_Out_MEM),.CntOut(Exe_CntWord));


//Stall Counter
REG1  Cnt_REGMEM (.clk(clk),.rst(rst),.IN(GCnt_Out_EXE),.OUT(GCnt_Out_MEM));// Copy Count to next Stage MEMORY


assign MULOUT = (MULSFT_EN) ? MULOp[63:32]:MULOp[31:0];
assign Cin = (Cin_Sel) ? 1'b0:SFTCout;

assign PC_SEL = Br_EN & CondOut;

assign RD_IN = (RD_IN_SEL) ? OP1_OUT:OP2_OUT; 
// assign Bin = (Bin_Sel) ? MULOUT:OP3_OUT;

MUX4 MUX4BinSel(.SEL(Bin_Sel),.IN0(SFTout),.IN1(OP3_OUT),.IN2(MULOUT),.IN3(ALUOutReg),.OUT(Bin));
assign Ain = (Ain_Sel) ? MULOUT:OP3_OUT;

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
//Copy OPCODE for Memory Stage
REG32  OP_MEM (.clk(clk),.rst(!CondOut),.load(OP_MEM_LOAD),.IN(OP_EXE_OUT),.OUT(OP_MEM_OUT));   //COPY OPCODE

//LOAD ALUOut Reg
REG32 	ALUReg (.clk(clk),.rst(rst),.load(ALUOutReg_LOAD),.IN(EXEOUT),.OUT(ALUOutReg));  // Copy ALUOutput

//Load Operand2 Bypass register
REG32 	OP2_Reg (.clk(clk),.rst(rst),.load(ALUOutReg_LOAD),.IN(RD_IN),.OUT(RD_Out));  // Copy RD_(OP2)_Out

REG32  MEM_ADD_Reg (.clk(clk),.rst(rst),.load(MEM_ADD_LOAD),.IN(MEM_ADD_IN),.OUT(MEM_ADD_OUT));  // Copy Mem Address

REG1  REG_MEM (.clk(clk),.rst(rst),.IN(GCnt_Out_MEM),.OUT(GCnt_Out_WB));// Copy Count to next Stage MEMORY

DATA_Cache DataMem(.clk(clk),.rst(rst),.MADD(MADD),.MDATA(MEM_DIN),.MDOUT(MEM_Out),.type(B_HW_W),.SignM(SignM),.RW(WRITE_MEM),.OUTPORT(OUTPORT),.INPORT(INPORT));

//Reg Destination Address Forward
REG4  REG4WB (.clk(clk),.rst(rst),.load(1'b1),.IN(DestAddMEM),.OUT(DestAddWB));

//Valid Forward Data MUX
MUX2 MUX2FWDMEM (.SEL(MEM_FWD_SEL),.IN0(WB_IN),.IN1(MEM_Out),.OUT(MEM_FWD));

// Cotrol Unit MEMORY Stage
assign {SignM,MEM_FWD_SEL,B_HW_W,WRITE_MEM,MEM_ADD_SUB,MEM_DIN_Sel,MEM_OUT_Sel} = Mem_CntWord;
MemROM Mem_ROM(.OPCODE(OP_MEM_OUT),.GCnt(GCnt_Out_MEM),.GCnt2(GCnt_Out_WB),.CntOut(Mem_CntWord));

assign WB_IN = (MEM_OUT_Sel) ? MADD:ALUOutReg;
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

// Copy OPCODE to Write Back Stage
REG32  OP_WB  (.clk(clk),.rst(rst),.load(OP_WB_LOAD),.IN(OP_MEM_OUT),.OUT(OP_WB_OUT));     //COPY OPCODE


REG32  WBDATAReg (.clk(clk),.rst(rst),.load(MEM_DATA_OUT_LOAD),.IN(MEM_Out),.OUT(MEM_DATA_out));  // Copy ALUOutput
REG32  WBReg (.clk(clk),.rst(rst),.load(WB_LOAD),.IN(WB_IN),.OUT(WB_Out));  // Copy ALUOutput

//Valid Forward Data MUX
MUX2 MUX2FWDWB (.SEL(WB_FWD_SEL),.IN0(WB_Out),.IN1(MEM_DATA_out),.OUT(WB_FWD));

// Control Unit Write Back Unit
assign {WB_FWD_SEL,REG_WDAT_Sel,REG_W } = WB_CntWord;
WBROM WB_ROM(.OPCODE(OP_WB_OUT),.GCnt(GCnt_Out_WB),.CntOut(WB_CntWord));

assign WDAT = (REG_WDAT_Sel) ? MEM_DATA_out:WB_Out;
assign WEN = REG_W;
 
 
assign WADD = DestAddWB;
	
	
endmodule
