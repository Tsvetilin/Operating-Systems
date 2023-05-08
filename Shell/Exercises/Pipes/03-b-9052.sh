# Използвайки файл population.csv, намерете през коя година в България има най-много население.

grep "BGR" population.csv | sort -k4nr -t ',' | head -n 1 |cut -d ',' -f3

#or

cat population.csv | egrep 'Bulgaria' | sort -r -n -k 4 -t ',' | head -n 1 | cut -d ',' -f 3

#or

cat population.csv | awk -F ',' 'BEGIN{year=0; max=0} $(NF-2)=="BGR"{if(max<$(NF)){max=$NF; year=$(NF-1)}} END{print year}'