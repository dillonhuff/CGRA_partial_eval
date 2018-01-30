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

TEST_CASE("Simplified switch box") {
  Context* c = newContext();

  CoreIRLoadLibrary_rtlil(c);

  SECTION("Folding a switch constant") {
    Module* topMod = loadModule(c, "switch_fold.json", "switch_fold"); //nullptr;

    assert(topMod->hasDef());

    auto def = topMod->getDef();

    c->setTop(topMod);

    c->runPasses({"rungenerators",
          "flatten"});

    SECTION("Evaluating before any optimizations") {
      SimulatorState originalState(topMod);
      originalState.setValue("self.in_0_0", BitVec(2, 1));
      originalState.setValue("self.in_2_0", BitVec(2, 2));
      originalState.setValue("self.in_3_0", BitVec(2, 3));
      originalState.setValue("self.pe_output_0", BitVec(2, 23));

      originalState.execute();

      REQUIRE(originalState.getBitVec("self.out_1_0") == BitVec(2, 23));
    }
    
    c->runPasses({
          "cullzexts",
          "removeconstduplicates",
          "packconnections",
          "clockifyinterface"});

    SECTION("Evaluating the original state") {
      SimulatorState originalState(topMod);
      originalState.setValue("self.in_0_0", BitVec(2, 1));
      originalState.setValue("self.in_2_0", BitVec(2, 2));
      originalState.setValue("self.in_3_0", BitVec(2, 3));
      originalState.setValue("self.pe_output_0", BitVec(2, 23));

      originalState.execute();

      REQUIRE(originalState.getBitVec("self.out_1_0") == BitVec(2, 23));
    }

    foldConstants(topMod);
    deleteDeadInstances(topMod);

    if (!saveToFile(c->getGlobal(), "switch_constant_folded.json", topMod)) {
      cout << "Could not save to json!!" << endl;
      c->die();
    }

    SECTION("Evaluating the system after partial evaluation") {
      SimulatorState evalState(topMod);
      evalState.setValue("self.in_0_0", BitVec(2, 1));
      evalState.setValue("self.in_2_0", BitVec(2, 2));
      evalState.setValue("self.in_3_0", BitVec(2, 3));
      evalState.setValue("self.pe_output_0", BitVec(2, 23));

      evalState.execute();

      REQUIRE(evalState.getBitVec("self.out_1_0") == BitVec(2, 23));
    }

  }

  deleteContext(c);
}

TEST_CASE("Registerizing switch box normal simulation") {
  Context* c = newContext();

  CoreIRLoadLibrary_rtlil(c);

  //  SECTION("Loading registerized switch box") {

  Module* topMod = loadModule(c, "registered_switch.json", "registered_switch");

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

  SECTION("Simulating before partial evaluation") {
    SimulatorState topState(topMod);

    // cout << "topState has main clock? " << topState.hasMainClock() << endl;
    topState.setClock("self.clk", 0, 1);
    topState.setValue("self.config_en", BitVec(1, 1));

    topState.setValue("self.in_0_0", BitVec(16, 0));
    topState.setValue("self.in_2_0", BitVec(16, 2));
    topState.setValue("self.in_3_0", BitVec(16, 3));
    topState.setValue("self.pe_output_0", BitVec(16, 34));
    
    BitStreamConfig bs =
      loadConfig("./bitstream/sb_1_bitstream.bs");

    cout << "Configuring pe tile" << endl;
    for (uint i = 0; i < bs.configAddrs.size(); i++) {

      cout << "Simulating config " << i << endl;
      cout << "config addr = " << bs.configAddrs[i] << endl;
      cout << "config data = " << bs.configDatas[i] << endl;

      topState.setValue("self.config_addr", bs.configAddrs[i]);
      topState.setValue("self.config_data", bs.configDatas[i]);

      topState.execute();
    }

    topState.setValue("self.config_en", BitVec(1, 0));
    topState.setValue("self.pe_output_0", BitVec(16, 34));

    topState.execute();

    REQUIRE(topState.getBitVec("self.out_1_0") == BitVec(16, 34));

    topState.setValue("self.pe_output_0", BitVec(16, 2));

    topState.execute();

    REQUIRE(topState.getBitVec("self.out_1_0") == BitVec(16, 2));
  }

  deleteContext(c);
}

