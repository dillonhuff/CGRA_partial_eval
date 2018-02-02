COREIR_HOME = /Users/dillon/CppWorkspace/coreir

pe_eval: eval_pe_tile.cpp
	g++ -std=c++11 eval_pe_tile.cpp -L$(COREIR_HOME)/lib/ -lcoreir -lcoreir-rtlil -o pe_eval

clean:
	rm -f partial_evaluator
