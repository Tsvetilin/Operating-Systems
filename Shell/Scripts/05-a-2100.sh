#Редактирайте вашия .bash_profile файл, за да ви поздравява
#(или да изпълнява някаква команда по ваш избор) всеки път, когато влезете в системата.

name=$(egrep "^$(whoami)" /etc/passwd | awk -F ':' '{print $5}' | awk '{print $1}')
echo "Hello, ${name}"
#! \bin\bash

read -p "Enter 3 files: " file1 file2 file3


