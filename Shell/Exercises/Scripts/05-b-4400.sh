#!/bin/bash

if [[ ${#} -lt 1 ]] ; then
        echo "Expected at least one arg - dir"
        exit 1
fi

if [[ ${#} -gt 2 ]] ; then
        echo "No more than 2 args"
        exit 2
fi

if [[ ! -d ${1} ]] ; then
        echo "Expected dir"
        exit 3
fi

destDir="${1}"
OUTPUT=$(date -I)

if [[ ${#} -eq 2 ]] ; then
        OUTPUT=${2}
fi

for FILE in $(find ${destDir} -mindepth 1 -maxdepth 1 -type f -mmin -45) ; do
        cp ${FILE} ${OUTPUT}
done

#or

cp -r $(find ${FILE} -mindepth 1 -maxdepth 1 -mmin -45) ${OUTPUT}

tar -cf arhive.tar ${OUTPUT}

#Напишете shell script, който да приема параметър име на директория, от която взимаме файлове, и опционално експлицитно
#име на директория, в която ще копираме файлове. Скриптът да копира файловете със съдържание, променено преди по-малко
#от 45 мин, от първата директория във втората директория. Ако втората директория не е подадена по име, нека да получи
#такова от днешната дата във формат, който ви е удобен. При желание новосъздадената директория да се архивира.
