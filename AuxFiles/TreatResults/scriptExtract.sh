#!/bin/bash

dirTestes='Testing'
dirDest='GHscaledResultsLastR'
stats='Stats'
time='Time'
dist='Dist'

mkdir -p $dirDest
mkdir -p ${dirDest}/${stats}
mkdir -p ${dirDest}/${dist}
mkdir -p ${dirDest}/${time}

for i in $(ls ${dirTestes}/nGHSscaled*.txt)
	do
		fileid=$(echo $i | cut -d '/' -f 2 | cut -d '.' -f 1)

		./exeTreatRes $i node
		mv dist${fileid}* ${dirDest}/${dist}/
		mv stats${fileid}* ${dirDest}/${stats}/
		mv time${fileid}* ${dirDest}/${time}/
	done
