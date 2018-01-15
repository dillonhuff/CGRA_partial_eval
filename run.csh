#!/bin/csh -f

# Convert verilog to CoreIR, save in top.json
yosys -p "proc; memory; pmuxtree; to_coreir" -m /Users/dillon/CppWorkspace/VerilogToCoreIR/to_coreir.so ./CGRA_verilog/*.v ./CGRA_verilog/*.sv

# Make the CoreIR process code
make partial_evaluator -j

# Run partial evaluation code
./partial_evaluator
