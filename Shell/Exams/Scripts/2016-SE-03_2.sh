#!/bin/bash

if [[ $(id -u) -eq 0 ]] ; then
    echo "Script must be run as root"
    exit 1
fi

cat /etc/passwd | awk -F ':' '{if($6=="") print $1}'

for file in $(cat /etc/passwd | awk -F ':' '{if($6!="") print $6}') ; do
        if [[ -d ${file} ]] ; then
                perm=$(stat -c "%a" ${file} | cut -c 1)
        if [[ ${perm} -ne 7 && ${perm} -ne 6 && ${perm} -ne 4 ]] ; then
                user=$(stat -c "%u" ${file})
                echo "${user}"
        fi
    fi
done

#Напишете shell скрипт който, ако се изпълнява от root, проверява кои потребители
#на системата нямат homedir или не могат да пишат в него.
#Примерен формат:
#root:x:0:0:root:/root:/bin/bash
#daemon:x:1:1:daemon:/usr/sbin:/usr/sbin/nologin
#bin:x:2:2:bin:/bin:/usr/sbin/nologin
