`timescale 10ps / 1ps
// DATA_Cache --- (MADD(),. MDATA(),. MDOUT(),. type(),.RW(),.OUTPORT(),.INPORT());
module DATA_Cache(clk,rst,MADD, MDATA, MDOUT, type, SignM, RW,OUTPORT,INPORT);

parameter DATA_WIDTH =8;
parameter ADDR_WIDTH =32;

input clk;
input rst;
input  [31:0] MADD;
input  [31:0] MDATA;
input  [1:0] type;
input  SignM;
input  [7:0]INPORT;
output reg [7:0]OUTPORT;
input  RW;
output [31:0] MDOUT;


reg [31:0] MDOUT_Temp;
reg [31:0] Signed;

reg [DATA_WIDTH-1:0]mem[0:ADDR_WIDTH-1];
integer i,file;

initial begin
    $readmemh("datmemory.mif", mem);
	// $monitor("Memory Operation time:%t\tOperation:%b\tType:%h\tAddress:%h\tData:%h", $time,RW,type, MADD, MDATA);
end 


assign MDOUT = (SignM) ?  (MDOUT_Temp|Signed):MDOUT_Temp;
// assign MDOUT = MDOUT_Temp;

initial	 
		file = $fopen ("MemFileLog.dat", "w");
	
always@(posedge clk)
		begin
		$fwrite(file,"\t time{%8h}",$time);
		for(i=0;i<=29;i=i+1)
			$fwrite(file,"\t MemDat%2d{%h}",i,mem[i]);
		$fwrite(file,"\n");
		end

		
always@(MADD, MDATA, Signed, type, RW)
	begin			//Memory Reading Process
		case(type)
			2'b01: Signed <= {{16{mem[{MADD[31:1],1'b1}][7]}},16'h0000};
			2'b10: 
				begin
				if(MADD == 32'h00000016)
					Signed <= {{24{INPORT[7]}},8'b00000000};
				else
					Signed <= {{24{mem[MADD][7]}},8'b00000000};
				end
			default:Signed<= 32'b0;
		endcase
	end
		
		

always@(MADD, MDATA, MDOUT_Temp, type, RW)
	begin			//Memory Reading Process
		case(type)
			2'b00: MDOUT_Temp <= {mem[{MADD[31:2],1'b1,1'b1}],mem[{MADD[31:2],1'b1,1'b0}],mem[{MADD[31:2],1'b0,1'b1}],mem[{MADD[31:2],1'b0,1'b0}]};
			2'b01: MDOUT_Temp <= {16'b0,mem[{MADD[31:1],1'b1}],mem[{MADD[31:1],1'b0}]};
			2'b10: 
				begin
				if(MADD == 32'h00000016)
					MDOUT_Temp <= {24'b0,INPORT};
				else
					MDOUT_Temp <= {24'b0,mem[MADD]};
				end
			default:MDOUT_Temp<= 32'bx;
		endcase
		// $display("Read Memory time:%t\tReadType:%hAddress:%h\tData:%h", $time,type, MADD, MDOUT_Temp);
	end

always@(negedge clk)	
	begin		//Memory Writting Process
	if(RW == 1'b1)
		begin
		case(type)
			2'b00:{mem[{MADD[31:2],1'b1,1'b1}],mem[{MADD[31:2],1'b1,1'b0}],mem[{MADD[31:2],1'b0,1'b1}],mem[{MADD[31:2],1'b0,1'b0}]} <= MDATA;
			2'b01:{mem[{MADD[31:1],1'b1}],mem[{MADD[31:1],1'b0}]} <= MDATA[15:0];
			2'b10: 
				begin
				if(MADD == 32'h00000015)
					OUTPORT <= MDATA[7:0];
				else
					mem[MADD] <= MDATA[7:0];
				end
			default: mem[MADD] <= 8'bx; 
		endcase
		$display("Written Memory time:%t\t Address:%h \tData:%h \tType:%h", $time, MADD, MDATA,type);
		end
	end

endmodule
