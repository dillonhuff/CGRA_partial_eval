module cgra_config_tb();

   reg [31:0] config_addr_in;
   reg [31:0] config_data_in;
   reg         clk_in;
   reg         reset_in;

   reg         config_done;
   
   reg         reset_done;
   
   integer    config_file;
   integer    scan_file;

   initial begin

      $display("Opening config file");
      
      config_file = $fopen("./conv_2_1.bs", "r");
      reset_done = 0;

      if (config_file == 0) begin
	 $finish;
      end

      #1 clk_in = 0;
      
      config_addr_in = 0;
      config_data_in = 0;

      #2 reset_in = 0;
      #2 reset_in = 1;
      #3 reset_in = 0;

      reset_done = 1;

      $display("Reset done.");

      config_done = 0;
      
      #5000 $finish();
      
   end

   always #2 clk_in = ~clk_in;

   // After reseting load data / configuration between rising clock edges
   always @(negedge clk_in) begin

      if (reset_done) begin
	 scan_file = $fscanf(config_file, "%h %h\n", config_addr_in, config_data_in);

         $display("config addr = %h, config data = %h", config_addr_in, config_data_in);

	 if (!$feof(config_file)) begin
	 end else begin
            $display("Config done");
	    config_done <= 1;
	    config_addr_in <= 0;

	 end
      end
      
   end // always @ (negedge clk_in)

   wire [15:0] S0_data_in;
   wire [15:0] S1_data_in;
   wire [15:0] S2_data_in;
   wire [15:0] S3_data_in;

   wire [15:0] S0_data_out;
   wire [15:0] S1_data_out;
   wire [15:0] S2_data_out;
   wire [15:0] S3_data_out;

   always @(negedge clk_in) begin
      if (config_done) begin

         $display("S0_data_out = %b", S0_data_out);
         $display("S1_data_out = %b", S1_data_out);
         $display("S2_data_out = %b", S2_data_out);
         $display("S3_data_out = %b", S3_data_out);
         
      end
   end
   
   assign S0_data_in = 16'b1010110101;
   assign S1_data_in = 16'b1010110101;
   assign S2_data_in = 16'b1010110101;
   assign S3_data_in = 16'b1010110101;

   wire  pad_S0_T0_in;
   wire  pad_S0_T1_in;
   wire  pad_S0_T2_in;
   wire  pad_S0_T3_in;
   wire  pad_S0_T4_in;
   wire  pad_S0_T5_in;
   wire  pad_S0_T6_in;
   wire  pad_S0_T7_in;
   wire  pad_S0_T8_in;
   wire  pad_S0_T9_in;
   wire  pad_S0_T10_in;
   wire  pad_S0_T11_in;
   wire  pad_S0_T12_in;
   wire  pad_S0_T13_in;
   wire  pad_S0_T14_in;
   wire  pad_S0_T15_in;

   wire  pad_S1_T0_in;
   wire  pad_S1_T1_in;
   wire  pad_S1_T2_in;
   wire  pad_S1_T3_in;
   wire  pad_S1_T4_in;
   wire  pad_S1_T5_in;
   wire  pad_S1_T6_in;
   wire  pad_S1_T7_in;
   wire  pad_S1_T8_in;
   wire  pad_S1_T9_in;
   wire  pad_S1_T10_in;
   wire  pad_S1_T11_in;
   wire  pad_S1_T12_in;
   wire  pad_S1_T13_in;
   wire  pad_S1_T14_in;
   wire  pad_S1_T15_in;

   wire  pad_S2_T0_in;
   wire  pad_S2_T1_in;
   wire  pad_S2_T2_in;
   wire  pad_S2_T3_in;
   wire  pad_S2_T4_in;
   wire  pad_S2_T5_in;
   wire  pad_S2_T6_in;
   wire  pad_S2_T7_in;
   wire  pad_S2_T8_in;
   wire  pad_S2_T9_in;
   wire  pad_S2_T10_in;
   wire  pad_S2_T11_in;
   wire  pad_S2_T12_in;
   wire  pad_S2_T13_in;
   wire  pad_S2_T14_in;
   wire  pad_S2_T15_in;

   wire  pad_S3_T0_in;
   wire  pad_S3_T1_in;
   wire  pad_S3_T2_in;
   wire  pad_S3_T3_in;
   wire  pad_S3_T4_in;
   wire  pad_S3_T5_in;
   wire  pad_S3_T6_in;
   wire  pad_S3_T7_in;
   wire  pad_S3_T8_in;
   wire  pad_S3_T9_in;
   wire  pad_S3_T10_in;
   wire  pad_S3_T11_in;
   wire  pad_S3_T12_in;
   wire  pad_S3_T13_in;
   wire  pad_S3_T14_in;
   wire  pad_S3_T15_in;

   // Maybe it should go 15 down to 0? Not sure how the chip inputs are wired
   assign {pad_S0_T0_in,
           pad_S0_T1_in,
           pad_S0_T2_in,
           pad_S0_T3_in,
           pad_S0_T4_in,
           pad_S0_T5_in,
           pad_S0_T6_in,
           pad_S0_T7_in,
           pad_S0_T8_in,
           pad_S0_T9_in,
           pad_S0_T10_in,
           pad_S0_T11_in,
           pad_S0_T12_in,
           pad_S0_T13_in,
           pad_S0_T14_in,
           pad_S0_T15_in} = S0_data_in;

   // Maybe it should go 15 down to 0? Not sure how the chip inputs are wired
   assign {pad_S1_T0_in,
           pad_S1_T1_in,
           pad_S1_T2_in,
           pad_S1_T3_in,
           pad_S1_T4_in,
           pad_S1_T5_in,
           pad_S1_T6_in,
           pad_S1_T7_in,
           pad_S1_T8_in,
           pad_S1_T9_in,
           pad_S1_T10_in,
           pad_S1_T11_in,
           pad_S1_T12_in,
           pad_S1_T13_in,
           pad_S1_T14_in,
           pad_S1_T15_in} = S1_data_in;

   assign {pad_S2_T0_in,
           pad_S2_T1_in,
           pad_S2_T2_in,
           pad_S2_T3_in,
           pad_S2_T4_in,
           pad_S2_T5_in,
           pad_S2_T6_in,
           pad_S2_T7_in,
           pad_S2_T8_in,
           pad_S2_T9_in,
           pad_S2_T10_in,
           pad_S2_T11_in,
           pad_S2_T12_in,
           pad_S2_T13_in,
           pad_S2_T14_in,
           pad_S2_T15_in} = S2_data_in;

   assign {pad_S3_T0_in,
           pad_S3_T1_in,
           pad_S3_T2_in,
           pad_S3_T3_in,
           pad_S3_T4_in,
           pad_S3_T5_in,
           pad_S3_T6_in,
           pad_S3_T7_in,
           pad_S3_T8_in,
           pad_S3_T9_in,
           pad_S3_T10_in,
           pad_S3_T11_in,
           pad_S3_T12_in,
           pad_S3_T13_in,
           pad_S3_T14_in,
           pad_S3_T15_in} = S3_data_in;

   // Data outputs
   assign S0_data_out = {pad_S0_T0_out,
                         pad_S0_T1_out,
                         pad_S0_T2_out,
                         pad_S0_T3_out,
                         pad_S0_T4_out,
                         pad_S0_T5_out,
                         pad_S0_T6_out,
                         pad_S0_T7_out,
                         pad_S0_T8_out,
                         pad_S0_T9_out,
                         pad_S0_T10_out,
                         pad_S0_T11_out,
                         pad_S0_T12_out,
                         pad_S0_T13_out,
                         pad_S0_T14_out,
                         pad_S0_T15_out};

   assign S1_data_out = {pad_S1_T0_out,
                         pad_S1_T1_out,
                         pad_S1_T2_out,
                         pad_S1_T3_out,
                         pad_S1_T4_out,
                         pad_S1_T5_out,
                         pad_S1_T6_out,
                         pad_S1_T7_out,
                         pad_S1_T8_out,
                         pad_S1_T9_out,
                         pad_S1_T10_out,
                         pad_S1_T11_out,
                         pad_S1_T12_out,
                         pad_S1_T13_out,
                         pad_S1_T14_out,
                         pad_S1_T15_out};

   assign S2_data_out = {pad_S2_T0_out,
                         pad_S2_T1_out,
                         pad_S2_T2_out,
                         pad_S2_T3_out,
                         pad_S2_T4_out,
                         pad_S2_T5_out,
                         pad_S2_T6_out,
                         pad_S2_T7_out,
                         pad_S2_T8_out,
                         pad_S2_T9_out,
                         pad_S2_T10_out,
                         pad_S2_T11_out,
                         pad_S2_T12_out,
                         pad_S2_T13_out,
                         pad_S2_T14_out,
                         pad_S2_T15_out};

   assign S3_data_out = {pad_S3_T0_out,
                         pad_S3_T1_out,
                         pad_S3_T2_out,
                         pad_S3_T3_out,
                         pad_S3_T4_out,
                         pad_S3_T5_out,
                         pad_S3_T6_out,
                         pad_S3_T7_out,
                         pad_S3_T8_out,
                         pad_S3_T9_out,
                         pad_S3_T10_out,
                         pad_S3_T11_out,
                         pad_S3_T12_out,
                         pad_S3_T13_out,
                         pad_S3_T14_out,
                         pad_S3_T15_out};
   
                       
   
   top cgra(.config_addr_in(config_addr_in),
            .config_data_in(config_data_in),
            .clk_in(clk_in),
            .reset_in(reset_in),

            .pad_S0_T0_in(pad_S0_T0_in),
            .pad_S0_T1_in(pad_S0_T1_in),
            .pad_S0_T2_in(pad_S0_T2_in),
            .pad_S0_T3_in(pad_S0_T3_in),
            .pad_S0_T4_in(pad_S0_T4_in),
            .pad_S0_T5_in(pad_S0_T5_in),
            .pad_S0_T6_in(pad_S0_T6_in),
            .pad_S0_T7_in(pad_S0_T7_in),
            .pad_S0_T8_in(pad_S0_T8_in),
            .pad_S0_T9_in(pad_S0_T9_in),
            .pad_S0_T10_in(pad_S0_T10_in),
            .pad_S0_T11_in(pad_S0_T11_in),
            .pad_S0_T12_in(pad_S0_T12_in),
            .pad_S0_T13_in(pad_S0_T13_in),
            .pad_S0_T14_in(pad_S0_T14_in),
            .pad_S0_T15_in(pad_S0_T15_in),

            .pad_S1_T0_in(pad_S1_T0_in),
            .pad_S1_T1_in(pad_S1_T1_in),
            .pad_S1_T2_in(pad_S1_T2_in),
            .pad_S1_T3_in(pad_S1_T3_in),
            .pad_S1_T4_in(pad_S1_T4_in),
            .pad_S1_T5_in(pad_S1_T5_in),
            .pad_S1_T6_in(pad_S1_T6_in),
            .pad_S1_T7_in(pad_S1_T7_in),
            .pad_S1_T8_in(pad_S1_T8_in),
            .pad_S1_T9_in(pad_S1_T9_in),
            .pad_S1_T10_in(pad_S1_T10_in),
            .pad_S1_T11_in(pad_S1_T11_in),
            .pad_S1_T12_in(pad_S1_T12_in),
            .pad_S1_T13_in(pad_S1_T13_in),
            .pad_S1_T14_in(pad_S1_T14_in),
            .pad_S1_T15_in(pad_S1_T15_in),

            .pad_S2_T0_in(pad_S2_T0_in),
            .pad_S2_T1_in(pad_S2_T1_in),
            .pad_S2_T2_in(pad_S2_T2_in),
            .pad_S2_T3_in(pad_S2_T3_in),
            .pad_S2_T4_in(pad_S2_T4_in),
            .pad_S2_T5_in(pad_S2_T5_in),
            .pad_S2_T6_in(pad_S2_T6_in),
            .pad_S2_T7_in(pad_S2_T7_in),
            .pad_S2_T8_in(pad_S2_T8_in),
            .pad_S2_T9_in(pad_S2_T9_in),
            .pad_S2_T10_in(pad_S2_T10_in),
            .pad_S2_T11_in(pad_S2_T11_in),
            .pad_S2_T12_in(pad_S2_T12_in),
            .pad_S2_T13_in(pad_S2_T13_in),
            .pad_S2_T14_in(pad_S2_T14_in),
            .pad_S2_T15_in(pad_S2_T15_in),

            .pad_S3_T0_in(pad_S3_T0_in),
            .pad_S3_T1_in(pad_S3_T1_in),
            .pad_S3_T2_in(pad_S3_T2_in),
            .pad_S3_T3_in(pad_S3_T3_in),
            .pad_S3_T4_in(pad_S3_T4_in),
            .pad_S3_T5_in(pad_S3_T5_in),
            .pad_S3_T6_in(pad_S3_T6_in),
            .pad_S3_T7_in(pad_S3_T7_in),
            .pad_S3_T8_in(pad_S3_T8_in),
            .pad_S3_T9_in(pad_S3_T9_in),
            .pad_S3_T10_in(pad_S3_T10_in),
            .pad_S3_T11_in(pad_S3_T11_in),
            .pad_S3_T12_in(pad_S3_T12_in),
            .pad_S3_T13_in(pad_S3_T13_in),
            .pad_S3_T14_in(pad_S3_T14_in),
            .pad_S3_T15_in(pad_S3_T15_in)
            
            );
   
endmodule
