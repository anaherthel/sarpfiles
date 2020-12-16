for i in $(ls scriptSARP*.sh) 
do 
	screen -dm ./$i
done
