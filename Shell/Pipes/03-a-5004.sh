#Изведете целите редове от /etc/passwd за хората от 03-a-5003

cat /etc/passwd | cut -d ':' -f 5 | cut -d ' ' -f 2 | cut -d ',' -f 1 | grep -v -E '.{8,}' | xargs -I{} grep {} /etc/passwd