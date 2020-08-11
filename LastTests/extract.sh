#!/bin/bash

#grep 'Solving' ttimesnode1A-2Bs18.txt | cut -d " " -f 3 > /tmp/instance
#grep 'Scenario' ttimesnode1A-2Bs18.txt | cut -d " " -f 3 > /tmp/scenario
##grep 'ParcelP' ttimesnode1A-2Bs18.txt | cut -d " " -f 3 > /tmp/parcelp
##grep '(root+branch&cut)' ttimesnode1A-2Bs18.txt | awk '{print $4}'> /tmp/ttime
##grep 'Sol status' ttimesnode1A-2Bs18.txt | cut -d " " -f 3 > /tmp/sstat
##grep 'Obj Val' ttimesnode1A-2Bs18.txt | cut -d " " -f 3 > /tmp/objval
##grep 'Number of Vehicles' ttimesnode1A-2Bs18.txt | cut -d " " -f 4 > /tmp/nveh
#grep 'Total passenger time' ttimesnode1A-2Bs18.txt | cut -d " " -f 4 > /tmp/tpass
#grep 'Total parcel time' ttimesnode1A-2Bs18.txt | cut -d " " -f 4 > /tmp/tpar
#grep 'Total combined transportation time' ttimesnode1A-2Bs18.txt | cut -d " " -f 5 > /tmp/tboth
#grep 'Total idle time driving' ttimesnode1A-2Bs18.txt | cut -d " " -f 5 > /tmp/tnone
#grep 'Total idle time still' ttimesnode1A-2Bs18.txt | cut -d " " -f 5 > /tmp/tstill


#grep 'Average Resets RVND' NewTest1.txt | cut -d " " -f 4 > /tmp/rvnd

#paste /tmp/instance /tmp/scenario /tmp/tpass /tmp/tpar /tmp/tboth /tmp/tnone /tmp/tstill > Statsttimesnode1A-2Bs18.txt


#paste /tmp/instance /tmp/scenario /tmp/ttime /tmp/sstat /tmp/objval /tmp/nveh /tmp/tpass /tmp/tpar /tmp/tboth /tmp/tnone /tmp/tstill > Statsttimesnode1A-2Bs18.txt


grep 'Solving' ttimesnode1A-2Bs18.txt | cut -d " " -f 3 > /tmp/instance
grep 'Scenario' ttimesnode1A-2Bs18.txt | cut -d " " -f 3 > /tmp/scenario
grep 'ParcelP' ttimesnode1A-2Bs18.txt | cut -d " " -f 3 > /tmp/parcelp
grep 'Total passenger distance' ttimesnode1A-2Bs18.txt | cut -d " " -f 4 > /tmp/dpass
grep 'Total parcel distance' ttimesnode1A-2Bs18.txt | cut -d " " -f 4 > /tmp/dpar
grep 'Total combined transportation distance' ttimesnode1A-2Bs18.txt | cut -d " " -f 5 > /tmp/dboth
grep 'Total idle distance' ttimesnode1A-2Bs18.txt | cut -d " " -f 4 > /tmp/dnone



##grep 'Average Resets RVND' NewTest1.txt | cut -d " " -f 4 > /tmp/rvnd

paste /tmp/instance /tmp/scenario /tmp/parcelp /tmp/dpass /tmp/dpar /tmp/dboth /tmp/dnone > DStatsttimesnode1A-2Bs18.txt


#paste /tmp/lline > Statsttimesnode1A-2Bs18.txt
# paste /tmp/instance /tmp/sstaus > Solstatusttimesnode1A-2Bs18.txt
