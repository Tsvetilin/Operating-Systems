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

#Изведете на екрана:
#	* статистика за броя редове, думи и символи за всеки един файл
printf "lines: $(wc -l  dir5/file1)\n" && printf "words: $(wc -w  dir5/file1)\n" && printf "symbols: $(wc -c  dir5/file1)\n"
printf "lines: $(wc -l  dir5/file2)\n" && printf "words: $(wc -w  dir5/file2)\n" && printf "symbols: $(wc -c  dir5/file2)\n"
printf "lines: $(wc -l  dir5/file3)\n" && printf "words: $(wc -w  dir5/file3)\n" && printf "symbols: $(wc -c  dir5/file3)\n"
wc dir5/* | head -n -1

#	* статистика за броя редове и символи за всички файлове
printf "lines: $(wc -l dir5/file1)\n" && printf "symbols: $(wc -c  dir5/file1)\n"
printf "lines: $(wc -l dir5/file2)\n" && printf "symbols: $(wc -c  dir5/file2)\n"
printf "lines: $(wc -l dir5/file3)\n" && printf "symbols: $(wc -c  dir5/file3)\n"
#or
wc -lc dir5/* | head -n -1

#	* общия брой редове на трите файла
wc -l dir5/* | tail -n 1