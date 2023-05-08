#!/bin/bash

if [[ $# -ne 2 ]] ; then
        echo "Expected 2 arguments - file and directory"
        exit 1
fi

file=${1}
directory=${2}

for FILE in $(find ${directory} -type f) ; do
        diff -q ${file} ${FILE} > /dev/null
        if [[ ${?} -eq 0 ]] ; then
                echo "${FILE}"
        fi
done


#Да се напише shell скрипт, който приема файл и директория. Скриптът проверява в подадената директория и нейните
#под-директории дали съществува копие на подадения файл и отпечатва имената на намерените копия, ако съществуват такива.
#
#NB! Под 'копие' разбираме файл със същото съдържание.
