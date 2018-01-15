COREIR_HOME = /Users/dillon/CppWorkspace/coreir

partial_evaluator:
	g++ -std=c++11 partial_evaluator.cpp -L$(COREIR_HOME)/lib/ -lcoreir -lcoreir-rtlil -o partial_evaluator
