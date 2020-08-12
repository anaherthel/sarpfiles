#!/bin/bash

#grep 'Solving' tbundles1Ai30.txt | cut -d " " -f 3 > /tmp/instance
#grep 'Scenario' tbundles1Ai30.txt | cut -d " " -f 3 > /tmp/scenario
##grep 'ParcelP' tbundles1Ai30.txt | cut -d " " -f 3 > /tmp/parcelp
##grep '(root+branch&cut)' tbundles1Ai30.txt | awk '{print $4}'> /tmp/ttime
##grep 'Sol status' tbundles1Ai30.txt | cut -d " " -f 3 > /tmp/sstat
##grep 'Obj Val' tbundles1Ai30.txt | cut -d " " -f 3 > /tmp/objval
##grep 'Number of Vehicles' tbundles1Ai30.txt | cut -d " " -f 4 > /tmp/nveh
#grep 'Total passenger time' tbundles1Ai30.txt | cut -d " " -f 4 > /tmp/tpass
#grep 'Total parcel time' tbundles1Ai30.txt | cut -d " " -f 4 > /tmp/tpar
#grep 'Total combined transportation time' tbundles1Ai30.txt | cut -d " " -f 5 > /tmp/tboth
#grep 'Total idle time driving' tbundles1Ai30.txt | cut -d " " -f 5 > /tmp/tnone
#grep 'Total idle time still' tbundles1Ai30.txt | cut -d " " -f 5 > /tmp/tstill


#grep 'Average Resets RVND' NewTest1.txt | cut -d " " -f 4 > /tmp/rvnd

#paste /tmp/instance /tmp/scenario /tmp/tpass /tmp/tpar /tmp/tboth /tmp/tnone /tmp/tstill > Statstbundles1Ai30.txt


#paste /tmp/instance /tmp/scenario /tmp/ttime /tmp/sstat /tmp/objval /tmp/nveh /tmp/tpass /tmp/tpar /tmp/tboth /tmp/tnone /tmp/tstill > Statstbundles1Ai30.txt


grep 'Solving' tbundles1Ai30.txt | cut -d " " -f 3 > /tmp/instance
grep 'Scenario' tbundles1Ai30.txt | cut -d " " -f 3 > /tmp/scenario
grep 'ParcelP' tbundles1Ai30.txt | cut -d " " -f 3 > /tmp/parcelp
grep 'Total passenger distance' tbundles1Ai30.txt | cut -d " " -f 4 > /tmp/dpass
grep 'Total parcel distance' tbundles1Ai30.txt | cut -d " " -f 4 > /tmp/dpar
grep 'Total combined transportation distance' tbundles1Ai30.txt | cut -d " " -f 5 > /tmp/dboth
grep 'Total idle distance' tbundles1Ai30.txt | cut -d " " -f 4 > /tmp/dnone



##grep 'Average Resets RVND' NewTest1.txt | cut -d " " -f 4 > /tmp/rvnd

paste /tmp/instance /tmp/scenario /tmp/parcelp /tmp/dpass /tmp/dpar /tmp/dboth /tmp/dnone > DStatstbundles1Ai30.txt


#paste /tmp/lline > Statstbundles1Ai30.txt
# paste /tmp/instance /tmp/sstaus > Solstatustbundles1Ai30.txt
