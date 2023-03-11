#Използвайки файл population.csv, намерете коя държава има най-много население през 2016.
grep ",2016," population.csv | cut -d '"' -f3 | sort -k 4n -t ',' | tail -n 1 | cut -d ',' -f1

# А коя е с най-малко население?
grep ",2016," population.csv | cut -d '"' -f3 | sort -k 4n -t ',' | head -n 1 | cut -d ',' -f1

#(Hint: Погледнете имената на държавите)

