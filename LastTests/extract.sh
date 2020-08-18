#!/bin/bash

#grep 'Solving' nttimesGH.txt | cut -d " " -f 3 > /tmp/instance
##grep 'Scenario' nttimesGH.txt | cut -d " " -f 3 > /tmp/scenario
#grep 'ParcelP' nttimesGH.txt | cut -d " " -f 3 > /tmp/parcelp
#grep '(root+branch&cut)' nttimesGH.txt | awk '{print $4}'> /tmp/ttime
#grep 'Sol status' nttimesGH.txt | cut -d " " -f 3 > /tmp/sstat
#grep 'Obj Val' nttimesGH.txt | cut -d " " -f 3 > /tmp/objval

###grep 'Average Resets RVND' NewTest1.txt | cut -d " " -f 4 > /tmp/rvnd

#paste /tmp/instance /tmp/parcelp /tmp/ttime /tmp/sstat /tmp/objval > StatsnttimesGH.txt


#grep 'Solving' nttimesGH.txt | cut -d " " -f 3 > /tmp/instance
#grep 'Scenario' nttimesGH.txt | cut -d " " -f 3 > /tmp/scenario
#grep 'ParcelP' nttimesGH.txt | cut -d " " -f 3 > /tmp/parcelp
#grep 'Total passenger distance' nttimesGH.txt | cut -d " " -f 4 > /tmp/dpass
#grep 'Total parcel distance' nttimesGH.txt | cut -d " " -f 4 > /tmp/dpar
#grep 'Total combined transportation distance' nttimesGH.txt | cut -d " " -f 5 > /tmp/dboth
#grep 'Total idle distance' nttimesGH.txt | cut -d " " -f 4 > /tmp/dnone

###grep 'Average Resets RVND' NewTest1.txt | cut -d " " -f 4 > /tmp/rvnd

#paste /tmp/instance /tmp/scenario /tmp/parcelp /tmp/dpass /tmp/dpar /tmp/dboth /tmp/dnone > DStatsnttimesGH.txt


#paste /tmp/lline > StatsnttimesGH.txt
# paste /tmp/instance /tmp/sstaus > SolstatusnttimesGH.txt


#grep 'Solving' nttimesGH.txt | cut -d " " -f 3 > /tmp/instance
#grep 'Scenario' nttimesGH.txt | cut -d " " -f 3 > /tmp/scenario
#grep 'ParcelP' nttimesGH.txt | cut -d " " -f 3 > /tmp/parcelp
##grep 'Number of Vehicles' nttimesGH.txt | cut -d " " -f 4 > /tmp/nveh
#grep 'Total passenger time' nttimesGH.txt | cut -d " " -f 4 > /tmp/tpass
#grep 'Total parcel time' nttimesGH.txt | cut -d " " -f 4 > /tmp/tpar
#grep 'Total combined transportation time' nttimesGH.txt | cut -d " " -f 5 > /tmp/tboth
#grep 'Total idle distance passenger' nttimesGH.txt | cut -d " " -f 5 > /tmp/tip
#grep 'Total idle distance goods' nttimesGH.txt | cut -d " " -f 5 > /tmp/tig
#grep 'Waiting time passenger' nttimesGH.txt | cut -d " " -f 4 > /tmp/wtp
#grep 'Waiting time goods' nttimesGH.txt | cut -d " " -f 4 > /tmp/wtg

##grep 'Total idle time still' nttimesGH.txt | cut -d " " -f 5 > /tmp/tstill

##paste /tmp/instance /tmp/scenario /tmp/tpass /tmp/tpar /tmp/tboth /tmp/tnone > tStatsnttimesGH.txt


#paste /tmp/instance /tmp/scenario /tmp/parcelp /tmp/tpass /tmp/tpar /tmp/tboth /tmp/tip /tmp/tig /tmp/wtp /tmp/wtg > stStatsnttimesGH.txt


grep 'Solving' nttimesGH.txt | cut -d " " -f 3 > /tmp/instance
grep 'Scenario' nttimesGH.txt | cut -d " " -f 3 > /tmp/scenario
grep 'ParcelP' nttimesGH.txt | cut -d " " -f 3 > /tmp/parcelp
grep 'Total passenger distance' nttimesGH.txt | cut -d " " -f 4 > /tmp/dpass
grep 'Total parcel distance' nttimesGH.txt | cut -d " " -f 4 > /tmp/dpar
grep 'Total combined transportation distance' nttimesGH.txt | cut -d " " -f 5 > /tmp/dboth
grep 'Total idle distance passenger' nttimesGH.txt | cut -d " " -f 5 > /tmp/dnonep
grep 'Total idle distance goods' nttimesGH.txt | cut -d " " -f 5 > /tmp/dnoneg

##grep 'Average Resets RVND' NewTest1.txt | cut -d " " -f 4 > /tmp/rvnd

paste /tmp/instance /tmp/scenario /tmp/parcelp /tmp/dpass /tmp/dpar /tmp/dboth /tmp/dnonep /tmp/dnoneg > DStatsnttimesGH.txt

