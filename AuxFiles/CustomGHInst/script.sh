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
#runs=10

#cat /dev/null > $output

for i1 in `ls ${dirInstancias}`
do
	./exeCustInst ./${dirInstancias}/${i1}

done


