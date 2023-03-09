#Намерете броя на уникалните символи, използвани в имената на потребителите от
#/etc/passwd.

cat /etc/passwd | cut -d ':' -f5 | cut -d ',' -f1 | grep -o . | sort -u | wc -l
