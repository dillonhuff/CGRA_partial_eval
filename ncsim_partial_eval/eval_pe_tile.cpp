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

Module* copyModule(const std::string& copyName,
                   CoreIR::Module* const topMod) {

  Context* c = topMod->getContext();

  // Add outputs for all registers
  vector<Wireable*> ports;
  for (auto port : topMod->getDef()->sel("self")->getSelects()) {
    ports.push_back(port.second);
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
    c->getGlobal()->getModule(copyName);

  return topMod_conf;

}

std::string sanitizeHighImpedance(const std::string& bits) {
  string sanitized;
  for (int i = 0; i < (int) bits.size(); i++) {
    if (bits[i] == 'z') {
      sanitized += 'x';
    } else {
      sanitized += bits[i];
    }
  }
  return sanitized;
}
void loadSpecializedState(CoreIR::Module* const topMod,
                          std::map<string, BitVec>& portValues,
                          const std::string& register_value_file) {
  auto c = topMod->getContext();
  
  //std::ifstream rv("./config_register_values.txt");
  std::ifstream rv(register_value_file);
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
      regMapAll.insert({name, BitVec(len, sanitizeHighImpedance(strs[2]))});
    }
  }

  cout << "# of instances in top before setting ports to constants = " << topMod->getDef()->getInstances().size() << endl;  

  for (auto p : portValues) {
    portToConstant(p.first, p.second, topMod);
  }

  partiallyEvaluateCircuit(topMod, regMapAll);
  for (auto reg : regMapAll) {
    setRegisterInit(reg.first, reg.second, topMod);
  }

  // Important: Make sure all connections make sense
  bool error = topMod->getDef()->validate();
  assert(!error);
}

bool allDriversFrom(CoreIR::Select* const sel,
                    const std::set<Select*>& dataSelects) {
  auto in0Sels = getSourceSelects(sel);
  
  bool allDataSourcesFromPorts = true;

  for (auto sel : in0Sels) {
    if (!any_of(dataSelects, [sel](Select* const s) {
          return isAncestorOf(s, sel);
        })) {
      allDataSourcesFromPorts = false;
      break;
    }
  }

  return allDataSourcesFromPorts;
}

void runVerilogSpecializer(CoreIR::Module* const topMod_conf,
                           const std::vector<std::string>& portsToConnect,
                           const std::string& testStub) {
  // Start of verilog testbench run
  cout << "Saving to verilog" << endl;
  int verilog_convert = system("coreir -i topMod_config.json -o topMod_config.v > verilog_conversion_log.txt");

  assert(verilog_convert == 0);

  // Write out the verilog main
  cout << "Writing out to verilator" << endl;
  std::ifstream t(testStub);
  std::string ts((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());

  ofstream outFile("specialize_test.v");
  outFile << ts << endl;

  map<string, int> regWires;
  for (auto instR : topMod_conf->getDef()->getInstances()) {
    Instance* inst = instR.second;

    if ((getQualifiedOpName(*inst) == "coreir.reg") ||
        (getQualifiedOpName(*inst) == "coreir.reg_arst") ||
        (getQualifiedOpName(*inst) == "corebit.dff")) {
      uint width = inst->getModuleRef()->getGenArgs().at("width")->get<int>();

      outFile << "\t wire [" << to_string(width - 1) << " : 0] " << inst->toString() << ";\n" << endl;

      regWires.insert({inst->toString(), width});
    }
  }

  outFile << "\talways @(negedge clk_in) begin\n";

  outFile << "\t\tif (config_done) begin\n" << endl;   
  for (auto w : regWires) {

    outFile << "\t\t$display(\"" << w.first << " " << to_string(w.second) << " %b\", " << w.first << ");" << endl;

  }
  outFile << "\t$finish();" << endl;
  outFile << "\t\tend" << endl;
  outFile << "\tend" << endl;

  
  outFile << "\ttopMod_config top(\n";

  for (auto port : portsToConnect) {
    outFile << "\t\t." << port << "(" << port << "),\n";
  }

  for (auto w : regWires) {
    outFile << "\t\t." << w.first << "_subcircuit_out(" << w.first << ")," << endl;
  }

  outFile << "\t\t.tile_id(tile_id)\n" << endl;
  outFile << "\t);" << endl;

  outFile << "endmodule" << endl;

  outFile.close();

  cout << "Running iverilog" << endl;

  int res = system("iverilog -o test_conf specialize_test.v topMod_config.v");

  assert(res == 0);

  res = system("vvp test_conf > config_register_values.txt;");

  assert(res == 0);
}

void specializeCircuit(const std::string& jsonFile,
                       const std::string& testStubFile,
                       map<string, BitVec>& fixedPorts,
                       const vector<std::string>& portsToConnect,
                       const std::string& moduleToSpecialize,
                       const std::string& jsonOutput) {
  // Load pe tile verilog
  Context* c = newContext();

  CoreIRLoadLibrary_rtlil(c);

  Module* topMod = loadModule(c, jsonFile, moduleToSpecialize);
  c->runPasses({"rungenerators",
        "add-dummy-inputs",
        "flatten",
        "removeconstduplicates",
        "sanitize-names",
        "deletedeadinstances",
        "packconnections",
        "cullzexts"});

  cout << "Starting to fold constants" << endl;
  foldConstants(topMod);
  cout << "Done folding constants" << endl;

  Module* topMod_conf = copyModule("topMod_config", topMod);
  c->runPasses({"add-dummy-inputs"});

  // Write this out as verilog
  if (!saveToFile(c->getGlobal(), "topMod_config.json", topMod_conf)) {
    cout << "Could not save to json!!" << endl;
    c->die();
  }

  runVerilogSpecializer(topMod_conf, portsToConnect, testStubFile);

  // Read the register values back in
  loadSpecializedState(topMod, fixedPorts, "./config_register_values.txt");

  cout << "# of instances in top before after folding constants = " << topMod->getDef()->getInstances().size() << endl;  

  c->runPasses({"fold-constants", "packconnections"});
  c->runPasses({"deletedeadinstances"});

  cout << "# of instances in top after folding constants = " << topMod->getDef()->getInstances().size() << endl;

  if (!saveToFile(c->getGlobal(), jsonOutput, topMod)) {
    cout << "Could not save to json!!" << endl;
    c->die();
  }
}

void runSpecializedPETests() {

  // This should be converted to a system verilog testbench
  int make_verilog_res = system("coreir -i mul_2_pe.json -o mul_2_pe.v");

  assert(make_verilog_res == 0);

  int verilator_res = system("make verilog_res");

  assert(verilator_res == 0);
}

int main() {

  vector<string> portsToConnect{"clk_in", "reset", "config_addr", "config_data"};

  map<string, BitVec> fixedPorts(
                                 {
                                   {"tile_id", BitVec(16, 1)},
                                     {"config_addr", BitVec(32, 0)},
                                       {"config_data", BitVec(32, 0)},
                                         {"reset", BitVec(1, 0)}
                                 }
                                 );
  
  
  specializeCircuit("pe_hwmaster_03_20_2018.json",
                    "verilog_test_stub.v",
                    fixedPorts,
                    portsToConnect,
                    "pe_tile_new_unq1",
                    "mul_2_pe.json");

  runSpecializedPETests();
}
