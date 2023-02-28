# Направете директория practice-test в home директорията ви. Вътре направете директория test1. Можете ли да направите тези две неща наведнъж? Разгледайте нужната man страница. След това създайте празен файл вътре, който да се казва test.txt, преместете го в practice-test чрез релативни пътища.

mkdir -p ~/practice-test/test1
touch ~/practice-test/test1/test.txt
mv ./practice-test/test1/test.txt ./practice-test