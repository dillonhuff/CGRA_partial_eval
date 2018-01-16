#include "coreir.h"

#include <string>
#include <fstream>
#include <streambuf>

#include "coreir/libs/rtlil.h"
#include "coreir/passes/transform/deletedeadinstances.h"
#include "coreir/passes/transform/unpackconnections.h"
#include "coreir/passes/transform/packconnections.h"

using namespace CoreIR;
using namespace std;

std::vector<std::string> splitString(const std::string& str,
                                     const std::string& delimiter) {
    std::vector<std::string> strings;

    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    while ((pos = str.find(delimiter, prev)) != std::string::npos)
    {
        strings.push_back(str.substr(prev, pos - prev));
        prev = pos + 1;
    }

    // To get the last substring (or only, if delimiter is not found)
    strings.push_back(str.substr(prev));

    return strings;
}

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


  cout << "# of instances in flattened circuit = " << topMod->getDef()->getInstances().size() << endl;

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

  deleteDeadInstances(topMod_conf);

  cout << "# of instances in subcircuit after deleting dead instances = " << topMod_conf->getDef()->getInstances().size() << endl;
  
  c->runPasses({"removeconstduplicates", "cullgraph"});

  cout << "# of instances in subcircuit after deleting duplicate constants = " << topMod_conf->getDef()->getInstances().size() << endl;
  
  cout << "Saving the config circuit" << endl;
  if (!saveToFile(c->getGlobal(), "topModConfig.json", topMod_conf)) {
    cout << "Could not save to json!!" << endl;
    c->die();
  }

  cout << "Clockifying the configuration circuit interface" << endl;

  c->runPasses({"clockifyinterface"});

  cout << "Loading configuration state" << endl;

  std::ifstream configFile("./bitstream/shell_bitstream.bs");
  std::string str((std::istreambuf_iterator<char>(configFile)),
                  std::istreambuf_iterator<char>());

  cout << "Config file text" << endl;
  cout << str << endl;

  auto configLines = splitString(str, "\n");
  cout << "Config lines" << endl;
  for (auto line : configLines) {
    cout << "\t" << line << endl;
  }

  vector<BitVector> configDatas;
  vector<BitVector> configAddrs;

  for (auto line : configLines) {
    auto entries = splitString(line, " ");

    assert(entries.size() == 2);

    string addrString = entries[0];
    string dataString = entries[1];

    cout << "addr string = " << addrString << endl;
    cout << "data string = " << dataString << endl;

    assert(addrString.size() == 8);
    assert(dataString.size() == 8);

    // Convert strings to bit vectors
    BitVector configAddr(32, 0);
    BitVector configData(32, 0);

    configAddrs.push_back(configAddr);
    configDatas.push_back(configData);
  }

  assert(configAddrs.size() == configLines.size());
  assert(configDatas.size() == configLines.size());

  cout << "Building simulator state for config" << endl;

  SimulatorState state(topMod_conf);
  state.setClock("self.clk", 0, 1);
  state.setValue("self.reset", BitVec(1, 0));

  cout << " Done building simulator state" << endl;

  // TODO: Split config into lines

  // TODO: Load simulate config line by line

  // TODO: Partially evaluate the original top module by config

  // TODO: Simulate the partially evaluated circuit
  deleteContext(c);
}
