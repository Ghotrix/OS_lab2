#!/bin/bash

NUMS=`cat log.txt |grep "10/Oct/2006"|awk '{print $12}'|sed 's/"//g'|sed 's/\/.*//g'|grep -v "-"|wc -l`;
cat log.txt |grep "10/Oct/2006"|awk '{print $12}'|sed 's/"//g'|sed 's/\/.*//g'|grep -v "-"|sort|uniq -c|sort -rnk 1|head -n 10|awk '{printf "%s - %d - %0.f%\n", $2, $1, $1 * 100 / "'$NUMS'"}';
