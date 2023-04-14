# Изведете имената на потребителите, които имат поне 2 процеса, чиято команда е vim (независимо какви са аргументите й)

ps -A -o user,cmd | grep "vim" | awk '{print $1}' | sort | uniq -c | awk '{for (i=NF;i>=1;i--) printf "%s\t",$i; printf "\n"}' | awk '{if ($2>=2) print $0}'
