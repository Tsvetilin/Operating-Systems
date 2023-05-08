#Създайте файл, който да съдържа само последните 10 реда от изхода на 02-a-5403

find /etc/ -mindepth 1 -maxdepth 1 -type d | tail -n 10 > result.txt

# or

find /etc/ -mindepth 1 -maxdepth 1 -type d | tail > result.txt
