COREIR_HOME = /Users/dillon/CppWorkspace/coreir

pe_eval: eval_pe_tile.cpp
	g++ -std=c++11 eval_pe_tile.cpp -L$(COREIR_HOME)/lib/ -lcoreir -lcoreir-rtlil -o pe_eval

clean:
	rm -f pe_eval

verilog:
	# Add Optimizations later!
	verilator -Wall -Wno-fatal -Wno-DECLFILENAME --cc topMod_config.v --exe verilator_main.cpp --top-module topMod_config -CFLAGS -O3 -CFLAGS -march=native
	make OPT_FAST="-O3" -C obj_dir -j -f VtopMod_config.mk VtopMod_config
	./obj_dir/VtopMod_config

verilog_res:
	# Add Optimizations later!
	verilator -Wall -Wno-fatal -Wno-DECLFILENAME --cc mul_2_pe.v --exe pe_testbench_main.cpp --top-module pe_tile_new_unq1 -CFLAGS -O3 -CFLAGS -march=native
	make OPT_FAST="-O3" -C obj_dir -j -f Vpe_tile_new_unq1.mk Vpe_tile_new_unq1
	./obj_dir/Vpe_tile_new_unq1
