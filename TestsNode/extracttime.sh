#!/bin/bash

#rm /tmp/{instance,scenario,parcelp,ttime,sstatus}

grep 'Solving' testservermetrics1B2B0-30s0-12.txt | cut -d " " -f 3 > /tmp/instance
grep 'Scenario' testservermetrics1B2B0-30s0-12.txt | cut -d " " -f 3 > /tmp/scenario
grep 'ParcelP' testservermetrics1B2B0-30s0-12.txt | cut -d " " -f 3 > /tmp/parcelp
##grep '(root+branch&cut)' testservermetrics1B2B0-30s0-12.txt | cut -d " " -f 1 > /tmp/time
grep '(root+branch&cut)' testservermetrics1B2B0-30s0-12.txt | awk '{print $4}'> /tmp/ttime
grep 'Sol status' testservermetrics1B2B0-30s0-12.txt | cut -d " " -f 3 > /tmp/sstatus


#grep 'Average Resets RVND' NewTest1.txt | cut -d " " -f 4 > /tmp/rvnd

#paste /tmp/instance /tmp/scenario  /tmp/time /tmp/sstatus  > TimeStatstestservermetrics1B2B0-30s0-12.txt

paste /tmp/instance /tmp/scenario /tmp/parcelp /tmp/ttime /tmp/sstatus > TimeStatstestservermetrics1B2B0-30s0-12.txt

# paste /tmp/instance /tmp/sstaus > Solstatustestservermetrics1B2B0-30s0-12.txt
