#Изведете имената на хората с второ име по-късо от 8 (<=7) символа според /etc/passwd 

cat /etc/passwd | cut -d ':' -f 5 | cut -d ',' -f1 | cut -d ' ' -f2 | grep -v -E '.{8,}$' | grep -E '[^a-zA-Z]' | sort | uniq | xargs -I{} grep -w {} /etc/passwd | wc -l
