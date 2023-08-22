#!/bin/bash

TASKS_FILE="os-problems.pdf"
CONF_FILE="tasks.config"

TEMP_FILE="${TASKS_FILE}.converted"

pdftotext "${TASKS_FILE}" "${TEMP_FILE}"


TASK_NUM_ID=$(cat "${TEMP_FILE}" | grep -o -E '\. [0-9]+ 20[0-9]+\-[A-Z]+-[0-9]+' | cut -c 3-)


CONF=$(cat "${CONF_FILE}" | grep -v -E '^#' | grep ':')


while read SECTION_LINE; do

    if ( echo "${SECTION_LINE}" | grep -q -v -E '^:' ) ; then
        continue
    fi

    SECTION_LINE=$(echo "${SECTION_LINE}" | tr -s ' ')
    SECTION_NAME=$(echo "${SECTION_LINE}" | cut -d ':' -f 2)
    NUMBER_FOLDERS=$(echo "${SECTION_LINE}" | cut -d ':' -f 3 | cut -d '@' -f 1)
    SECTION_PATH=$(echo "${SECTION_LINE}" | cut -d '@' -f 2  | awk '{$1=$1;print}')
    OUT_FILE="${SECTION_PATH}/README.md"

    FOLDERS_CONF=$(echo "${CONF}" | grep "${SECTION_LINE}" -A ${NUMBER_FOLDERS} | grep -v "${SECTION_LINE}")

    cp "${TASKS_FILE}" "${SECTION_PATH}"
    
    echo -e "# ${SECTION_NAME}\n" > "${OUT_FILE}"
    echo -e "###### All task statements in [os-problems.pdf](<./os-problems.pdf>)\n" >> "${OUT_FILE}"
    echo -e "###### Sample data for some of the tasks in [GitHub](<https://github.com/avelin/fmi-os/tree/master/data>)\n" >> "${OUT_FILE}"

    while read FOLDER; do

        DIR=$(echo "${FOLDER}" | cut -d ':' -f 1)
        START_TASK_NO=$(echo "${FOLDER}" | cut -d ':' -f 2 | cut -d '-' -f 1)
        END_TASK_NO=$(echo "${FOLDER}" | cut -d ':' -f 2 | cut -d '-' -f 2)

        FILES_EXTENSION=$(find "${DIR}" -type f )
        
        FILES_NO_EXTENSION=$(echo "${FILES_EXTENSION}"| sed -E "s/(.*).[sh|c]$/\1/" )
        FILES_BASENAMES=$(echo "${FILES_EXTENSION}" | xargs -I{} basename {})

        echo "### $(echo "${DIR}" | xargs -i{} basename {} | sed -e 's/_/ /g')" >> "${OUT_FILE}"
        echo "">> "${OUT_FILE}"
        echo "|Task no.|Task id|Solution|" >> "${OUT_FILE}"
        echo "|---|---|---|" >> "${OUT_FILE}"

        SPEC_TASKS=$(echo "${TASK_NUM_ID}" | awk -v start="${START_TASK_NO}" -v end="${END_TASK_NO}" '$1>=start && $1<=end');

        while read TASK_NUM TASK_ID ; do
            SOLUTION="MISSING"

            if echo "${FILES_BASENAMES}" | grep -q "${TASK_ID}" ; then
                SOLUTION="[Solution](<./$(echo ${DIR} | xargs -I{} basename {})/$(echo ${FILES_BASENAMES} | tr " " "\n" | grep "${TASK_ID}" | head -n 1)>)"
            fi

            echo "|${TASK_NUM}|${TASK_ID}|${SOLUTION}|" >> "${OUT_FILE}"

        done < <(echo "${SPEC_TASKS}")

        if [[ $(echo "${SPEC_TASKS}" | cut -d ' ' -f 2 | wc -l ) -ne $(echo "${SPEC_TASKS}" | cut -d ' ' -f 2 | sort | uniq | wc -l) ]] ; then
            echo "The following tasks in ${DIR} need revision because of missmatched numbering : "
            echo "${SPEC_TASKS}" | cut -d ' ' -f 2 | sort | uniq -c  | awk '$1>1{print $2}'
        fi

    done < <(echo "${FOLDERS_CONF}")

done < <(echo "${CONF}")



rm "${TEMP_FILE}"