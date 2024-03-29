#!/bin/bash

function collapse {
        in_braces=no

        while read line; do
                if echo "${line}" | grep -q -E "\($"; then
                        in_braces=yes
                        echo "${line}" | sed -E 's/\($//g' | tr -d '\n'
                elif echo "${line}" | grep -q -E "^\)"; then
                        in_braces=no
                        echo "${line}" | sed -E 's/^\)//g'
                elif [[ "${in_braces}" == yes ]]; then
                        echo -n " ${line}"
                else
                        echo "${line}"
                fi
        done < <(cat "${1}" | sed 's/\s*;.*//g')
}

function get_serial_num {
        collapse "${1}" | awk '
                $2 == "IN" && $3 == "SOA" { print $6 }
                $3 == "IN" && $4 == "SOA" { print $7 }
                '
}

function increase_serial {
        old_date="$(echo "${1}" | cut -c 1-8)"
        old_num="$(echo "${1}" | cut -c 9-10 | sed 's/^0*//g')"
        today="$(date +'%Y%m%d')"

        if [[ "${old_date}" == "${today}" ]]; then
                if [[ "${old_num}" -eq 99 ]]; then
                        echo "error processing ${2}: cannot increase 99" >&2
                        return 1
                fi

                new_num=$(( old_num + 1 ))
                new_date="${old_date}"
        elif [[ "${old_date}" < "${today}" ]]; then
                new_num=1
                new_date="${today}"
        else
                echo "error processing ${2}: serial number is from the future" >&2
                return 1
        fi

        new_num="$(printf '%02d\n' "${new_num}")"

        echo "${new_date}${new_num}"
}

function bump_serial {
        if ! { [[ -r "${1}"  ]] && [[ -w "${1}" ]] }; then
                echo "cannot process ${1}: cannot read or write" >&2
                exit 1
        fi

        old_serial="$(get_serial_num "${1}")"
        if [[ -z "${old_serial}" ]]; then
                echo "cannot process ${1}: cannot find SOA record" >&2
                exit 1
        fi


        new_serial="$(increase_serial "${old_serial}" "${1}")"
        if [[ $? -ne 0 ]]; then
                return 1
        fi

        sed -i "s/${old_serial}/${new_serial}/g" "${1}"
}

if [[ ${#} -eq 0 ]]; then
        echo "${0} bumps the serial numbers of zone files"
        echo "usage: ${0} <one or more zone files>"
        exit 1
fi

for file in "${@}"; do
        bump_serial "${file}"
done

#Един от често използваните DNS сървъри е BIND9, при който описанието на DNS зоните обикновенно стои в текстови файлове, наричани зонални файлове. За улеснение, в рамките на задачата,
#ще ползваме опростено описание на зоналните файлове.
#Под whitespace разбираме произволна комбинация от табове и интервали.
#Под FQDN разбираме низ, който има допустими символи малки латински букви, цифри и точка; не
#може да започва с точка, не може да има две или повече съседни точки, задължително завършва
#с точка.
#Зоналните файлове съдържат ресурсни записи, по един на ред. Общият вид на даден ресурсен запис
#e <ключ> <TTL> <клас> <тип> <RDATA>, разделени с whitespace, например:
#astero.openfmi.net. 3600 IN A 185.117.82.99
#Където:
#• ключ (astero.openfmi.net.) – FQDN
#• TTL (3600) – цифри; полето може да липсва
#• клас (IN) - главни латински букви; класът винаги е IN
#• тип (A) - главни латински букви; някое от SOA, NS, A, AAAA
#• RDATA (185.117.82.99) - данни на записа; различни за различните типове ресурсни записи;
#всичко след типа до края на реда.
#Знакът точка-и-запетая ; е знак за коментар, и всичко след него до края на реда се игнорира.
#Във всеки зонален файл трябва да има точно един SOA запис, и той трябва да е първият запис във
#файла. Пример за едноредов SOA запис:
#openfmi.net. 3600 IN SOA nimbus.fccf.net. root.fccf.net. 2021041901 86400 7200 3024000 3600
#RDATA-та на SOA запис се състои от два FQDN-а и пет числа, разделени с whitespace.
#Въпреки, че горното е валиден SOA запис, за прегледност в зоналните файлове често се ползва
#следният синтаксис (многоредов SOA запис, еквивалентен на горния):
#openfmi.net. 3600 IN SOA nimbus.fccf.net. root.fccf.net. (
#2021041901 ; serial
#86400 ; refresh
#7200 ; retry
#3024000 ; expire
#3600 ; negative TTL
#)
#т.е., поредицата от числа се разбива на няколко реда, оградени в обикновенни скоби, и за всяко
#число се слага коментар какво означава.
#Първото от тези числа (serial) представлява серийният номер на зоната, който трябва да се увеличава всеки път, когато нещо в зоналният файл се промени. Изключително важно е това число само
#да нараства, и никога да не намалява.
#Един от често използваните формати за сериен номер, който показва кога е настъпила последната
#промяна в зоналния файл представлява число във вида YYYYMMDDTT, т.е., четири цифри за
#година, две цифри за месец, две цифри за дата и още две цифри за поредна промяна в рамките на
#описания ден. За последните две цифри (ТТ) има ограничение да са от 00 до 99 (естествено, така
#не може да има повече от 100 промени в рамките на един ден).
#За удобство приемаме, че конкретен сериен номер (точната поредица цифри) се среща само на едно
#място в зоналния файл.
#Напишете шел скрипт, който по подадени имена на зонални файлове променя серийният номер в
#SOA записа на всеки файл по следният алгоритъм:
#18
#• ако датата в серийният номер е по-стара от днешната, новият сериен номер трябва да е от
#вида днешнадата00
#• ако датата в серийният номер е равна на днешната, серийният номер трябва да се увеличи с
#единица
#Важат следните условия:
#• скриптът трябва да може да обработва и едноредови, и многоредови SOA записи
#• за всеки зонален файл, който не е успял да обработи, скриптът трябва да вади съобщение за
#грешка, което включва и името на зоналния файл. Съобщенията трябва да са лесно обработваеми с познатите инструменти за обработка на текст.