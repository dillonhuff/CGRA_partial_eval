      reset_done = 0;

      if (config_file == 0) begin
	 //$display("config_file was null");
	 $finish;
      end else begin
	 //$display("Loaded config file, descriptor = %d", config_file);
      end

      #1 clk_in = 0;
      
      config_addr = 0;
      config_data = 0;

      #2 reset = 0;
      #2 reset = 1;
      #3 reset = 0;

      reset_done = 1;
      config_done = 0;
      
      //$display("DONE WITH RESET");

      #500 $finish();

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
   

