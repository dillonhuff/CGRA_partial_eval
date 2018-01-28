module switch_fold (
pe_output_0,

in_0_0,

out_1_0,

in_2_0,

in_3_0,

);

  input [1:0] pe_output_0;
  
  input [1:0] in_0_0;

  output [1:0] out_1_0;

  input [1:0] in_2_0;

  input [1:0] in_3_0;

   wire [31:0] config_sb;
   assign config_sb = 32'h00000C00;
   

   reg         out_1_0_i;
   
  always @(*) begin
    case (config_sb[11:10])
        2'd0: out_1_0_i = in_0_0;
        2'd1: out_1_0_i = in_2_0;
        2'd2: out_1_0_i = in_3_0;
        2'd3: out_1_0_i = pe_output_0;

    endcase
  end
  assign out_1_0 = out_1_0_i; 

endmodule


