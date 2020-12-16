#!/bin/bash

#rm /tmp/{instance,scenario,parcelp,objval,nveh,lline}

grep 'Solving' testserverunsolv2.txt | cut -d " " -f 3 > /tmp/instance
grep 'Scenario' testserverunsolv2.txt | cut -d " " -f 3 > /tmp/scenario
grep 'ParcelP' testserverunsolv2.txt | cut -d " " -f 3 > /tmp/parcelp
grep 'Obj Val' testserverunsolv2.txt | cut -d " " -f 3 > /tmp/objval
grep 'Number of Vehicles' testserverunsolv2.txt | cut -d " " -f 4 > /tmp/nveh
cat testserverunsolv2.txt | grep -B 2 'GUB cover cuts applied' | grep '^ [0-9]' | awk '{print $NF}'  > /tmp/lline

#grep 'Average Resets RVND' NewTest1.txt | cut -d " " -f 4 > /tmp/rvnd

paste /tmp/instance /tmp/scenario /tmp/parcelp /tmp/objval /tmp/nveh /tmp/lline > Statstestserverunsolv2.txt
#paste /tmp/lline > Statstestserverunsolv2.txt
# paste /tmp/instance /tmp/sstaus > Solstatustestserverunsolv2.txt
