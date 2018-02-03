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

  // Write out the register values
  cout << "Top registers" << endl;
  ofstream outstream("config_register_values.txt");


  outstream << "cbbit0DOLLARprocdffDOLLAR1235reg0 32" << " " << (int) top->cbbit0DOLLARprocdffDOLLAR1235reg0_subcircuit_out << endl;

  outstream << "cbbit1DOLLARprocdffDOLLAR1235reg0 32" << " " << (int) top->cbbit1DOLLARprocdffDOLLAR1235reg0_subcircuit_out << endl;

  outstream << "cbbit2DOLLARprocdffDOLLAR1235reg0 32" << " " << (int) top->cbbit2DOLLARprocdffDOLLAR1235reg0_subcircuit_out << endl;

  outstream << "cbdata0DOLLARprocdffDOLLAR1236reg0 32" << " " << (int) top->cbdata0DOLLARprocdffDOLLAR1236reg0_subcircuit_out << endl;

  outstream << "cbdata1DOLLARprocdffDOLLAR1236reg0 32" << " " << (int) top->cbdata1DOLLARprocdffDOLLAR1236reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1190reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1190reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1191reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1191reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1192reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1192reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1193reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1193reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1194reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1194reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1195reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1195reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1196reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1196reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1197reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1197reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1198reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1198reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1199reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1199reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1200reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1200reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1201reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1201reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1202reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1202reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1203reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1203reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1204reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1204reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1205reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1205reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1206reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1206reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1207reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1207reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1208reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1208reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1209reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1209reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1210reg0 64" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1210reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1211reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1211reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1212reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1212reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1213reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1213reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1214reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1214reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1215reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1215reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1216reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1216reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1217reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1217reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1218reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1218reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1219reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1219reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1220reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1220reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1221reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1221reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1222reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1222reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1223reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1223reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1224reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1224reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1225reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1225reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1226reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1226reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1227reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1227reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1228reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1228reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1229reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1229reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1230reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1230reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1231reg0 64" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1231reg0_subcircuit_out << endl;

  outstream << "testpeDOLLARprocdffDOLLAR1150reg0 16" << " " << (int) top->testpeDOLLARprocdffDOLLAR1150reg0_subcircuit_out << endl;

  outstream << "testpeDOLLARprocdffDOLLAR1151reg0 16" << " " << (int) top->testpeDOLLARprocdffDOLLAR1151reg0_subcircuit_out << endl;

  outstream << "testpetestoptregaDOLLARprocdffDOLLAR1147reg0 16" << " " << (int) top->testpetestoptregaDOLLARprocdffDOLLAR1147reg0_subcircuit_out << endl;

  outstream << "testpetestoptregbDOLLARprocdffDOLLAR1147reg0 16" << " " << (int) top->testpetestoptregbDOLLARprocdffDOLLAR1147reg0_subcircuit_out << endl;

  outstream << "testpetestoptregdDOLLARprocdffDOLLAR1148reg0 1" << " " << (int) top->testpetestoptregdDOLLARprocdffDOLLAR1148reg0_subcircuit_out << endl;

  outstream << "testpetestoptregeDOLLARprocdffDOLLAR1148reg0 1" << " " << (int) top->testpetestoptregeDOLLARprocdffDOLLAR1148reg0_subcircuit_out << endl;

  outstream << "testpetestoptregfDOLLARprocdffDOLLAR1148reg0 1" << " " << (int) top->testpetestoptregfDOLLARprocdffDOLLAR1148reg0_subcircuit_out << endl;

  outstream.close();


}

