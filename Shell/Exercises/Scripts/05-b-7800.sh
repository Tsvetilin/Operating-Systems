#!/bin/bash

if [[ $# -ne 1 ]] || [[ ! -d ${1} ]]; then
        echo "Expected 1 argument - directory"
        exit 1
fi

counter=0

for file in $(find ${1} -mindepth 1 -maxdepth 1 -type f) ; do
        if [[ -x ${file} ]] ; then
                counter=$(( counter + 1 ))
        fi
done

echo "${counter}"

#Да се напише shell скрипт, който намира броя на изпълнимите файлове в PATH.
#Hint: Предполага се, че няма спейсове в имената на директориите
#Hint2: Ако все пак искаме да се справим с този случай, да се разгледа IFS променливата и констуркцията while read -d
