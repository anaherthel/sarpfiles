#!/bin/bash

#rm /tmp/{instance,cost,times,resets,inits}

grep 'Solving' grubhubtest2a.txt | cut -d " " -f 3 > /tmp/instance
grep 'Obj Val' grubhubtest2a.txt | cut -d " " -f 3 > /tmp/profit
grep 'Total passenger time' grubhubtest2a.txt | cut -d " " -f 4 > /tmp/tpass
grep 'Total parcel time' grubhubtest2a.txt | cut -d " " -f 4 > /tmp/tpar
grep 'Total combined transportation time' grubhubtest2a.txt | cut -d " " -f 5 > /tmp/tboth
grep 'Total idle time' grubhubtest2a.txt | cut -d " " -f 4 > /tmp/tnone
grep 'Total passenger distance' grubhubtest2a.txt | cut -d " " -f 4 > /tmp/dpass
grep 'Total parcel distance' grubhubtest2a.txt | cut -d " " -f 4 > /tmp/dpar
grep 'Total combined transportation distance' grubhubtest2a.txt | cut -d " " -f 5 > /tmp/dboth
grep 'Total idle distance' grubhubtest2a.txt | cut -d " " -f 4 > /tmp/dnone


#grep 'Average Resets RVND' NewTest1.txt | cut -d " " -f 4 > /tmp/rvnd

paste /tmp/instance /tmp/profit /tmp/tpass /tmp/tpar /tmp/tboth /tmp/tnone /tmp/dpass /tmp/dpar /tmp/dboth /tmp/dnone > StatsGH2A.txt
