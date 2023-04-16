#!/bin/bash

rec=1

if [[ "${1}" == "-r" ]] ; then
        rec=0
        shift
fi

while [[ $# -ne 0 ]] ; do
        file="${1}"
        currentDate=$(date +'%Y-%m-%d %H:%M:%S')
        if [[ -f "${file}" ]] ; then
                echo "[${currentDate}] Removed file ${file}"
                rm ${file}
        elif [[ -d "${file}" ]] ; then
                filesCount=$(find "${file}" -mindepth 1 | wc -l)
                if [[ ${filesCount} -eq 0 ]] ; then
                        echo "[${currentDate}] Removed directory ${file}"
                        rmdir ${file}
                else
                        if [[ ${rec} -eq 0 ]] ; then
                                echo "[${currentDate}] Removed directory recursively ${file}"
                                rm -rf ${file}
                        fi
                fi
        fi
        shift
done

#Да се напише shell скрипт, който получава произволен брой аргументи файлове, които изтрива.
#Ако бъде подадена празна директория, тя бива изтрита. Ако подадения файл е директория с поне 1 файл, тя не се изтрива.
#За всеки изтрит файл (директория) скриптът добавя ред във log файл с подходящо съобщение.
#
#а) Името на log файла да се чете от shell environment променлива, която сте конфигурирали във вашия .bashrc.
#б) Добавете параметър -r на скрипта, който позволява да се изтриват непразни директории рекурсивно.
#в) Добавете timestamp на log съобщенията във формата: 2018-05-01 22:51:36
#
#Примери:
#$ export RMLOG_FILE=~/logs/remove.log
#$ ./rmlog -r f1 f2 f3 mydir/ emptydir/
#$ cat $RMLOG_FILE
#[2018-04-01 13:12:00] Removed file f1
#[2018-04-01 13:12:00] Removed file f2
#[2018-04-01 13:12:00] Removed file f3
#[2018-04-01 13:12:00] Removed directory recursively mydir/
#[2018-04-01 13:12:00] Removed directory emptydir/
