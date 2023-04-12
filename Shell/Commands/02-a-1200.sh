#Отпечатайте имената на всички директории в директорията /home.

ls -d /home/*/

#or

find /home -mindepth 1 -maxdepth 1 -type d

