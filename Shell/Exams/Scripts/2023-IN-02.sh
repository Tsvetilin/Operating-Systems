#!/bin/bash

[[ $# -ne 1 ]] && echo "1 param expected" && exit 1
[[ ! -d $1 ]] && echo "the 1st param must be a dir" && exit 2

files=$(find $1 -type f -printf '%i %n %p\n')

DIR=$(mktemp -d)
while read inode count name; do
	sha=$(sha256sum $name | cut -d ' ' -f 1)

	echo "$inode $count" >> $DIR/$sha
done < <(echo "$files")

files=$(find $DIR -type f)

while read sha; do
	file=$(cat $sha | sort -u)

	ones=$(echo "$file" | egrep " 1$" | cut -d ' ' -f 1)
	notOnes=$(echo "$file" | egrep -v " 1$" | cut -d ' ' -f 1)

	if [[ -z $notOnes ]]; then
		toDelete=$(echo "$ones" | tail -n +2)
		
		[[ -z $toDelete ]] && echo "File with inode number $ones is just 1" && continue

		while read inode; do
			find $1 -type f -inum "$inode"
		done < <(echo "$toDelete")
	elif [[ -z $ones ]]; then
		while read inode; do
			find $1 -type f -inum "$inode" | head -n 1
		done < <(echo "$notOnes")
	else
		while read inode; do
			find $1 -type f -inum "$inode"
		done < <(echo "$ones")

		while read inode; do
			find $1 -type f -inum "$inode" | head -n 1
		done < <(echo "$notOnes")
	fi
done < <(echo "$files")

rm -r $DIR
