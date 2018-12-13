#!/bin/bash
iter=$1
for ((i = 1; i <=5; i++))
do
	echo "training model $i"
	time ./train $iter ../model_init.txt ../seq_model_0$i.txt models/model_0$i.txt
done
