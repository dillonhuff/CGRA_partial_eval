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

      config_file = $fopen(
