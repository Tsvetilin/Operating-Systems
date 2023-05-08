#Запазете само потребителските имена от /etc/passwd във файл users във вашата home директория.

cat /etc/passwd | cut -f 1 -d ':' > ~/users