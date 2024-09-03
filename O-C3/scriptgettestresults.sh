#!/bin/bash

file_list="csarpinst.txt"

#for i in 1A
#for i in 1A
#do
#	for j in 10 20 30 40
#	do
	while IFS= read -r file_name
	do
		if [[ -n "$file_name" ]]; then
    		#echo "Processing file: $file_name"
			file_variable="$file_name"
			scp -P 8022 bia@q-primal.laser.ci.ufpb.br:/home/bia/sarpfiles/src/Results/bundleResults/${file_variable} ./ 
		fi
	done < "$file_list"
#	done
#done

file_list="ghsarpinst.txt"

#for i in 1A
#for i in 1A
#do
#	for j in 10 20 30 40
#	do
	while IFS= read -r file_name
	do
		if [[ -n "$file_name" ]]; then
    		#echo "Processing file: $file_name"
			file_variable="$file_name"
			scp -P 8022 bia@q-primal.laser.ci.ufpb.br:/home/bia/sarpfiles/src/Results/bundleResults/${file_variable} ./ 
		fi
	done < "$file_list"
#	done


file_list="sfsarpinst.txt"

#for i in 1A
#for i in 1A
#do
#	for j in 10 20 30 40
#	do
	while IFS= read -r file_name
	do
		if [[ -n "$file_name" ]]; then
    		#echo "Processing file: $file_name"
			file_variable="$file_name"
			scp -P 8022 bia@q-primal.laser.ci.ufpb.br:/home/bia/sarpfiles/src/Results/bundleResults/${file_variable} ./ 
		fi
	done < "$file_list"
#	done