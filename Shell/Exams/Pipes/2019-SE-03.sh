# 3 От всички файлове в home директорията на потребителя velin, изведете дълбочината
# на файл, който:
# • има същия inode като този на най-скоро променения файл сред тях
# • има минимална дълбочина
# Пояснение Под "дълбочина"да се разбира дълбочина в дървото на файловата система: например
# файлът /foo/bar/baz има дълбочина 3.

find ~ -printf "%i %f %d\n" | grep $(find ~ -printf "%T@ %i\n" | sort -nr | head -nr 1 | cut -d ' ' -f2) \
| sort -k 3n | head -n 1 | cut -d ' ' -f2

#or if you don't know about find -printf "%d"

find . -type f | xargs -I{} stat -c "%i %n" {} | awk -v var=$(stat -c "%i" $(find . -type f -printf "%T@ %p\n" \
| sort -nr | head -n1 | awk '{print $2}')) '{if (var==$1) print $2}' | tr '/' ' ' | awk '{print NF " " $0}' \
| sort -k1n | head -n1 | awk '{for (i=2;i<NF;i++) printf "%s/",$i} END{printf "%s\n",$NF}'


