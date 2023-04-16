#Напишете shell скрипт, който по подаден един позиционен параметър, ако този
#параметър е директория, намира всички symlink-ове в нея и под-директориите `и с несъществуващ
#destination.


 #!/bin/bash

if [[ $# -ne 1 ]]; then
    echo "Invalid number of arguments"
    exit 1
fi

if [[ ! -d $1 ]]; then
    echo "Not a directory"
    exit 2
fi

find $1 -type l -exec test ! -e {} \; -print

#or
for file in $(find ${1} -type l 2>/dev/null | xargs -I{} readlink {}) do

    if [[ ! -e ${file} ]] ; then
        echo ${file}
    fi
done
