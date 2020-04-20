#!/bin/bash

#rm /tmp/{instance,cost,times,resets,inits}

grep 'Solving' ghk22A.txt | cut -d " " -f 3 > /tmp/instance
grep 'Obj Val' ghk22A.txt | cut -d " " -f 3 > /tmp/profit
grep 'Total passenger time' ghk22A.txt | cut -d " " -f 4 > /tmp/tpass
grep 'Total parcel time' ghk22A.txt | cut -d " " -f 4 > /tmp/tpar
grep 'Total combined transportation time' ghk22A.txt | cut -d " " -f 5 > /tmp/tboth
grep 'Total idle time' ghk22A.txt | cut -d " " -f 4 > /tmp/tnone
grep 'Total passenger distance' ghk22A.txt | cut -d " " -f 4 > /tmp/dpass
grep 'Total parcel distance' ghk22A.txt | cut -d " " -f 4 > /tmp/dpar
grep 'Total combined transportation distance' ghk22A.txt | cut -d " " -f 5 > /tmp/dboth
grep 'Total idle distance' ghk22A.txt | cut -d " " -f 4 > /tmp/dnone
# grep 'Sol status' ghk22A.txt | cut -d " " -f 3 > /tmp/sstaus 

#grep 'Average Resets RVND' NewTest1.txt | cut -d " " -f 4 > /tmp/rvnd

paste /tmp/instance /tmp/profit /tmp/tpass /tmp/tpar /tmp/tboth /tmp/tnone /tmp/dpass /tmp/dpar /tmp/dboth /tmp/dnone > Statsghk22A.txt

# paste /tmp/instance /tmp/sstaus > Solstatusghk22A.txt
