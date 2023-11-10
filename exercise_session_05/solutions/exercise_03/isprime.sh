#!/bin/bash

num=$1

RE=^[1-9][0-9]*$
if [[ "$num" =~ $RE ]]
then
	echo "correct input: ${num}"
else
	echo "The given input is not a valid (integer) number: ${num}"
	exit
fi

# echo "$(factor $num)"
if [[ $(factor $num | wc -w) -eq 2 ]]
then
	echo "$num is prime"
else
	if [[ $num -eq 1 ]]
		then 
			echo "You inserted 1"
		else
			echo "$num is composite"
	fi
fi 

