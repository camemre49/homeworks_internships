`timescale 1ns / 1ps

module bh(input B, input H, input clk, output reg Q);

    initial begin
        Q = 1;
    end
	 
	 always@(posedge clk)
		case({B,H})
		2'b00 : Q <= ~Q;
		2'b01 : Q <= 1;
		2'b10 : Q <= 0;
		2'b11 : Q <= Q;
		endcase

endmodule

module XOR(
    output x,
    input a,
    input b
    );
	 
 	assign x = a ^ b;

endmodule

module OR(
    output x,
    input a,
    input b
    );
	 
 	assign x = a | b;

endmodule

module AND(
    output x,
    input a,
    input b
    );
	 
 	assign x = a & b;

endmodule

module NOR(
    output x,
    input a,
    input b
    );
	 
 	assign x = ~(a | b);

endmodule

module ic1337(// Inputs
              input A0,
              input A1,
              input A2,
              input clk,
              // Outputs
              output Q0,
              output Q1,
              output Z);

	wire w1, w2, w3, w4, w5;
	XOR g1(w1, A0, ~A1);
	OR g2(w2, w1, A2);
	NOR g3(w3, ~A0, A1);
	AND g4(w4, w3, A2);
	AND g5(w5, A0, ~A2);
			
	assign Z = ~(Q0 ^ Q1);
	bh BH1(w2, w5, clk, 	Q0);
	bh BH2(w5, w4, clk, Q1);
	
	
endmodule