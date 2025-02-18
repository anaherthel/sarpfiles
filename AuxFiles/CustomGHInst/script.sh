#!/bin/bash

#dirInstancias='instances/grubhub'
#output='testgh2505.txt'
##runs=10

#cat /dev/null > $output

#for i in 1A 2A 1B 2B
#do
#	for j in 0 10 20 30
#	do
#		for arquivo in `ls ${dirInstancias}` #lista todos os arquivos na pasta que contem as instancias que vc quer rodar
#		do
#			echo "Solving: " ${arquivo} ":" >> $output #output pra saber que instancia ta sendo rodada
#			echo "Scenario: " $i >> $output
#			echo "ParcelP: " $j >> $output
#			./exeSARP ${dirInstancias}/${arquivo} $i $j node >> $output
#		done
#	done
#done

dirInstancias='instances/grubhub'
dirInstancias2='instances/grubhub2'

#runs=10

#cat /dev/null > $output


files1=($(ls -1 "$dirInstancias"))
files2=($(ls -1 "$dirInstancias2"))

# Ensure both directories have the same number of files
if [ "${#files1[@]}" -ne "${#files2[@]}" ]; then
    echo "Error: Directories contain different numbers of files!"
    exit 1
fi

# Iterate over files using index
for i in "${!files1[@]}"; do
    file1="${files1[$i]}"
    file2="${files2[$i]}"
    
    echo "Processing: $file1 and $file2"
    ./exeCustInst "./$dirInstancias/$file1" "./$dirInstancias2/$file2"
done

#for i1 in `ls ${dirInstancias}`
#do
#	./exeCustInst ./${dirInstancias}/${i1} ./${dirInstancias2}/${i1}

#done






