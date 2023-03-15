#Колко файлове в /bin са 'shell script'-oве?
find usr/bin | xargs -I{} file {} | grep "shell script" | wc -l

#(Колко файлове в дадена директория са ASCII text?)
find usr/bin | xargs -I{} file {} | grep "ASCII text executable" | wc -l
