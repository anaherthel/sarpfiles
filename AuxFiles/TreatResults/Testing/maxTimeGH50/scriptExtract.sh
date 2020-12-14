#!/bin/bash

for i in `ls nGHSscaled*.txt`
	do

		
		cat $i >> nGHSscaledAll.txt

	done
