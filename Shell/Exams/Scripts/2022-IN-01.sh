#!/bin/bash

[[ $# -ne 2 ]] && echo "2 params expected" && exit 1
[[ ! -d $1 || ! -d $2 ]] && echo "the 2 params must be dirs" && exit 2

filesInSecond=$(find $2 -mindepth 1)
[[ ! -z $filesInSecond ]] && echo "2nd dir must be empty" && exit 3

files=$(find $1 -type f -printf '%P\n')

while read file; do
	basename=$(basename "$file")
	dirname=$(dirname "$file")

	if [[ $basename =~ ^\..+\.swp$ ]]; then
		actualName=$(echo "$basename" | sed -E 's/\.(.+)\.swp$/\1/g')

		realFile=$(echo "$files" | egrep "${actualName}$")
		[[ ! -z $realFile ]] && continue
	fi

	mkdir -p "$2"/"$dirname"
	cp "$1"/"$file" "$2"/"$dirname"
done < <(echo "$files")
