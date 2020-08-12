#!/bin/bash

#grep 'Solving' thybbundle18-301A.txt | cut -d " " -f 3 > /tmp/instance
#grep 'Scenario' thybbundle18-301A.txt | cut -d " " -f 3 > /tmp/scenario
##grep 'ParcelP' thybbundle18-301A.txt | cut -d " " -f 3 > /tmp/parcelp
##grep '(root+branch&cut)' thybbundle18-301A.txt | awk '{print $4}'> /tmp/ttime
##grep 'Sol status' thybbundle18-301A.txt | cut -d " " -f 3 > /tmp/sstat
##grep 'Obj Val' thybbundle18-301A.txt | cut -d " " -f 3 > /tmp/objval
##grep 'Number of Vehicles' thybbundle18-301A.txt | cut -d " " -f 4 > /tmp/nveh
#grep 'Total passenger time' thybbundle18-301A.txt | cut -d " " -f 4 > /tmp/tpass
#grep 'Total parcel time' thybbundle18-301A.txt | cut -d " " -f 4 > /tmp/tpar
#grep 'Total combined transportation time' thybbundle18-301A.txt | cut -d " " -f 5 > /tmp/tboth
#grep 'Total idle time driving' thybbundle18-301A.txt | cut -d " " -f 5 > /tmp/tnone
#grep 'Total idle time still' thybbundle18-301A.txt | cut -d " " -f 5 > /tmp/tstill


#grep 'Average Resets RVND' NewTest1.txt | cut -d " " -f 4 > /tmp/rvnd

#paste /tmp/instance /tmp/scenario /tmp/tpass /tmp/tpar /tmp/tboth /tmp/tnone /tmp/tstill > Statsthybbundle18-301A.txt


#paste /tmp/instance /tmp/scenario /tmp/ttime /tmp/sstat /tmp/objval /tmp/nveh /tmp/tpass /tmp/tpar /tmp/tboth /tmp/tnone /tmp/tstill > Statsthybbundle18-301A.txt


grep 'Solving' thybbundle18-301A.txt | cut -d " " -f 3 > /tmp/instance
grep 'Scenario' thybbundle18-301A.txt | cut -d " " -f 3 > /tmp/scenario
grep 'ParcelP' thybbundle18-301A.txt | cut -d " " -f 3 > /tmp/parcelp
grep 'Total passenger distance' thybbundle18-301A.txt | cut -d " " -f 4 > /tmp/dpass
grep 'Total parcel distance' thybbundle18-301A.txt | cut -d " " -f 4 > /tmp/dpar
grep 'Total combined transportation distance' thybbundle18-301A.txt | cut -d " " -f 5 > /tmp/dboth
grep 'Total idle distance' thybbundle18-301A.txt | cut -d " " -f 4 > /tmp/dnone



##grep 'Average Resets RVND' NewTest1.txt | cut -d " " -f 4 > /tmp/rvnd

paste /tmp/instance /tmp/scenario /tmp/parcelp /tmp/dpass /tmp/dpar /tmp/dboth /tmp/dnone > DStatsthybbundle18-301A.txt


#paste /tmp/lline > Statsthybbundle18-301A.txt
# paste /tmp/instance /tmp/sstaus > Solstatusthybbundle18-301A.txt
