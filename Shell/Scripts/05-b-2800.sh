#Да се напише shell скрипт, който приема точно един параметър и проверява дали подаденият му параметър се състои само от букви и цифри.

#!/bin/bash

# v1
if [[ $# -ne 1 ]] ; then
        echo "Invalid number of arguments"
        exit 1
fi

if [[ ! $1 =~ ^[a-zA-Z0-9]+$ ]] ; then
        echo "Invalid input"
        exit 2
fi

echo "yes"

# v2
if [[ ${#} -ne 1 ]] ; then
        echo "1 argument expected"
        exit 1
fi

if echo "${1}" | grep -q -E '^[a-zA-Z0-9]*$' ; then
        echo "yes"
else
        echo "no"
fi

#v3
grep -q -E -v '^[a-zA-Z0-9]*$' <(echo "${1}")
echo "${?}"