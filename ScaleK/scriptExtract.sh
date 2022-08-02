#!/bin/bash

dirTestes='Stests'
dirDest='ScaleK'

for i in $(ls ${dirTestes}/bundlessfsarpNS2.txt)
	do
		grep 'Instance Name' $i | cut -d " " -f 3 > /tmp/instance
		grep 'Number of Vehicles' $i | cut -d " " -f 4 > /tmp/k
		grep 'Used vehicles' $i | cut -d " " -f 3 > /tmp/u
		grep 'Vehicle numbers' $i | cut -d ":" -f 2 > /tmp/l
		paste /tmp/instance /tmp/k /tmp/u /tmp/l >> SsfsarpKbdl.txt
	done
