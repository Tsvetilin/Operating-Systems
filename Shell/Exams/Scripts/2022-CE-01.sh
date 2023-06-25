#!/bin/bash

[[ $# -ne 3 ]] && echo "3 params expected" && exit 1
[[ ! $1 =~ ^[0-9]+$ && ! $1 =~ ^[0-9]+\.[0-9]+$ ]] && echo "1st param must be a number" && exit 2
[[ ! $2 =~ ^[a-zA-Z]+$ && ! $3 =~ ^[a-zA-Z]+$ ]] && echo "Invalid 2nd or 3rd param" && exit 3

unitsFile="base.csv"
prefixFile="prefix.csv"

unitsLine=$(egrep ",$3," $unitsFile)
[[ -z $unitsLine ]] && echo "Unit not found" && exit 4

prefixLine=$(egrep ",$2," $prefixFile)
[[ -z $prefixLine ]] && echo "Prefix not found" && exit 5

measure=$(echo "$unitsLine" | cut -d ',' -f 3)
unitName=$(echo "$unitsLine" | cut -d ',' -f 1)

multiplier=$(echo "$prefixLine" | cut -d ',' -f 3)

num=$(echo "${1}*${multiplier}" | bc)

echo "$num $3 (${measure}, ${unitName})"
