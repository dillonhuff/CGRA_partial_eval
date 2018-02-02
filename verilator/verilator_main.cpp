#include <iostream>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <string>
#include <fstream>
#include <streambuf>

#include "VtopMod_config.h"
#include "verilated.h"

using namespace std;

int main(int argc, char** argv) {
  Verilated::commandArgs(argc, argv);
  VtopMod_config* top = new VtopMod_config;

  // Read in config bitstream
  std::ifstream t("../bitstream/shell_bitstream.bs");
  std::string configBits((std::istreambuf_iterator<char>(t)),
                         std::istreambuf_iterator<char>());  

  cout << "ConfigBits = " << endl << configBits << endl;
  // Run config on the circuit

  top->clk = 0;
  top->tile_id = 1;
  top->reset = 0;

  top->eval();


  std::vector<std::string> strings;

  std::string::size_type pos = 0;
  std::string::size_type prev = 0;
  char delimiter = '\n';
  string str = configBits;
  while ((pos = str.find(delimiter, prev)) != std::string::npos) {
    strings.push_back(str.substr(prev, pos - prev));
    prev = pos + 1;
  }

  // To get the last substring (or only, if delimiter is not found)
  strings.push_back(str.substr(prev));

  cout << "Config lines" << endl;
  for (int i = 0; i < strings.size(); i++) {
    cout << strings[i] << endl;
  }

  for (int i = 0; i < strings.size(); i++) {

    cout << "Evaluating " << i << endl;

    string addrStr = strings[i].substr(0, 8);

    unsigned int configAddr;
    std::stringstream ss;
    ss << std::hex << addrStr;
    ss >> configAddr;

    string dataStr = strings[i].substr(9, 18);    

    unsigned int configData;
    std::stringstream ss2;
    ss2 << std::hex << dataStr;
    ss2 >> configData;

    cout << "\taddrStr = " << addrStr << endl;
    cout << "\tdataStr = " << dataStr << endl;

    top->config_addr = configAddr; // Insert config
    top->config_data = configData; // Insert data
    top->clk = 0;
    top->eval();
    
    top->clk = 1;
    top->eval();

  }

  cout << "Top registers" << endl;
  cout << std::hex << (int) top->test_pe__024test_opt_reg_d__024___05FDOLLAR___05Fprocdff___05FDOLLAR___05F1148__024reg0_subcircuit_out << endl;
  cout << std::hex << (int) top->test_pe__024test_opt_reg_b__024___05FDOLLAR___05Fprocdff___05FDOLLAR___05F1147__024reg0_subcircuit_out << endl;
  
  // Write out the register values

}
