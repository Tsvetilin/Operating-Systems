#Копирайте най-малкия файл от тези, намиращи се в /etc, в home директорията си.

cp $(find /etc/ -type f -exec wc -c {} \; 2> /dev/null | sort -n | head -n 1 | cut -d ' ' -f 2) ~