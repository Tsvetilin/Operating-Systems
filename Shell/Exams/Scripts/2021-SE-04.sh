#!/bin/bash

[[ $# -ne 1 || ! $1 =~ ^[0-9]+$ || $1 -lt 2 ]] && echo "1 param that is a number bigger than 2 is expected" && exit 1

minutes=$(($1*60))

user=$(whoami)
[[ $user != "oracle" && $user != "grid" ]] && echo "You are not oracle or grid" && exit 2

[[ -z $ORACLE_HOME ]] && echo "ORACLE_HOME not found" && exit 3

adrci="$ORACLE_HOME/bin/adrci"

[[ ! -f $adrci ]] && echo "adrci executable not found" && exit 4

diag_dest="/u01/app/$user"

initialCommand="SET BASE $diag_dest;"

adrHomes=$($adrci exec="$initialCommand SHOW HOMES")

[[ $adrHomes == "No ADR homes are set" ]] && echo "No ADR Homes are set" && exit 5

regex="^([^\/]+\/\b(crs|tnslsnr|kfod|asm|rdbms)\b).*"
adrHomes=$(echo "$adrHomes" | egrep "$regex" | sed -E "s/$regex/\1/g") # diag/rdbms/.../.../ -> diag/rdbms/

while read home; do
	$adrci exec="$initialCommand SET HOMEPATH $home; PURGE -AGE $minutes"
done < <(echo "$adrHomes")
