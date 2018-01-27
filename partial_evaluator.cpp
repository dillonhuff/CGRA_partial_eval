#define CATCH_CONFIG_MAIN

#include "catch.hpp"

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

  c->runPasses({"cullgraph"});

  if (!saveToFile(c->getGlobal(), "preEvalTopMod.json", wholeTopMod)) {
    cout << "Could not save to json!!" << endl;
    c->die();
  }
  
  auto regMap = state.getCircStates().back().registers;
  cout << "Converting " << regMap.size() << " registers to constants" << endl;

  registersToConstants(wholeTopMod, state.getCircStates().back().registers);

  cout << "Deleting dead instances" << endl;
  deleteDeadInstances(wholeTopMod);

  cout << "# of instances partially evaluated top after deleting dead instances = " << wholeTopMod->getDef()->getInstances().size() << endl;

  c->runPasses({"cullgraph"});

  if (!saveToFile(c->getGlobal(), "deletedDeadInstances.json", wholeTopMod)) {
    cout << "Could not save to json!!" << endl;
    c->die();
  }

  c->runPasses({"cullgraph", "verifyconnectivity"});

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

// int main() {

//   string modName = "top"; //"pe_tile_new_unq1";
//   // NOTE: Must change every time yosys is run!
//   string fileName = "top.json"; //"__DOLLAR__paramod__BACKSLASH__test_lut__BACKSLASH__DataWidth__EQUALS__1.json";

//   //processTop("pe_tile_new_unq1.json", "pe_tile_new_unq1");
//   simulateConfig("topModConfig.json", "pe_tile_new_unq1.json", "pe_tile_new_unq1");

//   //simulateConfig("topModConfig.json", "top.json");
//   //simulateConfiguredState("partialEvalTopMod.json");
// }

void partiallyEvaluateCircuit(CoreIR::Module* const wholeTopMod,
                              std::unordered_map<std::string, BitVec>& regMap) {
  cout << "Converting " << regMap.size() << " registers to constants" << endl;

  registersToConstants(wholeTopMod, regMap);

  cout << "Deleting dead instances" << endl;
  deleteDeadInstances(wholeTopMod);

  cout << "# of instances partially evaluated top after deleting dead instances = " << wholeTopMod->getDef()->getInstances().size() << endl;

  cout << "Folding constants to finish partial evaluation" << endl;
  foldConstants(wholeTopMod);

  cout << "Done folding constants" << endl;

  deleteDeadInstances(wholeTopMod);

  cout << "# of instances partially evaluated top after constant folding = " << wholeTopMod->getDef()->getInstances().size() << endl;

}

