#Във file2 (inplace) подменете всички малки букви с главни.

sed -i 's/[a-z]/\U&/g' dir5/file2
#or
cat file2 | tr 'a-z' 'A-Z' > file2
