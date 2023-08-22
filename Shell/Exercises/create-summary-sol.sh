#!/bin/bash

# args = files to ignore

OUT_FILE="tasks.txt"

echo -n "" > "${OUT_FILE}"

while read DIR ; do

    if echo "${@}" | grep -q ${DIR} ; then
        continue
    fi

    echo -e "====== $(basename "${DIR} ") ======\n" >> "${OUT_FILE}" 

    while read FILE ; do
        echo "<--- $(basename "${FILE}" | sed -E 's/(.*)\.(.*)$/\1/') --->" >> "${OUT_FILE}"
        cat "${FILE}" >> "${OUT_FILE}"
        echo -e "\n" >> "${OUT_FILE}"
    done < <(find "${DIR}" -type f)

done < <(find . -mindepth 1 -maxdepth 2 -type d )