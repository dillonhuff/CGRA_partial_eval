#include "verilated.h"
#include "Vpe_tile_new_unq1.h"

#include <iostream>

using namespace std;

int main() {
  Vpe_tile_new_unq1* tile = new Vpe_tile_new_unq1();
  tile->clk_in = 0;
  tile->tile_id = 1;
  tile->reset = 0;
  tile->eval();


  tile->clk_in = 1;
  tile->config_addr = 0;
  tile->config_data = 0;

  tile->in_BUS16_S2_T0 = 4;

  tile->eval();

  cout << "tile->out_BUS16_S1_T0 == " << tile->out_BUS16_S1_T0 << endl;

  assert(tile->out_BUS16_S1_T0 == 4*2);

  return 0;
}
