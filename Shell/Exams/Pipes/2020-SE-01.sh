# За всички файлове, които (едновременно):
# • се намират някъде във вашата home директория;
# • имат права, каквито биха имали, ако ги бяхте създали с маска 0022
# променете правата им така, че group owner-а на файла да има право да пише в него.

find ~ -type f -perm 755 | xargs -I{} chmod g+w {}
#or
find ~ -type f -perm 755 -exec chmod g+w {} \;
#or
find ~ -type f 2>/dev/null | xargs -I{} stat -c "%a %n" {} | awk '{if ($1=="755") print $2}' | xargs -I{} chmod g+w {}
