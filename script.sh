#!/bin/sh

declare -A map

COUNT=0;

ARR=`cat log.txt |grep "10/Oct/2006"|awk '{print $12}'|sed 's/"//g'|sed 's/\/.*//g'`;

for line in $ARR
do
	let "COUNT++";
	let "map[${line}]=map[${line}]+1";
done

ARR=();

for browser in "${!map[@]}"
do
	ARR=("${ARR[@]}" "$browser - ${map[$browser]} - $((${map[$browser]} * 100 / COUNT))%\n");
done

echo -e ${ARR[@]} | sort -nrk 3|head -n 10
