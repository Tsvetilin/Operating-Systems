#!/bin/bash

if [[ ${#} -ne 2 ]] ; then
        echo "Expected 2 arguments"
        exit 1
fi

FILE="${1}"
DIR="${2}"

headFile=$(echo ${files} | head -n1)

echo "hostname,phy,vlans,hosts,failover,VPN-3DES-AES,peers,VLAN Trunk Ports,license,SN,key" >> "${FILE}"

while read file ; do
        baseName=$(echo "${file}" | sed - E 's/.*/(.*)$/\1/' | sed -E 's/\.log//')
        field="${baseName},"
        resultFile=$(cat "${file}" | sed -E '(This platform has a[n]{0,1})(.*)/\1: \2' | sed -E 's/license[.]$//' | egrep ":")
        while read line ; do
                field+=$(echo "${line}" | awk -F ':' '{printf "%s,", $2}' | xargs)
        done < <(cat ${resultFile})
        field=$(echo "${field}" | sed -E 's/,$//')
        echo "${field}" >> "${FILE}"
done < <(find "${DIR}" -mindepth 1 -type f -name "*.log")

#Напишете shell скрипт, който получава два задължителни позиционни параметъра
#- име на файл (bar.csv) и име на директория. Директорията може да съдържа текстови файлове
#с имена от вида foobar.log, всеки от които има съдържание от следния вид:
#Пример 1 (loz-gw.log):
#Licensed features for this platform:
#Maximum Physical Interfaces : 8
#VLANs : 20
#Inside Hosts : Unlimited
#Failover : Active/Standby
#VPN-3DES-AES : Enabled
#*Total VPN Peers : 25
#VLAN Trunk Ports : 8
#This platform has an ASA 5505 Security Plus license.
#Serial Number: JMX00000000
#Running Activation Key: 0e268e0c
#Пример 2 (border-lozenets.log):
#Licensed features for this platform:
#Maximum Physical Interfaces : 4
#VLANs : 16
#Inside Hosts : Unlimited
#Failover : Active/Active
#VPN-3DES-AES : Disabled
#*Total VPN Peers : 16
#VLAN Trunk Ports : 4
#This platform has a PIX 535 license.
#Serial Number: PIX5350007
#Running Activation Key: 0xd11b3d48
#Имената на лог файловете (loz-gw, border-lozenets) определят даден hostname, а съдържанието
#им дава детайли за определени параметри на съответният хост.
#Файлът bar.csv, който трябва да се генерира от вашия скрипт, е т.н. CSV (comma separated values)
#файл, тоест текстови файл - таблица, на който полетата на всеки ред са разделени със запетая.
#Първият ред се ползва за определяне на имената на колоните.
#Скриптът трябва да създава файла bar.csv на база на log файловете в директорията. Генерираният
#CSV файл от директория, която съдържа само loz-gw.log и border-lozenets.log би изглеждал
#така:
#hostname,phy,vlans,hosts,failover,VPN-3DES-AES,peers,VLAN Trunk Ports,license,SN,key
#loz-gw,8,20,Unlimited,Active/Standby,Enabled,25,8,ASA 5505 Security Plus,JMX00000000,0e268e0c
#border-lozenets,4,16,Unlimited,Active/Active,Disabled,16,4,PIX 535,PIX5350007,0xd11b3d48
#Полетата в генерирания от скрипта CSV файл не трябва да съдържат излишни trailing/leading
#интервали. За улеснение, приемете, че всички whitespace символи във входните файлове са символа
#“интервал”.
