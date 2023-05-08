#Да се отпечатат PID на всички процеси, които имат повече деца от родителския си процес.

ps -A -o pid,ppid | awk '{parent[$1]=$2;children[$2]++;} END{for (child in children) {if (children[child] > children[parent[child]]) print child}}'
