COREIR_HOME = /Users/dillon/CppWorkspace/coreir

partial_evaluator: partial_evaluator.cpp
	g++ -std=c++11 partial_evaluator.cpp -L$(COREIR_HOME)/lib/ -lcoreir -lcoreir-rtlil -o partial_evaluator

clean:
	rm -f partial_evaluator
