#!/bin/bash

if [[ $# -ne 2 ]] ; then
        echo "Expected 2 arguments - directory and number"
        exit 1
fi

if [[ ! -d "${1}" ]] ; then
        echo "First argument is not a directory"
        exit 1
fi

echo "${2}" | egrep -q "^[0-9]+$"
if [[ ${?} -ne 0 ]] ; then
        echo "Second argument is not an integer"
        exit 1
fi

for file in $(find ${1} -mindepth 1 -maxdepth 1 -type f) ; do
        fileSize=$(stat -c "%s" ${file})
        if [[ ${fileSize} -gt ${2} ]] ; then
                echo "${file}"
        fi
done

#or

find ${1} -mindepth 1 -maxdepth 1 -type f -size +${2}c

#Да се напише shell скрипт, който приема два параметъра - име на директория и число.
#Скриптът да извежда на стандартния изход имената на всички обикновени файлове във директорията,
#които имат размер, по-голям от подаденото число.
