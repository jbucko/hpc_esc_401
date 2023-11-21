#!/bin/bash
low=$1
high=$2

RE=^[1-9][0-9]*$
if [[ "$low" =~ $RE ]]
then
	echo "correct low input format: ${low}"
else
	echo "The given input for lower boundary is not a valid (integer) number: ${high}"
	exit
fi

if [[ "$high" =~ $RE ]]
then
	echo "correct low input format: ${high}"
else
	echo "The given input for higher boundary is not a valid (integer) number: ${high}"
	exit
fi


if [[ "$low" -gt "$high" ]]
then
	echo "lower boundary is larger than higher"
	exit
fi


for num in $(seq $low $high)
do
	if [[ $(factor $num | wc -w) -eq 2 ]]
	then
		echo "$num is prime"
	# else
	# 	if [[ $num -eq 1 ]]
	# 		then 
	# 			echo "You inserted 1"
	# 		else
	# 			echo "$num is composite"
	# 	fi
	fi 
done