TEST_CASE("Partially evaluating") {

  Context* c = newContext();

  CoreIRLoadLibrary_rtlil(c);

  SECTION("partially evaluating switch box") {

    Module* topMod = nullptr;

    if (!loadFromFile(c, "pe_tile_new_unq1.json", &topMod)) {
      cout << "Could not Load from json!!" << endl;
      c->die();
    }

    topMod = c->getGlobal()->getModule("sb_unq1");

    assert(topMod != nullptr);
    assert(topMod->hasDef());

    auto def = topMod->getDef();
  
    c->setTop(topMod);

    if (!saveToFile(c->getGlobal(), "sb_unq1_proper_top.json", topMod)) {
      cout << "Could not save to json!!" << endl;
      c->die();
    }

    c->runPasses({"rungenerators",
          "flatten",
          "cullzexts",
          "removeconstduplicates",
          "packconnections",
          "clockifyinterface"});

    foldConstants(topMod);

    if (!saveToFile(c->getGlobal(), "sb_unq1_flat_proc.json", topMod)) {
      cout << "Could not save to json!!" << endl;
      c->die();
    }

    vector<Wireable*> subCircuitPorts{def->sel("self")->sel("config_addr"),
        def->sel("self")->sel("config_data"),
        def->sel("self")->sel("clk"),
        def->sel("self")->sel("reset"),
        def->sel("self")->sel("config_en")};
  
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

    assert(topMod_conf != nullptr);
    assert(topMod_conf->hasDef());

    deleteDeadInstances(topMod_conf);

    cout << "# of instances in subcircuit after deleting dead instances = " << topMod_conf->getDef()->getInstances().size() << endl;

    c->setTop(topMod_conf);
    c->runPasses({"removeconstduplicates"}); //, "cullgraph"});

    cout << "# of instances in subcircuit after deleting duplicate constants = " << topMod_conf->getDef()->getInstances().size() << endl;
  
    cout << "Saving the config circuit" << endl;
    if (!saveToFile(c->getGlobal(), "topModConfig.json", topMod_conf)) {
      cout << "Could not save to json!!" << endl;
      c->die();
    }
    
    SimulatorState topState(topMod_conf);

    // cout << "topState has main clock? " << topState.hasMainClock() << endl;
    topState.setClock("self.clk", 0, 1);
    topState.setValue("self.reset", BitVec(1, 0));
    topState.setValue("self.config_en", BitVec(1, 1));
    
    BitStreamConfig bs =
      loadConfig("./bitstream/sb_1_bitstream.bs");

    cout << "Configuring pe tile" << endl;
    for (uint i = 0; i < bs.configAddrs.size(); i++) {

      cout << "Simulating config " << i << endl;

      topState.setValue("self.config_addr", bs.configAddrs[i]);
      topState.setValue("self.config_data", bs.configDatas[i]);

      topState.execute();
      topState.execute();
    
    }

    Module* wholeTopMod = nullptr;
    wholeTopMod = c->getGlobal()->getModule("sb_unq1");

    assert(wholeTopMod != nullptr);

    c->setTop(wholeTopMod);

    cout << "Done with configuration state" << endl;

    auto regMap = topState.getCircStates().back().registers;

    
    partiallyEvaluateCircuit(wholeTopMod, regMap);

    c->runPasses({"cullgraph"});
    c->runPasses({"packconnections"});

    c->setTop(wholeTopMod);

    if (!saveToFile(c->getGlobal(), "sb_unq1_partially_evaluated.json", wholeTopMod)) {
      cout << "Could not save to json!!" << endl;
      c->die();
    }

    // TODO: Add test of outputs, also what does 

    deleteContext(c);

    assert(false);

  }

  SECTION("full pe tile including connect and switch box") {
    Module* topMod = nullptr;

    if (!loadFromFile(c, "pe_tile_new_unq1.json", &topMod)) {
      cout << "Could not Load from json!!" << endl;
      c->die();
    }

    topMod = c->getGlobal()->getModule("pe_tile_new_unq1");

    assert(topMod != nullptr);
    assert(topMod->hasDef());

    auto def = topMod->getDef();
  
    c->setTop(topMod);

    if (!saveToFile(c->getGlobal(), "pe_tile_new_unq1_proper_top.json", topMod)) {
      cout << "Could not save to json!!" << endl;
      c->die();
    }

    c->runPasses({"rungenerators",
          "flatten",
          "cullzexts",
          "removeconstduplicates",
          "packconnections",
          //          "cullgraph",
          "clockifyinterface"});

    foldConstants(topMod);

    if (!saveToFile(c->getGlobal(), "pe_tile_new_unq1_flat_proc.json", topMod)) {
      cout << "Could not save to json!!" << endl;
      c->die();
    }

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

    assert(topMod_conf != nullptr);
    assert(topMod_conf->hasDef());

    deleteDeadInstances(topMod_conf);

    cout << "# of instances in subcircuit after deleting dead instances = " << topMod_conf->getDef()->getInstances().size() << endl;

    c->setTop(topMod_conf);
    c->runPasses({"removeconstduplicates"}); //, "cullgraph"});

    cout << "# of instances in subcircuit after deleting duplicate constants = " << topMod_conf->getDef()->getInstances().size() << endl;
  
    cout << "Saving the config circuit" << endl;
    if (!saveToFile(c->getGlobal(), "topModConfig.json", topMod_conf)) {
      cout << "Could not save to json!!" << endl;
      c->die();
    }
    
    SimulatorState topState(topMod_conf);

    // cout << "topState has main clock? " << topState.hasMainClock() << endl;
    topState.setClock("self.clk", 0, 1);
    topState.setValue("self.reset", BitVec(1, 0));
    topState.setValue("self.tile_id", BitVec(16, 1));
    
    BitStreamConfig bs =
      loadConfig("./bitstream/shell_bitstream.bs");

    cout << "Configuring pe tile" << endl;
    for (uint i = 0; i < bs.configAddrs.size(); i++) {

      cout << "Simulating config " << i << endl;

      topState.setValue("self.config_addr", bs.configAddrs[i]);
      topState.setValue("self.config_data", bs.configDatas[i]);

      topState.execute();
      topState.execute();
    
    }

    Module* wholeTopMod = nullptr;
    wholeTopMod = c->getGlobal()->getModule("pe_tile_new_unq1");

    assert(wholeTopMod != nullptr);

    c->setTop(wholeTopMod);

    cout << "Done with configuration state" << endl;

    auto regMap = topState.getCircStates().back().registers;

    
    partiallyEvaluateCircuit(wholeTopMod, regMap);

    c->runPasses({"cullgraph"});
    c->runPasses({"packconnections"});

    c->setTop(wholeTopMod);

    if (!saveToFile(c->getGlobal(), "pe_tile_new_partially_evaluated.json", wholeTopMod)) {
      cout << "Could not save to json!!" << endl;
      c->die();
    }

    // TODO: Add test of outputs, also what does 

    deleteContext(c);

    assert(false);
  }

  SECTION("test_pe") {
    Module* topMod = nullptr;

    if (!loadFromFile(c, "test_pe_unq1.json", &topMod)) {
      cout << "Could not Load from json!!" << endl;
      c->die();
    }

    topMod = c->getGlobal()->getModule("test_pe_unq1");

    assert(topMod != nullptr);
    assert(topMod->hasDef());

    auto def = topMod->getDef();
  
    c->setTop(topMod);

    if (!saveToFile(c->getGlobal(), "test_pe_unq1_proper_top.json", topMod)) {
      cout << "Could not save to json!!" << endl;
      c->die();
    }

    c->runPasses({"rungenerators",
          "flatten",
          "cullzexts",
          "removeconstduplicates",
          "packconnections",
          "cullgraph",
          "clockifyinterface"});

    foldConstants(topMod);

    if (!saveToFile(c->getGlobal(), "test_pe_unq1_flat_proc.json", topMod)) {
      cout << "Could not save to json!!" << endl;
      c->die();
    }
    
    SimulatorState topState(topMod);

    cout << "topState has main clock? " << topState.hasMainClock() << endl;
    topState.setClock("self.clk", 0, 1);
    topState.setValue("self.rst_n", BitVec(1, 0));
    topState.setValue("self.clk_en", BitVec(1, 1));
    topState.setValue("self.cfg_en", BitVec(1, 0));

    topState.setValue("self.data0", BitVec(16, 0));
    topState.setValue("self.data1", BitVec(16, 0));

    topState.setValue("self.bit0", BitVec(1, 0));
    topState.setValue("self.bit1", BitVec(1, 0));
    topState.setValue("self.bit2", BitVec(1, 0));

    // F1000001 00000002
    // FF000001 0002000B
    // 00020001 00000000
    // 00070001 00000C00
    
    BitStreamConfig bs =
      loadConfig("./bitstream/shell_bitstream.bs");

    BitVec tileId(16, 1);
    
    cout << "Configuring pe tile" << endl;
    for (uint i = 0; i < bs.configAddrs.size(); i++) {

      
      cout << "Simulating config " << i << endl;


      BitVec cfg_id(16, 0);
      for (int j = 0; j < 16; j++) {
        cfg_id.set(j, bs.configAddrs[i].get(j));
      }

      bool configTile = true;
      for (int j = 16; j < 24; j++) {
        if (bs.configAddrs[i].get(j) == 1) {
          configTile = false;
        }
      }

      if (configTile && (cfg_id == tileId)) {
        cout << "Configuring tile, addr = " << bs.configAddrs[i] << ", data = " <<
          bs.configDatas[i] << endl;

        topState.setValue("self.cfg_en", BitVec(1, 1));
      } else {
        topState.setValue("self.cfg_en", BitVec(1, 0));
      }
      
      BitVec cfg_a(8, 0);
      for (int j = 0; j < 8; j++) {
        cfg_a.set(j, bs.configAddrs[i].get(j + 24));
      }
      //topState.setValue("self.cfg_a", bs.configAddrs[i]);
      topState.setValue("self.cfg_a", cfg_a);
      topState.setValue("self.cfg_d", bs.configDatas[i]);

      topState.execute();
      topState.execute();
    
    }

    cout << "Done with configuration state" << endl;

    topState.setValue("self.cfg_en", BitVec(1, 0));
    topState.setValue("self.data0", BitVec(16, 3));
    topState.setValue("self.data1", BitVec(16, 15));

    topState.execute();

    REQUIRE(topState.getBitVec("self.res") == BitVec(16, 3*2));

    topState.setValue("self.cfg_en", BitVec(1, 0));
    topState.setValue("self.data0", BitVec(16, 72));
    topState.setValue("self.data1", BitVec(16, 13));

    topState.execute();

    REQUIRE(topState.getBitVec("self.res") == BitVec(16, 72*2));
    
    deleteContext(c);
    
  }

  SECTION("test_pe_comp_unq1") {
    Module* topMod = nullptr;

    if (!loadFromFile(c, "test_pe_comp_unq1.json", &topMod)) {
      cout << "Could not Load from json!!" << endl;
      c->die();
    }

    topMod = c->getGlobal()->getModule("test_pe_comp_unq1");

    assert(topMod != nullptr);
  
    c->setTop(topMod);

    c->runPasses({"rungenerators",
          "flatten",
          "cullzexts",
          "removeconstduplicates",
          "packconnections",
          "cullgraph",
          "clockifyinterface"});

    // F1000001 00000002
    // FF000001 0002000B
    // 00020001 00000000
    // 00070001 00000C00

    SimulatorState state(topMod);
    state.setValue("self.op_a", BitVec(16, 7));
    state.setValue("self.op_b", BitVec(16, 2));
    state.setValue("self.op_code", BitVec(9, 11));
    state.setValue("self.op_d_p", BitVec(1, 0));

    state.execute();

    REQUIRE(state.getBitVec("self.res") == BitVec(16, 7*2));

    state.setValue("self.op_a", BitVec(16, 7));
    state.setValue("self.op_b", BitVec(16, 2));
    state.setValue("self.op_code", BitVec(9, 0));
    state.setValue("self.op_d_p", BitVec(1, 0));

    state.execute();

    REQUIRE(state.getBitVec("self.res") == BitVec(16, 7 + 2));
  
    deleteContext(c);
  }
}
