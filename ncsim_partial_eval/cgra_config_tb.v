module cgra_config_tb();

   wire [31:0] config_addr_in;
   wire [31:0] config_data_in;
   reg         clk_in;
   reg         reset_in;

   top cgra(.config_addr_in(config_addr_in),
            .config_data_in(config_data_in),
            .clk_in(clk_in),
            .reset_in(reset_in));
   
endmodule
