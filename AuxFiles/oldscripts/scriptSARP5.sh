#!/bin/bash

dirInstancias='InstanceSm/csarp'
output='redobundlesmcsarp5.txt'
#runs=10

cat /dev/null > $output

for i in 1A
#for i in 1A
do
#	for j in 10 20 30 40
#	do
		for arquivo in `ls ${dirInstancias}` #lista todos os arquivos na pasta que contem as instancias que vc quer rodar
		do
			echo "Solving: " ${arquivo} ":" >> $output #output pra saber que instancia ta sendo rodada
			echo "Scenario: " $i >> $output
			echo "ParcelP: " $j >> $output
			./exeSARP ${dirInstancias}/${arquivo} $i 0 bundle >> $output
		done
#	done
done








