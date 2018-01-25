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

BitVector hexStringToBitVector(const std::string& str) {
  vector<char> addrBytes = hexToBytes(str);

  //assert(addrBytes.size() == 4);
  int numBits = str.size() * 4;

  reverse(addrBytes);

  BitVector configAddr(numBits, 0);

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

struct BitStreamConfig {
  vector<BitVector> configAddrs;
  vector<BitVector> configDatas;
};

BitStreamConfig loadConfig(const std::string& configFileName) {
  cout << "Loading configuration state" << endl;

  //configFile("./bitstream/shell_bitstream.bs")
  std::ifstream configFile(configFileName);
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
    BitVector configData = hexStringToBitVector(dataString);

    cout << "configAddr = " << configAddr << endl;
    cout << "configData = " << configData << endl;

    configAddrs.push_back(configAddr);
    configDatas.push_back(configData);
  }

  assert(configAddrs.size() == configLines.size());
  assert(configDatas.size() == configLines.size());

  return {configAddrs, configDatas};
}

void processTop(const std::string& fileName,
                const std::string& topModName) {
  Context* c = newContext();

  CoreIRLoadLibrary_rtlil(c);

  Module* topMod = nullptr;

  if (!loadFromFile(c, fileName, &topMod)) {
    cout << "Could not Load from json!!" << endl;
    c->die();
  }

  topMod = c->getGlobal()->getModule(topModName);
  c->setTop(topMod);

  assert(topMod->hasDef());

  c->runPasses({"rungenerators",
        //"removeconstduplicates",
        "flatten",
        //"removeconstduplicates",
        "cullzexts",
        "packconnections"});

  cout << "Done with packing connections" << endl;


  cout << "# of instances in flattened circuit = " << topMod->getDef()->getInstances().size() << endl;

  ModuleDef* def = topMod->getDef();

  // TODO: Pass in port path names
  vector<Wireable*> subCircuitPorts{def->sel("self")->sel("config_addr"),
      def->sel("self")->sel("config_data"),
      def->sel("self")->sel("clk"),
      def->sel("self")->sel("reset"),
      def->sel("self")->sel("tile_id")};
  
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

  c->setTop(topMod_conf);
  c->runPasses({"removeconstduplicates", "cullgraph"});

  cout << "# of instances in subcircuit after deleting duplicate constants = " << topMod_conf->getDef()->getInstances().size() << endl;
  
  cout << "Saving the config circuit" << endl;
  if (!saveToFile(c->getGlobal(), "topModConfig.json", topMod_conf)) {
    cout << "Could not save to json!!" << endl;
    c->die();
  }

  deleteContext(c);
}

