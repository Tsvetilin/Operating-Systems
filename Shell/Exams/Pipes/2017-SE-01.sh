#Намерете имената на топ 5 файловете в текущата директория с най-много hardlinks.

find ~ -type f | xargs -I{} stat -c "%h %n" {} | sort -k 1nr -t ' ' | head -n 5 | cut -d ' ' -f2
