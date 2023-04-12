#!/bin/bash

if [[ ${#} -le 1 || ${#} -gt 2 ]] ; then
        echo "Expected 1st arg -> dir name"
        echo "Optional 2nd arg: -a"
        exit 1
fi

if [[ ! -d ${1} ]] ; then
        echo "Not a directory"
        exit 2
fi

for file in $(find ${1} -mindepth 1 -maxdepth 1) ; do
        if [[ -d ${file} ]] ; then
                echo "${file} ($(find ${file} -mindepth 1 -maxdepth 1 | wc -l) entries)"
        elif [[ -f ${file} ]] ; then
                echo "${file} ($(stat -c "%s" ${file})b size)"
        fi
done

#Да се напише shell скрипт, който получава единствен аргумент директория и отпечатва списък с всички файлове и директории в нея (без скритите).
#До името на всеки файл да седи размера му в байтове, а до името на всяка директория да седи броят на елементите в нея (общ брой на файловете и директориите, без скритите).
#
#a) Добавете параметър -a, който указва на скрипта да проверява и скритите файлове и директории.
#
#Пример:
#$ ./list.sh .
#asdf.txt (250 bytes)
#Documents (15 entries)
#empty (0 entries)
#junk (1 entry)
#karh-pishtov.txt (8995979 bytes)
#scripts (10 entries)