# Използвайки файл population.csv, намерете през коя година в България има най-много население.

grep "BGR" population.csv | sort -k4nr -t ',' | head -n 1 |cut -d ',' -f3
