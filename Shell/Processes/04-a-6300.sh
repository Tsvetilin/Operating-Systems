#Изведете командата на най-стария процес

ps -e -o comm= --sort=etimes | tail -n 1