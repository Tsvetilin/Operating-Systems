#!/bin/bash

[[ $# -ne 3 ]] && echo "3 params expected" && exit 1
[[ ! -f $1 ]] && echo "The 1st param must be a file that exists" && exit 2
[[ -f $2 ]] && echo "The 2nd param must be a file that does not exist" && exit 3
[[ ! -d $3 ]] && echo "The 3rd param must be a folder" && exit 4

files=$(find $3 -type f -name '*.cfg')

regex="^\s*$|^#.*$|^\s*\{\s.+\s\};(\s*#.*)?$"

validFiles=""
while read file; do
	invalidLines=$(cat "$file" | egrep -v "$regex")
	basename=$(basename "$file")
	
	if [[ ! -z $invalidLines ]]; then
		echo "Errors in file $basename"
		cat -n "$file" | egrep "$invalidLines" | sed -E 's/^\s+([0-9]+)\s+/Line \1:/g'
		continue
	fi

	validFiles="$validFiles $file"
	
	username=$(echo "$basename" | sed -E 's/\.cfg//g')

	line=$(egrep "^$username:" $1)
	[[ ! -z $line ]] && continue

	pass=$(pwgen 16 1)
	echo "$username $pass"

	pass=$(mkpasswd "$pass")
	echo -ne "$username:$pass\n" >> $1
done < <(echo "$files")

cat $validFiles > $2
