#Използвайки файл population.csv, намерете колко е общото население на света
#през 2008 година. А през 2016?

cat population.csv | egrep ",2008," | awk -F ',' '{sum += $NF} END{print sum}'
cat population.csv | egrep ",2016," | awk -F ',' '{sum += $NF} END{print sum}'

#or

cat population.csv | awk -F ',' 'BEGIN {counter = 0} $3==2008 {counter+=$4} END {print counter}'

#or

cat population.csv | egrep ",2008," | cut -d ',' -f 4 | sed 's/.*/+ &/g' | xargs | tail -c +3 | bc