#!/bin/bash

if [[ $# -ne 1 ]] ; then
        echo "Expected 1 argument - directory"
        exit 1
fi

if [[ ! -d ${1} ]] ; then
        echo "Expected 1 argument - directory"
        exit 1
fi

for file in $(find "${1}" -mindepth 1 -maxdepth 1 -type f) ; do
        if [[ -f "${file}" ]] ; then
                for FILE in $(find "${1}" -mindepth 1 -maxdepth 1 -type f) ; do
                        if [[ -f "${FILE}" ]]&& [[ -f "${file}" ]] && [[ "${file}" != "${FILE}" ]] ; then
                                diff -q "${file}" "${FILE}" >/dev/null
                                if [[ ${?} -eq 0 ]] ; then
                                        if [[ "${file}" > "${FILE}" ]] ; then
                                                rm "${file}"
                                        else
                                                rm "${FILE}"
                                        fi
                                fi
                        fi
                done
        fi
done

#Да се напише shell скрипт, който получава единствен аргумент директория и изтрива всички повтарящи се
#(по съдържание) файлове в дадената директория. Когато има няколко еднакви файла, да се остави само този,
#чието име е лексикографски преди имената на останалите дублирани файлове.
#
#Примери:
#$ ls .
#f1 f2 f3 asdf asdf2
## asdf и asdf2 са еднакви по съдържание, но f1, f2, f3 са уникални
#
#$ ./rmdup .
#$ ls .
#f1 f2 f3 asdf
## asdf2 е изтрит