#Създайте файл permissions.txt в home директорията си. За него дайте единствено - read права на потребителя създал файла, write and exec на групата, read and exec на всички останали. Направете го и с битове, и чрез "буквички".

# rwx
touch ~/permissions.txt
chmod u=r,g=wx,o=rx ~/permissions.txt
chmod 435 ~/permissions.txt
