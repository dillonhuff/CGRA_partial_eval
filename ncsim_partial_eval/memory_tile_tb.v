// Full system test of PE tile
module test();

   reg clk_in;
   reg reset;

   wire tdo;
   wire trst_n;
   wire tdi;
   wire tms;
   
   wire tck;

   reg reset_done;
   
   reg [31:0] config_addr;
   reg [31:0] config_data;

   integer    config_file;
   integer    scan_file;

   reg 	      config_done;
   
   initial begin

      config_file = $fopen("conv_2_1.bs", "r");

      reset_done = 0;

      if (config_file == 0) begin
	 $display("config_file was null");
	 $finish;
      end else begin
	 $display("Loaded config file, descriptor = %d", config_file);
      end

      #1 clk_in = 0;
      
      config_addr = 0;
      config_data = 0;

      #2 reset = 0;
      #2 reset = 1;
      #3 reset = 0;

      reset_done = 1;
      config_done = 0;
      
      $display("DONE WITH RESET");

      #5000 $finish();

   end // initial begin

   always #2 clk_in = ~clk_in;

   reg [0:0] data_in_S0_T0_reg;
   
   // After reseting load data / configuration between rising clock edges
   always @(negedge clk_in) begin

      //$display("clk_in = %d", clk_in);
      //$display("config file = %d", config_file);

      if (reset_done) begin
	 scan_file = $fscanf(config_file, "%h %h\n", config_addr, config_data);

	 if (!$feof(config_file)) begin
	 
	    //$display("config addr = %h", config_addr);
	    //$display("config data = %h", config_data);
	 
	 end else begin
	    //$display("Reached end of file!");
	    config_done <= 1;
	    config_addr <= 0;
	 end
      end
   end

   wire [15:0] tile_id;
   assign tile_id = 16'h18;

   wire [15:0] in_0_BUS16_0_0;
   wire [15:0] in_0_BUS16_0_1;
   wire [15:0] in_0_BUS16_0_2;
   wire [15:0] in_0_BUS16_0_3;
   wire [15:0] in_0_BUS16_0_4;
   wire [15:0] in_0_BUS16_2_0;
   wire [15:0] in_0_BUS16_2_1;
   wire [15:0] in_0_BUS16_2_2;
   wire [15:0] in_0_BUS16_2_3;
   wire [15:0] in_0_BUS16_2_4;
   wire [15:0] in_0_BUS16_3_0;
   wire [15:0] in_0_BUS16_3_1;
   wire [15:0] in_0_BUS16_3_2;
   wire [15:0] in_0_BUS16_3_3;
   wire [15:0] in_0_BUS16_3_4;

   wire [15:0] in_1_BUS16_0_0;
   wire [15:0] in_1_BUS16_0_1;
   wire [15:0] in_1_BUS16_0_2;
   wire [15:0] in_1_BUS16_0_3;
   wire [15:0] in_1_BUS16_0_4;
   wire [15:0] in_1_BUS16_1_0;
   wire [15:0] in_1_BUS16_1_1;
   wire [15:0] in_1_BUS16_1_2;
   wire [15:0] in_1_BUS16_1_3;
   wire [15:0] in_1_BUS16_1_4;
   wire [15:0] in_1_BUS16_2_0;
   wire [15:0] in_1_BUS16_2_1;
   wire [15:0] in_1_BUS16_2_2;
   wire [15:0] in_1_BUS16_2_3;
   wire [15:0] in_1_BUS16_2_4;

   wire [15:0] out_0_BUS16_0_0;
   wire [15:0] out_0_BUS16_0_1;
   wire [15:0] out_0_BUS16_0_2;
   wire [15:0] out_0_BUS16_0_3;
   wire [15:0] out_0_BUS16_0_4;
   wire [15:0] out_0_BUS16_2_0;
   wire [15:0] out_0_BUS16_2_1;
   wire [15:0] out_0_BUS16_2_2;
   wire [15:0] out_0_BUS16_2_3;
   wire [15:0] out_0_BUS16_2_4;
   wire [15:0] out_0_BUS16_3_0;
   wire [15:0] out_0_BUS16_3_1;
   wire [15:0] out_0_BUS16_3_2;
   wire [15:0] out_0_BUS16_3_3;
   wire [15:0] out_0_BUS16_3_4;

   wire [15:0] out_1_BUS16_0_0;
   wire [15:0] out_1_BUS16_0_1;
   wire [15:0] out_1_BUS16_0_2;
   wire [15:0] out_1_BUS16_0_3;
   wire [15:0] out_1_BUS16_0_4;
   wire [15:0] out_1_BUS16_1_0;
   wire [15:0] out_1_BUS16_1_1;
   wire [15:0] out_1_BUS16_1_2;
   wire [15:0] out_1_BUS16_1_3;
   wire [15:0] out_1_BUS16_1_4;
   wire [15:0] out_1_BUS16_2_0;
   wire [15:0] out_1_BUS16_2_1;
   wire [15:0] out_1_BUS16_2_2;
   wire [15:0] out_1_BUS16_2_3;
   wire [15:0] out_1_BUS16_2_4;

   wire [31:0] read_data;
   
   

   memory_tile_unq1 top(
                        .in_0_BUS16_0_0(in_0_BUS16_0_0),
                        .in_0_BUS16_0_1(in_0_BUS16_0_1),
                        .in_0_BUS16_0_2(in_0_BUS16_0_2),
                        .in_0_BUS16_0_3(in_0_BUS16_0_3),
                        .in_0_BUS16_0_4(in_0_BUS16_0_4),
                        .in_0_BUS16_2_0(in_0_BUS16_2_0),
                        .in_0_BUS16_2_1(in_0_BUS16_2_1),
                        .in_0_BUS16_2_2(in_0_BUS16_2_2),
                        .in_0_BUS16_2_3(in_0_BUS16_2_3),
                        .in_0_BUS16_2_4(in_0_BUS16_2_4),
                        .in_0_BUS16_3_0(in_0_BUS16_3_0),
                        .in_0_BUS16_3_1(in_0_BUS16_3_1),
                        .in_0_BUS16_3_2(in_0_BUS16_3_2),
                        .in_0_BUS16_3_3(in_0_BUS16_3_3),
                        .in_0_BUS16_3_4(in_0_BUS16_3_4),

                        .out_0_BUS16_0_0(out_0_BUS16_0_0),
                        .out_0_BUS16_0_1(out_0_BUS16_0_1),
                        .out_0_BUS16_0_2(out_0_BUS16_0_2),
                        .out_0_BUS16_0_3(out_0_BUS16_0_3),
                        .out_0_BUS16_0_4(out_0_BUS16_0_4),
                        .out_0_BUS16_2_0(out_0_BUS16_2_0),
                        .out_0_BUS16_2_1(out_0_BUS16_2_1),
                        .out_0_BUS16_2_2(out_0_BUS16_2_2),
                        .out_0_BUS16_2_3(out_0_BUS16_2_3),
                        .out_0_BUS16_2_4(out_0_BUS16_2_4),
                        .out_0_BUS16_3_0(out_0_BUS16_3_0),
                        .out_0_BUS16_3_1(out_0_BUS16_3_1),
                        .out_0_BUS16_3_2(out_0_BUS16_3_2),
                        .out_0_BUS16_3_3(out_0_BUS16_3_3),
                        .out_0_BUS16_3_4(out_0_BUS16_3_4),

                        .in_1_BUS16_0_0(in_1_BUS16_0_0),
                        .in_1_BUS16_0_1(in_1_BUS16_0_1),
                        .in_1_BUS16_0_2(in_1_BUS16_0_2),
                        .in_1_BUS16_0_3(in_1_BUS16_0_3),
                        .in_1_BUS16_0_4(in_1_BUS16_0_4),
                        .in_1_BUS16_2_0(in_1_BUS16_2_0),
                        .in_1_BUS16_2_1(in_1_BUS16_2_1),
                        .in_1_BUS16_2_2(in_1_BUS16_2_2),
                        .in_1_BUS16_2_3(in_1_BUS16_2_3),
                        .in_1_BUS16_2_4(in_1_BUS16_2_4),
                        .in_1_BUS16_1_0(in_1_BUS16_1_0),
                        .in_1_BUS16_1_1(in_1_BUS16_1_1),
                        .in_1_BUS16_1_2(in_1_BUS16_1_2),
                        .in_1_BUS16_1_3(in_1_BUS16_1_3),
                        .in_1_BUS16_1_4(in_1_BUS16_1_4),
                        
                        .clk_in(clk_in),
                        .reset(rst_in));
   
      
endmodule
