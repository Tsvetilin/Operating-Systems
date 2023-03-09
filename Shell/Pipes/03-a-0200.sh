#Сортирайте /etc/passwd лексикографски по поле UserID.

cat /etc/passwd | sort -k 3 -t ':'