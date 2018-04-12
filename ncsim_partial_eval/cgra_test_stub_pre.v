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

      config_file = $fopen(
