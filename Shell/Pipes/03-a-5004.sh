#Изведете целите редове от /etc/passwd за хората от 03-a-5003
cat /etc/passwd | cut -d ':' -f5 | cut -d ',' -f1 | cut -d ' ' -f2 | grep -v -E '.{8,}$' | grep -E '[^a-zA-Z]' | sort | uniq | xargs -I{} grep -w {} /etc/passwd 
