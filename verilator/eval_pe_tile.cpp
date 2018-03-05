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

void setPEInputs(SimulatorState& state) {
  state.setValue("self.in_BUS16_S0_T0", BitVec(16, 0));
  state.setValue("self.in_BUS16_S0_T1", BitVec(16, 0));
  state.setValue("self.in_BUS16_S0_T2", BitVec(16, 0));
  state.setValue("self.in_BUS16_S0_T3", BitVec(16, 0));
  state.setValue("self.in_BUS16_S0_T4", BitVec(16, 0));
  state.setValue("self.in_BUS16_S1_T0", BitVec(16, 0));
  state.setValue("self.in_BUS16_S1_T1", BitVec(16, 0));
  state.setValue("self.in_BUS16_S1_T2", BitVec(16, 0));
  state.setValue("self.in_BUS16_S1_T3", BitVec(16, 0));
  state.setValue("self.in_BUS16_S1_T4", BitVec(16, 0));
  state.setValue("self.in_BUS16_S2_T0", BitVec(16, 0));
  state.setValue("self.in_BUS16_S2_T1", BitVec(16, 0));
  state.setValue("self.in_BUS16_S2_T2", BitVec(16, 0));
  state.setValue("self.in_BUS16_S2_T3", BitVec(16, 0));
  state.setValue("self.in_BUS16_S2_T4", BitVec(16, 0));
  state.setValue("self.in_BUS16_S3_T0", BitVec(16, 0));
  state.setValue("self.in_BUS16_S3_T1", BitVec(16, 0));
  state.setValue("self.in_BUS16_S3_T2", BitVec(16, 0));
  state.setValue("self.in_BUS16_S3_T3", BitVec(16, 0));
  state.setValue("self.in_BUS16_S3_T4", BitVec(16, 0));
  state.setValue("self.in_BUS1_S0_T0", BitVec(1, 0));
  state.setValue("self.in_BUS1_S0_T1", BitVec(1, 0));
  state.setValue("self.in_BUS1_S0_T2", BitVec(1, 0));
  state.setValue("self.in_BUS1_S0_T3", BitVec(1, 0));
  state.setValue("self.in_BUS1_S0_T4", BitVec(1, 0));
  state.setValue("self.in_BUS1_S1_T0", BitVec(1, 0));
  state.setValue("self.in_BUS1_S1_T1", BitVec(1, 0));
  state.setValue("self.in_BUS1_S1_T2", BitVec(1, 0));
  state.setValue("self.in_BUS1_S1_T3", BitVec(1, 0));
  state.setValue("self.in_BUS1_S1_T4", BitVec(1, 0));
  state.setValue("self.in_BUS1_S2_T0", BitVec(1, 0));
  state.setValue("self.in_BUS1_S2_T1", BitVec(1, 0));
  state.setValue("self.in_BUS1_S2_T2", BitVec(1, 0));
  state.setValue("self.in_BUS1_S2_T3", BitVec(1, 0));
  state.setValue("self.in_BUS1_S2_T4", BitVec(1, 0));
  state.setValue("self.in_BUS1_S3_T0", BitVec(1, 0));
  state.setValue("self.in_BUS1_S3_T1", BitVec(1, 0));
  state.setValue("self.in_BUS1_S3_T2", BitVec(1, 0));
  state.setValue("self.in_BUS1_S3_T3", BitVec(1, 0));
  state.setValue("self.in_BUS1_S3_T4", BitVec(1, 0));
}

int main() {

  // Load pe tile verilog
  Context* c = newContext();

  CoreIRLoadLibrary_rtlil(c);

  //Module* topMod = loadModule(c, "pe_tile_new_unq1.json", "pe_tile_new_unq1");
  Module* topMod = loadModule(c, "top_proc.json", "top");
  c->runPasses({"rungenerators",
        "flatten",
        "removeconstduplicates",
        "sanitize-names",
        "deletedeadinstances",
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

  cout << "Saving to verilog" << endl;
  int verilog_convert = system("coreir -i topMod_config.json -o topMod_config.v");

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

    assert((strs.size() == 3)); // || (strs.size() == 0));

    if (strs.size() == 3) {
      string name = strs[0];
      int len = stoi(strs[1]);
      long int value = stoi(strs[2]);
      regMapAll.insert({name, BitVec(len, value)});
    }
  }

  // Load registers from verilog here
  // unordered_map<string, BitVec> regMap;
  // unordered_map<string, BitVec> mixedRegs;

  // cout << "Splitting up registers" << endl;
  // for (auto reg : regMapAll) {
  //   cout << reg.first << endl;
  //   Instance* regInst = topMod->getDef()->getInstances()[reg.first];

  //   cout << "\t" << regInst->toString() << endl;

  //   if (elem(regInst, subCircuitInstances)) {
  //     cout << " is a pure config register" << endl;
  //     regMap.insert(reg);
  //   } else {
  //     cout << " is a mixed register" << endl;
  //     mixedRegs.insert(reg);
  //   }
  // }

  Module* wholeTopMod = topMod;
  c->setTop(wholeTopMod);
  // Partially evaluate the circuit given the registers
  //partiallyEvaluateCircuit(wholeTopMod, regMap);

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
  
  c->runPasses({"fold-constants", "packconnections"});
  c->runPasses({"deletedeadinstances"});

  // This should be a verilog testbench
  if (!saveToFile(c->getGlobal(), "mul_2_pe.json", wholeTopMod)) {
    cout << "Could not save to json!!" << endl;
    c->die();
  }

  int make_verilog_res = system("coreir -i mul_2_pe.json -o mul_2_pe.v");

  assert(make_verilog_res == 0);

  int verilator_res = system("make verilog_res");

  assert(verilator_res == 0);

  // Simulate with the other register defaults
  // SimulatorState state(wholeTopMod);

  // setPEInputs(state);

  // // // This should be changed to setting different register defaults
  // for (auto reg : mixedRegs) {
  //   state.setRegister(reg.first, reg.second);
  // }

  // state.setClock("self.clk", 0, 0);
  // state.setValue("self.reset", BitVec(1, 0));
  // state.setValue("self.tile_id", BitVec(16, 1));
  // state.setValue("self.config_addr", BitVec(32, 0));
  // state.setValue("self.config_data", BitVec(32, 0));

  // state.setValue("self.in_BUS16_S2_T0", BitVec(16, 34));

  // state.execute();

  // cout << "self.out_BUS16_S1_T0 = " << state.getBitVec("self.out_BUS16_S1_T0") << endl;

  // assert(state.getBitVec("self.out_BUS16_S1_T0") ==
  //        mul_general_width_bv(BitVec(16, 34), BitVec(16, 2)));

  // // Another test
  // state.setValue("self.in_BUS16_S2_T0", BitVec(16, 3));

  // state.execute();

  // cout << "self.out_BUS16_S1_T0 = " << state.getBitVec("self.out_BUS16_S1_T0") << endl;

  // assert(state.getBitVec("self.out_BUS16_S1_T0") ==
  //        mul_general_width_bv(BitVec(16, 3), BitVec(16, 2)));

}
