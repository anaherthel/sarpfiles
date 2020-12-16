#!/bin/bash

for i in `ls scriptSARP{1..7}.sh 2> /dev/null`
do
	./$i
done
