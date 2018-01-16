#include "coreir.h"

#include "coreir/libs/rtlil.h"
#include "coreir/passes/transform/deletedeadinstances.h"
#include "coreir/passes/transform/unpackconnections.h"
#include "coreir/passes/transform/packconnections.h"

using namespace CoreIR;
using namespace std;

int main() {
  Context* c = newContext();

  CoreIRLoadLibrary_rtlil(c);

  Module* topMod = nullptr;

  if (!loadFromFile(c, "top.json", &topMod)) {
    cout << "Could not Load from json!!" << endl;
    c->die();
  }

  assert(topMod->hasDef());

  c->runPasses({"rungenerators",
        //"removeconstduplicates",
        "flatten",
        //"removeconstduplicates",
        "packconnections"});

  cout << "Done with packing connections" << endl;

  ModuleDef* def = topMod->getDef();

  // Extract the configuration subcircuit
  vector<Wireable*> subCircuitPorts{def->sel("self")->sel("config_addr"),
      def->sel("self")->sel("config_data"),
      def->sel("self")->sel("clk"),
      def->sel("self")->sel("reset")};

  auto subCircuitInstances =
    extractSubcircuit(topMod, subCircuitPorts);

  cout << "# of instances in subciruit = " << subCircuitInstances.size() << endl;

  // Create the subcircuit for the config
  addSubcircuitModule("topMod_config",
                      topMod,
                      subCircuitPorts,
                      subCircuitInstances,
                      c,
                      c->getGlobal());

  Module* topMod_conf =
    c->getGlobal()->getModule("topMod_config");

  
  // cout << "topMod_config interface" << endl;
  // cout << topMod_conf->toString() << endl;

  assert(topMod_conf != nullptr);
  assert(topMod_conf->hasDef());
  
  cout << "Saving the config circuit" << endl;
  if (!saveToFile(c->getGlobal(), "topModConfig.json", topMod_conf)) {
    cout << "Could not save to json!!" << endl;
    c->die();
  }

  cout << "Clockifying the configuration circuit interface" << endl;

  c->runPasses({"clockifyinterface"});

  cout << "Building simulator state for config" << endl;

  SimulatorState state(topMod_conf);
  state.setClock("self.clk", 0, 1);
  state.setValue("self.reset", BitVec(1, 0));

  cout << " Done building simulator state" << endl;

  cout << "Loading configuration state" << endl;

  std::ifstream configFile("./bitstream/shell_bitstream.bs");
  std::string str((std::istreambuf_iterator<char>(configFile)),
                  std::istreambuf_iterator<char>());

  cout << "Config file text" << endl;
  cout << str << endl;

  // TODO: Split config into lines

  // TODO: Load simulate config line by line

  // TODO: Partially evaluate the original top module by config

  // TODO: Simulate the partially evaluated circuit
  deleteContext(c);
}
