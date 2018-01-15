#!/bin/csh -f

yosys -p "proc; memory; pmuxtree; to_coreir" -m /Users/dillon/CppWorkspace/VerilogToCoreIR/to_coreir.so ./CGRA_verilog/*.v ./CGRA_verilog/*.sv

