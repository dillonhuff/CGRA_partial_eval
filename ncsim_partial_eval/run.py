import os

#run_cmd = 'irun -status -top test -timescale 1ns/1ns -l irun.log -access +rwc -notimingchecks -input cmd.tcl ../../CGRAGenerator/hardware/generator_z/top/genesis_verif/*.v ../../CGRAGenerator/hardware/generator_z/top/genesis_verif/*.sv ./test.v'

#run_original_pe_cmd = 'irun -status -top test -timescale 1ns/1ns -l irun.log -access +rwc -notimingchecks -input cmd.tcl ./pe_hwmaster_03_20_2018.v ./test.v'

#res = os.system(run_cmd)

run_original_pe_cmd = 'irun -status -top test -timescale 1ns/1ns -l irun.log -access +rwc -notimingchecks -input cmd.tcl ./topMod_config.v ./specialize_test.v'
res = os.system(run_original_pe_cmd)

assert(res == 0)
