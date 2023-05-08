#Изведете статистика за най-често срещаните символи в трите файла.

grep -o '.' dir5/file{1,2,3} | sort | uniq -c | sort -nr

#or

cat file{1,2,3} | sed 's/./&\n/g' | sort | uniq -c | sort -nr