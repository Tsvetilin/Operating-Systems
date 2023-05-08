#Отпечатайте потребителските имена и техните home директории от /etc/passwd.

cat /etc/passwd | cut -f 1,6 -d ':'| tr ':' ' '