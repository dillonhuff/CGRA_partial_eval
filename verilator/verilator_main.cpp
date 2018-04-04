#include <iostream>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <string>
#include <fstream>
#include <streambuf>
#include <bitset>

#include "VtopMod_config.h"
#include "verilated.h"

using namespace std;

int main(int argc, char** argv) {
  Verilated::commandArgs(argc, argv);
  VtopMod_config* top = new VtopMod_config;

  // Read in config bitstream
  //std::ifstream t("../bitstream/shell_bitstream.bs");
  std::ifstream t("../bitstream/hwmaster_pw2_sixteen.bsa");
  std::string configBits((std::istreambuf_iterator<char>(t)),
                         std::istreambuf_iterator<char>());  

  cout << "ConfigBits = " << endl << configBits << endl;
  // Run config on the circuit

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

  // Write out the register values
  cout << "Top registers" << endl;
  ofstream outstream("config_register_values.txt");


  outstream << "DOLLARprocdffDOLLAR1466reg0 1" << " " << bitset<1>(top->DOLLARprocdffDOLLAR1466reg0_subcircuit_out) << endl;

  outstream << "cbbit0DOLLARprocdffDOLLAR1469reg0 32" << " " << bitset<32>(top->cbbit0DOLLARprocdffDOLLAR1469reg0_subcircuit_out) << endl;

  outstream << "cbbit1DOLLARprocdffDOLLAR1469reg0 32" << " " << bitset<32>(top->cbbit1DOLLARprocdffDOLLAR1469reg0_subcircuit_out) << endl;

  outstream << "cbbit2DOLLARprocdffDOLLAR1469reg0 32" << " " << bitset<32>(top->cbbit2DOLLARprocdffDOLLAR1469reg0_subcircuit_out) << endl;

  outstream << "cbcgenDOLLARprocdffDOLLAR1468reg0 32" << " " << bitset<32>(top->cbcgenDOLLARprocdffDOLLAR1468reg0_subcircuit_out) << endl;

  outstream << "cbdata0DOLLARprocdffDOLLAR1470reg0 32" << " " << bitset<32>(top->cbdata0DOLLARprocdffDOLLAR1470reg0_subcircuit_out) << endl;

  outstream << "cbdata1DOLLARprocdffDOLLAR1470reg0 32" << " " << bitset<32>(top->cbdata1DOLLARprocdffDOLLAR1470reg0_subcircuit_out) << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1424reg0 1" << " " << bitset<1>(top->sb1bDOLLARprocdffDOLLAR1424reg0_subcircuit_out) << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1425reg0 1" << " " << bitset<1>(top->sb1bDOLLARprocdffDOLLAR1425reg0_subcircuit_out) << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1426reg0 1" << " " << bitset<1>(top->sb1bDOLLARprocdffDOLLAR1426reg0_subcircuit_out) << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1427reg0 1" << " " << bitset<1>(top->sb1bDOLLARprocdffDOLLAR1427reg0_subcircuit_out) << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1428reg0 1" << " " << bitset<1>(top->sb1bDOLLARprocdffDOLLAR1428reg0_subcircuit_out) << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1429reg0 1" << " " << bitset<1>(top->sb1bDOLLARprocdffDOLLAR1429reg0_subcircuit_out) << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1430reg0 1" << " " << bitset<1>(top->sb1bDOLLARprocdffDOLLAR1430reg0_subcircuit_out) << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1431reg0 1" << " " << bitset<1>(top->sb1bDOLLARprocdffDOLLAR1431reg0_subcircuit_out) << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1432reg0 1" << " " << bitset<1>(top->sb1bDOLLARprocdffDOLLAR1432reg0_subcircuit_out) << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1433reg0 1" << " " << bitset<1>(top->sb1bDOLLARprocdffDOLLAR1433reg0_subcircuit_out) << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1434reg0 1" << " " << bitset<1>(top->sb1bDOLLARprocdffDOLLAR1434reg0_subcircuit_out) << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1435reg0 1" << " " << bitset<1>(top->sb1bDOLLARprocdffDOLLAR1435reg0_subcircuit_out) << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1436reg0 1" << " " << bitset<1>(top->sb1bDOLLARprocdffDOLLAR1436reg0_subcircuit_out) << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1437reg0 1" << " " << bitset<1>(top->sb1bDOLLARprocdffDOLLAR1437reg0_subcircuit_out) << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1438reg0 1" << " " << bitset<1>(top->sb1bDOLLARprocdffDOLLAR1438reg0_subcircuit_out) << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1439reg0 1" << " " << bitset<1>(top->sb1bDOLLARprocdffDOLLAR1439reg0_subcircuit_out) << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1440reg0 1" << " " << bitset<1>(top->sb1bDOLLARprocdffDOLLAR1440reg0_subcircuit_out) << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1441reg0 1" << " " << bitset<1>(top->sb1bDOLLARprocdffDOLLAR1441reg0_subcircuit_out) << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1442reg0 1" << " " << bitset<1>(top->sb1bDOLLARprocdffDOLLAR1442reg0_subcircuit_out) << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1443reg0 1" << " " << bitset<1>(top->sb1bDOLLARprocdffDOLLAR1443reg0_subcircuit_out) << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1444reg0 64" << " " << bitset<64>(top->sb1bDOLLARprocdffDOLLAR1444reg0_subcircuit_out) << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1445reg0 16" << " " << bitset<16>(top->sbwideDOLLARprocdffDOLLAR1445reg0_subcircuit_out) << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1446reg0 16" << " " << bitset<16>(top->sbwideDOLLARprocdffDOLLAR1446reg0_subcircuit_out) << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1447reg0 16" << " " << bitset<16>(top->sbwideDOLLARprocdffDOLLAR1447reg0_subcircuit_out) << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1448reg0 16" << " " << bitset<16>(top->sbwideDOLLARprocdffDOLLAR1448reg0_subcircuit_out) << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1449reg0 16" << " " << bitset<16>(top->sbwideDOLLARprocdffDOLLAR1449reg0_subcircuit_out) << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1450reg0 16" << " " << bitset<16>(top->sbwideDOLLARprocdffDOLLAR1450reg0_subcircuit_out) << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1451reg0 16" << " " << bitset<16>(top->sbwideDOLLARprocdffDOLLAR1451reg0_subcircuit_out) << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1452reg0 16" << " " << bitset<16>(top->sbwideDOLLARprocdffDOLLAR1452reg0_subcircuit_out) << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1453reg0 16" << " " << bitset<16>(top->sbwideDOLLARprocdffDOLLAR1453reg0_subcircuit_out) << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1454reg0 16" << " " << bitset<16>(top->sbwideDOLLARprocdffDOLLAR1454reg0_subcircuit_out) << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1455reg0 16" << " " << bitset<16>(top->sbwideDOLLARprocdffDOLLAR1455reg0_subcircuit_out) << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1456reg0 16" << " " << bitset<16>(top->sbwideDOLLARprocdffDOLLAR1456reg0_subcircuit_out) << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1457reg0 16" << " " << bitset<16>(top->sbwideDOLLARprocdffDOLLAR1457reg0_subcircuit_out) << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1458reg0 16" << " " << bitset<16>(top->sbwideDOLLARprocdffDOLLAR1458reg0_subcircuit_out) << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1459reg0 16" << " " << bitset<16>(top->sbwideDOLLARprocdffDOLLAR1459reg0_subcircuit_out) << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1460reg0 16" << " " << bitset<16>(top->sbwideDOLLARprocdffDOLLAR1460reg0_subcircuit_out) << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1461reg0 16" << " " << bitset<16>(top->sbwideDOLLARprocdffDOLLAR1461reg0_subcircuit_out) << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1462reg0 16" << " " << bitset<16>(top->sbwideDOLLARprocdffDOLLAR1462reg0_subcircuit_out) << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1463reg0 16" << " " << bitset<16>(top->sbwideDOLLARprocdffDOLLAR1463reg0_subcircuit_out) << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1464reg0 16" << " " << bitset<16>(top->sbwideDOLLARprocdffDOLLAR1464reg0_subcircuit_out) << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1465reg0 64" << " " << bitset<64>(top->sbwideDOLLARprocdffDOLLAR1465reg0_subcircuit_out) << endl;

  outstream << "testpeDOLLARprocdffDOLLAR1471reg0 16" << " " << bitset<16>(top->testpeDOLLARprocdffDOLLAR1471reg0_subcircuit_out) << endl;

  outstream << "testpeDOLLARprocdffDOLLAR1472reg0 16" << " " << bitset<16>(top->testpeDOLLARprocdffDOLLAR1472reg0_subcircuit_out) << endl;

  outstream << "testpetestoptregaDOLLARprocdffDOLLAR1395reg0 16" << " " << bitset<16>(top->testpetestoptregaDOLLARprocdffDOLLAR1395reg0_subcircuit_out) << endl;

  outstream << "testpetestoptregdDOLLARprocdffDOLLAR1400reg0 1" << " " << bitset<1>(top->testpetestoptregdDOLLARprocdffDOLLAR1400reg0_subcircuit_out) << endl;

  outstream << "testpetestoptregeDOLLARprocdffDOLLAR1400reg0 1" << " " << bitset<1>(top->testpetestoptregeDOLLARprocdffDOLLAR1400reg0_subcircuit_out) << endl;

  outstream << "testpetestoptregfDOLLARprocdffDOLLAR1400reg0 1" << " " << bitset<1>(top->testpetestoptregfDOLLARprocdffDOLLAR1400reg0_subcircuit_out) << endl;

  outstream << "testpetestoptregfileDOLLARprocdffDOLLAR1396reg0 16" << " " << bitset<16>(top->testpetestoptregfileDOLLARprocdffDOLLAR1396reg0_subcircuit_out) << endl;

  outstream << "testpetestoptregfileDOLLARprocdffDOLLAR1397reg0 16" << " " << bitset<16>(top->testpetestoptregfileDOLLARprocdffDOLLAR1397reg0_subcircuit_out) << endl;

  outstream << "testpetestoptregfileDOLLARprocdffDOLLAR1398reg0 16" << " " << bitset<16>(top->testpetestoptregfileDOLLARprocdffDOLLAR1398reg0_subcircuit_out) << endl;

  outstream << "testpetestoptregfileDOLLARprocdffDOLLAR1399reg0 16" << " " << bitset<16>(top->testpetestoptregfileDOLLARprocdffDOLLAR1399reg0_subcircuit_out) << endl;

  outstream.close();


}

