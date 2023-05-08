#Колко файлове в /bin са 'shell script'-oве?
find /bin | xargs -I{} file {} | grep "shell script" | wc -l
#or
find /bin -type f -exec file {} \; | grep "shell script" | wc -l
#or
find /bin -type f -exec cat {} \; | grep "#!/bin/" | wc -l

#(Колко файлове в дадена директория са ASCII text?)
find /bin | xargs -I{} file {} | grep "ASCII text executable" | wc -l
