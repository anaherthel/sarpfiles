#!/bin/bash

#rm /tmp/{instance,scenario,parcelp,dpass,dpar,dboth,dboth,dnone}

grep 'Solving' testserverunsolv2.txt | cut -d " " -f 3 > /tmp/instance
grep 'Scenario' testserverunsolv2.txt | cut -d " " -f 3 > /tmp/scenario
grep 'ParcelP' testserverunsolv2.txt | cut -d " " -f 3 > /tmp/parcelp
grep 'Total passenger distance' testserverunsolv2.txt | cut -d " " -f 4 > /tmp/dpass
grep 'Total parcel distance' testserverunsolv2.txt | cut -d " " -f 4 > /tmp/dpar
grep 'Total combined transportation distance' testserverunsolv2.txt | cut -d " " -f 5 > /tmp/dboth
grep 'Total idle distance' testserverunsolv2.txt | cut -d " " -f 4 > /tmp/dnone

#grep 'Average Resets RVND' NewTest1.txt | cut -d " " -f 4 > /tmp/rvnd

paste /tmp/instance /tmp/scenario /tmp/parcelp /tmp/dpass /tmp/dpar /tmp/dboth /tmp/dnone > munsolved2.txt

