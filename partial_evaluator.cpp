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

std::vector<char> hexToBytes(const std::string& hex) {
  std::vector<char> bytes;

  for (unsigned int i = 0; i < hex.length(); i += 2) {
    std::string byteString = hex.substr(i, 2);
    char byte = (char) strtol(byteString.c_str(), NULL, 16);
    bytes.push_back(byte);
  }

  return bytes;
}

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

void processTop() {
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

BitVector hexStringToBitVector(const std::string& str) {
  vector<char> addrBytes = hexToBytes(str);

  assert(addrBytes.size() == 4);

  reverse(addrBytes);

  BitVector configAddr(32, 0);

  int offset = 0;
  for (auto byte : addrBytes) {
    BitVec tmp(8, byte);
    for (uint i = 0; i < (uint) tmp.bitLength(); i++) {
      configAddr.set(offset, tmp.get(i));
      offset++;
    }
  }

  assert(offset == 32);

  return configAddr;
}

int main() {
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

    cout << "# of entries == " << entries.size() << endl;
    cout << "Line = " << line << endl;

    assert(entries.size() == 2);

    string addrString = entries[0];
    string dataString = entries[1];

    cout << "addr string = " << addrString << endl;
    cout << "data string = " << dataString << endl;

    assert(addrString.size() == 8);
    assert(dataString.size() == 8);

    // Convert strings to bit vectors
    vector<char> addrBytes = hexToBytes(addrString);
    assert(addrBytes.size() == 4);

    BitVector configAddr = hexStringToBitVector(addrString);

    // int offset = 0;
    // for (auto byte : addrBytes) {
    //   BitVec tmp(8, byte);
    //   for (uint i = 0; i < (uint) tmp.bitLength(); i++) {
    //     configAddr.set(offset, tmp.get(i));
    //     offset++;
    //   }
    // }

    // assert(offset == 32);

    //vector<char> dataBytes = hexToBytes(dataString);
    BitVector configData = hexStringToBitVector(dataString);

    cout << "configAddr = " << configAddr << endl;
    cout << "configData = " << configData << endl;

    configAddrs.push_back(configAddr);
    configDatas.push_back(configData);
  }

  assert(configAddrs.size() == configLines.size());
  assert(configDatas.size() == configLines.size());

}
