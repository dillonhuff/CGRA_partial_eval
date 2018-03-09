//
//--------------------------------------------------------------------------------
//          THIS FILE WAS AUTOMATICALLY GENERATED BY THE GENESIS2 ENGINE        
//  FOR MORE INFORMATION: OFER SHACHAM (CHIP GENESIS INC / STANFORD VLSI GROUP)
//    !! THIS VERSION OF GENESIS2 IS NOT FOR ANY COMMERCIAL USE !!
//     FOR COMMERCIAL LICENSE CONTACT SHACHAM@ALUMNI.STANFORD.EDU
//--------------------------------------------------------------------------------
//
//  
//	-----------------------------------------------
//	|            Genesis Release Info             |
//	|  $Change: 11904 $ --- $Date: 2013/08/03 $   |
//	-----------------------------------------------
//	
//
//  Source file: /Users/dillon/VerilogWorkspace/CGRAGenerator/hardware/generator_z/sb/sb.vp
//  Source template: sb
//
// --------------- Begin Pre-Generation Parameters Status Report ---------------
//
//	From 'generate' statement (priority=5):
// Parameter registered_outputs 	= 11111
// Parameter sides 	= 4
// Parameter pe_output_count 	= 1
// Parameter sb_fs 	= 10000#10000#10000
// Parameter is_bidi 	= 0
// Parameter width 	= 1
// Parameter num_tracks 	= 5
// Parameter feedthrough_outputs 	= 00000
//
//		---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
//
//	From Command Line input (priority=4):
//
//		---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
//
//	From XML input (priority=3):
//
//		---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
//
//	From Config File input (priority=2):
//
// ---------------- End Pre-Generation Pramameters Status Report ----------------

// width (_GENESIS2_INHERITANCE_PRIORITY_) = 1
//
// num_tracks (_GENESIS2_INHERITANCE_PRIORITY_) = 5
//
// sides (_GENESIS2_INHERITANCE_PRIORITY_) = 4
//
// feedthrough_outputs (_GENESIS2_INHERITANCE_PRIORITY_) = 00000
//
// registered_outputs (_GENESIS2_INHERITANCE_PRIORITY_) = 0x2b67
//
// pe_output_count (_GENESIS2_INHERITANCE_PRIORITY_) = 1
//
// is_bidi (_GENESIS2_INHERITANCE_PRIORITY_) = 0
//
// sb_fs (_GENESIS2_INHERITANCE_PRIORITY_) = 10000#10000#10000
//



