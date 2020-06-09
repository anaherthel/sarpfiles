#!/bin/bash

#rm /tmp/{instance,scenario,parcelp,sstatus,objval,nveh}

grep 'Solving' testgh2-13-2B-0to30p.txt | cut -d " " -f 3 > /tmp/instance
grep 'Scenario' testgh2-13-2B-0to30p.txt | cut -d " " -f 3 > /tmp/scenario
grep 'ParcelP' testgh2-13-2B-0to30p.txt | cut -d " " -f 3 > /tmp/parcelp
grep 'Sol status' testgh2-13-2B-0to30p.txt | cut -d " " -f 3 > /tmp/sstatus
grep 'Obj Val' testgh2-13-2B-0to30p.txt | cut -d " " -f 3 > /tmp/objval
grep 'Number of Vehicles' testgh2-13-2B-0to30p.txt | cut -d " " -f 4 > /tmp/nveh

#grep 'Average Resets RVND' NewTest1.txt | cut -d " " -f 4 > /tmp/rvnd

paste /tmp/instance /tmp/scenario /tmp/parcelp /tmp/sstatus /tmp/objval /tmp/nveh > Statstestgh2-13-2B-0to30p.txt

# paste /tmp/instance /tmp/sstaus > Solstatustestgh2-13-2B-0to30p.txt
