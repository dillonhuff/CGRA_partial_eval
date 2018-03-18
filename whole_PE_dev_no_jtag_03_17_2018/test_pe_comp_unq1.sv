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
//  Source file: /Users/dillon/VerilogWorkspace/CGRAGenerator/hardware/generator_z/pe_new/pe/rtl/test_pe_comp.svp
//  Source template: test_pe_comp
//
// --------------- Begin Pre-Generation Parameters Status Report ---------------
//
//	From 'generate' statement (priority=5):
// Parameter use_abs 	= 1
// Parameter use_relu 	= 0
// Parameter mult_mode 	= 1
// Parameter get_carry 	= 1
// Parameter use_max_min 	= 1
// Parameter en_double 	= 0
// Parameter use_cntr 	= 0
// Parameter debug 	= 0
// Parameter en_opt 	= 1
// Parameter use_div 	= 0
// Parameter is_msb 	= 0
// Parameter en_ovfl 	= 1
// Parameter en_trick 	= 0
// Parameter use_bool 	= 1
// Parameter use_add 	= 1
// Parameter use_shift 	= 1
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

// use_add (_GENESIS2_INHERITANCE_PRIORITY_) = 1
//
// use_cntr (_GENESIS2_INHERITANCE_PRIORITY_) = 0
//
// use_bool (_GENESIS2_INHERITANCE_PRIORITY_) = 1
//
// use_shift (_GENESIS2_INHERITANCE_PRIORITY_) = 1
//
// mult_mode (_GENESIS2_INHERITANCE_PRIORITY_) = 1
//
// use_div (_GENESIS2_INHERITANCE_PRIORITY_) = 0
//
// use_abs (_GENESIS2_INHERITANCE_PRIORITY_) = 1
//
// use_max_min (_GENESIS2_INHERITANCE_PRIORITY_) = 1
//
// use_relu (_GENESIS2_INHERITANCE_PRIORITY_) = 0
//
// en_opt (_GENESIS2_INHERITANCE_PRIORITY_) = 1
//
// en_trick (_GENESIS2_INHERITANCE_PRIORITY_) = 0
//
// en_ovfl (_GENESIS2_INHERITANCE_PRIORITY_) = 1
//
// is_msb (_GENESIS2_INHERITANCE_PRIORITY_) = 0
//
// en_double (_GENESIS2_INHERITANCE_PRIORITY_) = 0
//
// get_carry (_GENESIS2_INHERITANCE_PRIORITY_) = 1
//
// debug (_GENESIS2_INHERITANCE_PRIORITY_) = 0
//

/*
$use_add  = 1
*/


module   test_pe_comp_unq1  #(
  parameter DataWidth = 16
) (
  input [7:0]                    op_code,

  input  [DataWidth-1:0]         op_a,
  input  [DataWidth-1:0]         op_b,
  input                          op_d_p,

  output logic [0:0]             carry_out,


  output logic [DataWidth-1:0]   res,
  output logic                   ovfl,
  output logic                   res_p
);

localparam DATA_MSB      = DataWidth - 1;

localparam PE_ADD_OP     = 6'h0;

localparam PE_SUB_OP     = 6'h1;

localparam PE_ABS_OP     = 6'h3;

localparam PE_GTE_MAX_OP = 6'h4;
localparam PE_LTE_MIN_OP = 6'h5;


localparam PE_SEL_OP     = 6'h8;

localparam PE_RSHFT_OP   = 6'hF;
localparam PE_LSHFT_OP   = 6'h11;

localparam PE_MULT_0_OP  = 6'hB;
localparam PE_MULT_1_OP  = 6'hC;
localparam PE_MULT_2_OP  = 6'hD;

localparam PE_OR_OP      = 6'h12;
localparam PE_AND_OP     = 6'h13;
localparam PE_XOR_OP     = 6'h14;

logic [2*DataWidth-1:0] mult_res;

logic [7:0]        nc_op_code;
assign             nc_op_code = op_code;


logic [DATA_MSB:0] res_w;
logic              res_p_w;

logic                 is_signed;
//logic                 dual_mode;

logic                 cmpr_lte;
logic                 cmpr_gte;
logic                 cmpr_eq;

localparam NUM_ADDERS = 1;
localparam ADD_MSB = NUM_ADDERS - 1;

logic [DATA_MSB:0]  add_a     [ADD_MSB:0];
logic [DATA_MSB:0]  add_b     [ADD_MSB:0];
logic               add_c_in  [ADD_MSB:0];

wire  [DATA_MSB:0]  add_res   [ADD_MSB:0];
wire                add_c_out [ADD_MSB:0];


genvar ggg;

generate
  for(ggg=0;ggg<NUM_ADDERS;ggg=ggg+1) begin : GEN_ADD
  test_full_add #(.DataWidth(DataWidth)) full_add
    (
      .a     (add_a[ggg]),
      .b     (add_b[ggg]),
      .c_in  (add_c_in[ggg]),

//      .dual_mode (dual_mode),

      .res   (add_res[ggg]),
      .c_out (add_c_out[ggg])
    );

  assign carry_out[ggg] = add_c_out[ggg];
  end

