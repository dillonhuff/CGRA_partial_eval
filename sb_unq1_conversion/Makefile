COREIR_HOME = /Users/dillon/CppWorkspace/coreir

verilog:
	yosys -p "hierarchy; proc; memory -nomap; pmuxtree; to_coreir" -ql log.txt -m /Users/dillon/CppWorkspace/VerilogToCoreIR/to_coreir.so ../original_pe_verilator/*.sv ./*sb_unq1.v
	coreir -i __DOLLAR__paramod__BACKSLASH__test_debug_reg__BACKSLASH__DataWidth__EQUALS__1.json -o hwmaster_pe_from_yosys_to_coreir_03_08_2018.v --load_libs rtlil
	verilator -Wall -Wno-fatal -Wno-DECLFILENAME --cc hwmaster_pe_from_yosys_to_coreir_03_08_2018.v --exe verilator_main.cpp --top-module sb_unq1 -CFLAGS -O3 -CFLAGS -march=native
	make OPT_FAST="-O3" -C obj_dir -j -f Vsb_unq1.mk Vsb_unq1
	./obj_dir/Vsb_unq1

verilator_only:
	verilator -Wall -Wno-fatal -Wno-DECLFILENAME --cc hwmaster_pe_from_yosys_to_coreir_03_08_2018.v --exe verilator_main.cpp --top-module sb_unq1 -CFLAGS -O3 -CFLAGS -march=native
	make OPT_FAST="-O3" -C obj_dir -j -f Vsb_unq1.mk Vsb_unq1
	./obj_dir/Vsb_unq1
