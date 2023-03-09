#Създайте следната файлова йерархия в home директорията ви:
#dir5/file1
#dir5/file2
#dir5/file3
#
#Посредством vi въведете следното съдържание:
#file1:
#1
#2
#3
#
#file2:
#s
#a
#d
#f
#
#file3:
#3
#2
#1
#45
#42
#14
#1
#52

mkdir -p dir5
echo -e '1\n2\n3' > dir5/f1
echo -e 's\na\nd\nf' > dir5/f2
echo -e '3\n2\n1\n45\n42\n14\n1\n52' > dir5/f3

#Изведете на екрана:
#	* статистика за броя редове, думи и символи за всеки един файл
printf "lines: $(wc -l  dir5/file1)\n" && printf "words: $(wc -w  dir5/file1)\n" && printf "symbols: $(wc -c  dir5/file1)\n"
printf "lines: $(wc -l  dir5/file2)\n" && printf "words: $(wc -w  dir5/file2)\n" && printf "symbols: $(wc -c  dir5/file2)\n"
printf "lines: $(wc -l  dir5/file3)\n" && printf "words: $(wc -w  dir5/file3)\n" && printf "symbols: $(wc -c  dir5/file3)\n"
# or
wc dir5/* | head -n -1 
# or
wc dir5/f[123] | head -n -1

#	* статистика за броя редове и символи за всички файлове
printf "lines: $(wc -l dir5/file1)\n" && printf "symbols: $(wc -c  dir5/file1)\n"
printf "lines: $(wc -l dir5/file2)\n" && printf "symbols: $(wc -c  dir5/file2)\n"
printf "lines: $(wc -l dir5/file3)\n" && printf "symbols: $(wc -c  dir5/file3)\n"
#or
wc -lc dir5/* | head -n -1
# or
wc -l -c dir5/f[123] | head -n -1

#	* общия брой редове на трите файла
wc -l dir5/* | tail -n 1
# or
wc -l dir5/f[123] | tail -n 1