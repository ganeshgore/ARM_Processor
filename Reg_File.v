`timescale 10ps / 1ps
// RegFile(clk(),.RADD1(),.RADD2(),.RADD3(),.RDAT1(),.RDAT2(),.RDAT3(),. WADD(),. WDAT(),.WEN());
module RegFile(clk,rst,RADD1, RADD2,RADD3,RDAT1,RDAT2,RDAT3, WADD, WDAT,WEN );
parameter DATA_WIDTH = 32;
parameter REGFILE_WIDTH = 4;

//pin declarations
input wire WEN, clk,rst;
input wire [REGFILE_WIDTH-1:0] RADD1, RADD2,RADD3, WADD;
input wire [DATA_WIDTH-1:0] WDAT;
output wire [DATA_WIDTH-1:0] RDAT1, RDAT2,RDAT3;

reg [REGFILE_WIDTH-1:0] ra1, ra2,ra3;

//memory declarations
// reg [DATA_WIDTH - 1:0] Mem[0:1 << (REGFILE_WIDTH-1)];
reg [DATA_WIDTH - 1:0] Mem[0:16];
integer i;

//write logic
always@(posedge clk) begin
	if(rst == 1'b1)
	begin
		$display("Memory Initialization");
		for(i=0;i<=15;i=i+1)
			begin
				Mem[i]<= 32'h00000001<<i;
				// $display("Initialization:%t\tAddress:%d\tData:%h", $time, i, Mem[i]);
			end
	end
	else if(WEN == 1) begin
	$display("writing time:%t\tAddress:%h\tData:%h", $time, WADD, WDAT);
	Mem[WADD] <= WDAT;
	end
end

always@(negedge clk) begin
ra1 <= RADD1;
ra2 <= RADD2;
ra3 <= RADD3;
// $display("Reading time:%t\tAddress:%d\t%d\t%d\t Data: %h\t%h\t%h\t", $time, ra1,ra2,ra3,Mem[ra1],Mem[ra2],Mem[ra3]);
end


assign RDAT1 = Mem[ra1];
assign RDAT2 = Mem[ra2];
assign RDAT3 = Mem[ra3];

endmodule