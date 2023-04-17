#!/bin/bash

max_time=0
max_file=""
max_user=""

for file in $(cat /etc/passwd | awk -F ':' '{print $1 " " $6}') ; do
    user=$(echo ${file} | awk '{print $1}')
    dir=$(echo ${file} | awk '{print $2}')
    time_file=$(find ${dir} -type f -printf "%T@ %p" 2>/dev/null | sort -k1nr | head -n1)
    time=$(echo "${time_file}" | awk '{print $1}')
    t_file=$(echo "${time_file}" | awk '{print $2}')
    if [[ ${time} > ${max_time} ]] ; then
                max_time=${time}
                max_file=${t_file}
                max_user=${user}
    fi
done

echo "${max_user} ${max_file}"
