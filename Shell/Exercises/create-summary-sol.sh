#!/bin/bash

# args = files to ignor

TEMP_FILE="tasks.txt"

echo -n "" > "${TEMP_FILE}"

while read DIR ; do

    if echo "${@}" | grep -q ${DIR} ; then
        continue
    fi

    echo -e "====== $(basename "${DIR} ") ======\n" >> "${TEMP_FILE}" 

    while read FILE ; do
        echo "<--- $(basename "${FILE}" | sed -E 's/(.*)\.(.*)$/\1/') --->" >> "${TEMP_FILE}"
        cat "${FILE}" >> "${TEMP_FILE}"
        echo -e "\n" >> "${TEMP_FILE}"
    done < <(find "${DIR}" -type f)

done < <(find . -mindepth 1 -maxdepth 2 -type d )