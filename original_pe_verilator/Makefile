COREIR_HOME = /Users/dillon/CppWorkspace/coreir

verilog:
	verilator -Wall -Wno-fatal -Wno-DECLFILENAME --cc pe_tile_new_unq1.v --exe verilator_main.cpp --top-module pe_tile_new_unq1 -CFLAGS -O3 -CFLAGS -march=native
	make OPT_FAST="-O3" -C obj_dir -j -f Vpe_tile_new_unq1.mk Vpe_tile_new_unq1
	./obj_dir/Vpe_tile_new_unq1
