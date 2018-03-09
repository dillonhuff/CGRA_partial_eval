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
//  Source file: /Users/dillon/VerilogWorkspace/CGRAGenerator/hardware/generator_z/pe_new/pe/rtl/test_debug_reg.svp
//  Source template: test_debug_reg
//
// --------------- Begin Pre-Generation Parameters Status Report ---------------
//
//	From 'generate' statement (priority=5):
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

module  test_debug_reg  #(
  parameter DataWidth = 16
) (
  input                  cfg_clk,
  input                  cfg_rst_n,
  input  [DataWidth-1:0] cfg_d,
  input                  cfg_en,

  input  [DataWidth-1:0] data_in,

  output logic           debug_irq
);

logic [DataWidth-1:0] debug_val;

always_ff @(posedge cfg_clk or negedge cfg_rst_n) begin
  if(~cfg_rst_n) begin
    debug_val <= {DataWidth{1'b0}};
  end else if(cfg_en) begin
    debug_val <= cfg_d[DataWidth-1:0];
  end
end

   assign debug_irq = 1'b0;
 //(data_in === debug_val);

logic [15:0] nc_cfg_d;
assign nc_cfg_d = cfg_d;

endmodule




