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
      regMapAll.insert({name, BitVec(len, strs[2])});
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

std::set<Wireable*> dataSources(Instance* inst) {
  string op = getQualifiedOpName(*inst);

  vector<string> binops{"coreir.and", "corebit.and", "coreir.mux", "coreir.or", "coreir.eq", "coreir.lshr", "coreir.neq", "coreir.ult", "coreir.add", "coreir.sub", "coreir.mul", "coreir.xor", "coreir.ashr"};
  vector<string> unops{"coreir.not", "corebit.not", "coreir.orr", "coreir.zext", "coreir.reg_arst", "coreir.reg", "coreir.slice", "coreir.andr", "coreir.xorr"};

  if (elem(op, binops)) {
    set<Wireable*> srcs;
    auto in0Sels = getSourceSelects(inst->sel("in0"));
    for (auto s : in0Sels) {
      srcs.insert(s->getTopParent());
    }
    auto in1Sels = getSourceSelects(inst->sel("in1"));
    for (auto s : in1Sels) {
      srcs.insert(s->getTopParent());
    }

    return srcs;
  } else if (elem(op, unops)) {

    set<Wireable*> srcs;
    auto in0Sels = getSourceSelects(inst->sel("in"));
    for (auto s : in0Sels) {
      srcs.insert(s->getTopParent());
    }

    return srcs;
  } else if ((op == "coreir.wrap") || (op == "coreir.const") || (op == "corebit.const")) {
    return {};
  } else {
    cout << "Unsupported node in data source: " << op << endl;
    assert(false);
  }
  return {};
}

void cullInputSources(const std::vector<std::string>& ports,
                      CoreIR::Module* const mod) {

  assert(mod->hasDef());
  auto def = mod->getDef();

  // How about this:
  // 1. Set of all instances that receive at least one input from a constant
  // 2. Set of all instances that receive at least one input from datapath port?

  map<Wireable*, set<Wireable*> > finalDataSources;
  map<Wireable*, set<Wireable*> > tentativeDataSources;
  for (auto instR : def->getInstances()) {
    tentativeDataSources.insert({instR.second, dataSources(instR.second)});
    finalDataSources.insert({instR.second, {}});
  }

  while (tentativeDataSources.size() > 0) {
    auto& src = *begin(tentativeDataSources);

    bool allSrcAreFinal;
    for (auto s : src.second) {
      // s is a final data source
      if (!isa<Instance>(s) || dataSources(cast<Instance>(s)).size() == 0) {
        //cout << "Erasing " << s->toString() << endl;

        finalDataSources.at(src.first).insert(s);
        src.second.erase(s);
        break;

      } else {

        for (auto sSrc : dataSources(cast<Instance>(s))) {

          auto parent = sSrc->getTopParent();

          // Loops are not allowed
          if (parent != src.first) {
            tentativeDataSources.at(src.first).insert(parent);
            cout << "Adding " << parent->toString() << " as source" << endl;
          }
        }

        src.second.erase(s);
        break;

      }
    }

    if (src.second.size() == 0) {
      cout << "Finalizing " << src.first->toString() << " data sources" << endl;
      tentativeDataSources.erase(src.first);
    }
  }

  set<Select*> dataPorts;
  for (auto port : ports) {
    auto s = def->sel("self")->sel(port);
    dataPorts.insert(s);
  }

  assert(dataPorts.size() > 0);

  for (auto fd : finalDataSources) {
    bool allFromData = true;
    for (auto src : fd.second) {

      if (!isa<Select>(src) || !elem(cast<Select>(src), dataPorts)) {
        allFromData = false;
        break;
      }
    }

    if (allFromData) {
      cout << "All inputs to " << fd.first->toString() << " are from the data path" << endl;
    }
  }

  assert(false);

  set<Instance*> insts;
  set<Select*> dataSelects;

  // All constants are dead outputs
  for (auto instR : def->getInstances()) {
    if ((getQualifiedOpName(*(instR.second)) == "coreir.const") ||
        (getQualifiedOpName(*(instR.second)) == "corebit.const")) {
      dataSelects.insert(instR.second->sel("out"));
    }
  }
  cout << "Culling module" << endl;
  //mod->print();
  
  for (auto port : ports) {
    dataSelects.insert(cast<Select>(def->sel("self")->sel(port)));
  }

  // Now: Cull instances that receive data inputs from dataSelects and their
  // ancestors

  cout << "# of instances before port culling = " << def->getInstances().size() << endl;

  bool removedInstance = true;
  while (removedInstance) {
    removedInstance = false;

    for (auto instR : def->getInstances()) {
      auto inst = instR.second;

      string op = getQualifiedOpName(*inst);
      if ((op == "coreir.mux") ||
          (op == "coreir.and") || (op == "corebit.and") ||
          (op == "coreir.or") || (op == "corebit.or") ||
          (op == "coreir.eq") || (op == "coreir.add") ||
          (op == "coreir.xor") || (op == "corebit.xor")) {
        if (allDriversFrom(inst->sel("in0"), dataSelects) &&
            allDriversFrom(inst->sel("in1"), dataSelects)) {
          removedInstance = true;

          dataSelects.insert(inst->sel("out"));

          cout << "Removing " << inst->toString() << endl;

          def->removeInstance(inst);
          break;
        }

      } else if ((op == "coreir.orr") || (op == "coreir.zext") ||
                 (op == "coreir.not") || (op == "corebit.not") ||
                 (op == "coreir.slice")) {

        if (allDriversFrom(inst->sel("in"), dataSelects)) {
          removedInstance = true;

          dataSelects.insert(inst->sel("out"));

          cout << "Removing " << inst->toString() << endl;

          def->removeInstance(inst);
          break;
        }
        
      } else if ((op == "coreir.reg_arst")) {
        // For now ignore whether the reset port is called
        // if (allDriversFrom(inst->sel("in"), dataSelects)) {
        //   removedInstance = true;

        //   dataSelects.insert(inst->sel("out"));

        //   //cout << "Removing " << inst->toString() << endl;

        //   //def->removeInstance(inst);
        //   break;
        // }
      } else {
        //cout << "Unsupported type in culling: " << inst->toString() << " : " << op << endl;
        //assert(false);
      }
    }
  }

  cout << "# of instances after port culling = " << def->getInstances().size() << endl;
  //assert(false);
}

