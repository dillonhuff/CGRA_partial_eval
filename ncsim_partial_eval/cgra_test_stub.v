// Full system test of CGRA tile
module test();

   reg clk_in;
   reg reset_in;

   wire tdo;
   wire trst_n;
   wire tdi;
   wire tms;
   
   wire tck;

   reg reset_in_done;
   
   reg [31:0] config_addr_in;
   reg [31:0] config_data_in;

   integer    config_file;
   integer    scan_file;

   reg 	      config_done;
   
   initial begin

      config_file = $fopen("./pw2_16x16_only_config_lines.bsa", "r");
      reset_in_done = 0;

      if (config_file == 0) begin
	 //$display("config_file was null");
	 $finish;
      end else begin
	 //$display("Loaded config file, descriptor = %d", config_file);
      end

      #1 clk_in = 0;
      
      config_addr_in = 0;
      config_data_in = 0;

      #2 reset_in = 0;
      #2 reset_in = 1;
      #3 reset_in = 0;

      reset_in_done = 1;
      config_done = 0;
      
      //$display("DONE WITH RESET_IN");

      #500 $finish();

   end // initial begin

   always #2 clk_in = ~clk_in;

   reg [0:0] data_in_S0_T0_reg;
   
   // After reset_ining load data / configuration between rising clock edges
   always @(negedge clk_in) begin

      //$display("clk_in = %d", clk_in);
      //$display("config file = %d", config_file);

      if (reset_in_done) begin
	 scan_file = $fscanf(config_file, "%h %h\n", config_addr_in, config_data_in);

	 if (!$feof(config_file)) begin
	 
	    //$display("config addr = %h", config_addr_in);
	    //$display("config data = %h", config_data_in);
	 
	 end else begin
	    //$display("Reached end of file!");
	    config_done <= 1;
	    config_addr_in <= 0;
	 end
      end
   end

