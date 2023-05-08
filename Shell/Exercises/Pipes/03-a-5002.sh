#Изведете само имената на хората с второ име по-дълго от 6 (>6) символа според /etc/passwd

cat /etc/passwd | cut -d ':' -f 5 | cut -d ' ' -f 2 | cut -d ',' -f 1 | grep -E '.{7,}$'

#or

cut /etc/passwd -d ':' -f5 | cut -d ',' -f1 | egrep ".* .{7,}"
