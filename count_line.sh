#!/bin/bash

awk 'END{print NR}' *.*

#for str in $(ls *.*)
#do
#	awk -v path="$str"  'END{print NR,path}' ${str}	
#done
