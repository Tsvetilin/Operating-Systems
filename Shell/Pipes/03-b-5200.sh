#Намерете броя на символите, различни от буквата 'а' във файла /etc/passwd

grep -o "." /etc/passwd | grep -v "a|а" | sort | uniq -c | sort -nr

#or

cat /etc/passwd | sed 's/а//g' | wc -m
