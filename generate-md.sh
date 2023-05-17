#!/bin/bash

if [[ ${#} -ne 7 ]] ; then
    echo "Invalid arguments! Usege: ${1} <pdf file with problems> <name of file to generate> <start task> <end task> <directory with solutions> <solution files extensions> <root dir for md>" >&2
    exit 1
fi

# TODO: check if files can be parsesd

FILE=${1}
OUT_FILE=${2}
START_TASK_NUM=${3}
END_TASK_NUM=${4}
DIR=${5}
FILES_EXTENSION=${6}
ROOT_DIR=${7}

pdftotext "${FILE}" "${FILE}.converted"

FILES_NO_EXTENSION=$(find "${DIR}" -type f -iname "*${FILES_EXTENSION}" | sed -E "s/(.*)${FILE_EXTENSION}$/\1/" )
FILES_BASENAMES=$(echo "${FILES_NO_EXTENSION}" | xargs -I{} basename {} )
TASK_NUM_ID=$(cat "${FILE}.converted" | grep -o -E '\. [0-9]+ 20[0-9]+\-[A-Z]+-[0-9]+' | cut -c 3- | awk -v start="${START_TASK_NUM}" -v end="${END_TASK_NUM}" '$1>=start && $1<=end')

echo "|Task no.|Task id|Solution|" > "${OUT_FILE}"
echo "|---|---|---|" >> "${OUT_FILE}"

while read TASK_NUM TASK_ID ; do
    SOLUTION="MISSING"

    if echo "${FILES_BASENAMES}" | grep -q "${TASK_ID}" ; then
        SOLUTION="[Solution](<${ROOT_DIR}${TASK_ID}${FILES_EXTENSION}>)"
    fi

    echo "|${TASK_NUM}|${TASK_ID}|${SOLUTION}|" >> "${OUT_FILE}"


done < <(echo "${TASK_NUM_ID}")

if [[ $(echo "${TASK_NUM_ID}" | cut -d ' ' -f 2 | wc -l ) -ne $(echo "${TASK_NUM_ID}" | cut -d ' ' -f 2 | sort | uniq | wc -l) ]] ; then
    echo "The following tasks need revision: "
    echo "${TASK_NUM_ID}" | cut -d ' ' -f 2 | sort | uniq -c  | awk '$1>1{print $2}'
fi

rm "${FILE}.converted"