#1. Изведете GID-овете на 5-те най-големи групи спрямо броя потребители, за които
#съответната група е основна (primary).
#
#2. (*) Изведете имената на съответните групи.
#
#Hint: /etc/passwd

cat /etc/passwd | cut -d ':' -f4 | sort -n | uniq -c | sort -nr | head -n 5 | awk '{print $2}' | xargs -I{} getent group {} | cut -d ':' -f1

#or 

cat /etc/passwd | cut -d ':' -f4 | sort -n | uniq -c | sort -nr | head -n 5 | awk '{print $2}' | xargs -I{} grep ':{}:' /etc/group | cut -d ':' -f 1