#!/bin/bash

#grep 'Solving' nttimesHG.txt | cut -d " " -f 3 > /tmp/instance
##grep 'Scenario' nttimesHG.txt | cut -d " " -f 3 > /tmp/scenario
#grep 'ParcelP' nttimesHG.txt | cut -d " " -f 3 > /tmp/parcelp
#grep '(root+branch&cut)' nttimesHG.txt | awk '{print $4}'> /tmp/ttime
#grep 'Sol status' nttimesHG.txt | cut -d " " -f 3 > /tmp/sstat
#grep 'Obj Val' nttimesHG.txt | cut -d " " -f 3 > /tmp/objval

###grep 'Average Resets RVND' NewTest1.txt | cut -d " " -f 4 > /tmp/rvnd

#paste /tmp/instance /tmp/parcelp /tmp/ttime /tmp/sstat /tmp/objval > StatsnttimesHG.txt


#grep 'Solving' nttimesHG.txt | cut -d " " -f 3 > /tmp/instance
#grep 'Scenario' nttimesHG.txt | cut -d " " -f 3 > /tmp/scenario
#grep 'ParcelP' nttimesHG.txt | cut -d " " -f 3 > /tmp/parcelp
#grep 'Total passenger distance' nttimesHG.txt | cut -d " " -f 4 > /tmp/dpass
#grep 'Total parcel distance' nttimesHG.txt | cut -d " " -f 4 > /tmp/dpar
#grep 'Total combined transportation distance' nttimesHG.txt | cut -d " " -f 5 > /tmp/dboth
#grep 'Total idle distance' nttimesHG.txt | cut -d " " -f 4 > /tmp/dnone

###grep 'Average Resets RVND' NewTest1.txt | cut -d " " -f 4 > /tmp/rvnd

#paste /tmp/instance /tmp/scenario /tmp/parcelp /tmp/dpass /tmp/dpar /tmp/dboth /tmp/dnone > DStatsnttimesHG.txt


#paste /tmp/lline > StatsnttimesHG.txt
# paste /tmp/instance /tmp/sstaus > SolstatusnttimesHG.txt


grep 'Solving' nttimesHG.txt | cut -d " " -f 3 > /tmp/instance
grep 'Scenario' nttimesHG.txt | cut -d " " -f 3 > /tmp/scenario
grep 'ParcelP' nttimesHG.txt | cut -d " " -f 3 > /tmp/parcelp
#grep 'Number of Vehicles' nttimesHG.txt | cut -d " " -f 4 > /tmp/nveh
grep 'Total passenger time' nttimesHG.txt | cut -d " " -f 4 > /tmp/tpass
grep 'Total parcel time' nttimesHG.txt | cut -d " " -f 4 > /tmp/tpar
grep 'Total combined transportation time' nttimesHG.txt | cut -d " " -f 5 > /tmp/tboth
grep 'Total idle time passenger' nttimesHG.txt | cut -d " " -f 5 > /tmp/tip
grep 'Total idle time goods' nttimesHG.txt | cut -d " " -f 5 > /tmp/tig
grep 'Waiting time passenger' nttimesHG.txt | cut -d " " -f 4 > /tmp/wtp
grep 'Waiting time goods' nttimesHG.txt | cut -d " " -f 4 > /tmp/wtg

##grep 'Total idle time still' nttimesHG.txt | cut -d " " -f 5 > /tmp/tstill

##paste /tmp/instance /tmp/scenario /tmp/tpass /tmp/tpar /tmp/tboth /tmp/tnone > tStatsnttimesHG.txt


paste /tmp/instance /tmp/scenario /tmp/parcelp /tmp/tpass /tmp/tpar /tmp/tboth /tmp/tip /tmp/tig /tmp/wtp /tmp/wtg > stStatsnttimesHG.txt


#grep 'Solving' nttimesHG.txt | cut -d " " -f 3 > /tmp/instance
#grep 'Scenario' nttimesHG.txt | cut -d " " -f 3 > /tmp/scenario
#grep 'ParcelP' nttimesHG.txt | cut -d " " -f 3 > /tmp/parcelp
#grep 'Total passenger distance' nttimesHG.txt | cut -d " " -f 4 > /tmp/dpass
#grep 'Total parcel distance' nttimesHG.txt | cut -d " " -f 4 > /tmp/dpar
#grep 'Total combined transportation distance' nttimesHG.txt | cut -d " " -f 5 > /tmp/dboth
#grep 'Total idle distance passenger' nttimesHG.txt | cut -d " " -f 5 > /tmp/dnonep
#grep 'Total idle distance goods' nttimesHG.txt | cut -d " " -f 5 > /tmp/dnoneg

###grep 'Average Resets RVND' NewTest1.txt | cut -d " " -f 4 > /tmp/rvnd

#paste /tmp/instance /tmp/scenario /tmp/parcelp /tmp/dpass /tmp/dpar /tmp/dboth /tmp/dnonep /tmp/dnoneg > DStatsnttimesHG.txt

