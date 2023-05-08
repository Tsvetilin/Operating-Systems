#Изпишете всички usernames от /etc/passwd с главни букви.

cat /etc/passwd | cut -f 1 -d ':' | tr a-z A-Z > ~/users