#Прочетете текстов файл file1 и направете всички главни букви малки като
#запишете резултата във file2.

sed 's/[A-Z]/\L&/g' dir5/file1 > dir5/file2

#or

cat dir5/file1 | tr A-Z a-z > dir5/file2
