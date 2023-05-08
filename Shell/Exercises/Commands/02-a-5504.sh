#Изведете всички обикновени файлове в директорията /tmp които са от вашата група, които имат write права за достъп за група или за останалите(o=w)

find /tmp -group 512 -perm -g=w,o=w

#or

find /tmp/ -type f -group $(id -g) -perm /022 2>/dev/null