endgenerate

assign cmpr_eq   =  ~|(op_a ^ op_b);



test_cmpr  cmpr
(
  .a_msb     (op_a[DATA_MSB]),
  .b_msb     (op_b[DATA_MSB]),
  .diff_msb  (add_res[0][DATA_MSB]),
  .is_signed (is_signed),
  .eq        (cmpr_eq),

  .lte       (cmpr_lte),
  .gte       (cmpr_gte)
);



logic                 mult_c_out;

test_mult_add #(.DataWidth(DataWidth)) test_mult_add
(
  .is_signed (is_signed),

  .a  (op_a),
  .b  (op_b),

//  .dual_mode(dual_mode),

  .res   (mult_res),
  .c_out (mult_c_out)
);





logic [DATA_MSB:0] shift_res;

test_shifter_unq1 #(.DataWidth(DataWidth)) test_shifter
(
  .is_signed (is_signed),
  .dir_left(op_code[5:0]==PE_LSHFT_OP),

  .a  (op_a),
  .b   (op_b[$clog2(DataWidth)-1+0:0]),

  .res (shift_res)
);


assign is_signed   = op_code[6];
//assign dual_mode   = op_code[8]; //Save the OP code bit for half precision support


logic diff_sign;
assign diff_sign = add_res[0][DATA_MSB];


localparam DataMSB = DataWidth-1;
logic [ADD_MSB:0] ovfl_add_signed;

assign ovfl_add_signed[0] = (op_a[DataMSB] == op_b[DataMSB]) &
                            (op_a[DataMSB] != add_res[0][DataMSB]);

logic  ovfl_sub_signed;
always_comb begin
  ovfl_sub_signed = ((op_a[DataMSB] != op_b[DataMSB]) & (op_a[DataMSB] != add_res[0][DataMSB]));
end


always_comb begin : proc_alu
  add_a[0] = op_a;
  add_b[0] = op_b;
  add_c_in[0] = 1'b0;


  res_w   = add_res[ADD_MSB];
  res_p_w = add_c_out[ADD_MSB];

  ovfl = 1'b0;


  unique case (op_code[5:0])
    PE_ADD_OP: begin
        add_c_in[0] = op_d_p;
        res_p_w     = add_c_out[0];
        ovfl        = (is_signed) ? (|ovfl_add_signed) : res_p_w;
      end
    PE_SUB_OP: begin
        add_b[0]    = ~op_b;
        add_c_in[0] = 1'b1;
        ovfl        = (is_signed) ? ovfl_sub_signed : 1'b0;
      end
    PE_ABS_OP: begin
        add_a[0]    = ~op_a;
        add_b[0]    = 0;
        add_c_in[0] = 1'b1;
        res_p_w     = op_a[DATA_MSB];

        ovfl        = res_w[DATA_MSB];

        res_w       = diff_sign ? op_a : add_res[0];
    end

    PE_GTE_MAX_OP: begin
        add_b[0]    = ~op_b;
        add_c_in[0] = 1'b1;
        res_p_w     = cmpr_gte;
        res_w       = res_p_w ? op_a : op_b;
      end
    PE_LTE_MIN_OP: begin
        add_b[0]    = ~op_b;
        add_c_in[0] = 1'b1;
        res_p_w     = cmpr_lte;
        res_w       = res_p_w ? op_a : op_b;
      end
      PE_SEL_OP: begin
        res_w = op_d_p ? op_a : op_b;
      end
    PE_RSHFT_OP: begin
        res_w = shift_res;
      end
    PE_LSHFT_OP: begin
        res_w = shift_res;
      end
    PE_MULT_0_OP: begin
        res_w   = mult_res[DATA_MSB:0];
        res_p_w = mult_c_out;
        if (is_signed) begin
          ovfl = (op_a[DATA_MSB] == op_b[DATA_MSB]) ?
                                    mult_res[DATA_MSB] :
                                    ~mult_res[DATA_MSB] & |{op_a, op_b};
        end else begin
          ovfl = |mult_res[2*DataWidth-1:DataWidth];
        end
      end
    PE_MULT_1_OP: begin
        res_w   = mult_res[3*DataWidth/2-1:DataWidth/2];
        res_p_w = mult_c_out;
        if (is_signed) begin
          ovfl = (op_a[DATA_MSB] == op_b[DATA_MSB]) ?
                                    mult_res[DATA_MSB] :
                                    ~mult_res[DATA_MSB] & |{op_a, op_b};
        end else begin
          ovfl = |mult_res[2*DataWidth-1:3*DataWidth/2];
        end
      end
    PE_MULT_2_OP: begin
        res_w   = mult_res[2*DataWidth-1:DataWidth];
        res_p_w = mult_c_out;
        ovfl = 1'b0;
      end


    PE_OR_OP: begin
        res_w = op_a | op_b;
      end
    PE_AND_OP: begin
        res_w = op_a & op_b;
      end
    PE_XOR_OP: begin
        res_w = op_a ^ op_b;
      end


    default: begin
        res_w   = op_a;
        res_p_w = op_d_p;
      end
  endcase
end


assign res   = res_w;
assign res_p = res_p_w;

endmodule
