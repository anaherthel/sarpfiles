#!/bin/bash

dirTestes='tests'
dirDest='ScaleK'

for i in $(ls ${dirTestes}/testingKsize*.txt)
	do
		grep 'Instance Name' $i | cut -d " " -f 3 > /tmp/instance
		grep 'Number of Vehicles' $i | cut -d " " -f 4 > /tmp/k
		paste /tmp/instance /tmp/k >> testScaleK.txt
	done
