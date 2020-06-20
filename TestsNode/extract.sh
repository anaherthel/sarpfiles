#!/bin/bash

#rm /tmp/{instance,scenario,parcelp,sstatus,objval,nveh}

grep 'Solving' testserver2B0-20.txt | cut -d " " -f 3 > /tmp/instance
grep 'Scenario' testserver2B0-20.txt | cut -d " " -f 3 > /tmp/scenario
grep 'ParcelP' testserver2B0-20.txt | cut -d " " -f 3 > /tmp/parcelp
grep 'Sol status' testserver2B0-20.txt | cut -d " " -f 3 > /tmp/sstatus
grep 'Obj Val' testserver2B0-20.txt | cut -d " " -f 3 > /tmp/objval
grep 'Number of Vehicles' testserver2B0-20.txt | cut -d " " -f 4 > /tmp/nveh

#grep 'Average Resets RVND' NewTest1.txt | cut -d " " -f 4 > /tmp/rvnd

paste /tmp/instance /tmp/scenario /tmp/parcelp /tmp/sstatus /tmp/objval /tmp/nveh > Statstestserver2B0-20.txt

# paste /tmp/instance /tmp/sstaus > Solstatustestserver2B0-20.txt
