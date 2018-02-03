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

  Module* topMod = loadModule(c, "pe_tile_new_unq1.json", "pe_tile_new_unq1");
  c->runPasses({"rungenerators",
        "flatten",
        "cullzexts",
        "removeconstduplicates",
        "packconnections",
        "sanitize-names",
        "clockifyinterface"});

  foldConstants(topMod);

  // Add outputs for all registers
  vector<Wireable*> ports;
  for (auto port : topMod->getDef()->sel("self")->getSelects()) {
    if (port.second->getType()->getDir() == Type::DK_Out) {
      ports.push_back(port.second);
    }
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

  // CoreIRLoadVerilog_coreir(c);
  // CoreIRLoadVerilog_corebit(c);
  
  // Write this out as verilog

  if (!saveToFile(c->getGlobal(), "topMod_config.json", topMod_conf)) {
    cout << "Could not save to json!!" << endl;
    c->die();
  }

  system("coreir -i topMod_config.json -o topMod_config.v");

  // Write out the verilog main

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
      outFile << "cout << \"" << inst->toString() << "\" << \" \" << (int) top->" << inst->toString() << "_subcircuit_out << endl;\n" << endl;
    }
  }

  outFile << "  outstream.close();\n" << endl;

  outFile << "\n}\n" << endl;

  outFile.close();

  int res = system("make verilog");

  assert(res == 0);

  // Read the register values back in

//   outstream << std::hex << (int) top->testpetestoptregaDOLLARprocdffDOLLAR1147reg0_subcircuit_out << endl;
//   outstream << std::hex << (int) top->testpetestoptregbDOLLARprocdffDOLLAR1147reg0_subcircuit_out << endl;

//   outstream.close();
  

// }
  
  // // Load registers from verilog here
  // unordered_map<string, BitVec> regMap;
  // unordered_map<string, BitVec> mixedRegs;

  // cout << "Splitting up registers" << endl;
  // for (auto reg : regMapAll) {
  //   cout << reg.first << endl;
  //   Instance* regInst = def->getInstances()[reg.first];

  //   cout << "\t" << regInst->toString() << endl;

  //   if (elem(regInst, subCircuitInstances)) {
  //     cout << " is a pure config register" << endl;
  //     regMap.insert(reg);
  //   } else {
  //     cout << " is a mixed register" << endl;
  //     mixedRegs.insert(reg);
  //   }
  // }

  // Module* wholeTopMod = topMod;
  // // Partially evaluate the circuit given the registers
  // partiallyEvaluateCircuit(wholeTopMod, regMap);

  // c->runPasses({"packconnections"});
  // c->runPasses({"deletedeadinstances"});

  // // Simulate with the other register defaults
  // SimulatorState state(wholeTopMod);
  // for (auto reg : mixedRegs) {
  //   state.setRegister(reg.first, reg.second);
  // }

}
