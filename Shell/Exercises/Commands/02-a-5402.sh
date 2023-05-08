#Изведете всички обикновени ("regular") файлове, които само преките поддиректории на /etc съдържат

find /etc/ -mindepth 2 -maxdepth 2 -type f
