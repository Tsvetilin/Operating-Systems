#Използвайки файл population.csv, намерете колко е общото население на света
#през 2008 година. А през 2016?

cat population.csv | egrep ",2008," | awk -F ',' '{sum += $NF} END{print sum}'

#or

cat population.csv | egrep ",2016," | awk -F ',' '{sum += $NF} END{print sum}'