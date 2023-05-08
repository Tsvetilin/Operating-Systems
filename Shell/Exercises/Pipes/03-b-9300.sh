#Дадени са ви 2 списъка с email адреси - първият има 12 валидни адреса, а
#вторията има само невалидни. Филтрирайте всички адреси, така че да останат
#само валидните. Колко кратък регулярен израз можете да направите за целта?
#
#Валидни email адреси (12 на брой):
#email@example.com
#firstname.lastname@example.com
#email@subdomain.example.com
#email@123.123.123.123
#1234567890@example.com
#email@example-one.com
#_______@example.com
#email@example.name
#email@example.museum
#email@example.co.jp
#firstname-lastname@example.com
#unusually.long.long.name@example.com
#
#Невалидни email адреси:
##@%^%#$@#$@#.com
#@example.com
#myemail
#Joe Smith <email@example.com>
#email.example.com
#email@example@example.com
#.email@example.com
#email.@example.com
#email..email@example.com
#email@-example.com
#email@example..com
#Abc..123@example.com
#(),:;<>[\]@example.com
#just"not"right@example.com
#this\ is"really"not\allowed@example.com


cat emails.txt | grep '^[_0-9a-zA-Z][a-zA-Z0-9_.-]*[a-zA-Z0-9_]@[a-z0-9A-Z][a-zA-Z0-9_.-]*.[a-zA-Z]*$' | grep -v '.*[.][.].*'