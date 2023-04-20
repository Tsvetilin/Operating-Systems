#!/bin/bash

counter=0

while read line ; do
    if [[ ${line} == "exit" ]] ; then
        exit 0
    fi
    echo "${line}" | egrep -q "^[-]{0,1}[0-9]*$"
    if [[ ${?} -eq 0 ]] ; then
                arr[${counter}]=${line}
                counter=$(( counter + 1 ))
                absNumber=$(echo "${line}" | tr -d '-')
                if [[ ${absNumber} -gt ${maxNumber} ]] ; then
                maxNumber=${absNumber}
                fi
                if [[ ${calcNumber} -ge ${maxCalcNumber} ]] ; then
                        maxCalcNumber=${calcNumber}
                fi
    fi
done < "${1:-/dev/stdin}"

minNumber=${maxNumber}
for num in ${arr[@]} ; do
        absNum=$(echo "${num}" | tr -d '-')
        calcNumber=$(echo "${absNum}" | grep -o "." | awk '{sum+=$1} END{print sum}')
        if [[ ${calcNumber} -ge ${maxCalcNumber} ]] ; then
                maxCalcNumber=${calcNumber}
                if [[ ${num} -lt ${minNumber} ]] ; then
                        minNumber=${num}
                fi
        fi
    if [[ ${absNum} -eq ${maxNumber} ]] ; then
                result+="${num}"
                result+=$'\n'
    fi
done

echo "${result}" | sort -u
echo "${minNumber}"


#Напишете два скрипта (по един за всяка подточка), които четат редове от STDIN.
#Скриптовете трябва да обработват само редовете, които съдържат цели положителни или отрицателни числа; останалите редове се игнорират. Скриптовете трябва да извежда на STDOUT:
#а) всички уникални числа, чиято абсолютна стойност е равна на максималната абсолютна стойност сред всички числа
#б) всички най-малки уникални числа от тези, които имат максимална сума на цифрите си
#Примерен вход:
#We don’t
#n11d n0
#educat10n
#12.3
#6
#33
#-42
#-42
#111
#111
#-111
#Примерен изход за а):
#-111
#111
