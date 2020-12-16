#!/bin/bash

grep 'Solving' nGHS4.txt | cut -d " " -f 3 > /tmp/instance
grep 'Scenario' nGHS4.txt | cut -d " " -f 3 > /tmp/scenario
grep 'ParcelP' nGHS4.txt | cut -d " " -f 3 > /tmp/parcelp
#grep '(root+branch&cut)' nGHS4.txt | awk '{print $4}'> /tmp/ttime
#grep 'Sol status' nGHS4.txt | cut -d " " -f 3 > /tmp/sstat
grep 'Obj Val' nGHS4.txt | cut -d " " -f 3 > /tmp/objval
grep 'Served parcels' nGHS4.txt | cut -d " " -f 3 > /tmp/servpar
grep 'size of n' nGHS4.txt | cut -d " " -f 4 > /tmp/sn
grep 'size of m' nGHS4.txt | cut -d " " -f 4 > /tmp/sm
##grep 'Average Resets RVND' NewTest1.txt | cut -d " " -f 4 > /tmp/rvnd

paste /tmp/instance /tmp/parcelp /tmp/sn /tmp/sm /tmp/objval /tmp/servpar > StatsnGHS4.txt


#grep 'Solving' nGHS4.txt | cut -d " " -f 3 > /tmp/instance
#grep 'Scenario' nGHS4.txt | cut -d " " -f 3 > /tmp/scenario
#grep 'ParcelP' nGHS4.txt | cut -d " " -f 3 > /tmp/parcelp
#grep 'Total passenger distance' nGHS4.txt | cut -d " " -f 4 > /tmp/dpass
#grep 'Total parcel distance' nGHS4.txt | cut -d " " -f 4 > /tmp/dpar
#grep 'Total combined transportation distance' nGHS4.txt | cut -d " " -f 5 > /tmp/dboth
#grep 'Total idle distance' nGHS4.txt | cut -d " " -f 4 > /tmp/dnone

###grep 'Average Resets RVND' NewTest1.txt | cut -d " " -f 4 > /tmp/rvnd

#paste /tmp/instance /tmp/scenario /tmp/parcelp /tmp/dpass /tmp/dpar /tmp/dboth /tmp/dnone > DStatsnGHS4.txt


#paste /tmp/lline > StatsnGHS4.txt
# paste /tmp/instance /tmp/sstaus > SolstatusnGHS4.txt

#*************************************************************
#grep 'Solving' nGHS4.txt | cut -d " " -f 3 > /tmp/instance
#grep 'Scenario' nGHS4.txt | cut -d " " -f 3 > /tmp/scenario
#grep 'ParcelP' nGHS4.txt | cut -d " " -f 3 > /tmp/parcelp
##grep 'Number of Vehicles' nGHS4.txt | cut -d " " -f 4 > /tmp/nveh
#grep 'Total passenger time' nGHS4.txt | cut -d " " -f 4 > /tmp/tpass
#grep 'Total parcel time' nGHS4.txt | cut -d " " -f 4 > /tmp/tpar
#grep 'Total combined transportation time' nGHS4.txt | cut -d " " -f 5 > /tmp/tboth
#grep 'Total idle time passenger' nGHS4.txt | cut -d " " -f 5 > /tmp/tip
#grep 'Total idle time goods' nGHS4.txt | cut -d " " -f 5 > /tmp/tig
#grep 'Waiting time passenger' nGHS4.txt | cut -d " " -f 4 > /tmp/wtp
#grep 'Waiting time goods' nGHS4.txt | cut -d " " -f 4 > /tmp/wtg

###grep 'Total idle time still' nGHS4.txt | cut -d " " -f 5 > /tmp/tstill

###paste /tmp/instance /tmp/scenario /tmp/tpass /tmp/tpar /tmp/tboth /tmp/tnone > tStatsnGHS4.txt


#paste /tmp/instance /tmp/scenario /tmp/parcelp /tmp/tpass /tmp/tpar /tmp/tboth /tmp/tip /tmp/tig /tmp/wtp /tmp/wtg > stStatsnGHS4.txt

#*************************************************************

#grep 'Solving' nGHS4.txt | cut -d " " -f 3 > /tmp/instance
#grep 'Scenario' nGHS4.txt | cut -d " " -f 3 > /tmp/scenario
#grep 'ParcelP' nGHS4.txt | cut -d " " -f 3 > /tmp/parcelp
#grep 'Total passenger distance' nGHS4.txt | cut -d " " -f 4 > /tmp/dpass
#grep 'Total parcel distance' nGHS4.txt | cut -d " " -f 4 > /tmp/dpar
#grep 'Total combined transportation distance' nGHS4.txt | cut -d " " -f 5 > /tmp/dboth
#grep 'Total idle distance passenger' nGHS4.txt | cut -d " " -f 5 > /tmp/dnonep
#grep 'Total idle distance goods' nGHS4.txt | cut -d " " -f 5 > /tmp/dnoneg

###grep 'Average Resets RVND' NewTest1.txt | cut -d " " -f 4 > /tmp/rvnd

#paste /tmp/instance /tmp/scenario /tmp/parcelp /tmp/dpass /tmp/dpar /tmp/dboth /tmp/dnonep /tmp/dnoneg > DStatsnGHS4.txt

#*************************************************************






