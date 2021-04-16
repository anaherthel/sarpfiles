#!/bin/bash

#dirInstancias='Instance5/csarp'
output='ncsarp4-2.txt'
#runs=10

cat /dev/null > $output

for i in 2A 1B 2B
#for i in 1A
do
#	for j in 10 20 30 40
#	do
#		for arquivo in `ls ${dirInstancias}` #lista todos os arquivos na pasta que contem as instancias que vc quer rodar
#		do
			echo "Solving: " ${arquivo} ":" >> $output #output pra saber que instancia ta sendo rodada
			echo "Scenario: " $i >> $output
			echo "ParcelP: " $j >> $output
			./exeSARP Instance4/csarp/sarp-15-10-B-3.txt $i 0 node >> $output
#		done
#	done
done








