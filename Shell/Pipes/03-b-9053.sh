#Използвайки файл population.csv, намерете коя държава има най-много население през 2016.
grep ",2016," population.csv | cut -d '"' -f3 | sort -k 4n -t ',' | tail -n 1 | cut -d ',' -f2 | xargs -I{} grep {} population.csv | head -n 1 | rev | cut -d ',' -f4 | rev | tr -d '"'

# А коя е с най-малко население?
grep ",2016," population.csv | cut -d '"' -f3 | sort -k 4n -t ',' | head -n 1 | cut -d ',' -f2 | xargs -I{} grep {} population.csv | head -n 1 | rev | cut -d ',' -f4 | rev | tr -d '"'

#or

cat population.csv | grep ",2016," | awk -F ',' '{print $0 ": " $NF}' | sort -k 2 -n -r -t ':' | cut -d ':' -f1 | awk -F ',' '{for (i=1; i < NF-2;i++) {printf "%s, ", $i;} printf "\n"}'| sed 's/"//g' | sed -E 's/(.*),/\1/g' | head -n 1

#(Hint: Погледнете имената на държавите)

