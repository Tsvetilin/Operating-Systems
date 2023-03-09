#Направете нов файл с име по ваш избор, чието съдържание е конкатенирани
#съдържанията на file{1,2,3}.

cat dir5/file1 dir5/file2 dir5/file3 > temp.txt
# or
cat dir5/file[123] > temp.txt