#Отпечатайте последната цифра на UID на всички редове между 28-ми и 46-ред в /etc/passwd.

cat /etc/passwd | head -n 46 | tail -n 28 | cut -d ':' -f 3 | egrep -o '.$'
#or
cat /etc/passwd | head -n 46 | tail -n 28 | cut -d ':' -f 3 | rev | cut -c 1 
#or
cat /etc/passwd | head -n 46 | tail -n 28 | cut -d ':' -f 3 | sed 's/.*\(.\)/\1/'