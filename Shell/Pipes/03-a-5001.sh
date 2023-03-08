#Изведете колко потребители не изпозват /bin/bash за login shell според /etc/passwd
#(hint: 'man 5 passwd' за информация какъв е форматът на /etc/passwd)

cat /etc/passwd | cut -d ':' -f 7 | grep -v '/bin/bash' | wc -l