void simulateConfig(const std::string& configFileName,
                    const std::string& topFileName,
                    const std::string& topFileTopMod) {

  Context* c = newContext();

  CoreIRLoadLibrary_rtlil(c);

  Module* topMod_conf = nullptr;

  if (!loadFromFile(c, configFileName, &topMod_conf)) {
    cout << "Could not Load from json!!" << endl;
    c->die();
  }

  assert(topMod_conf->hasDef());

  c->runPasses({"clockifyinterface"});

  if (!saveToFile(c->getGlobal(), "topModConfigClock.json", topMod_conf)) {
    cout << "Could not save to json!!" << endl;
    c->die();
  }
  
  cout << "Building simulator state for config" << endl;

  SimulatorState state(topMod_conf);
  state.setClock("self.clk", 0, 1);
  state.setValue("self.reset", BitVec(1, 0));

  // Setting tile ID
  state.setValue("self.tile_id", BitVec(16, 1));

  cout << " Done building simulator state" << endl;

  BitStreamConfig bs = //loadConfig("./bitstream/shell_bitstream.bs");
    loadConfig("./bitstream/tile_zero_conf.bs");
  for (uint i = 0; i < bs.configAddrs.size(); i++) {
    cout << "Simulating config " << i << endl;
    state.setValue("self.config_addr", bs.configAddrs[i]);
    state.setValue("self.config_data", bs.configDatas[i]);

    state.execute();
    state.execute();
    
  }

  cout << "Done simulating config" << endl;

  Module* wholeTopMod = nullptr;

  if (!loadFromFile(c, topFileName, &wholeTopMod)) {
    cout << "Could not Load from json!!" << endl;
    c->die();
  }

  wholeTopMod = c->getGlobal()->getModule(topFileTopMod);

  assert(wholeTopMod != nullptr);
  
  c->setTop(wholeTopMod);

  c->runPasses({"rungenerators",
        // TODO: Maybe pack connections here to help with inlining speed?
        "flatten",
        "cullzexts",
        "removeconstduplicates",
        "packconnections"});
  //"cullgraph"});


  // if (!saveToFile(c->getGlobal(), "preEvalTopMod.json", wholeTopMod)) {
  //   cout << "Could not save to json!!" << endl;
  //   c->die();
  // }
  
  auto regMap = state.getCircStates().back().registers;
  cout << "Converting " << regMap.size() << " registers to constants" << endl;

  registersToConstants(wholeTopMod, state.getCircStates().back().registers);

  cout << "Deleting dead instances" << endl;
  deleteDeadInstances(wholeTopMod);

  cout << "# of instances partially evaluated top after deleting dead instances = " << wholeTopMod->getDef()->getInstances().size() << endl;

  if (!saveToFile(c->getGlobal(), "deletedDeadInstances.json", wholeTopMod)) {
    cout << "Could not save to json!!" << endl;
    c->die();
  }
  
  // NOTE: I would like to get rid of this line, but I'm not sure that can be done
  // safely.
  //unpackConnections(wholeTopMod);

  assert(wholeTopMod->getDef()->hasSel("test_pe$test_pe_comp$__DOLLAR__or__DOLLAR____DOT__/pe_verilog/test_pe_comp_unq1__DOT__sv__COLON__298__DOLLAR__368$op0.in0"));

  Select* sel = cast<Select>(wholeTopMod->getDef()->sel("test_pe$test_pe_comp$__DOLLAR__or__DOLLAR____DOT__/pe_verilog/test_pe_comp_unq1__DOT__sv__COLON__298__DOLLAR__368$op0.in0"));

  cout << "# of connected wireables for " << sel->toString() << endl;
  cout << sel->getConnectedWireables().size() << endl;
  for (auto wb : sel->getConnectedWireables()) {
    cout << "\t" << wb->toString() << endl;
  }

  cout << "Folding constants to finish partial evaluation" << endl;
  foldConstants(wholeTopMod);

  cout << "Done folding constants" << endl;

  deleteDeadInstances(wholeTopMod);

  cout << "# of instances partially evaluated top after constant folding = " << wholeTopMod->getDef()->getInstances().size() << endl;

  c->runPasses({"packconnections"});

  c->setTop(wholeTopMod);

  c->runPasses({"cullgraph"});
  if (!saveToFile(c->getGlobal(), "partialEvalTopMod.json", wholeTopMod)) {
    cout << "Could not save to json!!" << endl;
    c->die();
  }

  deleteContext(c);
}

void simulateConfiguredState(const std::string& fileName) {

  Context* c = newContext();

  CoreIRLoadLibrary_rtlil(c);

  Module* topMod = nullptr;

  if (!loadFromFile(c, fileName, &topMod)) {
    cout << "Could not Load from json!!" << endl;
    c->die();
  }

  // vector<Wireable*> subCircuitPorts{def->sel("self")->sel("clk"),
  //     def->sel("self")->sel("reset")};
  //     //def->sel("self")->sel("tile_id")};
  
  // auto subCircuitInstances =
  //   extractSubcircuit(topMod, subCircuitPorts);
  assert(topMod->hasDef());

  c->runPasses({"clockifyinterface"});


  deleteContext(c);
}

int main() {

  string modName = "top"; //"pe_tile_new_unq1";
  // NOTE: Must change every time yosys is run!
  string fileName = "top.json"; //"__DOLLAR__paramod__BACKSLASH__test_lut__BACKSLASH__DataWidth__EQUALS__1.json";

  //processTop("pe_tile_new_unq1.json", "pe_tile_new_unq1");
  simulateConfig("topModConfig.json", "pe_tile_new_unq1.json", "pe_tile_new_unq1");

  //simulateConfig("topModConfig.json", "top.json");
  //simulateConfiguredState("partialEvalTopMod.json");
}
