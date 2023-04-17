
#!/bin/bash

if [[ $# -ne 1 ]] ; then
    echo "Expected 1 argument - username"
    exit 1
fi

if [[ "$EUID" -eq 0 ]] ; then
    echo "Script must be executed as root"
    exit 1
fi

prCount=$(ps -e -o user,pid | grep "${1}" | wc -l)
ps -e -o user,pid | awk '{print $1}' | sort | uniq -c | awk -v var=${prCount} '{if ($(NF-1) > var) print $NF}'
counter=$(ps -e -o pid | wc -l)

for time in $(ps -e -o pid,time | awk '{print $2}' | tail -n +2) ; do
        timestamp=$(date -u -d "1970-01-01 ${time}" +%s)
        seconds=$(( timestamp % 86400 ))
        totalSeconds=$(( totalSeconds + seconds ))
done
totalSeconds=$(( totalSeconds / counter ))

time_value=$(date -u -d "@${totalSeconds}" +'%H:%M:%S')

totalSeconds=$(( totalSeconds * 2 ))
for process in $(ps -e -o user,pid,time | grep "${1}" | tail -n +2) ; do
        pr=$(echo "${process}" | awk '{print $2}')
    time=$(echo "${process}" | awk '{print $3}')
    timestamp=$(date -d "1970-01-01 ${time}" +%s)
        seconds=$(( timestamp % 86400 ))
        totalSeconds1=$(( totalSeconds + seconds ))
    if [[ ${totalSeconds1} -gt ${totalSeconds} ]] ; then
                echo "${pr}"
    fi
done

#Напишете скрипт, който приема задължителен позиционен аргумент - име на потребител FOO. Ако скриптът се изпълнява от root:
#а) да извежда имената на потребителите, които имат повече на брой процеси от FOO, ако има
#такива;
#б) да извежда средното време (в секунди), за което са работили процесите на всички потребители
#на системата (TIME, във формат HH:MM:SS);
#в) ако съществуват процеси на FOO, които са работили над два пъти повече от средното време,
#скриптът да прекратява изпълнението им по подходящ начин.
#За справка:
#$ ps -e -o user,pid,%cpu,%mem,vsz,rss,tty,stat,time,command | head -5
#USER PID %CPU %MEM VSZ RSS TT STAT TIME COMMAND
#root 1 0.0 0.0 15820 1920 ? Ss 00:00:05 init [2]
#root 2 0.0 0.0 0 0 ? S 00:00:00 [kthreadd]
#root 3 0.0 0.0 0 0 ? S 00:00:01 [ksoftirqd/0]
#root 5 0.0 0.0 0 0 ? S< 00:00:00 [kworker/0:0H]
