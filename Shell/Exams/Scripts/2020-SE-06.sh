#!/bin/bash

[[ $# -ne 3 ]] && echo "3 params expected" && exit 1
[[ ! -f $1 ]] && echo "1st param must be a dir" && exit 2
[[ ! $2 =~ ^[a-zA-Z0-9_]+$ || ! $3 =~ ^[a-zA-Z0-9_]+$ ]] && echo "Ivalid key or value" && exit 3

line=$(egrep "^\s*$2" $1)

user=$(whoami)
date=$(date)

if [[ -z "$line" ]]; then
	echo "$2 = $3 # added at $date by $user" >> $1
	exit 0
fi

value=$(echo "$line" | cut -d '#' -f 1 | cut -d '=' -f 2 | sed -E -e 's/^\s+//g' -e 's/\s+$//g')

[[ $value == $3 ]] && echo "Value already set" && exit 0

sed -i -E "/$line/{
	s/^/# /
	s/$/ # edited at $date by $user/
	a $2 = $3 # added at $date by $user
	}" $1
