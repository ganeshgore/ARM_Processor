`timescale 10ps / 1ps
// DATA_Cache --- (MADD(),. MDATA(),. MDOUT(),. type(),.RW(),.OUTPORT(),.INPORT());
module DATA_Cache(MADD, MDATA, MDOUT, type, RW,OUTPORT,INPORT);

parameter DATA_WIDTH =8;
parameter ADDR_WIDTH =32;

input  [31:0] MADD;
input  [31:0] MDATA;
input  [1:0] type;
input  [7:0]INPORT;
output reg [7:0]OUTPORT;
input  RW;
output reg [31:0] MDOUT;

reg [DATA_WIDTH-1:0]mem[0:ADDR_WIDTH-1];
integer i,file;

initial begin
    $readmemh("datmemory.mif", mem);
	// $monitor("Memory Operation time:%t\tOperation:%b\tType:%h\tAddress:%h\tData:%h", $time,RW,type, MADD, MDATA);
end 

// initial	 
		// file = $fopen ("MemFileLog.dat", "w");
	
// always@(posedge clk)
		// begin
		// $fwrite(file,"\t time{%8h}",$time);
		// for(i=0;i<=29;i=i+1)
			// $fwrite(file,"\t MemDat%2d{%8h}",i,Mem[i]);
		// $fwrite(file,"\n");
		// end
		
		

always@(MADD, MDATA, MDOUT, type, RW)
begin
if(!RW)
	begin			//Memory Reading Process
		case(type)
			2'b00: MDOUT <= {mem[{MADD[31:2],1'b1,1'b1}],mem[{MADD[31:2],1'b1,1'b0}],mem[{MADD[31:2],1'b0,1'b1}],mem[{MADD[31:2],1'b0,1'b0}]};
			2'b01: MDOUT <= {16'b0,mem[{MADD[31:1],1'b1}],mem[{MADD[31:1],1'b0}]};
			2'b10: 
				begin
				if(MADD == 32'h00000016)
					MDOUT <= {24'b0,INPORT};
				else
					MDOUT <= {24'b0,mem[MADD]};
				end
			default:MDOUT<= 32'bx;
		endcase
		// $display("Read Memory time:%t\tReadType:%hAddress:%h\tData:%h", $time,type, MADD, MDOUT);
	end
else
	begin		//Memory Writting Process
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
	end
end
endmodule
