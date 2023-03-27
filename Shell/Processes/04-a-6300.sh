#Изведете командата на най-стария процес

ps -e -o comm= --sort=vsz | tail -n 1