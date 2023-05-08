#Изведете имената на потребителите, които не са логнати в момента, но имат живи процеси

comm -1 -3 <(who | cut -d ' ' -f 1 | sort -u ) <(ps -e -o user= | sort -u)

#or

ps -e -o user= | grep -Fvf <(who | cut -d ' ' -f 1 | sort -u) | sort | uniq