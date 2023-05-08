#!/bin/bash

if [[ $# -lt 2 ]] ; then
        echo "Expected 2 arguments"
        exit 1
fi

fileName=$1
shift

word=$@

grep -q "${word}" ${fileName}
status=$?

if [[ ${status} == 0 ]] ; then
        echo "${fileName} contains ${word}"
else
        echo "${fileName} does not contain ${word}"
fi

#Да се напише shell скрипт, който чете от стандартния вход име на файл и символен низ, проверява дали низа се съдържа
#във файла и извежда на стандартния изход кода на завършване на командата с която сте проверили наличието на низа.
#
#NB! Символният низ може да съдържа интервал (' ') в себе си.