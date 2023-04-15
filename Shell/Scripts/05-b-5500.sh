#!/bin/bash

users=$(who | awk '{print $1}' | sort -u)

echo "<table>"
echo "  <tr>"
echo "    <th>Username</th>"
echo "    <th>group</th>"
echo "    <th>login shell</th>"
echo "    <th>GECOS</th>"
echo "  </tr>"
while read username ; do
        echo "  <tr>"
        groupId=$(egrep "^${username}" /etc/passwd | awk -F ':' '{print $4}')
        group=$(egrep "${groupId}:$" /etc/group | awk -F ':' '{print $1}')
        loginShell=$(egrep "^${username}" /etc/passwd | awk -F ':' '{print $NF}')
        fullName=$(egrep "^${username}" /etc/passwd | awk -F ':' '{print $5}' | awk -F ',' '{print $1}')
        echo "    <td>${username}</td>"
        echo "    <td>${group}</td>"
        echo "    <td>${loginShell}</td>"
        echo "    <td>${fullName}</td>"
        echo "  </tr>"
done < <(echo "${users}")
echo "</table>"

#Да се напише shell script, който генерира HTML таблица съдържаща описание на
#потребителите във виртуалката ви. Таблицата трябва да има:
#- заглавен ред с имената нa колоните
#- колони за username, group, login shell, GECOS field (https://en.wikipedia.org/wiki/Gecos_field)
#
#Пример:
#$ ./passwd-to-html.sh > table.html
#$ cat table.html
#<table>
#  <tr>
#    <th>Username</th>
#    <th>group</th>
#    <th>login shell</th>
#    <th>GECOS</th>
#  </tr>
#  <tr>
#    <td>root</td>
#    <td>root</td>
#    <td>/bin/bash</td>
#    <td>GECOS here</td>
#  </tr>
#  <tr>
#    <td>ubuntu</td>
#    <td>ubuntu</td>
#    <td>/bin/dash</td>
#    <td>GECOS 2</td>
#  </tr>
#</table>