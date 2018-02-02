COREIR_HOME = /Users/dillon/CppWorkspace/coreir

pe_eval: eval_pe_tile.cpp
	g++ -std=c++11 eval_pe_tile.cpp -L$(COREIR_HOME)/lib/ -lcoreir -lcoreir-rtlil -o pe_eval

clean:
	rm -f partial_evaluator

verilog:
	# Add Optimizations later!
	verilator -Wall -Wno-fatal -Wno-DECLFILENAME --cc topMod_config.v --exe verilator_main.cpp --top-module topMod_config -CFLAGS -O3 -CFLAGS -march=native
	make OPT_FAST="-O3" -C obj_dir -j -f VtopMod_config.mk VtopMod_config
	./obj_dir/VtopMod_config

