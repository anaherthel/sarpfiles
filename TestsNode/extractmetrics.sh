#!/bin/bash

#rm /tmp/{instance,scenario,parcelp,dpass,dpar,dboth,dboth,dnone}

grep 'Solving' testservermetrics1B2B0-30s0-12.txt | cut -d " " -f 3 > /tmp/instance
grep 'Scenario' testservermetrics1B2B0-30s0-12.txt | cut -d " " -f 3 > /tmp/scenario
grep 'ParcelP' testservermetrics1B2B0-30s0-12.txt | cut -d " " -f 3 > /tmp/parcelp
grep 'Total passenger distance' testservermetrics1B2B0-30s0-12.txt | cut -d " " -f 4 > /tmp/dpass
grep 'Total parcel distance' testservermetrics1B2B0-30s0-12.txt | cut -d " " -f 4 > /tmp/dpar
grep 'Total combined transportation distance' testservermetrics1B2B0-30s0-12.txt | cut -d " " -f 5 > /tmp/dboth
grep 'Total idle distance' testservermetrics1B2B0-30s0-12.txt | cut -d " " -f 4 > /tmp/dnone

#grep 'Average Resets RVND' NewTest1.txt | cut -d " " -f 4 > /tmp/rvnd

paste /tmp/instance /tmp/scenario /tmp/parcelp /tmp/dpass /tmp/dpar /tmp/dboth /tmp/dnone > mStats1B2B30.txt

