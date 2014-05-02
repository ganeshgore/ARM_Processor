`timescale 10ps / 1ps

module ARM7_TB;

	// Inputs
	reg clk;
	reg rst;
	reg [7:0]INPORT;
	wire [7:0]OUTPORT;
	integer file;
	parameter clk_period = 10;
	
	// Instantiate the Unit Under Test (UUT)
	ARM7 uut (
		.clk(clk), 
		.rst(rst),
		.INPORT(INPORT),
		.OUTPORT(OUTPORT)
	);

	initial clk = 0; 
		always #clk_period clk = ~clk;
		
	initial
	begin	 
		file = $fopen ("data.dat", "w");
	end
	
	always@(posedge clk)
		#2 $fwrite(file,"\t time{%8h}",$time,
				 "\t RST{%b}",rst,
				 "\t PC_4{%8h}",uut.PC_4,
				 "\t INPORT{%8b}",uut.INPORT,
				 "\t GCnt_Out_DEC{%b}",uut.GCnt_Out_DEC,
				 "\t GCnt_Out_EXE{%b}",uut.GCnt_Out_EXE,
				 "\t GCnt_Out_MEM{%b}",uut.GCnt_Out_MEM,
				 "\t GCnt_Out_WB{%b}",uut.GCnt_Out_WB,
				 "\t OUTPORT{%8b}",uut.OUTPORT,
				 "\t Vout{%b}",uut.Vout,
				 "\t MULOP{%b}",uut.MULT_DEC0.Store_Opcode,
				 "\t NoAdd{%b}",uut.NoAdd,
				 "\t CondOut{%b}",uut.CondOut,
				 "\t PC_SEL{%b}",uut.PC_SEL,
				 "\t PC_OUT{%8h}",uut.PC_OUT,
				 "\t WDAT{%8h}",uut.WDAT,
				 "\t PC_DECODE{%8h}",uut.PC_DECODE,
				 "\t I_OUT{%8h}",uut.I_OUT,
				 "\t INT_REG_OUT{%8h}",uut.INT_REG_OUT,
				 "\t RDAT1{%8h}",uut.RDAT1,
				 "\t MUL_ADD{%h}",uut.MUL_ADD,
				 "\t RDAT2{%8h}",uut.RDAT2,
				 "\t RDAT3{%8h}",uut.RDAT3,		 
				 "\t R1_IN{%b}",uut.R1_IN,		 
				 "\t R2_IN{%b}",uut.R2_IN,		 
				 "\t R3_IN{%b}",uut.R3_IN,		 
				 "\t OP1_FWD_Sel{%b}",uut.OP1_FWD_Sel,		 
				 "\t OP2_FWD_Sel{%b}",uut.OP2_FWD_Sel,		 
				 "\t OP3_FWD_Sel{%b}",uut.OP3_FWD_Sel,		 
				 "\t Cin{%b}",uut.Cin,		 
				 "\t Cout{%b}",uut.Cout,		 
				 "\t Dec_CntWord{%b}",uut.Dec_CntWord,		 
				 "\t Exe_CntWord{%b}",uut.Exe_CntWord,		 
				 "\t Mem_CntWord{%b}",uut.Mem_CntWord,		 
				 "\t WB_CntWord{%b}",uut.WB_CntWord,	
				 "\t OP1_IN{%8h}",uut.OP1_IN,
				 "\t OP2_IN{%8h}",uut.OP2_IN,
				 "\t OP3_IN{%8h}",uut.OP3_IN,				 
				 "\t OP1_OUT{%8h}",uut.OP1_OUT,
				 "\t OP2_OUT{%8h}",uut.OP2_OUT,
				 "\t OP3_OUT{%8h}",uut.OP3_OUT,
				 "\t MULOUT{%8h}",uut.MULOUT,
				 "\t Ain{%8h}",uut.Ain,
				 "\t Bin{%8h}",uut.Bin,
				 "\t EXEOUT{%8h}",uut.EXEOUT,
				 "\t MEM_FWD{%8h}",uut.MEM_FWD,
				 "\t WB_FWD{%8h}",uut.WB_FWD,
				 "\t WB_IN{%8h}",uut.WB_IN,
				 "\t MEM_Out{%8h}",uut.MEM_Out,
				 "\t OP_EXE_OUT{%8h}",uut.OP_EXE_OUT,
				 "\t DestReg_Gen{%b}",uut.DestReg_Gen,
				 "\t DecROM_Decode{%8h}",uut.Dec_ROM.Decode,
				 "\t ExeROM_Decode{%8h}",uut.Exe_ROM.Decode,
				 "\t WBROM_Decode{%8h}",uut.WB_ROM.Decode,
				 "\t SFTout{%8h}",uut.SFTout,
				 "\t MEM_ADD{%8h}",uut.MEM_ADD,
				 "\t OP_temp{%4b}",uut.My_ALU.OP_temp,
				 "\t ALUout{%8h}",uut.ALUout,
				 "\t MEM_ADD_OUT{%8h}",uut.MEM_ADD_OUT,
				 "\t MEM_ADD_IN{%8h}",uut.MEM_ADD_IN,
				 "\t MADD{%8h}",uut.MADD,
				 "\t RD_Out{%8h}",uut.RD_Out,
				 "\t MEM_DIN{%8h}",uut.MEM_DIN,
				 "\t B_HW_W{%b}",uut.B_HW_W,
				 "\t WRITE_MEM{%b}",uut.WRITE_MEM,
				 "\t MEM_MUX_OUT{%8h}",uut.MEM_MUX_OUT,
				 "\t CPSR_Out{%b}",uut.CPSR_Out[31:28],
				 "\t ALUOutReg{%8h}",uut.ALUOutReg,
				 "\t WB_Out{%8h}",uut.WB_Out,
				 "\t MEM_DATA_out{%8h}",uut.MEM_DATA_out,
				 "\t WADD{%8h}",uut.WADD,
				 "\t WEN{%b}",uut.WEN,
				 "\t MUL_ADD{%b}",uut.MUL_ADD,
				 "\t OP_MEM_OUT{%8h}",uut.OP_MEM_OUT,
				 "\t OP_WB_OUT{%8h}",uut.OP_WB_OUT,
				 "\t DestAddEXE{%b}",uut.DestAddEXE,
				 "\t DestAddMEM{%b}",uut.DestAddMEM,
				 "\t DestAddWB{%b}",uut.DestAddWB,
				 "\t Dest_Reg_ADD{%b}",{uut.DestADD_Val,uut.Dest_Reg_ADD},
				 "\n ");
				 
	// always@(posedge clk)
		// $display("\t RST{%b}",rst,
				 // "\t PC_OUT{%8h}",uut.PC_OUT,
				 // "\t I_OUT{%8h}",uut.I_OUT,
				 // "\t INT_REG_OUT{%8h}",uut.INT_REG_OUT,
				 // "\t RDAT1{%8h}",uut.RDAT1,
				 // "\t RDAT2{%8h}",uut.RDAT2,
				 // "\t RDAT3{%8h}",uut.RDAT3,		 
				 // "\t OP1_OUT{%8h}",uut.OP1_OUT,
				 // "\t OP2_OUT{%8h}",uut.OP2_OUT,
				 // "\t OP3_OUT{%8h}",uut.OP3_OUT,
				 // "\t OP_EXE_OUT{%8h}",uut.OP_EXE_OUT,
				 // "\t ALUOutReg{%8h}",uut.ALUOutReg,
				 // "\t ALUOutReg_WB{%8h}",uut.ALUOutReg_WB,
				 // "\t OP_MEM_OUT{%8h}",uut.OP_MEM_OUT,
				 // "\t OP_WB_OUT{%8h}",uut.OP_WB_OUT,
				 // "\n ");
	
	
	
	initial begin
		// Initialize Inputs
		clk = 0;
		rst = 1;

		// Wait 100 ns for global reset to finish
		#60;
        #(clk_period*2);
		rst = 0;
		// Add stimulus here
		#10000 $stop;$finish;
	end
      
endmodule

