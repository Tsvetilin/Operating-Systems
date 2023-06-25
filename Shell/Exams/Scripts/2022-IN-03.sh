#!/bin/bash

user=$(whoami)
[[ $user != "oracle" && $user != "grid" ]] && echo "You are not oracle or grid" && exit 1

[[ $# -ne 1 ]] && echo "1 param expected" && exit 1
[[ ! $1 =~ ^[0-9]+$ ]] && echo "the param must be a number" && exit 1

machine=$(hostname -s)

[[ -z $ORACLE_BASE ]] && echo "ORACLE_BASE not found" && exit 2
[[ -z $ORACLE_HOME ]] && echo "ORACLE_HOME not found" && exit 3
[[ -z $ORACLE_SID ]] && echo "ORACLE_SID not found" && exit 4

sqlplus=$(find $ORACLE_HOME/bin/sqlplus -executable)
[[ -z $sqlplus ]] && echo "Sqlplus executable file not found" && exit 5

role="SYSDBA"
[[ $user == "grid" ]] && role="SYSASM"

sqlplusOutput=$($sqlplus -SL "/ as $role" @foo.sql)  
exitStatus=$?

diag_base="$ORACLE_HOME"
[[ $exitStatus -eq 0 ]] && diag_base=$(echo "$sqlplusOutput" | tail -n +4 | head -n 1)

diag_dir="$diag_base/diag"
[[ ! -d $diag_dir ]] && echo "Diag_dir not found" && exit 6

fileEnd=".*_[0-9]+\."
trcTrm="${fileEnd}(trc|trm)$"
log="${fileEnd}log$"
xml="${fileEnd}xml$"

if [[ $user == "grid" ]]; then
	crsPath="$diag_dir/crs/$machine/crs/trace"
	crsFiles=$(find $crsPath -type f -mtime +$1 -printf '%s %p\n' | egrep "$trcTrm" | cut -d ' ' -f 1)
	size=$(echo "$crsFiles" | awk '{ sum += $0 } END {print sum/1024}')
	
	echo "CRS: $size"

	tnslsnrPath="$diag_dir/tnslsnr/$machine"
	tnslsnrFiles=$(find $tnslsnrPath -mindepth 3 -maxdepth 3 -type f -path "$tnslsnrPath/*/alert/*" -o -path "$tnslsnrPath/*/trace/*" -mtime +$1 -printf '%s %p\n' | egrep "(alert/$xml|trace/$log)" | cut -d ' ' -f 1)
	size=$(echo "$tnslsnrFiles" | awk '{ sum += $0 } END {print sum/1024}')

	echo "TNSLSNR: $size"
else
 	rdbmsPath="$diag_dir/rdbms"
 	rdbmsFiles=$(find $rdbmsPath -mindepth 2 -maxdepth 2 -type f -mtime +$1 -printf '%s %p\n' | egrep "$trcTrm" | cut -d ' ' -f 1)
 	size=$(echo "$rdbmsFiles" | awk '{ sum += $0 } END {print sum/1024}')

 	echo "RDBMS: $size"
fi
