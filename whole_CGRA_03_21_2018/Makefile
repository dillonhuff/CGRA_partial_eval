COREIR_HOME = /Users/dillon/CppWorkspace/coreir

verilog:
	coreir -i /Users/dillon/CoreIRWorkspace/CGRA_coreir/top.json -o top_split_inouts_03_21_2018.v -p rungenerators,add-dummy-inputs,split-inouts --load_libs rtlil
	verilator -Wall -Wno-fatal -Wno-DECLFILENAME --cc top_split_inouts_03_21_2018.v --exe verilator_main.cpp --top-module top -CFLAGS -O3 -CFLAGS -march=native
	make OPT_FAST="-O3" -C obj_dir -j -f Vtop.mk Vtop
	./obj_dir/Vtop
