#!/bin/bash

RE="Temperature\: ([0-9]+\.[0-9]+) deg at time\: ([0-9]+\.[0-9]+) sec"
TEMP=0
n=0
FILES=$(ls ./../../measured/*.txt)
echo "files to inspect: ${FILES}"
:>out.txt
for file in $FILES; do
	echo "reading file: ${file}"
	
	while read line; do
		# echo "line: ${line}"
		if [[ "$line" =~ $RE ]] ; then
			echo "${BASH_REMATCH[1]}   ${BASH_REMATCH[2]}" >> out.txt
			TEMP=$(echo "scale=5;${TEMP} + ${BASH_REMATCH[1]}" | bc)
			n=$((n+1))
	                echo "temperature accumulated: ${TEMP} n: ${n}" 

	 	fi
	 done < ${file}

done

AVERAGE=$(echo "scale=5;$TEMP/$n" | bc)
echo "Average temperature: ${AVERAGE}"