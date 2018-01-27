
module sb_unq1 (
clk, reset,
pe_output_0,

in_0_0,

out_1_0,

in_2_0,

in_3_0,

config_addr,
config_data,
config_en
);

  input  clk;
  input  reset;
  input  config_en;
  input [31:0] config_data;

  input [31:0] config_addr;

  input [15:0] pe_output_0;
  
  input [15:0] in_0_0;

  output [15:0] out_1_0;

  input [15:0] in_2_0;

  input [15:0] in_3_0;

  reg [31:0] config_sb;
  
  always @(posedge clk) begin
    if (reset==1'b1) begin

      config_sb <= 64'd0;

    end else begin
      if (config_en==1'b1) begin

           config_sb[31:0] <= config_data;

      end
    end
  end

  reg [15:0] out_1_0_i;

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


