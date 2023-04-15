#!/bin/bash

while [ ${#} -ne 0 ] ; do
        FILE=${1}
        shift
        if [[ -f ${FILE} ]] ; then
                if [[ -r ${FILE} ]] ; then
                        echo "${FILE} is readable"
                else
                        echo "${FILE} is not readable"
                fi
        elif [[ -d ${FILE} ]] ; then
                echo "${FILE} is a directory"
                echo "Files in ${FILE}:"
                find "${FILE}" -mindepth 1 -type f -size $(find ${FILE} -mindepth 1 -maxdepth 1 | wc -l)
        else
                echo "${FILE} is not a file or directory"
        fi
done

#Да се напише shell скрипт, който приема произволен брой аргументи - имена на файлове или директории.
#Скриптът да извежда за всеки аргумент подходящо съобщение:
#	- дали е файл, който може да прочетем
#	- ако е директория - имената на файловете в нея, които имат размер, по-малък от броя на файловете в директорията.