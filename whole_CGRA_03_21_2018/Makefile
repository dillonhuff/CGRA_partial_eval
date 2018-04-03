COREIR_HOME = /Users/dillon/CppWorkspace/coreir

verilog:
	#coreir -i /Users/dillon/CoreIRWorkspace/CGRA_coreir/top.json -o top_split_inouts_03_21_2018.v -p rungenerators,split-inouts,delete-unused-inouts,deletedeadinstances,add-dummy-inputs --load_libs rtlil
	verilator -Wall -Wno-fatal -Wno-DECLFILENAME --cc top_split_inouts_03_21_2018.v --exe verilator_main.cpp --top-module top
	make -C obj_dir -j -f Vtop.mk Vtop
	./obj_dir/Vtop
