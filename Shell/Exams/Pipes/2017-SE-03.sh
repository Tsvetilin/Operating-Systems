#!/bin/bash

if [[ "$EUID" -eq 0 ]] ; then
    echo "Script must be run as root"
    exit 1
fi

usr=0
while read uid pid rss ; do
    if [[ "${uid}" != "${usr}" ]] ; then
                avg=$(( mem * 2 / counter ))
                if [[ ${maxrss} -gt ${avg} ]] ; then
                        echo "Kill ${maxpid}"
                fi
                echo "${usr} ${counter} ${mem}"
                usr="${uid}"
                mem=0
                counter=0
                maxpid=0
                maxrss=0
    fi
    if [[ ${rss} -gt ${maxrss} ]] ; then
         maxrss=${rss}
         maxpid=${pid}
    fi
    mem=$(( mem + rss))
    counter=$(( counter + 1 ))
done < <(ps -e -o uid,pid,rss | tail -n +2 |sort -k1n)

#Напишете скрипт, който ако се изпълнява от root потребителя:
#а) извежда обобщена информация за броя и общото количество активна памет (RSS - resident set
#size, non-swaped physical memory that a task has used) на текущите процеси на всеки потребител;
#б) ако процесът с най-голяма активна памет на даден потребител използва два пъти повече памет
#от средното за потребителя, то скриптът да прекратява изпълнението му по подходящ начин.
#За справка:
#$ ps aux | head -5
#USER PID %CPU %MEM VSZ RSS TTY STAT START TIME COMMAND
#root 1 0.0 0.0 15820 1052 ? Ss Apr21 0:06 init [2]
#root 2 0.0 0.0 0 0 ? S Apr21 0:00 [kthreadd]
#root 3 0.0 0.0 0 0 ? S Apr21 0:02 [ksoftirqd/0]
#root 5 0.0 0.0 0 0 ? S< Apr21 0:00 [kworker/0:0H]
#Алтернативно, може да ползвате изхода от ps -e -o uid,pid,rss