#Копирайте <РЕПО>/exercises/data/ssa-input.txt във вашата home директория.
#Общият вид на файла е:
#
#- заглавна част:
#	Smart Array P440ar in Slot 0 (Embedded)
#
#- една или повече секции за масиви:
#	Array A
#	Array B
#	...
#	като буквата (A, B, ...) е името на масива
#
#- във всяка таква секция има една или повече подсекции за дискове:
#	physicaldrive 2I:0:5
#	physicaldrive 2I:0:6
#	...
#
#	като 2I:0:5 е името на диска
#
#- във всяка подсекция за диск има множество параметри във вида:
#	key name: value
#	като за нас са интересни само:
#
#		Current Temperature (C): 35
#		Maximum Temperature (C): 36
#
#Напишете поредица от команди която обработва файл в този формат, и генерира
#следният изход:
#
#A-2I:0:5 35 36
#A-2I:0:6 34 35
#B-1I:1:1 35 50
#B-1I:1:2 35 49
#
#x-yyyyyy zz ww
#
#където:
#	- x е името на масива
#	- yyyyyy е името на диска
#	- zz е current temperature
#	- ww е max temperature

cp /srv/fmi-os/exercises/data/ssa-input.txt ~
cat ssa-input.txt | awk '{if ($1=="Array") array = $2} {if ($1=="physicaldrive") drive = $2} {if ($2=="Temperature" && $1=="Current") ct = $4;} {if  ($2=="Temperature" && $1=="Maximum") {mt = $4; printf "%s-%s %s %s",array,drive,ct,mt; printf"\n"}}'
