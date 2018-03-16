#include <iostream>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <string>
#include <fstream>
#include <streambuf>

#include "Vpe_tile_new_unq1.h"
#include "verilated.h"

using namespace std;

int main(int argc, char** argv) {
  Verilated::commandArgs(argc, argv);
  Vpe_tile_new_unq1* top = new Vpe_tile_new_unq1;

  // Read in config bitstream
  std::ifstream t("../bitstream/hwmaster_pw2_sixteen.bsa");
  std::string configBits((std::istreambuf_iterator<char>(t)),
                         std::istreambuf_iterator<char>());  

  cout << "ConfigBits = " << endl << configBits << endl;

  top->clk_in = 0;
  top->tile_id = 0x15;
  top->reset = 1;

  top->eval();
  
  top->clk_in = 0;
  top->tile_id = 0x15;
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

    top->clk_in = 0;
    top->eval();

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
    top->clk_in = 0;
    top->eval();
    
    top->clk_in = 1;
    top->eval();

  }

  top->clk_in = 0;
  top->tile_id = 0;
  top->eval();


  top->clk_in = 1;
  top->config_addr = 0;
  top->config_data = 0;

  int top_val = 13;

  top->in_BUS16_S2_T0 = top_val;

  top->in_BUS16_S0_T0 = top_val;
  top->in_BUS16_S0_T1 = top_val;
  top->in_BUS16_S0_T2 = top_val;
  top->in_BUS16_S0_T3 = top_val;
  top->in_BUS16_S0_T4 = top_val;
  top->in_BUS16_S1_T0 = top_val;
  top->in_BUS16_S1_T1 = top_val;
  top->in_BUS16_S1_T2 = top_val;
  top->in_BUS16_S1_T3 = top_val;
  top->in_BUS16_S1_T4 = top_val;
  top->in_BUS16_S2_T0 = top_val;
  top->in_BUS16_S2_T1 = top_val;
  top->in_BUS16_S2_T2 = top_val;
  top->in_BUS16_S2_T3 = top_val;
  top->in_BUS16_S2_T4 = top_val;
  top->in_BUS16_S3_T0 = top_val;
  top->in_BUS16_S3_T1 = top_val;
  top->in_BUS16_S3_T2 = top_val;
  top->in_BUS16_S3_T3 = top_val;
  top->in_BUS16_S3_T4 = top_val;
  
  top->eval();

  top->clk_in = 0;
  top->eval();

  top->clk_in = 1;
  top->eval();

  top->clk_in = 0;
  top->eval();

  top->clk_in = 1;
  top->eval();

  top->clk_in = 0;
  top->eval();
  
  cout << top->out_BUS16_S0_T0 << endl;
  cout << top->out_BUS16_S0_T1 << endl;
  cout << top->out_BUS16_S0_T2 << endl;
  cout << top->out_BUS16_S0_T3 << endl;
  cout << top->out_BUS16_S0_T4 << endl;
  cout << top->out_BUS16_S1_T0 << endl;
  cout << top->out_BUS16_S1_T1 << endl;
  cout << top->out_BUS16_S1_T2 << endl;
  cout << top->out_BUS16_S1_T3 << endl;
  cout << top->out_BUS16_S1_T4 << endl;
  cout << top->out_BUS16_S2_T0 << endl;
  cout << top->out_BUS16_S2_T1 << endl;
  cout << top->out_BUS16_S2_T2 << endl;
  cout << top->out_BUS16_S2_T3 << endl;
  cout << top->out_BUS16_S2_T4 << endl;
  cout << top->out_BUS16_S3_T0 << endl;
  cout << top->out_BUS16_S3_T1 << endl;
  cout << top->out_BUS16_S3_T2 << endl;
  cout << top->out_BUS16_S3_T3 << endl;
  cout << top->out_BUS16_S3_T4 << endl;

  assert(top->out_BUS16_S3_T4 == top_val*2);
  assert(top->out_BUS16_S0_T0 == top_val*2);

}
