#!/bin/bash

#rm /tmp/{instance,scenario,parcelp,szn,szm,sstatus,objval,nveh,dpass,dpar,dboth,dnone}

grep 'Solving' tnewfix0-151A-2B0-30.txt | cut -d " " -f 3 > /tmp/instance
grep 'Scenario' tnewfix0-151A-2B0-30.txt | cut -d " " -f 3 > /tmp/scenario
grep 'ParcelP' tnewfix0-151A-2B0-30.txt | cut -d " " -f 3 > /tmp/parcelp
grep 'size of n' tnewfix0-151A-2B0-30.txt | cut -d " " -f 4 > /tmp/szn
grep 'size of m' tnewfix0-151A-2B0-30.txt | cut -d " " -f 4 > /tmp/szm
grep 'Sol status' tnewfix0-151A-2B0-30.txt | cut -d " " -f 3 > /tmp/sstatus
grep 'Obj Val' tnewfix0-151A-2B0-30.txt | cut -d " " -f 3 > /tmp/objval
grep 'Number of Vehicles' tnewfix0-151A-2B0-30.txt | cut -d " " -f 4 > /tmp/nveh
grep 'Total passenger distance' tnewfix0-151A-2B0-30.txt  | cut -d " " -f 4 > /tmp/dpass
grep 'Total parcel distance' tnewfix0-151A-2B0-30.txt | cut -d " " -f 4 > /tmp/dpar
grep 'Total combined transportation distance' tnewfix0-151A-2B0-30.txt  | cut -d " " -f 5 > /tmp/dboth
grep 'Total idle distance' tnewfix0-151A-2B0-30.txt | cut -d " " -f 4 > /tmp/dnone

#grep 'Average Resets RVND' NewTest1.txt | cut -d " " -f 4 > /tmp/rvnd

paste /tmp/instance /tmp/scenario /tmp/parcelp /tmp/szn /tmp/szm /tmp/sstatus /tmp/objval /tmp/nveh /tmp/dpass /tmp/dpar /tmp/dboth /tmp/dnone > statstnewfix0-151A-2B0-30.txt
#paste /tmp/lline > Statstestserverunsolv2.txt
# paste /tmp/instance /tmp/sstaus > Solstatustestserverunsolv2.txt
