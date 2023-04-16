#!/bin/bash

if [[ $# -ne 2 ]] ; then
        echo "Expected 2 arguments"
        exit 1
fi

for arg in $@ ; do
        echo "${arg}" | egrep -q "^[0-9]*$"
        if [[ ${?} -ne 0 ]] ; then
                echo "Argument is not an integer"
                exit 1
        fi
done

mkdir "a"
mkdir "b"
mkdir "c"

for file in $(find "test" -type f) ; do
        lines=$(cat ${file} | wc -l)
        if [[ ${lines} -lt ${1} ]] ; then
                mv ${file} "a"
        elif [[ ${lines} -ge ${1} && ${lines} -le ${2} ]] ; then
                mv ${file} "b"
        else
                mv ${file} "c"
        fi
done

#В текущата директория има само обикновени файлове (без директории). Да се
#напише bash script, който приема 2 позиционни параметъра – числа, който мести файловете от
#текущата директория към нови директории (a, b и c, които трябва да бъдат създадени), като
#определен файл се мести към директория ’a’, само ако той има по-малко редове от първи позиционен
#параметър, мести към директория ’b’, ако редове са между първи и втори позиционен параметър
#и в ’c’ в останалите случаи.