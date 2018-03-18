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
//  Source file: /Users/dillon/VerilogWorkspace/CGRAGenerator/hardware/generator_z/global_controller/global_controller.svp
//  Source template: global_controller
//
// --------------- Begin Pre-Generation Parameters Status Report ---------------
//
//	From 'generate' statement (priority=5):
// Parameter dummy_value 	= 0
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

// IFC (_GENESIS2_DECLARATION_PRIORITY_) = 
//	InstancePath:top.global_controller.jtag_controller.ifc (template_ifc_unq1)
//
// dummy_value (_GENESIS2_INHERITANCE_PRIORITY_) = 0
//

module global_controller_unq1 (
clk_in, reset_in,
config_data_in,
config_addr_out,
config_data_out,
clk_out,
reset_out,
cgra_stalled,
tdi,
tdo,
tms,
tck,
trst_n
);
  
  input  tck;
  input  clk_in;
  input  reset_in;
  input  tdi;
  input  tms;
  input  trst_n;
  input [15:0] config_data_in;

  output reg [35:0] config_addr_out;
  output reg [15:0] config_data_out;
  output reg clk_out;
  output reg reset_out;
  output reg cgra_stalled;
  output tdo;

//   wire [35:0] config_addr_jtag_out;
//   wire [15:0] config_data_jtag_out;
//   reg [15:0] config_data_jtag_in;
//   wire [4:0] op;
//   reg clk;
//   reg clk_domain;
//   reg sys_clk_activated;
//   //separate op out from address field
// template_ifc_unq1  jtag_ifc (.Clk(clk), .Reset(reset_in));
// jtag_unq1  jtag_controller (.ifc(jtag_ifc),
// 			    .sys_clk_activated(sys_clk_activated),
// 			    .bsr_tdi(),
// 			    .bsr_sample(),
// 			    .bsr_intest(),
// 			    .bsr_extest(),
// 			    .bsr_update_en(),
// 			    .bsr_capture_en(),
// 		 	    .bsr_shift_dr(),
// 			    .bsr_tdo()
// 			    );

// //Connect signals to jtag_ifc ports
//       assign jtag_ifc.tck=tck;
//       assign jtag_ifc.tdi=tdi;
//       assign jtag_ifc.tms=tms;
//       assign jtag_ifc.trst_n=trst_n;
//       assign jtag_ifc.config_data_from_gc=config_data_jtag_in; 
//       assign tdo = jtag_ifc.tdo;
//       assign config_data_jtag_out = jtag_ifc.config_data_to_gc;
//       assign config_addr_jtag_out = jtag_ifc.config_addr_to_gc;
 
//   //OPCODES
//   parameter NOP = 5'd0;
//   parameter write_config =5'd1;
//   parameter read_config = 5'd2;
//   //parameter set_base_reg = 5'd3;
//   //parameter set_rw_and_count = 5'd4;
//   //parameter burst_start = 5'd5;
//   parameter write_data = 5'd5;
//   parameter read_data = 5'd6;
//   parameter write_A050 = 5'd7;
//   parameter write_TST = 5'd8;
//   parameter read_TST = 5'd9;
//   parameter global_reset = 5'd10;
//   parameter reset_tile = 5'd11; 
//   parameter stall = 5'd12;
//   parameter advance_clk = 5'd13;
//   parameter resume_clk = 5'd14;
//   parameter sys_clk_on = 5'd15;
//   parameter wr_rd_delay_reg = 5'd16;
//   parameter rd_rd_delay_reg = 5'd17;

//   //STATES FOR IGNORING INCOMING INSTRUCTIONS
//   parameter ready = 2'd0;
//   parameter reading = 2'd1;
//   parameter resetting = 2'd2;
//   parameter advancing_clk = 2'd3;

//   reg [1:0] state;
 
//   reg all_stalled_tck;
//   reg all_stalled_sys;
//   reg pulsing;
//   reg clk_switch_request;
//   reg [15:0] TST;
//   reg [15:0] counter;
//   reg [15:0] rd_delay_reg;
//   reg [5:0] clk_switch_counter;
//   assign op = config_addr_jtag_out[35:31];
//   always_comb begin
//     	clk_domain = (sys_clk_activated) ? clk_in : tck;//Are we running the GC/CGRA on sys clk or test clk?
// 	clk = ((all_stalled_tck & !sys_clk_activated) | (all_stalled_sys & sys_clk_activated )) ? 0 : clk_domain; //Are we stalling both GC and CGRA during clk domain switch?
//     	//clk_out = (cgra_stalled & (state != advancing_clk)) ? 0 : clk; //Are we stalling CGRA and not GC?
// 	clk_out = clk;
//    	reset_out = (state==resetting) ? 1 : reset_in;
//   end
//   reg [2:0] sync_counter;
//   //Clock switch counter block (Always on test_clk)
//   always @ (posedge tck or posedge reset_in) begin
// 	if (reset_in==1) begin
// 		clk_switch_counter <= 16'd20;
// 		sys_clk_activated <= 0;
// 		all_stalled_tck <= 0;
// 	end
// 	else if ((clk_switch_counter > 0) & (clk_switch_request == 1)) begin
// 		all_stalled_tck <= 1;//Deactivate clk to rest of GC
// 		clk_switch_counter <= clk_switch_counter - 1;	
// 	end
// 	else if (!sys_clk_activated & clk_switch_counter <= 0 ) begin
// 		sys_clk_activated <= 1;
// 	end	
//   end
  
