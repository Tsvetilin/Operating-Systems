#от предната задача: когато вече сте получили myetc с файлове, архивирайте всички от тях, които започват с 'c' в архив, който се казва c_start.tar
#изтрийте директорията myetc и цялото и съдържание
#изтрийте архива c_start.tar

tar -c -f c_start.tar $(find myetc/ -iname 'c*') # or tar -caf c_start.tar.zst $(find myetc/ -type f -iname 'c*')
rm -rf myetc/
rm -i c_start.tar