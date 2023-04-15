#!/bin/bash

if [[ $# -ne 1 ]] ; then
        echo "Expected one argument"
        exit 1
fi

max=0
currentCount=0

while read ch; do
        if [[ ${ch} == "{" ]] ; then
                currentCount=$(( currentCount + 1 ))
        else

                if [[ max -lt currentCount ]] ; then
                    max=${currentCount}
                fi

                currentCount=$(( currentCount - 1 ))
        fi

done < <( cat "${1}" | grep -E -o '[{}]' )

echo "Deepest nesting: ${max}"

#Имате компилируем (a.k.a няма синтактични грешки) source file на езика C. Напишете shell script, който да покaзва
#колко е дълбоко най-дълбокото nest-ване (влагане).
#Примерен .c файл:
#
##include <stdio.h>
#
#int main(int argc, char *argv[]) {
#
#  if (argc == 1) {
#		printf("There is only 1 argument");
#	} else {
#		printf("There are more than 1 arguments");
#	}
#
#	return 0;
#}
#Тук влагането е 2, понеже имаме main блок, а вътре в него if блок.
#
#Примерно извикване на скрипта:
#
#./count_nesting sum_c_code.c
#
#Изход:
#The deepest nesting is 2 levels deep.