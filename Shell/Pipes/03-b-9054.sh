#Използвайки файл population.csv, намерете коя държава е на 42-ро място по
#население през 1969. Колко е населението й през тази година?

cat population.csv | egrep ",1969," | awk -F ',' '{print $0 ": " $(NF)}' | sort -t ':' -k2n | cut -d ':' -f1 | tail -n 1| awk -F ',' '{for (i=1; i< (NF-2);i+=1) printf "%s,", $i} {print " " $(NF)}'
