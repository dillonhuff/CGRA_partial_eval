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
        "clockifyinterface"});

  foldConstants(topMod);

  // Add outputs for all registers
  // Lots of outputs here, need to create a new module with all pe_tiles?

  // Write this out as verilog
  c->runPasses({"verilog"});

  auto vpass = static_cast<Passes::Verilog*>(c->getPassManager()->getAnalysisPass("verilog"));

  std::ofstream sout("pe_tile_new_unq1.v");
  vpass->writeToStream(sout);
  sout.close();

  // Run the verilog

  // Q: What happens in the verilog main file?

  // Read the register values back in

  // Split up registers

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
