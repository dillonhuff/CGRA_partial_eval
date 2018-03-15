#include <iostream>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <string>
#include <fstream>
#include <streambuf>


#include "verilated.h"
#include "Vpe_tile_new_unq1.h"

using namespace std;

int main() {
  Vpe_tile_new_unq1* top = new Vpe_tile_new_unq1();

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

  top->clk = 1;
  top->config_addr = 0;
  top->config_data = 0;

  top->in_BUS16_S2_T0 = 4;

  top->eval();

  cout << "top->out_BUS16_S1_T0 == " << top->out_BUS16_S1_T0 << endl;

  assert(top->out_BUS16_S1_T0 == 4*2);

  std::clock_t start, end;

  int nRuns = 1e8;

  start = std::clock();

  for (int i = 0; i < nRuns; i++) {
    top->clk = i % 2;
    top->eval();

    if (top->clk == 1) {

      top->in_BUS16_S2_T0 = i;

    }
  }

  end = std::clock();

  cout << "DONE." << endl;

  double time_ms =
    (end - start) / (double)(CLOCKS_PER_SEC / 1000);

  cout << "Time to compute " << nRuns << " half cycles = " << time_ms << " ms" << endl;
  double time_per_cycle = time_ms / nRuns;
  cout << "Time per cycle for " << nRuns << " = " << time_per_cycle << " ms" << endl;

  cout << "top->out_BUS16_S1_T0 == " << top->out_BUS16_S1_T0 << endl;

  return 0;
}
