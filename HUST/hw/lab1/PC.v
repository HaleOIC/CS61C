`timescale 1ns / 1ps
module PC(D, Clk, Reset, Q);
  parameter n=32;
// codes
  input [n - 1: 0] D;
  input Clk, Reset;
  output reg [n - 1: 0] Q;
  
  always @(posedge Clk, Reset) begin
	Q <= D;
	if (Reset == 1'b1) Q <= 32'b0;
  end
endmodule