TEST_CASE("Registerizing switch box partial evaluation") {

  Context* c = newContext();

  CoreIRLoadLibrary_rtlil(c);

  Module* topMod = loadModule(c, "registered_switch.json", "registered_switch");

  assert(topMod->hasDef());

  auto def = topMod->getDef();

  c->setTop(topMod);

  c->runPasses({"rungenerators",
        "flatten",
        "cullzexts",
        "removeconstduplicates",
        "packconnections",
        "clockifyinterface"});

  foldConstants(topMod);

  if (!saveToFile(c->getGlobal(), "registered_switch_proc_flat.json", topMod)) {
    cout << "Could not save to json!!" << endl;
    c->die();
  }

  // Insert partial eval code
  vector<Wireable*> subCircuitPorts{def->sel("self")->sel("config_addr"),
      def->sel("self")->sel("config_data"),
      def->sel("self")->sel("clk"),
      def->sel("self")->sel("config_en")};
  
  auto subCircuitInstances =
    extractSubcircuit(topMod, subCircuitPorts);

  cout << "# of instances in subciruit = " << subCircuitInstances.size() << endl;

  Module* topMod_conf =
    createSubCircuit(topMod,
                     subCircuitPorts,
                     subCircuitInstances,
                     c);
  
  SimulatorState topState(topMod_conf);

  // cout << "topState has main clock? " << topState.hasMainClock() << endl;
  topState.setClock("self.clk", 0, 1);
  topState.setValue("self.config_en", BitVec(1, 1));
    
  BitStreamConfig bs =
    loadConfig("./bitstream/sb_1_bitstream.bs");

  cout << "Configuring pe tile" << endl;
  for (uint i = 0; i < bs.configAddrs.size(); i++) {

    cout << "Simulating config " << i << endl;
    cout << "config addr = " << bs.configAddrs[i] << endl;
    cout << "config data = " << bs.configDatas[i] << endl;

    topState.setValue("self.config_addr", bs.configAddrs[i]);
    topState.setValue("self.config_data", bs.configDatas[i]);

    topState.execute();
  }

  Module* wholeTopMod = nullptr;
  wholeTopMod = c->getGlobal()->getModule("registered_switch");
  assert(wholeTopMod != nullptr);
  c->setTop(wholeTopMod);

  auto regMap = topState.getCircStates().back().registers;
  cout << "Partially evaluating the switch box" << endl;
  partiallyEvaluateCircuit(wholeTopMod, regMap);

  cout << "Saving the partially evaluated circuit" << endl;
  cout << "# of instances in partially evaluated circuit = " << wholeTopMod->getDef()->getInstances().size() << endl;
  if (!saveToFile(c->getGlobal(),
                  "registered_switch_partially_evaluated.json",
                  wholeTopMod)) {
    cout << "Could not save to json!!" << endl;
    c->die();
  }

  SimulatorState state(wholeTopMod);

  state.setClock("self.clk", 0, 1);
  state.setValue("self.config_en", BitVec(1, 0));

  state.setValue("self.in_0_0", BitVec(16, 1));
  state.setValue("self.in_2_0", BitVec(16, 2));
  state.setValue("self.in_3_0", BitVec(16, 3));
  state.setValue("self.pe_output_0", BitVec(16, 34));
    
  state.setValue("self.config_en", BitVec(1, 0));
  state.setValue("self.pe_output_0", BitVec(16, 34));

  state.execute();

  REQUIRE(state.getBitVec("self.out_1_0") == BitVec(16, 34));

  state.setValue("self.pe_output_0", BitVec(16, 2));

  state.execute();

  REQUIRE(state.getBitVec("self.out_1_0") == BitVec(16, 2));

  deleteContext(c);
}

