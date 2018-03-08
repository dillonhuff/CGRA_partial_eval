#include <string>
#include <fstream>
#include <streambuf>

#include "coreir/libs/rtlil.h"
#include "coreir/passes/analysis/verilog.h"
#include "coreir/passes/transform/deletedeadinstances.h"
#include "coreir/passes/transform/unpackconnections.h"
#include "coreir/passes/transform/packconnections.h"

using namespace CoreIR;
using namespace std;

int main() {

  // Load pe tile verilog
  Context* c = newContext();

  CoreIRLoadLibrary_rtlil(c);

  //Module* topMod = loadModule(c, "pe_tile_new_unq1.json", "pe_tile_new_unq1");
  //Module* topMod = loadModule(c, "top_proc.json", "top");
  Module* topMod = loadModule(c, "pe_hwmaster_03_07_2018.json", "pe_tile_new_unq1");
  c->runPasses({"rungenerators",
        //"add-dummy-inputs",
        "flatten",
        "removeconstduplicates",
        "sanitize-names",
        "deletedeadinstances",
        "cullzexts"});

  foldConstants(topMod);

  // Add outputs for all registers
  vector<Wireable*> ports;
  for (auto port : topMod->getDef()->sel("self")->getSelects()) {
    // if ((port.second->getType()->getDir() == Type::DK_Out) ||
    //     (port.second->getType()->getDir() == Type::DK_InOut)) {
    ports.push_back(port.second);
    //}
  }

  cout << "All ports" << endl;
  for (auto pt : ports) {
    cout << "\t" << pt->toString() << endl;
  }

  auto subCircuitInstances =
    extractSubcircuit(topMod, ports);

  cout << "# of instances in subcircuit = " << subCircuitInstances.size() << endl;

  // Create the subcircuit for the config
  addSubcircuitModule("topMod_config",
                      topMod,
                      ports,
                      subCircuitInstances,
                      c,
                      c->getGlobal());

  Module* topMod_conf =
    c->getGlobal()->getModule("topMod_config");

  // Write this out as verilog
  if (!saveToFile(c->getGlobal(), "topMod_config.json", topMod_conf)) {
    cout << "Could not save to json!!" << endl;
    c->die();
  }

  cout << "Saving to verilog" << endl;
  int verilog_convert = system("coreir -i topMod_config.json -o topMod_config.v > verilog_conversion_log.txt");

  assert(verilog_convert == 0);

  // Write out the verilog main
  cout << "Writing out to verilator" << endl;
  std::ifstream t("./verilator_main_template.cpp");
  std::string ts((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());
  
  ofstream outFile("verilator_main.cpp");
  outFile << ts << endl;

  for (auto instR : topMod_conf->getDef()->getInstances()) {
    Instance* inst = instR.second;

    if ((getQualifiedOpName(*inst) == "coreir.reg") ||
        (getQualifiedOpName(*inst) == "coreir.regrst") ||
        (getQualifiedOpName(*inst) == "corebit.dff")) {
      uint width = inst->getModuleRef()->getGenArgs().at("width")->get<int>();
      outFile << "  outstream << \"" << inst->toString() << " " << width << "\" << \" \" << (int) top->" << inst->toString() << "_subcircuit_out << endl;\n" << endl;
    }
  }

  outFile << "  outstream.close();\n" << endl;

  outFile << "\n}\n" << endl;

  outFile.close();

  int res = system("make verilog");

  assert(res == 0);

  // Read the register values back in

  std::ifstream rv("./config_register_values.txt");
  std::string regVals((std::istreambuf_iterator<char>(rv)),
                      std::istreambuf_iterator<char>());

  unordered_map<string, BitVec> regMapAll;  

  auto allStrs = splitStr(regVals, "\n");

  for (uint i = 0; i < allStrs.size() - 1; i++) {
    auto str = allStrs[i];
    cout << str << endl;

    auto strs = splitStr(str, " ");

    assert((strs.size() == 3));

    if (strs.size() == 3) {
      string name = strs[0];
      int len = stoi(strs[1]);
      long int value = stoi(strs[2]);
      regMapAll.insert({name, BitVec(len, value)});
    }
  }

  Module* wholeTopMod = topMod;
  c->setTop(wholeTopMod);

  portToConstant("tile_id", BitVec(16, 1), topMod);
  portToConstant("config_addr", BitVec(32, 0), topMod);
  portToConstant("config_data", BitVec(32, 0), topMod);
  portToConstant("reset", BitVec(1, 0), topMod);
  for (auto reg : regMapAll) {
    setRegisterInit(reg.first, reg.second, topMod);
  }

  // Important: Make sure all connections make sense
  bool error = topMod->getDef()->validate();
  assert(!error);

  c->runPasses({"packconnections"});

  // This should be a verilog testbench
  // if (!saveToFile(c->getGlobal(), "mul_2_pe_pre_evaluation.json", wholeTopMod)) {
  //   cout << "Could not save to json!!" << endl;
  //   c->die();
  // }

  cout << "# of instances in top before after folding constants = " << topMod->getDef()->getInstances().size() << endl;  

  c->runPasses({"fold-constants", "packconnections"});
  c->runPasses({"deletedeadinstances"});

  cout << "# of instances in top after folding constants = " << topMod->getDef()->getInstances().size() << endl;

  // This should be a verilog testbench
  if (!saveToFile(c->getGlobal(), "mul_2_pe.json", wholeTopMod)) {
    cout << "Could not save to json!!" << endl;
    c->die();
  }

  int make_verilog_res = system("coreir -i mul_2_pe.json -o mul_2_pe.v");

  assert(make_verilog_res == 0);

  int verilator_res = system("make verilog_res");

  assert(verilator_res == 0);

}