int main() {

  // Load pe tile verilog
  Context* c = newContext();

  CoreIRLoadLibrary_rtlil(c);

  Module* topMod = loadModule(c, "pe_hwmaster_03_20_2018.json", "pe_tile_new_unq1");
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
  
  // TODO: Add datapath culling
  vector<string> dataOnlyPorts;
  dataOnlyPorts.push_back("reset");
  for (int s = 0; s < 4; s++) {
    for (int t = 0; t < 5; t++) {
      dataOnlyPorts.push_back("in_BUS16_S" + to_string(s) + "_T" + to_string(t));
      dataOnlyPorts.push_back("in_BUS1_S" + to_string(s) + "_T" + to_string(t));
    }
  }
  cullInputSources(dataOnlyPorts, topMod_conf);
  c->runPasses({"add-dummy-inputs"});

  // Write this out as verilog
  if (!saveToFile(c->getGlobal(), "topMod_config.json", topMod_conf)) {
    cout << "Could not save to json!!" << endl;
    c->die();
  }

  cout << "Saving to verilog" << endl;
  int verilog_convert = system("coreir -i topMod_config.json -o topMod_config.v > verilog_conversion_log.txt");

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
        (getQualifiedOpName(*inst) == "coreir.reg_arst") ||
        (getQualifiedOpName(*inst) == "corebit.dff")) {
      uint width = inst->getModuleRef()->getGenArgs().at("width")->get<int>();

      outFile << "  outstream << \"" << inst->toString() << " " << width << "\" << \" \" << bitset<" + to_string(width) + ">(top->" << inst->toString() << "_subcircuit_out) << endl;\n" << endl;
    }
  }

  outFile << "  outstream.close();\n" << endl;

  outFile << "\n}\n" << endl;

  outFile.close();

  int res = system("make verilog");

  assert(res == 0);

  // Read the register values back in
  map<string, BitVec> fixedPorts(
                                 {
                                   {"tile_id", BitVec(16, 1)},
                                     {"config_addr", BitVec(32, 0)},
                                       {"config_data", BitVec(32, 0)},
                                         {"reset", BitVec(1, 0)}
                                 }
                                 );
  
  loadSpecializedState(topMod, fixedPorts, "./config_register_values.txt");

  cout << "# of instances in top before after folding constants = " << topMod->getDef()->getInstances().size() << endl;  

  c->runPasses({"fold-constants", "packconnections"});
  c->runPasses({"deletedeadinstances"});

  cout << "# of instances in top after folding constants = " << topMod->getDef()->getInstances().size() << endl;

  // This should be a verilog testbench
  if (!saveToFile(c->getGlobal(), "mul_2_pe.json", topMod)) {
    cout << "Could not save to json!!" << endl;
    c->die();
  }

  int make_verilog_res = system("coreir -i mul_2_pe.json -o mul_2_pe.v");

  assert(make_verilog_res == 0);

  int verilator_res = system("make verilog_res");

  assert(verilator_res == 0);

}
