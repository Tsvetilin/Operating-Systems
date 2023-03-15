#Във file2 (inplace) подменете всички малки букви с главни.

sed -i 's/[a-z]/\U&/g' dir5/file2
