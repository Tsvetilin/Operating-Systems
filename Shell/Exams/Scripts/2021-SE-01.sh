#!/bin/bash

user=$(whoami)
[[ $user != "oracle" ]] && [[ $user != "grid" ]] && echo "You are not oracle or grid" && exit 1

[[ -z $ORACLE_HOME ]] && echo "ORACLE_HOME not found" && exit 2

adrci="$ORACLE_HOME/bin/adrci"

[[ ! -f $adrci ]] && echo "adrci executable not foud" && exit 3

diag_dest="/u01/app/$user"

adrHomes=$($adrci exec="show homes")

[[ $adrHomes =~ ^No ]] && echo "No ADR Homes found" && exit 4

adrHomes=$(echo "$adrHomes" | tail -n +2)

while read home; do
	home=$(echo "$home" | sed -E -e 's/^\s+//' -e 's/\s+$//')
	path="$diag_test/$home"

	bytes=$(find $path -printf '%s\n')

	kb=$(($bytes/1024))
	mb=$(($kb/1024))

	echo "$mb $path"
done < <(echo "$adrHomes") 
