#Напишете серия от команди, извеждащи на екрана само inode-а на най-скоро променения (по съдържание) файл, намиращ се в home директорията на потребител pesho (или нейните
#под-директории), който има повече от едно име.

find ~ -type f -iname '* *' -printf "%A@ %p\n" | sort -k 1nr -t ' ' | head -1 | cut -d ' ' -f 2- | xargs -I{} stat {} | grep "Inode" | cut -d ':' -f3 | cut -d ' ' -f2

#or

find ~ -type f -iname '* *' -printf "%A@:%p:%i\n" | sort -k 1nr -t ' ' | head -1 | cut -d ':' -f3