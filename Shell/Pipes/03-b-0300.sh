Намерете само Group ID-то си от файлa /etc/passwd.

grep $(id -u) /etc/passwd | cut -d ':' -f4
