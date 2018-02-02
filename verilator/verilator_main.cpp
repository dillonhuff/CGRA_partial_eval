#include <iostream>
#include <ctime>
#include <cstdlib>

#include "VDesignTop.h"
#include "verilated.h"

using namespace std;

int main(int argc, char** argv) {
  Verilated::commandArgs(argc, argv);
  VtopMod_config* top = new VtopMod_config;

  // Read in config bitstream

  // Run config on the circuit

  // Write out the register values

  // 

}
