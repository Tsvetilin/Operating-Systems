#!/bin/bash

rec=1

if [[ "${1}" == "-r" ]] ; then
        rec=0
        shift
fi

while [[ $# -ne 0 ]] ; do
        file="${1}"
        currentDate=$(date +'%Y-%m-%d-%H-%M-%S')
        if [[ -f "${file}" ]] ; then
                echo "[${currentDate}] Removed file ${file}"
                tar -czf "${BACKUP_DIR}/${file}_${currentDate}.gz" "${file}"
                rm ${file}
        elif [[ -d "${file}" ]] ; then
                        dirName=$(echo "${file}" | sed -E 's/(.*)\//\1/g')
                filesCount=$(find "${file}" -mindepth 1 | wc -l)
                if [[ ${filesCount} -eq 0 ]] ; then
                        echo "[${currentDate}] Removed directory ${file}"
                        tar -czf "${BACKUP_DIR}/${dirName}_${currentDate}.tgz" "${file}"
                        rmdir ${file}
                else
                        if [[ ${rec} -eq 0 ]] ; then
                                echo "[${currentDate}] Removed directory recursively ${file}"
                                tar -czf "${BACKUP_DIR}/${dirName}_${currentDate}.tgz" "${file}"
                                rm -rf ${file}
                        else
                                echo "error: ${file} is not empty, will not detele"
                        fi
                fi
        fi
        shift
done

#Да се напише shell скрипт, който получава произволен брой аргументи файлове, които изтрива.
#Ако бъде подадена празна директория, тя бива изтрита. Ако подадения файл е директория с поне 1 файл, тя не се изтрива.
#
#Да се дефинира променлива BACKUP_DIR (или друго име), в която:
#- изтритите файлове се компресират и запазват
#- изтритите директории се архивират, комприсират и запазват
#- имената на файловете е "filename_yyyy-mm-dd-HH-MM-SS.{gz,tgz}", където filename е оригиналното име на файла
#(директорията) преди да бъде изтрит
#
#а) Добавете параметър -r на скрипта, който позволява да се изтриват непразни директории рекурсивно и съответно
#да се запазят в BACKUP_DIR
#
#Примери:
#$ export BACKUP_DIR=~/.backup/
#
## full-dir/ има файлове и не може да бъде изтрита без параметър -r
#$ ./trash f1 f2 full-dir/ empty-dir/
#error: full-dir/ is not empty, will not detele
#$ ls $BACKUP_DIR
#f1_2018-05-07-18-04-36.gz
#f2_2018-05-07-18-04-36.gz
#empty-dir_2018-05-07-18-04-36.tgz
#
#$ ./trash -r full-dir/
#
#$ ls $BACKUP_DIR
#f1_2018-05-07-18-04-36.gz
#f2_2018-05-07-18-04-36.gz
#full-dir_2018-05-07-18-04-50.tgz
#empty-dir_2018-05-07-18-04-36.tgz
#
## можем да имаме няколко изтрити файла, които се казват по един и същ начин
#$ ./trash somedir/f1
#
#$ ls $BACKUP_DIR
#f1_2018-05-07-18-04-36.gz
#f1_2018-05-07-18-06-01.gz
#f2_2018-05-07-18-04-36.gz
#full-dir_2018-05-07-18-04-50.tgz
#empty-dir_2018-05-07-18-04-36.tgz