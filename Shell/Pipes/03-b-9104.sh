#Да се изведат всички албуми, сортирани по година.

find songs -mindepth 1| cut -d '-' -f2 | cut -c 2- | cut -d '(' -f2 | cut -d ')' -f1 | sort -k 2n -t ',' | uniq | cut -d ',' -f1
