#Създайте директорията practice/01 във вашата home директория.
#Създайте 3 файла в нея - f1, f2, f3.
#Копирайте файловете f1, f2, f3 от директорията practice/01/ в директория dir1, намираща се във вашата home директория. Ако нямате такава, създайте я.

mkdir -p ~/practice/01
touch ~/practice/01/f1
touch ~/practice/01/f2
touch ~/practice/01/f3
mkdir -p ~/dir1
cp ~/practice/01/f1 ~/practice/01/f2 ~/practice/01/f3 ~/dir1