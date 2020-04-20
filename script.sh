#!/bin/bash

dirInstancias='Instances/grubhub'
#runs=10
# dirInstancias='Instances/sf_data'

#for fator in 4 # se voce precisar variar algum argumento do codigo
#do
	for arquivo in `ls ${dirInstancias}` #lista todos os arquivos na pasta que contem as instancias que vc quer rodar
	do
		#echo $arquivo
		#idioma=`echo ${arquivo} | cut -d "_" -f 1 | cut -d "." -f 1` 
		
		#echo ${idioma}

		#for iter in {1..1} #se for heuristica vc vai querer rodar mais de uma vez
		#do
			echo "Solving: " ${arquivo} ":" #output pra saber que instancia ta sendo rodada
			./exeSARP ${dirInstancias}/${arquivo} 2A #2A 1B 2B
		#done
	done
#done

# dirInstancias='Instances/sf_data'
# #runs=10

# for fator in 4 # se voce precisar variar algum argumento do codigo
# do
# 	for arquivo in `ls ${dirInstancias}` #lista todos os arquivos na pasta que contem as instancias que vc quer rodar
# 	do
# 		#echo $arquivo
# 		#idioma=`echo ${arquivo} | cut -d "_" -f 1 | cut -d "." -f 1` 
		
# 		#echo ${idioma}

# 		for iter in {1..1} #se for heuristica vc vai querer rodar mais de uma vez
# 		do
# 			echo "Solving: " ${arquivo} ":" #output pra saber que instancia ta sendo rodada
# 			./exeSARP ${dirInstancias}/${arquivo} 2A #2A 1B 2B
# 		done
# 	done
# done
