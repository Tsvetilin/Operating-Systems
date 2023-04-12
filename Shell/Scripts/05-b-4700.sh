#!/bin/bash

separator=' '

if [[ ${#} -lt 1 ]] || [[ ${#} -gt 2 ]] ; then
        echo "Expected #1 argument - number"
        echo "Optional #2 argument - separator"
        exit 1
fi

if [[ ${#} -eq 2 ]] ; then

    if echo ${2} | grep -E -q '^.$' ; then
        separator=${2}
    else
        echo "Single character separator expected"
        exit 2
    fi
fi

if ! ( echo ${1} | grep -q -E '^(-){0,1}[0-9]+$' ) ; then
        echo "Invalid number"
        exit 2
fi

echo $(echo "${1}" | rev | sed -E 's/(.)/\1\n/g' | awk -v separator="${separator}" 'BEGIN {counter = 0;} {counter++; printf $0} counter%3==0{printf separator}' | rev | sed -E 's/^'${separator}'//g')

#Да се напише shell скрипт, който форматира големи числа, за да са по-лесни за четене.
#Като пръв аргумент на скрипта се подава цяло число.
#Като втори незадължителен аргумент се подава разделител. По подразбиране цифрите се разделят с празен интервал.