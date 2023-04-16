#Напишете shell script, който получава задължителен първи позиционен параметър
#– директория и незадължителен втори – име на файл. Скриптът трябва да намира в подадената
#директория и нейните под-директории всички symlink-ове и да извежда (при подаден аргумент
#файл – добавяйки към файла, а ако не е – на стандартния изход) за тях следната информация:
#• ако destination-a съществува – името на symlink-а -> името на destination-а;
#• броя на symlink-овете, чийто destination не съществува.
#Примерен изход:
#8
#lbaz -> /foo/bar/baz
#lqux -> ../../../qux
#lquux -> /foo/quux
#Broken symlinks: 34


#!/bin/bash

if [[ $# -eq 0]]; then
    echo "Invalid number of arguments"
    exit 1
fi

if [[ ! -d $1 ]]; then
    echo "Not a directory"
    exit 2
fi

DIRECTORY=$1
BROKEN_LINKS=0

for read FILE TYPE ; do
    if [[ $TYPE == 'N' ]]; then
        if [[ ${#} -eq 2 ]]; then
            echo "${FILE} -> $(readlink ${FILE})" >> ${2}
        else
            echo "${FILE} -> $(readlink ${FILE})"
        fi
    else
        BROKEN_LINKS=$((BROKEN_LINKS + 1))
    fi
done < <(find ${DIRECTORY} -type l -printf "%f %Y\n" 2>/dev/null)

echo "\nBroken symlinks: ${BROKEN_LINKS}"