#!/bin/bash

for ((iter = 400; iter <= 1000; iter+=100))
do
	for ((j = 1; j <=5; j++))
	do
		echo "training: $iter model $j"
		./train $iter ../model_init.txt ../seq_model_0$j.txt model_0$j.txt
	done
	./test modellist.txt ../testing_data1.txt result.txt
	for ((j = 1; j <=5; j++))
	do
		mv model_0$j.txt model_0$j.$iter.txt
	done
done
