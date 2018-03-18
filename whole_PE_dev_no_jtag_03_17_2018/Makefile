COREIR_HOME = /Users/dillon/CppWorkspace/coreir

verilog:
	yosys -p "hierarchy; proc; memory; pmuxtree; to_coreir" -m /Users/dillon/CppWorkspace/VerilogToCoreIR/to_coreir.so ./*.sv ./*.v
	coreir -i top.json -o top_flat_inouts_split.v --load_libs rtlil -p rungenerators,cullzexts,removeconstduplicates,packconnections,flatten,split-inouts
	verilator -Wall -Wno-fatal -Wno-DECLFILENAME --cc top_flat_inouts_split.v --exe verilator_main.cpp --top-module top -CFLAGS -O3 -CFLAGS -march=native
	make OPT_FAST="-O3" -C obj_dir -j -f Vtop.mk Vtop
	./obj_dir/Vtop
