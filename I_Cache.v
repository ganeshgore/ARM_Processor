`timescale 10ps / 1ps
module I_Cache(ADD, OUT);

parameter DATA_WIDTH =32;
parameter ADDR_WIDTH =32;

input  [31:0] ADD;
output [31:0] OUT;

reg [DATA_WIDTH-1:0]mem[0:ADDR_WIDTH-1];

initial begin
    $readmemh("program.mif", mem);
end 

assign OUT = mem[{1'b0,1'b0,ADD[31:2]}];

endmodule
