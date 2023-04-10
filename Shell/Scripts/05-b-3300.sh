#!/bin/bash

if [[ ! -f ${file1} ]] ; then
  echo "${file1} is not found"
elif  [[ ! -f ${file2} ]] ; then
  echo "${file2} is not found"
elif  [[ ! -f ${file3} ]] ; then
  echo"${file3} is not found"
fi

$(paste file1 file2 | sort > ${file3})
cat "${file3}"

#Да се напише shell скрипт, който чете от стандартния вход имената на 3 файла,
#обединява редовете на първите два (man paste), подрежда ги по азбучен ред и
#резултата записва в третия файл.