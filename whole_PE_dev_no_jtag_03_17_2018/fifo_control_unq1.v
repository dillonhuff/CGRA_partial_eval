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
//  Source file: /Users/dillon/VerilogWorkspace/CGRAGenerator/hardware/generator_z/memory_core/fifo_control.vp
//  Source template: fifo_control
//
// --------------- Begin Pre-Generation Parameters Status Report ---------------
//
//	From 'generate' statement (priority=5):
// Parameter ddepth 	= 1024
// Parameter dwidth 	= 16
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

///////////////////////////////////////////////////////////////////
// CGRA memory generator
//////////////////////////////////////////////////////////////////
`define xassert(condition, message) if(condition) begin $display(message); $finish(1); end

// dwidth (_GENESIS2_INHERITANCE_PRIORITY_) = 16
//
// ddepth (_GENESIS2_INHERITANCE_PRIORITY_) = 0x400
//


module fifo_control_unq1(
clk,
reset,
data_in,
wen,
data_out,
stall_read,
valid,
almost_full,
almost_empty,
almost_count,
depth,
addr_to_mem,
data_to_mem,
data_from_mem,
wen_to_mem,
ren_to_mem
);

input clk;
input reset;
input [15:0] data_in;
input wen;
input stall_read;
input [3:0] almost_count;
input [31:0] data_from_mem;
input [13:0] depth;

output valid;
output wen_to_mem;
output ren_to_mem;
output [15:0] data_out;
output [31:0] data_to_mem;
output reg [15:0] addr_to_mem;
output reg almost_full;
output reg almost_empty;

reg phase;
reg input_sr_ren_w1;
reg input_sr_ren_w2;
reg output_sr_ren;
reg output_sr_wen_w1;
reg output_sr_wen_w2;
reg prev_stall_read;
reg delay_outputs;
reg warp;

reg [1:0] read_state;
reg [1:0] read_state_d1;
reg [1:0] read_state_d2;
reg [1:0] read_state_d3;
reg [15:0] read_addr;
reg [15:0] write_addr;
reg [15:0] num_words_mem;
reg [15:0] num_words_read;
reg [15:0] data_out_d1;
reg valid_d1;

wire input_sr_a1;
wire input_sr_a2;
wire move_last_pixel_from_sr_to_sr;
wire [31:0] output_sr_data_in;
wire [15:0] warp_addr;
wire [15:0] total_word_count;
wire [1:0] input_sr_count;
wire [15:0] data_out_d0;
wire valid_d0;
wire [15:0] depth_even;
wire stall_read_int;


input_sr_unq1  input_sr(
.clk(clk),
.reset(reset),
.data_in(data_in),
.wen(wen),
.data_out(data_to_mem),
.avail_w1(input_sr_a1),
.avail_w2(input_sr_a2),
.ren_w1(input_sr_ren_w1),
.ren_w2(input_sr_ren_w2),
.count(input_sr_count)
);

output_sr_unq1  output_sr(
.clk(clk),
.reset(reset),
.data_in(output_sr_data_in),
.wen_w1(output_sr_wen_w1),
.wen_w2(output_sr_wen_w2),
.data_out(data_out_d0),
.avail(output_sr_data_avail),
.ren(output_sr_ren),
.valid(valid_d0)
);

always @(posedge clk or posedge reset) begin
  if (reset==1'b1) begin
    phase <= 1'b0;
  end else begin
    phase <= ~phase;
  end
end

assign move_last_pixel_from_sr_to_sr = (read_state_d2==2'd1) && (read_state_d1==2'd0) && (input_sr_count==2'd1) && (((num_words_read+1'b1)==depth)) ; 
assign output_sr_data_in = (input_sr_ren_w1==1'b1)?data_to_mem:data_from_mem;
assign wen_to_mem = input_sr_ren_w2;
assign depth_even = ((depth>>1)<<1);

always @(posedge clk or posedge reset) begin
  if (reset==1'b1) begin
    input_sr_ren_w1 <= 1'b0;
    input_sr_ren_w2 <= 1'b0;
  end else begin
    input_sr_ren_w2 <= 1'b0;
    input_sr_ren_w1 <= 1'b0;
    if (phase==1'b0) begin // phase=1=WRITE to mem;1=READ from mem
      if (input_sr_a2==1'b1) begin
        input_sr_ren_w2 <= 1'b1;
        input_sr_ren_w1 <= 1'b0;
      end else if (move_last_pixel_from_sr_to_sr==1'b1) begin
        input_sr_ren_w2 <= 1'b0;
        input_sr_ren_w1 <= 1'b1;
      end
    end else begin 
    end
  end
end

assign warp_addr = ((depth + almost_count) >> 1) - 16'd1;
assign total_word_count = num_words_mem + input_sr_count;
assign ren_to_mem = (phase==1'b0) & (read_state==2'd1);

always @(posedge clk or posedge reset) begin
  if (reset==1'b1) begin
    num_words_mem <= 16'd0;
    read_addr <= 16'd0;
    write_addr <= 16'd0;
  end else begin
    if (phase==1'b1) begin 
      if (input_sr_ren_w2 == 1'b1) begin
        if (write_addr>=warp_addr) begin
          write_addr <= 16'd0;
        end else begin
        write_addr <= write_addr + 1;
        end
        num_words_mem <= num_words_mem + 2;
      end
    end else begin
      if (read_state==2'd1) begin
        if (read_addr>=warp_addr) begin
          read_addr <= 16'd0;
        end else begin
          read_addr <= read_addr + 1;
        end
        num_words_mem <= num_words_mem - 2;
      end
    end
  end
end

assign stall_read_int = (total_word_count<(depth-3'd5)); // 3'd5 = offset to start reading for 0 latency 

always @(posedge clk or posedge reset) begin
  if (reset==1'b1) begin
    read_state <= 2'd0;
    num_words_read <= 16'd0;
    delay_outputs <= 1'b0;
    warp <= 1'b0;
  end else begin
    if (phase==1'b0) begin
      warp <= 1'b0;
      if ((read_state == 2'd0) && (stall_read==1'b0) && (stall_read_int==1'b0)) begin //00 = not reading
        read_state <= 2'd1; 
        delay_outputs <= prev_stall_read;
      end else if ((read_state == 2'd1) && (stall_read==1'b0) && ((num_words_read+2'd2)<depth_even)) begin 
        read_state <= 2'd1; // keep reading
        num_words_read <= (warp==1'b1)?16'd2:num_words_read + 16'd2;
      end else if ((read_state == 2'd1) && (stall_read==1'b1) && ((num_words_read+2'd2)<depth_even)) begin
        read_state <= 2'd2; // stalled during read
      end else if ((read_state == 2'd1) && ((num_words_read+2'd2)>=depth_even)) begin
        if ((stall_read==1'b0) && (total_word_count>=depth)) begin
          read_state <= 2'd1;
        end else begin
          read_state <= 2'd0;
          delay_outputs <= 1'b0;
        end
        warp <= 1'b1;        
        num_words_read <= num_words_read + 16'd2;
      end else if ((read_state == 2'd2) && (stall_read==1'b0)) begin
        read_state <= 2'd1; // restore stalled read
        delay_outputs <= prev_stall_read;
      end
    end
  end
end

always @(posedge clk or posedge reset) begin
  if (reset==1'b1) begin
    output_sr_wen_w1 <= 1'b0;
    output_sr_wen_w2 <= 1'b0;
    output_sr_ren <= 1'b0;
  end else begin
    output_sr_wen_w1 <= 1'b0;
    output_sr_wen_w2 <= 1'b0;
    output_sr_ren <= 1'b0;
    if (phase==1'b0) begin
      if (read_state_d1==2'd1) begin
        output_sr_wen_w2 <= 1'b1;
      end
      if ((read_state_d2==2'd1) && (read_state_d1==2'd0) && (input_sr_count==2'd1) && (((num_words_read+1'b1)==depth))) begin
        output_sr_wen_w1 <= 1'b1;
      end
    end
    if ((output_sr_data_avail==1'b1)&&(stall_read==1'b0)) begin
      output_sr_ren <= 1'b1;
    end
  end
end

always @(posedge clk) begin
  prev_stall_read <= (stall_read || stall_read_int);
  read_state_d1 <= read_state;
  read_state_d2 <= read_state_d1;
  read_state_d3 <= read_state_d2;
  valid_d1 <= valid_d0;
  data_out_d1 <= data_out_d0;
  almost_full <= (total_word_count>=depth);
  almost_empty  <= (total_word_count<=almost_count);
end

assign data_out = (delay_outputs==1'b1)?data_out_d1:data_out_d0;
assign valid = (delay_outputs==1'b1)?valid_d1:valid_d0;

always @(*) begin
  if (phase==1'b1) begin
    addr_to_mem = read_addr;
  end else begin
    addr_to_mem = write_addr;
  end
end

endmodule
