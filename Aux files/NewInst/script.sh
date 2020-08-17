#!/bin/bash

#dirInstancias='Instances/grubhub'
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

dirInstancias1='Instances1'
dirInstancias2='Instances2'
#output='tnewfix0-151A-2B0-30.txt'
#runs=10

cat /dev/null > $output

for i1 in `ls ${dirInstancias1}`
do
	for i2 in `ls ${dirInstancias2}`
	do
		./exeHybIns ${dirInstancias1}/${i1} ${dirInstancias2}/${i2}
	done
done