TEST_CASE("Partially evaluating the entire PE") {

  Context* c = newContext();

  CoreIRLoadLibrary_rtlil(c);

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

  SimulatorState state(wholeTopMod);

  cout << "Setting values" << endl;
  state.setValue("self.reset", BitVec(1, 0));

  state.setValue("self.config_addr", BitVec(32, 123));
  state.setValue("self.config_data", BitVec(32, 0));
  state.setClock("self.clk", 0, 1);
  state.setValue("self.tile_id", BitVec(32, 1));

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

  state.setValue("self.in_BUS16_S2_T0", BitVec(16, 34));

  cout << "Executing" << endl;

  state.execute();

  cout << "Getting state value" << endl;

  BitVec res(16, 34*2);
  for (uint i = 0; i < res.bitLength(); i++) {
    cout << "i = " << i << endl;
    REQUIRE(topState.getBitVec(wholeTopMod->getDef()->sel("self")->sel("out_BUS16_S1_T0")->sel(i)) == res.get(i));
  }
  //REQUIRE(topState.getBitVec("self.out_BUS16_S1_T0") == BitVec(16, 34*2));

  deleteContext(c);

}

TEST_CASE("Partially evaluating") {

  Context* c = newContext();

  CoreIRLoadLibrary_rtlil(c);

  SECTION("Run switch box") {

    Module* topMod = loadModule(c, "sb_unq1.json", "sb_unq1");

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
    
    SimulatorState topState(topMod);

    // cout << "topState has main clock? " << topState.hasMainClock() << endl;
    topState.setClock("self.clk", 0, 1);
    topState.setValue("self.reset", BitVec(1, 0));
    topState.setValue("self.config_en", BitVec(1, 1));

    topState.setValue("self.in_0_0", BitVec(16, 0));
    topState.setValue("self.in_2_0", BitVec(16, 2));
    topState.setValue("self.in_3_0", BitVec(16, 3));
    topState.setValue("self.pe_output_0", BitVec(16, 34));
    
    BitStreamConfig bs =
      loadConfig("./bitstream/sb_1_bitstream.bs");

    cout << "Configuring pe tile" << endl;
    for (uint i = 0; i < bs.configAddrs.size(); i++) {

      cout << "Simulating config " << i << endl;
      cout << "config addr = " << bs.configAddrs[i] << endl;
      cout << "config data = " << bs.configDatas[i] << endl;

      topState.setValue("self.config_addr", bs.configAddrs[i]);
      topState.setValue("self.config_data", bs.configDatas[i]);

      topState.execute();
    }

    topState.setValue("self.config_en", BitVec(1, 0));
    topState.setValue("self.pe_output_0", BitVec(16, 34));

    topState.execute();

    REQUIRE(topState.getBitVec("self.out_1_0") == BitVec(16, 34));

    topState.setValue("self.pe_output_0", BitVec(16, 2));

    topState.execute();

    REQUIRE(topState.getBitVec("self.out_1_0") == BitVec(16, 2));
  }

  SECTION("partially evaluating switch box") {

    Module* topMod = loadModule(c, "sb_simplified_unq1.json", "sb_unq1");
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
      loadConfig("./bitstream/sb_2_bitstream.bs");

    cout << "Configuring pe tile" << endl;
    for (uint i = 0; i < bs.configAddrs.size(); i++) {

      cout << "Simulating config " << i << endl;
      cout << "config addr = " << bs.configAddrs[i] << endl;
      cout << "config data = " << bs.configDatas[i] << endl;

      topState.setValue("self.config_addr", bs.configAddrs[i]);
      topState.setValue("self.config_data", bs.configDatas[i]);

      topState.execute();
    }

    Module* wholeTopMod = nullptr;
    wholeTopMod = c->getGlobal()->getModule("sb_unq1");

    assert(wholeTopMod != nullptr);

    c->setTop(wholeTopMod);

    cout << "Done with configuration state" << endl;

    auto regMap = topState.getCircStates().back().registers;

    cout << "Partially evaluating the switch box" << endl;
    partiallyEvaluateCircuit(wholeTopMod, regMap);

    c->runPasses({"cullgraph"});
    c->runPasses({"packconnections"});

    c->setTop(wholeTopMod);

    if (!saveToFile(c->getGlobal(), "sb_unq1_partially_evaluated.json", wholeTopMod)) {
      cout << "Could not save to json!!" << endl;
      c->die();
    }

    deleteContext(c);

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
