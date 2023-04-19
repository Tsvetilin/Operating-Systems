#!/bin/bash

if [[ ${#} -ne 3 ]] ; then
    echo "Expected 3 args"
    exit 1
fi

if [[ "$EUID" -eq 0 ]] ; then
    echo "Script must be run as root"
    exit 1
fi

SRC="${1}"
DST="${2}"
ABC="${3}"

for file in $(find "${SRC}" -mindepth 1 -type f) ; do
    file_name=$(echo "${file}" | awk -F '/' '{print $NF}')
    echo "${file_name}" | grep -q "${ABC}"
    if [[ ${?} -eq 0 ]] ; then
                dir=$(echo "${file}" | awk -F '/' '{for (i=2;i<NF;i++) printf "%s/",$i}')
                mkdir -p "${DST}/${dir}"
                mv ${file} "${DST}/${dir}"
    fi
done

#Напишете скрипт, който приема три задължителни позиционни параметра - директория SRC, директория DST
#(която не трябва да съдържа файлове) и низ АBC. Ако скриптът се
#изпълнява от root потребителя, то той трябва да намира всички файлове в директорията SRC и
#нейните под-директории, които имат в името си като под-низ АBC, и да ги мести в директорията
#DST, запазвайки директорийната структура (но без да запазва мета-данни като собственик и права,
#т.е. не ни интересуват тези параметри на новите директории, които скриптът би генерирал в DST).
#Пример:
#• в SRC (/src) има следните файлове:
#/src/foof.txt
#/src/1/bar.txt
#/src/1/foo.txt
#/src/2/1/foobar.txt
#/src/2/3/barf.txt
#• DST (/dst) е празна директория
#• зададения низ е foo
#Резултат:
#• в SRC има следните файлове:
#/src/1/bar.txt
#/src/2/3/barf.txt
#• в DST има следните файлове:
#/dst/foof.txt
#/dst/1/foo.txt
#/dst/2/1/foobar.txt