//   always @ (posedge clk_in or posedge reset_in) begin //NEGEDGE TO PREVENT SHORT CLK CYCLE (TIME BETWEEN RISING EDGES)
// 	if (reset_in==1) begin
// 		all_stalled_sys <= 1;
// 		sync_counter <= 3'd4;
// 	end
// 	else if (sys_clk_activated && sync_counter > 0)
// 		sync_counter <= sync_counter - 1;
// 	else if (sys_clk_activated && sync_counter <= 0)
// 		all_stalled_sys <= 0;
//   end
//   always @ (posedge clk or posedge reset_in) begin
//     	if (reset_in==1) begin
// 		cgra_stalled <= 0;
// 		pulsing <= 0;
//        		clk_switch_request <= 0;
// 		rd_delay_reg <= 16;
// 		TST <= 0;
// 		counter <= 0;
// 		config_addr_out <= 0;
// 		config_data_out <= 0;
// 		config_data_jtag_in <= 0;
//     	end

//     	else begin
//     		if (state==ready) begin
//     			case(op)
// 			NOP: begin
// 				config_addr_out <= {op,config_addr_out[30:0]};
// 				config_data_out <= config_data_out;
// 				config_data_jtag_in <= config_data_jtag_in;	
// 			end
// 			write_config: begin
// 				config_addr_out <= config_addr_jtag_out;
// 				config_data_out <= config_data_jtag_out; 
// 			end
// 			read_config: begin
// 				config_addr_out <= config_addr_jtag_out;
// 				config_data_out <= config_data_jtag_out; 
// 				counter <= rd_delay_reg-1;
// 				state <= reading;
// 			end
// 			write_data: begin
// 				config_addr_out <= config_addr_jtag_out;
// 				config_data_out <= config_data_jtag_out; 
// 			end
// 			read_data: begin
// 				config_addr_out <= config_addr_jtag_out;
// 				config_data_out <= config_data_jtag_out; 
// 				counter <= rd_delay_reg-1;
// 				state <= reading;
// 			end
// 			write_A050: begin
// 				config_data_jtag_in <= 16'hA050;
// 			end
// 			write_TST: begin
// 				TST <= config_data_jtag_out;
// 			end
// 			read_TST: begin
// 				config_data_jtag_in <= TST;
// 			end
// 			global_reset: begin
// 				state <= resetting;
// 				counter <= (config_data_jtag_out > 0) ? config_data_jtag_out-1 : 16'd19;
// 			end
// 			reset_tile: begin
// 				config_addr_out <= config_addr_jtag_out;
// 				config_data_out <= config_data_jtag_out; 
// 			end
// 			stall: begin
// 				cgra_stalled <= 1;
// 			end			
// 			advance_clk: begin
// 				if (cgra_stalled) begin
// 					counter <= config_data_jtag_out-1;
// 					state <= advancing_clk;	
// 					cgra_stalled <= 0;
// 				end
// 			end
// 			resume_clk: begin
// 				cgra_stalled <= 0;
// 			end
// 			sys_clk_on: begin
// 				clk_switch_request <= 1;
// 			end
// 			wr_rd_delay_reg: begin
// 				rd_delay_reg <= config_data_jtag_out;
// 			end
// 			rd_rd_delay_reg: begin
// 				config_data_jtag_in <= rd_delay_reg;
// 			end
// 			default: begin
// 				config_addr_out <= {NOP,config_addr_out[30:0]};
// 				config_data_out <= config_data_out;
// 				config_data_jtag_in <= config_data_jtag_in;	
// 			end
// 			endcase
// 		end
// 		//Counter for any state that needs to block incoming instructions
// 		else begin
// 			config_addr_out <= {NOP,config_addr_out[30:0]};
// 			if (counter > 0) begin
// 				counter <= counter-1;
// 			end
// 			else begin
// 				state <= ready;
// 				if (state == reading) 
// 					config_data_jtag_in <= config_data_in;
// 				if (state == advancing_clk)
// 					cgra_stalled <= 1;
// 			end
// 		end    	
//   	end
//   end
	

endmodule
