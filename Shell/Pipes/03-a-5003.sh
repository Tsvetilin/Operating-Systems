#Изведете имената на хората с второ име по-късо от 8 (<=7) символа според /etc/passwd 

cat /etc/passwd | cut -d ':' -f 5 | cut -d ' ' -f 2 | cut -d ',' -f 1 | grep -v -E '.{8,}'