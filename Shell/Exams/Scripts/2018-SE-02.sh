#!/bin/bash

if [[ ${#} -ne 2 ]] ; then
        echo "Expected 2 arguments"
        exit 1
fi

dir=${1}
all_types=$(find ${dir} | wc -l)
files_count=$(find ${dir} -type f | wc -l)
dirs_count=$(find ${dir} -type d | wc -l)
if [[ $(( files_count + dirs_count )) -ne ${all_types} ]] ; then
        echo "Directory should not contain objects"
        exit 1
fi

file=${2}
counter=1
declare -A arr
touch "${dir}/dict.txt"
while read line ; do
        name=$(echo "${line}" | awk '{print $1 " " $2}' | awk -F ':' '{print $1}' | awk -F '(' '{print $1}')
        file_name=$(echo "${name}" | tr ' ' '_')
        echo "${file_name}"
        if [[ ! -v arr[${file_name}]  ]] ; then
                arr[${file_name}]=${counter}
                touch "${dir}/${counter}.txt"
                echo "${name};${counter}" >> "${dir}/dict.txt"
                echo "${line}" >> "${dir}/${arr[${file_name}]}.txt"
                counter=$(( counter + 1 ))
        else
                echo "${line}" >> "${dir}/${arr[${file_name}]}.txt"
        fi
        echo "${arr[${file_name}]}"
done < <(cat ${file})

#Напишете скрипт, който приема два позиционни аргумента – име на текстови файл
#и директория. Директорията не трябва да съдържа обекти, а текстовият файл (US-ASCII)
#е стенограма и всеки ред е в следния формат:
#ИМЕ ФАМИЛИЯ (уточнения): Реплика
#където:
#• ИМЕ ФАМИЛИЯ присъстват задължително;
#• ИМЕ и ФАМИЛИЯ се състоят само от малки/главни латински букви и тирета;
#• (уточнения) не е задължително да присъстват;
#• двоеточието ‘:’ присъства задължително;
#• Репликата не съдържа знаци за нов ред;
#• в стринга преди двоеточието ‘:’ задължително има поне един интервал между ИМЕ и ФАМИЛИЯ;
#• наличието на други интервали където и да е на реда е недефинирано.
#Примерен входен файл:
#John Lennon (The Beatles): Time you enjoy wasting, was not wasted.
#Roger Waters: I’m in competition with myself and I’m losing.
#John Lennon:Reality leaves a lot to the imagination.
#Leonard Cohen:There is a crack in everything, that’s how the light gets in.
#Скриптът трябва да:
#• създава текстови файл dict.txt в посочената директория, който на всеки ред да съдържа:
#ИМЕ ФАМИЛИЯ;НОМЕР
#където:
#– ИМЕ ФАМИЛИЯ е уникален участник в стенограмата (без да се отчитат уточненията);
#– НОМЕР е уникален номер на този участник, избран от вас.
#• създава файл НОМЕР.txt в посочената директория, който съдържа всички (и само) редовете
#на дадения участник.

