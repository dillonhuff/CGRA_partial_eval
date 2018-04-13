module cgra_config_tb();

   wire [31:0] config_addr_in;
   wire [31:0] config_data_in;
   reg         clk_in;
   reg         reset_in;

   reg         reset_done;
   
   integer    config_file;
   integer    scan_file;

   initial begin

      config_file = $fopen("./cgra_tb.bsa", "r");
      reset_done = 0;

      if (config_file == 0) begin
	 $finish;
      end

      #1 clk_in = 0;
      
      config_addr = 0;
      config_data = 0;

      #2 reset = 0;
      #2 reset = 1;
      #3 reset = 0;

      reset_done = 1;
      config_done = 0;
      
      #5000 $finish();
      
   end

   always #2 clk_in = ~clk_in;

   // After reseting load data / configuration between rising clock edges
   always @(negedge clk_in) begin

      if (reset_done) begin
	 scan_file = $fscanf(config_file, "%h %h\n", config_addr, config_data);

	 if (!$feof(config_file)) begin
	 end else begin
	    config_done <= 1;
	    config_addr <= 0;
	 end
      end
   end
   
   top cgra(.config_addr_in(config_addr_in),
            .config_data_in(config_data_in),
            .clk_in(clk_in),
            .reset_in(reset_in));
   
endmodule
