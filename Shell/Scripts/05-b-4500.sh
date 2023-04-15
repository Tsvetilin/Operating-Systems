#!/bin/bash

if [[ ${#} -ne 1 ]] ; then
        echo "Expected 1 argument - user id"
        exit 1
fi

if ! ( cat /etc/passwd | grep -q "${1}" ) ; then
        echo "User ${1} does not exists"
        exit 2
fi
# or
#
#if ! id "${1}" &> /dev/null ; then
#        echo "Invalid user id"
#        exit 2
#fi

echo "Waiting for user ${1} to log in"

while true ; do
        if who | grep -q "${1}" ; then
                echo "User ${1} is logged in"
                exit 0
        fi

        sleep 1
done

#v2

userName=${1}

egrep -q "${userName}" /etc/passwd
isUserExisting=${?}

if [[ ${isUserExisting} -ne 0 ]] ; then
        echo "User not existing"
        exit 2
fi

who | egrep -q "^${userName}"
isLogged=${?}

if [[ ${isLogged} -eq 0 ]] ; then
        echo "${userName} is logged"
else
        while true ; do
                echo "Waiting for user to log in"
                who | egrep -q "^${userName}"
                isLogged=${?}
                if [[ ${isLogged} -eq 0 ]] ; then
                        echo "${userName} is logged"
                        exit 0
                fi
                sleep 1
        done
fi

#Да се напише shell скрипт, който получава при стартиране като параметър в командния ред идентификатор на потребител.
#Скриптът периодично (sleep(1)) да проверява дали потребителят е log-нат, и ако да - да прекратява изпълнението си,
#извеждайки на стандартния изход подходящо съобщение.