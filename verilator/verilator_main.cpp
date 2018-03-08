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

  top->clk_in = 0;
  top->tile_id = 1;
  top->reset = 0;
  top->eval();


  top->clk_in = 1;
  top->config_addr = 0;
  top->config_data = 0;

  top->in_BUS16_S2_T0 = 34;

  top->in_BUS16_S0_T0 = 34;
  top->in_BUS16_S0_T1 = 34;
  top->in_BUS16_S0_T2 = 34;
  top->in_BUS16_S0_T3 = 34;
  top->in_BUS16_S0_T4 = 34;
  top->in_BUS16_S1_T0 = 34;
  top->in_BUS16_S1_T1 = 34;
  top->in_BUS16_S1_T2 = 34;
  top->in_BUS16_S1_T3 = 34;
  top->in_BUS16_S1_T4 = 34;
  top->in_BUS16_S2_T0 = 34;
  top->in_BUS16_S2_T1 = 34;
  top->in_BUS16_S2_T2 = 34;
  top->in_BUS16_S2_T3 = 34;
  top->in_BUS16_S2_T4 = 34;
  top->in_BUS16_S3_T0 = 34;
  top->in_BUS16_S3_T1 = 34;
  top->in_BUS16_S3_T2 = 34;
  top->in_BUS16_S3_T3 = 34;
  top->in_BUS16_S3_T4 = 34;
  
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
  
  //cout << "top->out_BUS16_S1_T0 == " << top->out_BUS16_S1_T0 << endl;

  assert(top->out_BUS16_S1_T0 == 4*2);
  
  // Write out the register values
  cout << "Top registers" << endl;
  ofstream outstream("config_register_values.txt");


  outstream << "DOLLARprocdffDOLLAR1192reg0reg0 1" << " " << (int) top->DOLLARprocdffDOLLAR1192reg0reg0_subcircuit_out << endl;

  outstream << "cbbit0DOLLARprocdffDOLLAR1190reg0reg0 32" << " " << (int) top->cbbit0DOLLARprocdffDOLLAR1190reg0reg0_subcircuit_out << endl;

  outstream << "cbbit1DOLLARprocdffDOLLAR1190reg0reg0 32" << " " << (int) top->cbbit1DOLLARprocdffDOLLAR1190reg0reg0_subcircuit_out << endl;

  outstream << "cbbit2DOLLARprocdffDOLLAR1190reg0reg0 32" << " " << (int) top->cbbit2DOLLARprocdffDOLLAR1190reg0reg0_subcircuit_out << endl;

  outstream << "cbcgenDOLLARprocdffDOLLAR1189reg0reg0 32" << " " << (int) top->cbcgenDOLLARprocdffDOLLAR1189reg0reg0_subcircuit_out << endl;

  outstream << "cbdata0DOLLARprocdffDOLLAR1191reg0reg0 32" << " " << (int) top->cbdata0DOLLARprocdffDOLLAR1191reg0reg0_subcircuit_out << endl;

  outstream << "cbdata1DOLLARprocdffDOLLAR1191reg0reg0 32" << " " << (int) top->cbdata1DOLLARprocdffDOLLAR1191reg0reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1168reg0reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1168reg0reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1169reg0reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1169reg0reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1170reg0reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1170reg0reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1171reg0reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1171reg0reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1172reg0reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1172reg0reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1173reg0reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1173reg0reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1174reg0reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1174reg0reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1175reg0reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1175reg0reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1176reg0reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1176reg0reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1177reg0reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1177reg0reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1178reg0reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1178reg0reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1179reg0reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1179reg0reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1180reg0reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1180reg0reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1181reg0reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1181reg0reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1182reg0reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1182reg0reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1183reg0reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1183reg0reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1184reg0reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1184reg0reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1185reg0reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1185reg0reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1186reg0reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1186reg0reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1187reg0reg0 1" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1187reg0reg0_subcircuit_out << endl;

  outstream << "sb1bDOLLARprocdffDOLLAR1188reg0reg0 64" << " " << (int) top->sb1bDOLLARprocdffDOLLAR1188reg0reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1147reg0reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1147reg0reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1148reg0reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1148reg0reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1149reg0reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1149reg0reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1150reg0reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1150reg0reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1151reg0reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1151reg0reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1152reg0reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1152reg0reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1153reg0reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1153reg0reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1154reg0reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1154reg0reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1155reg0reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1155reg0reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1156reg0reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1156reg0reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1157reg0reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1157reg0reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1158reg0reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1158reg0reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1159reg0reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1159reg0reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1160reg0reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1160reg0reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1161reg0reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1161reg0reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1162reg0reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1162reg0reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1163reg0reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1163reg0reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1164reg0reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1164reg0reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1165reg0reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1165reg0reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1166reg0reg0 16" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1166reg0reg0_subcircuit_out << endl;

  outstream << "sbwideDOLLARprocdffDOLLAR1167reg0reg0 64" << " " << (int) top->sbwideDOLLARprocdffDOLLAR1167reg0reg0_subcircuit_out << endl;

  outstream << "testpeDOLLARprocdffDOLLAR1145reg0reg0 16" << " " << (int) top->testpeDOLLARprocdffDOLLAR1145reg0reg0_subcircuit_out << endl;

  outstream << "testpeDOLLARprocdffDOLLAR1146reg0reg0 16" << " " << (int) top->testpeDOLLARprocdffDOLLAR1146reg0reg0_subcircuit_out << endl;

  outstream << "testpetestoptregaDOLLARprocdffDOLLAR1131reg0reg0 16" << " " << (int) top->testpetestoptregaDOLLARprocdffDOLLAR1131reg0reg0_subcircuit_out << endl;

  outstream << "testpetestoptregdDOLLARprocdffDOLLAR1136reg0reg0 1" << " " << (int) top->testpetestoptregdDOLLARprocdffDOLLAR1136reg0reg0_subcircuit_out << endl;

  outstream << "testpetestoptregeDOLLARprocdffDOLLAR1136reg0reg0 1" << " " << (int) top->testpetestoptregeDOLLARprocdffDOLLAR1136reg0reg0_subcircuit_out << endl;

  outstream << "testpetestoptregfDOLLARprocdffDOLLAR1136reg0reg0 1" << " " << (int) top->testpetestoptregfDOLLARprocdffDOLLAR1136reg0reg0_subcircuit_out << endl;

  outstream << "testpetestoptregfileDOLLARprocdffDOLLAR1132reg0reg0 16" << " " << (int) top->testpetestoptregfileDOLLARprocdffDOLLAR1132reg0reg0_subcircuit_out << endl;

  outstream << "testpetestoptregfileDOLLARprocdffDOLLAR1133reg0reg0 16" << " " << (int) top->testpetestoptregfileDOLLARprocdffDOLLAR1133reg0reg0_subcircuit_out << endl;

  outstream << "testpetestoptregfileDOLLARprocdffDOLLAR1134reg0reg0 16" << " " << (int) top->testpetestoptregfileDOLLARprocdffDOLLAR1134reg0reg0_subcircuit_out << endl;

  outstream << "testpetestoptregfileDOLLARprocdffDOLLAR1135reg0reg0 16" << " " << (int) top->testpetestoptregfileDOLLARprocdffDOLLAR1135reg0reg0_subcircuit_out << endl;

  outstream.close();


}

