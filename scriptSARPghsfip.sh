#!/bin/bash

dirInstancias='Instances/ghsarp'
output='fipn12sghsarpA.txt'
#runs=10

cat /dev/null > $output

#for i in 1A
#for i in 1A
#do
#	for j in 10 20 30 40
#	do
		for arquivo in `ls ${dirInstancias}` #lista todos os arquivos na pasta que contem as instancias que vc quer rodar
		do
#			echo "Solving: " ${arquivo} ":" >> $output #output pra saber que instancia ta sendo rodada
#			echo "Scenario: 1A bundle "  >> $output
			./exeSARP ${dirInstancias}/${arquivo} 1A fip >> $output
		done
#	done
#done








