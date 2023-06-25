#!/bin/bash

[[ $# -ne 2 ]] && echo "2 params expected" && exit 1
[[ ! -d $1 ]] && echo "1st param must be a dir" && exit 2
[[ -d $2 ]] && echo "2nd param must be a dir that does not exit" && exit 3

files=$(find $1 -type f -name '*.jpg')

while read file; do
	basename=$(basename "$file")
	
	name=$(echo "$basename" | sed -E -e 's/\([^\)]*\)//g' -e 's/\s+/ /g' -e 's/\.jpg//g')
	album=$(echo "$basename" | sed -E 's/.*\((.*)\)[^\(\)]*\.jpg$/\1/g')
	[[ "$album" == "$basename" ]] && album="misc"
	date=$(find "$file" -printf '%TF\n')

	sha=$(sha256sum "$file" | cut -d ' ' -f 1 | cut -c 1-16)
	
	echo "$name $album $date $sha"

	mkdir -p $2
	mkdir -p $2/images
	mkdir -p $2/by-date/"$date"/by-album/"$album"/by-title
	mkdir -p $2/by-date/"$date"/by-title
	mkdir -p $2/by-album/"$album"/by-date/"$date"/by-title
	mkdir -p $2/by-album/"$album"/by-title
	mkdir -p $2/by-title

	cp "$file" $2/images/$sha.jpg

	ln -s ../../../../../images/$sha.jpg $2/by-date/"$date"/by-album/"$album"/by-title/"$name".jpg

	ln -s ../../../images/$sha.jpg $2/by-date/"$date"/by-title/"$name".jpg
	
	ln -s ../../../../../images/$sha.jpg $2/by-album/"$album"/by-date/"$date"/by-title/"$name".jpg

	ln -s ../../../images/$sha.jpg $2/by-album/"$album"/by-title/"$name".jpg

	ln -s ../images/$sha.jpg $2/by-title/"$name".jpg
done < <(echo "$files")
