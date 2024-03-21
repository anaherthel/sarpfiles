#!/bin/bash

dirInstancias='data/C1/csarp'
dirSol='sol/sol_csarps1'
output='bap0scsarp.txt'
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
			./bin/sharearidec -b config/CVRPbc.cfg -a config/CVRPapp.cfg -i ${dirInstancias}/${arquivo} --cutOffValue $(cat ${dirSol}/${arquivo}) --scenario=0 >> $output

			#./exeSARP ${dirInstancias}/${arquivo} 2MM node >> $output
		done
#	done
#done







