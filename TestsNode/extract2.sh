#!/bin/bash

#rm /tmp/{instance,scenario,parcelp,objval,veh}

grep 'Solving' testgh2-13-2B-0to30p.txt | cut -d " " -f 3 > /tmp/instance
grep 'Scenario' testgh2-13-2B-0to30p.txt | cut -d " " -f 3 > /tmp/scenario
grep 'ParcelP' testgh2-13-2B-0to30p.txt | cut -d " " -f 3 > /tmp/parcelp
grep 'Obj Val' testgh2-13-2B-0to30p.txt | cut -d " " -f 3 > /tmp/objval
grep 'Vehicles' testgh2-13-2B-0to30p.txt | cut -d ":" -f 2 > /tmp/nveh
grep 'Vehicle 0' testgh2-13-2B-0to30p.txt | cut -d ":" -f 2 > /tmp/veh1
grep 'Vehicle 0' testgh2-13-2B-0to30p.txt | cut -d ":" -f 2 > /tmp/vsh1

grep 'Vehicle 1' testgh2-13-2B-0to30p.txt | cut -d ":" -f 2 > /tmp/veh2
grep 'Vehicle 1' testgh2-13-2B-0to30p.txt | cut -d ":" -f 2 > /tmp/vsh2

grep 'Vehicle 2' testgh2-13-2B-0to30p.txt | cut -d ":" -f 2 > /tmp/veh3
grep 'Vehicle 2' testgh2-13-2B-0to30p.txt | cut -d ":" -f 2 > /tmp/vsh3

#grep 'Average Resets RVND' NewTest1.txt | cut -d " " -f 4 > /tmp/rvnd

paste /tmp/instance /tmp/scenario /tmp/parcelp /tmp/objval /tmp/nveh /tmp/veh1 /tmp/vsh1 /tmp/veh2 /tmp/vsh2 /tmp/veh3 /tmp/vsh3 > TypeStatstestgh2-13-2B-0to30p.txt

# paste /tmp/instance /tmp/sstaus > Solstatustestgh2-13-2B-0to30p.txt
