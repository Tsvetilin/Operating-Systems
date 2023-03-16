#Във file3 (inplace) изтрийте всички "1"-ци.

sed -i 's/1//g' dir5/file3
#or
cat file3 | tr -d '1' > file3
