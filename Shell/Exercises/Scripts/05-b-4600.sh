#!/bin/bash

if [[ ${#} -ne 3 ]] ; then
        echo "Expected 3 arguments"
        exit 4
fi

for arg in "${@}" ; do
        if ! ( echo "${arg}" | grep -q -E '^(-){0,1}[0-9]+$' ) ; then
                echo "Invalid number"
                exit 3
        fi
done

if [[ ${3} -lt ${2} ]] ; then
        echo "Invalid interval"
        exit 2
fi

if [[ ${1} -ge ${2} && ${1} -le ${3} ]] ; then
        echo "Number is in interval"
        exit 0
else
        echo "Number is not in interval"
        exit 1
fi

#Да се напише shell скрипт, който валидира дали дадено цяло число попада в целочислен интервал.
#Скриптът приема 3 аргумента: числото, което трябва да се провери; лява граница на интервала; дясна граница на интервала.
#Скриптът да връща exit status:
#- 4, когато броят на аргументите е различен от 3
#- 3, когато поне един от трите аргумента не е цяло число
#- 2, когато границите на интервала са обърнати
#- 1, когато числото не попада в интервала
#- 0, когато числото попада в интервала
