#!/bin/bash

dirTestes='Testing'
#dirDest='largebundlemd'
stats='Stats'
time='Time'
dist='Dist'

mkdir -p $dirDest
mkdir -p ${dirDest}/${stats}
mkdir -p ${dirDest}/${dist}
mkdir -p ${dirDest}/${time}
mkdir -p ${dirDest}/${detours}

for i in $(ls ${dirTestes}/bundlemcsarpF*.txt)
	do
		fileid=$(echo $i | cut -d '/' -f 2 | cut -d '.' -f 1)

		./exeTreatRes $i bundle
		mv dist${fileid}* ${dirDest}/${dist}/
		mv stats${fileid}* ${dirDest}/${stats}/
		mv time${fileid}* ${dirDest}/${time}/
		mv detours${fileid}* ${dirDest}/${detours}/
	done
