#!/bin/bash

#rm /tmp/{instance,scenario,parcelp,nveh}

grep 'Solving' testserverunsolv2.txt | cut -d " " -f 3 > /tmp/instance
grep 'Scenario' testserverunsolv2.txt | cut -d " " -f 3 > /tmp/scenario
grep 'ParcelP' testserverunsolv2.txt | cut -d " " -f 3 > /tmp/parcelp
grep 'Number of Vehicles' testserverunsolv2.txt | cut -d ":" -f 2 > /tmp/nveh

#grep 'Average Resets RVND' NewTest1.txt | cut -d " " -f 4 > /tmp/rvnd

paste /tmp/instance /tmp/scenario /tmp/parcelp /tmp/nveh > Vehiclestestserverunsolv2.txt
#paste /tmp/instance /tmp/nveh > Vehiclestestserverunsolv1.txt
# paste /tmp/instance /tmp/sstaus > Solstatustestserverunsolv1.txt