module sb_unq2 (
clk, reset,
pe_output_0,
out_0_0,
in_0_0,
out_0_1,
in_0_1,
out_0_2,
in_0_2,
out_0_3,
in_0_3,
out_0_4,
in_0_4,
out_1_0,
in_1_0,
out_1_1,
in_1_1,
out_1_2,
in_1_2,
out_1_3,
in_1_3,
out_1_4,
in_1_4,
out_2_0,
in_2_0,
out_2_1,
in_2_1,
out_2_2,
in_2_2,
out_2_3,
in_2_3,
out_2_4,
in_2_4,
out_3_0,
in_3_0,
out_3_1,
in_3_1,
out_3_2,
in_3_2,
out_3_3,
in_3_3,
out_3_4,
in_3_4,
config_addr,
config_data,
config_en
);

  input  clk;
  input  reset;
  input  config_en;
  input [31:0] config_data;

  // FIXME
  // Bits [31:24] are for switch box, [23:0] for connection box.
  // So bits  [23:0] go unused in this module.  Until this is fixed,
  // we have to let the linter know (verilator directives below) or it complains:
  // %Warning-UNUSED: .../sb_unq1.v:115: Bits of signal are not used: config_addr[23:0]
  /* verilator lint_off UNUSED */
  input [31:0] config_addr;
  /* verilator lint_on UNUSED */

  input [0:0] pe_output_0;
  
  output [0:0] out_0_0;
  input [0:0] in_0_0;
  output [0:0] out_0_1;
  input [0:0] in_0_1;
  output [0:0] out_0_2;
  input [0:0] in_0_2;
  output [0:0] out_0_3;
  input [0:0] in_0_3;
  output [0:0] out_0_4;
  input [0:0] in_0_4;
  output [0:0] out_1_0;
  input [0:0] in_1_0;
  output [0:0] out_1_1;
  input [0:0] in_1_1;
  output [0:0] out_1_2;
  input [0:0] in_1_2;
  output [0:0] out_1_3;
  input [0:0] in_1_3;
  output [0:0] out_1_4;
  input [0:0] in_1_4;
  output [0:0] out_2_0;
  input [0:0] in_2_0;
  output [0:0] out_2_1;
  input [0:0] in_2_1;
  output [0:0] out_2_2;
  input [0:0] in_2_2;
  output [0:0] out_2_3;
  input [0:0] in_2_3;
  output [0:0] out_2_4;
  input [0:0] in_2_4;
  output [0:0] out_3_0;
  input [0:0] in_3_0;
  output [0:0] out_3_1;
  input [0:0] in_3_1;
  output [0:0] out_3_2;
  input [0:0] in_3_2;
  output [0:0] out_3_3;
  input [0:0] in_3_3;
  output [0:0] out_3_4;
  input [0:0] in_3_4;




  // reg [63:0] config_sb;

  // Note not all config bits are used by this module.  Is this on purpose?
  // As a result, however, verilator needs lint directives or it complains:
  // %Warning-UNUSED: .../sb_unq1.v:164: Bits of signal are not used: config_sb[63:60]
  // %Warning-UNUSED: .../sb_unq3.v:164: Bits of signal are not used: config_sb[63:40]
  // [FIXME Owner please confirm that this is correct and delete this FIXME line]

  /* verilator lint_off UNUSED */
  reg [63:0] config_sb;
  /* verilator lint_on UNUSED */
  
  always @(posedge clk or posedge reset) begin
    if (reset==1'b1) begin

   // #// Seems like the entire register should be initialized, not just a subset of the bits, yes?
   // #// FIXME Owner please check that this is still correct!
   // #// config_sb <= 60'd0;
   // #   config_sb <= 64'd0;

   // 12/2017 Nikhil changed default to pe_output to avoid loops
   // config_sb <= ~(64'd0);
   // 
   // 12/2017 SR changed default so that pe_output is selected AND registers are off
   // Want regs initialized to zero, everything else to 1 e.g. 64'h0000,00FF,FFFF,FFFF
      config_sb <= 64'h000000FFFFFFFFFF;

    end else begin
      if (config_en==1'b1) begin
         case (config_addr[31:24])
           8'd0: config_sb[31:0] <= config_data;
           8'd1: config_sb[63:32] <= config_data;

           default: ;

         endcase
      end
    end
  end

  reg [0:0] out_0_0_i;
  always @(*) begin
    case (config_sb[1:0])
        2'd0: out_0_0_i = in_1_0;
        2'd1: out_0_0_i = in_2_0;
        2'd2: out_0_0_i = in_3_0;
        2'd3: out_0_0_i = pe_output_0;
        default: out_0_0_i = 0;
    endcase
  end
  reg [0:0] out_0_0_id1;
  always @(posedge clk or posedge reset) begin
     if (reset == 1'b1) begin
        out_0_0_id1 <= 0;
     end else begin
        out_0_0_id1 <= out_0_0_i;
     end
  end
  assign out_0_0 = config_sb[40]?out_0_0_id1:out_0_0_i; 
  reg [0:0] out_0_1_i;
  always @(*) begin
    case (config_sb[3:2])
        2'd0: out_0_1_i = in_1_1;
        2'd1: out_0_1_i = in_2_1;
        2'd2: out_0_1_i = in_3_1;
        2'd3: out_0_1_i = pe_output_0;
        default: out_0_1_i = 0;
    endcase
  end
  reg [0:0] out_0_1_id1;
  always @(posedge clk or posedge reset) begin
     if (reset == 1'b1) begin
        out_0_1_id1 <= 0;
     end else begin
        out_0_1_id1 <= out_0_1_i;
     end
  end
  assign out_0_1 = config_sb[41]?out_0_1_id1:out_0_1_i; 
  reg [0:0] out_0_2_i;
  always @(*) begin
    case (config_sb[5:4])
        2'd0: out_0_2_i = in_1_2;
        2'd1: out_0_2_i = in_2_2;
        2'd2: out_0_2_i = in_3_2;
        2'd3: out_0_2_i = pe_output_0;
        default: out_0_2_i = 0;
    endcase
  end
  reg [0:0] out_0_2_id1;
  always @(posedge clk or posedge reset) begin
     if (reset == 1'b1) begin
        out_0_2_id1 <= 0;
     end else begin
        out_0_2_id1 <= out_0_2_i;
     end
  end
  assign out_0_2 = config_sb[42]?out_0_2_id1:out_0_2_i; 
  reg [0:0] out_0_3_i;
  always @(*) begin
    case (config_sb[7:6])
        2'd0: out_0_3_i = in_1_3;
        2'd1: out_0_3_i = in_2_3;
        2'd2: out_0_3_i = in_3_3;
        2'd3: out_0_3_i = pe_output_0;
        default: out_0_3_i = 0;
    endcase
  end
  reg [0:0] out_0_3_id1;
  always @(posedge clk or posedge reset) begin
     if (reset == 1'b1) begin
        out_0_3_id1 <= 0;
     end else begin
        out_0_3_id1 <= out_0_3_i;
     end
  end
  assign out_0_3 = config_sb[43]?out_0_3_id1:out_0_3_i; 
  reg [0:0] out_0_4_i;
  always @(*) begin
    case (config_sb[9:8])
        2'd0: out_0_4_i = in_1_4;
        2'd1: out_0_4_i = in_2_4;
        2'd2: out_0_4_i = in_3_4;
        2'd3: out_0_4_i = pe_output_0;
        default: out_0_4_i = 0;
    endcase
  end
  reg [0:0] out_0_4_id1;
  always @(posedge clk or posedge reset) begin
     if (reset == 1'b1) begin
        out_0_4_id1 <= 0;
     end else begin
        out_0_4_id1 <= out_0_4_i;
     end
  end
  assign out_0_4 = config_sb[44]?out_0_4_id1:out_0_4_i; 
  reg [0:0] out_1_0_i;
  always @(*) begin
    case (config_sb[11:10])
        2'd0: out_1_0_i = in_0_0;
        2'd1: out_1_0_i = in_2_0;
        2'd2: out_1_0_i = in_3_0;
        2'd3: out_1_0_i = pe_output_0;
        default: out_1_0_i = 0;
    endcase
  end
  reg [0:0] out_1_0_id1;
  always @(posedge clk or posedge reset) begin
     if (reset == 1'b1) begin
        out_1_0_id1 <= 0;
     end else begin
        out_1_0_id1 <= out_1_0_i;
     end
  end
  assign out_1_0 = config_sb[45]?out_1_0_id1:out_1_0_i; 
  reg [0:0] out_1_1_i;
  always @(*) begin
    case (config_sb[13:12])
        2'd0: out_1_1_i = in_0_1;
        2'd1: out_1_1_i = in_2_1;
        2'd2: out_1_1_i = in_3_1;
        2'd3: out_1_1_i = pe_output_0;
        default: out_1_1_i = 0;
    endcase
  end
  reg [0:0] out_1_1_id1;
  always @(posedge clk or posedge reset) begin
     if (reset == 1'b1) begin
        out_1_1_id1 <= 0;
     end else begin
        out_1_1_id1 <= out_1_1_i;
     end
  end
  assign out_1_1 = config_sb[46]?out_1_1_id1:out_1_1_i; 
  reg [0:0] out_1_2_i;
  always @(*) begin
    case (config_sb[15:14])
        2'd0: out_1_2_i = in_0_2;
        2'd1: out_1_2_i = in_2_2;
        2'd2: out_1_2_i = in_3_2;
        2'd3: out_1_2_i = pe_output_0;
        default: out_1_2_i = 0;
    endcase
  end
  reg [0:0] out_1_2_id1;
  always @(posedge clk or posedge reset) begin
     if (reset == 1'b1) begin
        out_1_2_id1 <= 0;
     end else begin
        out_1_2_id1 <= out_1_2_i;
     end
  end
  assign out_1_2 = config_sb[47]?out_1_2_id1:out_1_2_i; 
  reg [0:0] out_1_3_i;
  always @(*) begin
    case (config_sb[17:16])
        2'd0: out_1_3_i = in_0_3;
        2'd1: out_1_3_i = in_2_3;
        2'd2: out_1_3_i = in_3_3;
        2'd3: out_1_3_i = pe_output_0;
        default: out_1_3_i = 0;
    endcase
  end
  reg [0:0] out_1_3_id1;
  always @(posedge clk or posedge reset) begin
     if (reset == 1'b1) begin
        out_1_3_id1 <= 0;
     end else begin
        out_1_3_id1 <= out_1_3_i;
     end
  end
  assign out_1_3 = config_sb[48]?out_1_3_id1:out_1_3_i; 
  reg [0:0] out_1_4_i;
  always @(*) begin
    case (config_sb[19:18])
        2'd0: out_1_4_i = in_0_4;
        2'd1: out_1_4_i = in_2_4;
        2'd2: out_1_4_i = in_3_4;
        2'd3: out_1_4_i = pe_output_0;
        default: out_1_4_i = 0;
    endcase
  end
  reg [0:0] out_1_4_id1;
  always @(posedge clk or posedge reset) begin
     if (reset == 1'b1) begin
        out_1_4_id1 <= 0;
     end else begin
        out_1_4_id1 <= out_1_4_i;
     end
  end
  assign out_1_4 = config_sb[49]?out_1_4_id1:out_1_4_i; 
  reg [0:0] out_2_0_i;
  always @(*) begin
    case (config_sb[21:20])
        2'd0: out_2_0_i = in_0_0;
        2'd1: out_2_0_i = in_1_0;
        2'd2: out_2_0_i = in_3_0;
        2'd3: out_2_0_i = pe_output_0;
        default: out_2_0_i = 0;
    endcase
  end
  reg [0:0] out_2_0_id1;
  always @(posedge clk or posedge reset) begin
     if (reset == 1'b1) begin
        out_2_0_id1 <= 0;
     end else begin
        out_2_0_id1 <= out_2_0_i;
     end
  end
  assign out_2_0 = config_sb[50]?out_2_0_id1:out_2_0_i; 
  reg [0:0] out_2_1_i;
  always @(*) begin
    case (config_sb[23:22])
        2'd0: out_2_1_i = in_0_1;
        2'd1: out_2_1_i = in_1_1;
        2'd2: out_2_1_i = in_3_1;
        2'd3: out_2_1_i = pe_output_0;
        default: out_2_1_i = 0;
    endcase
  end
  reg [0:0] out_2_1_id1;
  always @(posedge clk or posedge reset) begin
     if (reset == 1'b1) begin
        out_2_1_id1 <= 0;
     end else begin
        out_2_1_id1 <= out_2_1_i;
     end
  end
  assign out_2_1 = config_sb[51]?out_2_1_id1:out_2_1_i; 
  reg [0:0] out_2_2_i;
  always @(*) begin
    case (config_sb[25:24])
        2'd0: out_2_2_i = in_0_2;
        2'd1: out_2_2_i = in_1_2;
        2'd2: out_2_2_i = in_3_2;
        2'd3: out_2_2_i = pe_output_0;
        default: out_2_2_i = 0;
    endcase
  end
  reg [0:0] out_2_2_id1;
  always @(posedge clk or posedge reset) begin
     if (reset == 1'b1) begin
        out_2_2_id1 <= 0;
     end else begin
        out_2_2_id1 <= out_2_2_i;
     end
  end
  assign out_2_2 = config_sb[52]?out_2_2_id1:out_2_2_i; 
  reg [0:0] out_2_3_i;
  always @(*) begin
    case (config_sb[27:26])
        2'd0: out_2_3_i = in_0_3;
        2'd1: out_2_3_i = in_1_3;
        2'd2: out_2_3_i = in_3_3;
        2'd3: out_2_3_i = pe_output_0;
        default: out_2_3_i = 0;
    endcase
  end
  reg [0:0] out_2_3_id1;
  always @(posedge clk or posedge reset) begin
     if (reset == 1'b1) begin
        out_2_3_id1 <= 0;
     end else begin
        out_2_3_id1 <= out_2_3_i;
     end
  end
  assign out_2_3 = config_sb[53]?out_2_3_id1:out_2_3_i; 
  reg [0:0] out_2_4_i;
  always @(*) begin
    case (config_sb[29:28])
        2'd0: out_2_4_i = in_0_4;
        2'd1: out_2_4_i = in_1_4;
        2'd2: out_2_4_i = in_3_4;
        2'd3: out_2_4_i = pe_output_0;
        default: out_2_4_i = 0;
    endcase
  end
  reg [0:0] out_2_4_id1;
  always @(posedge clk or posedge reset) begin
     if (reset == 1'b1) begin
        out_2_4_id1 <= 0;
     end else begin
        out_2_4_id1 <= out_2_4_i;
     end
  end
  assign out_2_4 = config_sb[54]?out_2_4_id1:out_2_4_i; 
  reg [0:0] out_3_0_i;
  always @(*) begin
    case (config_sb[31:30])
        2'd0: out_3_0_i = in_0_0;
        2'd1: out_3_0_i = in_1_0;
        2'd2: out_3_0_i = in_2_0;
        2'd3: out_3_0_i = pe_output_0;
        default: out_3_0_i = 0;
    endcase
  end
  reg [0:0] out_3_0_id1;
  always @(posedge clk or posedge reset) begin
     if (reset == 1'b1) begin
        out_3_0_id1 <= 0;
     end else begin
        out_3_0_id1 <= out_3_0_i;
     end
  end
  assign out_3_0 = config_sb[55]?out_3_0_id1:out_3_0_i; 
  reg [0:0] out_3_1_i;
  always @(*) begin
    case (config_sb[33:32])
        2'd0: out_3_1_i = in_0_1;
        2'd1: out_3_1_i = in_1_1;
        2'd2: out_3_1_i = in_2_1;
        2'd3: out_3_1_i = pe_output_0;
        default: out_3_1_i = 0;
    endcase
  end
  reg [0:0] out_3_1_id1;
  always @(posedge clk or posedge reset) begin
     if (reset == 1'b1) begin
        out_3_1_id1 <= 0;
     end else begin
        out_3_1_id1 <= out_3_1_i;
     end
  end
  assign out_3_1 = config_sb[56]?out_3_1_id1:out_3_1_i; 
  reg [0:0] out_3_2_i;
  always @(*) begin
    case (config_sb[35:34])
        2'd0: out_3_2_i = in_0_2;
        2'd1: out_3_2_i = in_1_2;
        2'd2: out_3_2_i = in_2_2;
        2'd3: out_3_2_i = pe_output_0;
        default: out_3_2_i = 0;
    endcase
  end
  reg [0:0] out_3_2_id1;
  always @(posedge clk or posedge reset) begin
     if (reset == 1'b1) begin
        out_3_2_id1 <= 0;
     end else begin
        out_3_2_id1 <= out_3_2_i;
     end
  end
  assign out_3_2 = config_sb[57]?out_3_2_id1:out_3_2_i; 
  reg [0:0] out_3_3_i;
  always @(*) begin
    case (config_sb[37:36])
        2'd0: out_3_3_i = in_0_3;
        2'd1: out_3_3_i = in_1_3;
        2'd2: out_3_3_i = in_2_3;
        2'd3: out_3_3_i = pe_output_0;
        default: out_3_3_i = 0;
    endcase
  end
  reg [0:0] out_3_3_id1;
  always @(posedge clk or posedge reset) begin
     if (reset == 1'b1) begin
        out_3_3_id1 <= 0;
     end else begin
        out_3_3_id1 <= out_3_3_i;
     end
  end
  assign out_3_3 = config_sb[58]?out_3_3_id1:out_3_3_i; 
  reg [0:0] out_3_4_i;
  always @(*) begin
    case (config_sb[39:38])
        2'd0: out_3_4_i = in_0_4;
        2'd1: out_3_4_i = in_1_4;
        2'd2: out_3_4_i = in_2_4;
        2'd3: out_3_4_i = pe_output_0;
        default: out_3_4_i = 0;
    endcase
  end
  reg [0:0] out_3_4_id1;
  always @(posedge clk or posedge reset) begin
     if (reset == 1'b1) begin
        out_3_4_id1 <= 0;
     end else begin
        out_3_4_id1 <= out_3_4_i;
     end
  end
  assign out_3_4 = config_sb[59]?out_3_4_id1:out_3_4_i; 
endmodule


