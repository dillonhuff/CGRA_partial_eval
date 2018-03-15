COREIR_HOME = /Users/dillon/CppWorkspace/coreir

verilog:
	yosys -p "hierarchy; proc; memory -nomap; pmuxtree; write_verilog hwmaster_pe_from_yosys_03_08_2018.v" -m /Users/dillon/CppWorkspace/VerilogToCoreIR/to_coreir.so ../original_pe_verilator/*.sv ../original_pe_verilator/*.v
	verilator -Wall -Wno-fatal -Wno-DECLFILENAME --cc hwmaster_pe_from_yosys_03_08_2018.v --exe verilator_main.cpp --top-module pe_tile_new_unq1 -CFLAGS -O3 -CFLAGS -march=native
	make OPT_FAST="-O3" -C obj_dir -j -f Vpe_tile_new_unq1.mk Vpe_tile_new_unq1
	./obj_dir/Vpe_tile_new_unq1
