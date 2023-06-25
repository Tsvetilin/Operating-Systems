#!/bin/bash

[[ $# -ne 1 || ! -d $1 ]] && echo "1 param expected that is a dir" && exit 1

DIR=$(find $1/cfg -maxdepth 0 -type d)
[[ -z $DIR ]] && echo "cfg directory not foun" && exit 2

validate=$(find "$1/validate.sh" -executable)
[[ -z $validate ]] && echo "validate script not found" && exit 3

passFile=$(find "$1/foo.pwd" -type f)
[[ -z $passFile ]] && echo "password file not found" && exit 4

configFile=$(find "$1/foo.conf" -type f)
[[ -z $configFile ]] && echo "password file not found" && exit 5

files=$(find $DIR -type f -name '*.cfg')

echo "" > $configFile
while read file; do
	basename=$(basename $file)

	output=$($validate $file)
	exitCode=$?

	[[ $exitCode -eq 2 ]] && echo "Problem validating file: $file" && continue
	if [[ $exitCode -eq 1 ]]; then
		echo "$output" | awk -v name="$basename" '{ print name":"$0 }' 1>&2
		continue
	fi
	
	cat $file >> $configFile

	username=$(echo "$basename" | sed -E 's/\.cfg$//g')

	userLine=$(egrep "^$username" $passFile)
	[[ ! -z $userLine ]] && continue

	newPass=$(pwgen 10 1)
	hashedPass=$(mkpasswd $newPass)

	echo "${username}:${newPass}"
	echo "${username}:${hashedPass}" >> $passFile
done < <(echo "$files")
