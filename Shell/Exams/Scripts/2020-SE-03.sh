#!/bin/bash

if [[ ${#} -ne 3 ]] ; then
        echo "Expected 3 arguments"
        exit 1
fi

FILE="${1}"
key=${2}
value=${3}

echo "${key}" | egrep -q "^[a-zA-Z0-9_]+$"

if [[ ${?} -ne 0 ]] ; then
        echo "Invalid key"
        exit 1
fi

echo "${key}" | egrep -q "^[a-zA-Z0-9_]+$"

if [[ ${?} -ne 0 ]] ; then
        echo "Invalid value"
        exit 1
fi

line=$(cat "${FILE}" | awk -v var=${key} '{if ($1==var) print $0}')
echo "${line}"
if [[ -z ${line} ]] ; then
        echo "${key} = ${value} # added at $(date) by $(whoami)" >> ${FILE}
else
        sed -i -E "s/(${line})/#\1 # edited at $(date) by $(whoami) \n${key} = ${value} # added at $(date) by $(whoami)/" ${FILE}
fi

#Под конфигурационен файл ще разбираме файл, в който има редове от вида key=value,
#където key и value могат да се състоят от букви, цифри и знак “долна черта” (“_”). Освен това,
#във файла може да има празни редове; може да има произволен whitespace в началото и в края на
#редовете, както и около символа “=”. Също така са допустими и коментари в даден ред: всичко след
#символ “#” се приема за коментар.
#Под <date> ще разбираме текущото време, върнато от командата date без параметри; под <user>
#ще разбираме името на текущият потребител.
#Напишете shell скрипт set_value.sh, който приема 3 позиционни аргумента – име на конфигурационен файл, ключ (foo) и стойност (bar). Ако ключът:
#• присъства във файла с друга стойност, скриптът трябва да:
#– да закоментира този ред като сложи # в началото на реда и добави в края на реда #
#edited at <date> by <user>
#– да добави нов ред foo = bar # added at <date> by <user> точно след стария ред
#• не присъства във файла, скриптът трябва да добави ред от вида foo = bar # added at
#<date> by <user> на края на файла
#Примерен foo.conf:
## route description
#from = Sofia
#to = Varna # my favourite city!
#type = t2_1
#Примерно извикване:
#./set_value.sh foo.conf to Plovdiv
#Съдържание на foo.conf след извикването:
## route description
#from = Sofia
## to = Varna # my favourite city! # edited at Tue Aug 25 15:48:29 EEST 2020 by human
#to = Plovdiv # added at Tue Aug 25 15:48:29 EEST 2020 by human
#type = t2_1
