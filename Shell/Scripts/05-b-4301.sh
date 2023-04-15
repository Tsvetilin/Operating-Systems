#!/bin/bash

if [[ $# -ne 3 ]] ; then
        echo "Expected 3 arguments"
        exit 1
fi

fileName=${1}
fullName=${2}
nickName=${3}

userName=$(grep "${fullName}" /etc/passwd | awk -F ':' '{print $1}')
candidates=$(echo "${userName}" | wc -l)

if [[ ${userName} == "" ]] ; then
        echo "User not found"
        exit 1
fi

if [[ ${candidates} -gt 1 ]] ; then
        index=1
        while read line ; do
                echo "${index}.${line}"
                arr[index]=${line}
                index=$(( index + 1 ))
        done < <(echo "${userName}")
        echo "0.Exit"
        read -p "Choose from 1-${candidates}: " choice
        if [[ ${choice} -lt 0 ]] || [[ ${choice} -gt ${candidates} ]] ; then
                echo "Invalid choice"
                exit 1
        else
                if [[ ${choice} -eq 0 ]] ; then
                        exit 0
                else
                        userName=${arr[${choice}]}
                fi
        fi
fi

echo "${nickName} ${userName}" >> ${fileName}

#Напишете shell script, който автоматично да попълва файла указател от предната задача по подадени аргументи:
#име на файла указател, пълно име на човека (това, което очакваме да е в /etc/passwd) и избран за него nickname.
#Файлът указател нека да е във формат:
#<nickname, който лесно да запомните> <username в os-server>
#// може да сложите и друг delimiter вместо интервал
#
#Примерно извикване:
#./pupulate_address_book myAddressBook "Ben Dover" uncleBen
#
#Добавя към myAddressBook entry-то:
#uncleBen <username на Ben Dover в os-server>
#
#***Бонус: Ако има няколко съвпадения за въведеното име (напр. има 10 човека Ivan Petrov в /etc/passwd),
#всички те да се показват на потребителя, заедно с пореден номер >=1,
#след което той да може да въведе някой от номерата (или 0 ако не си хареса никого), и само избраният да бъде добавен
#към указателя.