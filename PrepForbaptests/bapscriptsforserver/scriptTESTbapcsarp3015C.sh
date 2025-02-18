#!/bin/bash

dirInstancias='data/C4/csarp'
dirSol='sol_test/sol_csarp'
output='TESTbapcsarp3015Ca.txt'
#runs=10

cat /dev/null > $output

##for i in 1A
##for i in 1A
##do
##	for j in 10 20 30 40
##	do
#		for arquivo in `ls ${dirInstancias}` #lista todos os arquivos na pasta que contem as instancias que vc quer rodar
#		do
##			echo "Solving: " ${arquivo} ":" >> $output #output pra saber que instancia ta sendo rodada
##			echo "Scenario: 1A bundle "  >> $output
#			./bin/sharearidec -b config/CVRPbc.cfg -a config/CVRPapp.cfg -i ${dirInstancias}/${arquivo} --cutOffValue $(cat ${dirSol}/${arquivo}) --setting 1 >> $output
#			#./exeSARP ${dirInstancias}/${arquivo} 2MM node >> $output
#		done
##	done
##done
./bin/sharearidec -b config/CVRPbc.cfg -a config/CVRPapp.cfg -i data/C4/csarp/sarp-30-15-C-2.txt --cutOffValue -68.2 --setting 1
./bin/sharearidec -b config/CVRPbc.cfg -a config/CVRPapp.cfg -i data/C4/csarp/sarp-30-15-C-3.txt --cutOffValue -97.8 --setting 1
./bin/sharearidec -b config/CVRPbc.cfg -a config/CVRPapp.cfg -i data/C4/csarp/sarp-30-15-C-4.txt --cutOffValue -44.1 --setting 1
./bin/sharearidec -b config/CVRPbc.cfg -a config/CVRPapp.cfg -i data/C4/csarp/sarp-30-15-C-5.txt --cutOffValue -76.1 --setting 1






